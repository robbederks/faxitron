/*
 ## Cypress FX2G2 Firmware Example Source (cyfx2g2_uvc.c)
 ## ===========================
 ##
 ##  Copyright Cypress Semiconductor Corporation, 2010-2018,
 ##  All Rights Reserved
 ##  UNPUBLISHED, LICENSED SOFTWARE.
 ##
 ##  CONFIDENTIAL AND PROPRIETARY INFORMATION
 ##  WHICH IS THE PROPERTY OF CYPRESS.
 ##
 ##  Use of this file is governed
 ##  by the license agreement included in the file
 ##
 ##     <install>/license/license.txt
 ##
 ##  where <install> is the Cypress software
 ##  installation root directory path.
 ##
 ## ===========================
 */

/* This application example implemnets a USB UVC 1.1 compliant uncompressed video camera device
 * on the Cypress FX2G2 device. The video data is generated within the device firmware, and sent
 * up to the USB host with the appropriate video headers.
 * The following video formats are supported:
 *      1. Uncompressed 16 bit YUV2 640x480 @30 fps over USB Hi-Speed
 *      2. Uncompressed 16 bit YUV2 320x240 @5 fps over USB Full Speed
 */

#include "cyu3system.h"
#include "cyu3os.h"
#include "cyu3dma.h"
#include "cyu3error.h"
#include "cyu3usb.h"
#include "cyu3i2c.h"
#include "cyu3uart.h"
#include "cyu3gpio.h"
#include "cyu3utils.h"
#include "cyu3pib.h"
#include "cyu3socket.h"
#include "sock_regs.h"
#include "cyfx2g2_uvc.h"

static CyU3PThread     UvcAppThread;            /* UVC streaming application thread. */
static CyU3PEvent      UvcAppEvent;             /* Event flags group used for thread communication. */
static CyU3PDmaChannel UvcDmaHandle;            /* DMA Channel Handle for UVC Stream  */

/* UVC Header Buffer */
static uint8_t glUVCHeader[UVC_HEADER_SIZE] =
{
    0x0C,                                       /* Header Length */
    0x8C,                                       /* Bit field header field */
    0x00,0x00,0x00,0x00,                        /* Presentation time stamp field */
    0x00,0x00,0x00,0x00,0x00,0x00               /* Source clock reference field */
};

/* Video Probe Commit Control */
static uint8_t glCommitCtrl[UVC_MAX_PROBE_SETTING_ALIGNED];

static volatile uint32_t UvcFrameBufferCount = 0;       /* Number of buffers committed for the current frame. */
static volatile uint16_t UvcDataPerBuffer    = 0;       /* Size of data per DMA buffer. */
static volatile CyBool_t UvcAppUpdateData    = CyTrue;  /* Whether data in DMA buffer needs to be updated. */
static volatile CyBool_t UvcClearEpHalt      = CyFalse; /* Flag to indicate that a CLEAR EP HALT request is active. */
static volatile CyBool_t UvcStreamActive     = CyFalse; /* Whether the UVC video stream is active. */
static volatile CyBool_t UvcDataInFlight     = CyFalse; /* Whether host has started reading UVC data. */
static volatile uint8_t  UvcFrameIndex       = 1;       /* Frame index selected. We only support a single frame. */

#ifdef RESET_TIMER_ENABLE

/* Maximum frame transfer time in milli-seconds. */
#define TIMER_PERIOD    (500)

/* Timer used to track frame transfer time. */
static CyU3PTimer        UvcTimer;              /* Timer used to calculate frame transfer time. */

/* This callback function is called when the timer expires, and indicates that a particular video frame has
 * taken too long to transfer. We abort and restart the stream when this happens.
 */
static void
UvcAppProgressTimer (
        uint32_t arg)
{
    /* This frame has taken too long to complete. Notify the thread to abort the frame and restart streaming. */
    CyU3PEventSet (&UvcAppEvent, DMA_RESET_EVENT, CYU3P_EVENT_OR);
}

#endif

/* Critical error handler: No meaningful actions are taken as of now. This function can be used to trigger
 * a message or LED that indicates error occurence; or to reset the FX2G2 device. */
static void
AppErrorHandler (
        CyU3PReturnStatus_t status)
{
    /* Application failed with the error code status */

    /* Add custom debug or recovery actions here */

    /* Loop indefinitely */
    for (;;)
    {
        CyU3PThreadSleep (100);
    }
}

#ifdef USE_ISOCHRONOUS_ENDPOINT

/* Definitions for the DEV_EPI_CS register on FX3/FX2G2. */
#define FX3_USB2_INEP_CFG_ADDR_BASE     (0xe0031418)
#define FX3_USB2_INEP_MULT_MASK         (0x00003000)
#define FX3_USB2_INEP_MULT_POS          (12)

/* Definitions for the EEPM_ENDPOINT register on FX3/FX2G2 */
#define FX3_USB2_INEP_EPM_ADDR_BASE     (0xe0031c40)
#define FX3_USB2_INEP_EPM_READY_MASK    (0x40000000)
#define FX3_USB2_INEP_EPM_DSIZE_MASK    (0x07FFF800)
#define FX3_USB2_INEP_EPM_DSIZE_POS     (11)

/* This function sets the MULT setting for ISOCHRONOUS video streaming endpoint based on the availability of data
 * in the DMA buffer. This function is called everytime there is a consume event, so that we can ensure that the
 * setting matches the actual data availability in the device. This is a work-around for an FX3/FX2G2 device errata
 * where the PID of an isochronous packet is not calculated based on the size of data available on the device.
 */
static void
UvcAppSetMultByEpm (
        uint8_t ep)
{
    uint32_t val1 = *((uvint32_t *)(FX3_USB2_INEP_CFG_ADDR_BASE + (4 * ep)));
    uint32_t val2 = *((uvint32_t *)(FX3_USB2_INEP_EPM_ADDR_BASE + (4 * ep)));
    uint8_t  multVal = 0;

    /* If the EPM is ready, find out how much data is present and then update the MULT setting. */
    if ((val2 & FX3_USB2_INEP_EPM_READY_MASK) != 0)
    {
        val2 = (val2 & FX3_USB2_INEP_EPM_DSIZE_MASK) >> FX3_USB2_INEP_EPM_DSIZE_POS;
        multVal = (val2 / 1024) + 1;
    }

    /* Adjust multVal to a value between 1 and 3. */
    multVal = CY_U3P_MIN (multVal, 3);
    multVal = CY_U3P_MAX (multVal, 1);

    val1 = (val1 & ~FX3_USB2_INEP_MULT_MASK) | (multVal << FX3_USB2_INEP_MULT_POS);
    *((uvint32_t *)(FX3_USB2_INEP_CFG_ADDR_BASE + (4 * ep))) = val1;
}

#endif

/* Function that adds the UVC header to the top of the current DMA buffer. This needs to be called for each
 * DMA buffer that is getting transferred.
 */
static void
UvcAddHeader (
        uint8_t *buffer_p)
{
    /* Copy header to buffer */
    CyU3PMemCopy (buffer_p, (uint8_t *)glUVCHeader, UVC_HEADER_SIZE);

    /* Check if we have completed the number of buffers required for this frame. */
    UvcFrameBufferCount++;
    if (((CyU3PUsbGetSpeed () == CY_U3P_HIGH_SPEED) && (UvcFrameBufferCount == UVC_BUF_PER_FRAME_HS)) ||
            ((CyU3PUsbGetSpeed () == CY_U3P_FULL_SPEED) && (UvcFrameBufferCount == UVC_BUF_PER_FRAME_FS)))
    {
        /* Indicate End of Frame in the buffer */
        buffer_p[1] |=  UVC_HEADER_EOF;

        /* Modify UVC header to toggle Frame ID from the next buffer onwards. */
        glUVCHeader[1] ^= UVC_HEADER_FRAME_ID;

        /* Clear the buffers per frame counter. */
        UvcFrameBufferCount = 0;
    }
}

/* This function fills all DMA buffers mapped to the UVC streaming channel with data to be
 * sent to the USB host. The buffers will be filled with the data pattern if the updateData
 * flag is true, and will only be marked occupied if not. */
static CyU3PReturnStatus_t
UvcAppFillBuffers (
        CyBool_t updateData)
{
    CyU3PDmaBuffer_t    dmaInfo;
    CyU3PReturnStatus_t status;
    uint32_t            i;

    for (i = 0; i < UVC_STREAM_BUF_COUNT; i++)
    {
        status = CyU3PDmaChannelGetBuffer (&UvcDmaHandle, &dmaInfo, CYU3P_WAIT_FOREVER);
        if (status != CY_U3P_SUCCESS)
        {
            CyU3PDebugPrint (4, "DMA Get Buffer failed, Code=%d\r\n", status);
            return status;
        }

        /* Fill the buffer, if required. This is done only the very first time the buffers are used. We are
           filling different data into each buffer and using a buffer count that does not divide the number of
           frames per buffer. This will create a changing video effect when viewing the stream. */
        if (updateData)
        {
            CyU3PMemSet (dmaInfo.buffer + UVC_HEADER_SIZE, (uint8_t)(0xFF - i * 0x11), UvcDataPerBuffer);
        }

        /* Add the UVC header and commit the buffer. */
        UvcAddHeader (dmaInfo.buffer);
        status = CyU3PDmaChannelCommitBuffer (&UvcDmaHandle, UvcDataPerBuffer + UVC_HEADER_SIZE, 0);
        if (status != CY_U3P_SUCCESS)
        {
            CyU3PDebugPrint (4, "DMA Commit Buffer failed, Code=%d\r\n", status);
            return status;
        }
    }

    return status;
}

/* This function is called to start the actual video streaming from the firmware.
 * This gets called when the SET_INTERFACE request (in case of UVC over Isochronous Endpoint) or the
 * SET_CUR:COMMIT_CONTROL request (in case of UVC over Bulk Endpoint) is received by the firmware.
 */
static CyU3PReturnStatus_t
UvcApplnStart (
        void)
{
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

#ifdef RESET_TIMER_ENABLE
    CyU3PTimerStop (&UvcTimer);
#endif

    UvcFrameBufferCount = 0;
    UvcDataInFlight     = CyFalse;

    /* Set the data size for each DMA buffer depending on USB connection type. */
    if (CyU3PUsbGetSpeed () == CY_U3P_HIGH_SPEED)
        UvcDataPerBuffer = UVC_VALID_DATA_SIZE_HS;
    else
        UvcDataPerBuffer = UVC_VALID_DATA_SIZE_FS;

    /* Place the EP in NAK mode before cleaning up the pipe. */
    CyU3PUsbSetEpNak (EP_VIDEO_STREAM, CyTrue);
    CyU3PBusyWait (USB_MICROFRAME_DURATION);

    /* Reset USB EP and DMA */
    CyU3PUsbFlushEp (EP_VIDEO_STREAM);
    status = CyU3PDmaChannelReset (&UvcDmaHandle);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "UVC Dma Channel reset failed, Code=%d\r\n", status);
        goto StartFunctionExit;
    }

    status = CyU3PDmaChannelSetXfer (&UvcDmaHandle, 0);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "UVC Dma Channel start failed, Code=%d\r\n", status);
        goto StartFunctionExit;
    }

    status = UvcAppFillBuffers (UvcAppUpdateData);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "UVC Buffer Fill failed, Code=%d\r\n", status);
        goto StartFunctionExit;
    }

    /* The data pattern needs to be filled into the buffers only once. */
    UvcAppUpdateData = CyFalse;

    /* Streaming is now active. */
    UvcStreamActive  = CyTrue;

#ifdef RESET_TIMER_ENABLE
    /* As data has been committed into the DMA buffers, we can start the frame timer running at this point. */
    CyU3PTimerModify (&UvcTimer, TIMER_PERIOD, 0);
    CyU3PTimerStart (&UvcTimer);
#endif

#ifdef USE_ISOCHRONOUS_ENDPOINT
    /* Update the MULT setting for the endpoint based on data availability. On a full-speed link, this will get
     * set to 1 and will not be modified thereafter. For a Hi-Speed link, this is continuously updated by the
     * DMA callback. */
    UvcAppSetMultByEpm (EP_VIDEO_STREAM & 0x0F);
#endif

StartFunctionExit:

    /* Free the endpoint from NAK state to start data transfer. */
    CyU3PUsbSetEpNak (EP_VIDEO_STREAM, CyFalse);
    CyU3PBusyWait (USB_MICROFRAME_DURATION);

    return status;
}

/* This function stops the video streaming and reverts the application state machine
 * to an idle state. */
static void
UvcApplnStop (
        void)
{
#ifdef RESET_TIMER_ENABLE
    CyU3PTimerStop (&UvcTimer);
#endif

    /* Mark streaming as stopped. */
    UvcStreamActive     = CyFalse;
    UvcDataInFlight     = CyFalse;
    UvcFrameBufferCount = 0;
    UvcDataPerBuffer    = (CyU3PUsbGetSpeed () == CY_U3P_HIGH_SPEED) ? UVC_VALID_DATA_SIZE_HS : UVC_VALID_DATA_SIZE_FS;

    /* Clear up the DMA channel, keeping the endpoint NAKed (disabled) for the duration. */
    CyU3PUsbSetEpNak (EP_VIDEO_STREAM, CyTrue);
    CyU3PBusyWait (USB_MICROFRAME_DURATION);

    /* Reset the DMA channel and flush the endpoint memory block. */
    CyU3PDmaChannelReset (&UvcDmaHandle);
    CyU3PUsbFlushEp (EP_VIDEO_STREAM);

    CyU3PUsbSetEpNak (EP_VIDEO_STREAM, CyFalse);
    CyU3PBusyWait (USB_MICROFRAME_DURATION);

    /* Clear the stall condition and sequence numbers if a CLEAR_FEATURE(EP_HALT) request has been received. */
    if (UvcClearEpHalt)
    {
        CyU3PUsbStall (EP_VIDEO_STREAM, CyFalse, CyTrue);
        UvcClearEpHalt = CyFalse;
    }
}

/* DMA callback function which is called whenever one data buffer has been emptied by the USB host.
 * We use this to prepare the next data buffer for sending to the USB host.
 */
void
UvcAppDmaCallback (
        CyU3PDmaChannel   *chHandle,
        CyU3PDmaCbType_t   type,
        CyU3PDmaCBInput_t *input)
{
    CyU3PDmaBuffer_t    dmaInfo;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    if (type == CY_U3P_DMA_CB_CONS_EVENT)
    {
        /* Set flag indicating that host has started to fetch video data. */
        UvcDataInFlight = CyTrue;

        /* Get the next buffer pointer. There is no need to wait because at least one free buffer is guaranteed
         * by the call sequence.
         */
        status = CyU3PDmaChannelGetBuffer (chHandle, &dmaInfo, CYU3P_NO_WAIT);
        if (status != CY_U3P_SUCCESS)
        {
            /* We deal with any error by triggering a streaming re-start. */
            CyU3PEventSet (&UvcAppEvent, DMA_RESET_EVENT, CYU3P_EVENT_OR);
            return;
        }

        /* Update the UVC header in the buffer. There is no need to touch the actual video data as it is fixed. */
        UvcAddHeader (dmaInfo.buffer);

        status = CyU3PDmaChannelCommitBuffer (chHandle, UvcDataPerBuffer + UVC_HEADER_SIZE, 0);
        if (status != CY_U3P_SUCCESS)
        {
            /* We deal with any error by triggering a streaming re-start. */
            CyU3PEventSet (&UvcAppEvent, DMA_RESET_EVENT, CYU3P_EVENT_OR);
            return;
        }

#ifdef RESET_TIMER_ENABLE
        if (UvcFrameBufferCount == 0)
        {
            /* If we are done with one video frame, restart the frame timer. */
            CyU3PTimerStop (&UvcTimer);
            CyU3PTimerModify (&UvcTimer, TIMER_PERIOD, 0);
            CyU3PTimerStart (&UvcTimer);
        }
#endif

#ifdef USE_ISOCHRONOUS_ENDPOINT
        if (CyU3PUsbGetSpeed () == CY_U3P_HIGH_SPEED)
        {
            /* Update the MULT setting for the endpoint based on data availability. As only one buffer is consumed
             * per micro-frame, we will have plenty of time to set this up. */
            UvcAppSetMultByEpm (EP_VIDEO_STREAM & 0x0F);
        }
#endif
    }
}


/* USB Event Callback function: Initiates application specific actions corresponding to USB link events. */
static void
UvcAppUsbEventCB (
        CyU3PUsbEventType_t evtype,
        uint16_t            evdata)
{
    switch (evtype)
    {
#ifdef USE_ISOCHRONOUS_ENDPOINT
        /* In case of UVC over Isochronous endpoints, the SET_INTERFACE request is used to start/stop
         * video streaming. */
        case CY_U3P_USB_EVENT_SETINTF:
            {
                uint8_t interface = 0, altSetting = 0;

                /* If setting 1 on interface 1 is selected, start streaming.
                 * If setting 0 on interface 1 is selected, stop streaming.
                 */
                interface  = CY_U3P_GET_MSB (evdata);
                altSetting = CY_U3P_GET_LSB (evdata);

                if (interface == UVC_STREAM_INTERFACE)
                {
                    /* If the streaming was already running, stop it and then restart. */
                    if (UvcStreamActive)
                    {
                        UvcApplnStop ();
                    }

                    if (altSetting != 0)
                    {
                        UvcApplnStart ();
                    }
                }
            }
            break;
#endif

        case CY_U3P_USB_EVENT_SUSPEND:
            {
                /* Notify the application to put FX2G2 device into low power suspend mode. */
                CyU3PEventSet (&UvcAppEvent, USB_SUSP_EVENT_FLAG, CYU3P_EVENT_OR);
            }
            /* Intentional fall-through. */

            /* We handle all of these events by stopping video streaming. */
        case CY_U3P_USB_EVENT_RESET:
        case CY_U3P_USB_EVENT_DISCONNECT:
        case CY_U3P_USB_EVENT_CONNECT:
        case CY_U3P_USB_EVENT_SETCONF:
            {
                /* Stop streaming operation at this stage. */
                if (UvcStreamActive)
                {
                    UvcApplnStop ();
                }
            }
            break;

        default:
            break;
    }
}

/* This request handles the UVC SET_CUR request. In case of UVC over Bulk, the decision to start video
 * streaming is taken when a SET_CUR request for the Commit Control is received.
 */
static void
UvcHandleSetCurRequest (
        uint16_t wValue)
{
    CyU3PReturnStatus_t status;
    uint16_t readCount = 0;

    /* Receive the request data sent by the host. */
    status = CyU3PUsbGetEP0Data (UVC_MAX_PROBE_SETTING_ALIGNED, glCommitCtrl, &readCount);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "GetEp0Data failed, Code=%d\r\n", status);
        return;
    }

    /* Make sure we did not receive more data than expected. */
    if (readCount > (uint16_t)UVC_MAX_PROBE_SETTING)
    {
        CyU3PDebugPrint (4, "SET_CUR request: Invalid length=%d bytes\r\n", readCount);
        return;
    }

    /* Set Probe Control */
    if (wValue == UVC_VS_PROBE_CONTROL)
    {
        UvcFrameIndex = glCommitCtrl[3];
        if (UvcFrameIndex != 1)
        {
            CyU3PDebugPrint (4, "SET_CUR request: Invalid frame index %d selected\r\n", UvcFrameIndex);
        }
    }
    else
    {
        /* Set Commit Control and Start Streaming*/
        if (wValue == UVC_VS_COMMIT_CONTROL)
        {
            /* We support only a single frame resolution and rate at each USB connection speed. Just use this
             * request to start video streaming. Make sure ongoing streaming is stopped before it is restarted.
             */
            if (UvcStreamActive)
            {
                UvcApplnStop ();
            }

            UvcApplnStart ();
        }
    }
}

/* Callback to handle the USB Setup Requests and UVC Class events */
static CyBool_t
UvcAppUsbSetupCB (
        uint32_t setupdat0,
        uint32_t setupdat1)
{
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;
    uint8_t   bmReqType, bRequest, bType, bTarget;
    uint16_t  wValue, wIndex;
    uint8_t   ep0Buf[2];
    CyBool_t  isHandled = CyFalse;
    uint8_t  *ctrl_src = 0;

    /* Decode the fields from the setup request. */
    bmReqType = (setupdat0 & CY_U3P_USB_REQUEST_TYPE_MASK);
    bType     = (bmReqType & CY_U3P_USB_TYPE_MASK);
    bTarget   = (bmReqType & CY_U3P_USB_TARGET_MASK);
    bRequest  = ((setupdat0 & CY_U3P_USB_REQUEST_MASK) >> CY_U3P_USB_REQUEST_POS);
    wValue    = ((setupdat0 & CY_U3P_USB_VALUE_MASK)   >> CY_U3P_USB_VALUE_POS);
    wIndex    = ((setupdat1 & CY_U3P_USB_INDEX_MASK)   >> CY_U3P_USB_INDEX_POS);

    /* ClearFeature(EP_HALT) received on the Streaming Endpoint. Stop Streaming */
    if ((bTarget == CY_U3P_USB_TARGET_ENDPT) && (bRequest == CY_U3P_USB_SC_CLEAR_FEATURE)
            && (wIndex == EP_VIDEO_STREAM) && (wValue == CY_U3P_USBX_FS_EP_HALT))
    {
        if ((UvcStreamActive) && (UvcDataInFlight))
        {
            /* Call the function that stops the video stream. Set a flag to indicate that this is being
             * called as a result of a CLEAR_FEATURE(EP_HALT) request. */
            UvcClearEpHalt = CyTrue;
            UvcApplnStop ();
        }

        /* Return CyFalse to indicate that the driver should ACK the request. */
        return CyFalse;
    }

    /* Check for UVC Class Requests */
    if (bType == CY_U3P_USB_CLASS_RQT)
    {
        /* Requests to the Video Streaming Interface (IF 1) */
        if ((wIndex & 0x00FF) == UVC_STREAM_INTERFACE)
        {
            switch (bRequest)
            {
                case USB_UVC_GET_INFO_REQ:
                    ep0Buf[0] = 3;
                    CyU3PUsbSendEP0Data (1, (uint8_t *)ep0Buf);
                    isHandled = CyTrue;
                    break;

                case USB_UVC_GET_LEN_REQ:
                    ep0Buf[0] = UVC_MAX_PROBE_SETTING;
                    CyU3PUsbSendEP0Data (1, (uint8_t *)ep0Buf);
                    isHandled = CyTrue;
                    break;

                    /* As we only have one valid stream setting, we can return the same value for MIN, MAX, DEFAULT
                     * and CURRENT settings. */
                case USB_UVC_GET_CUR_REQ:
                case USB_UVC_GET_MIN_REQ:
                case USB_UVC_GET_MAX_REQ:
                case USB_UVC_GET_DEF_REQ:

                    /* Host requests for probe data of 34 bytes (UVC 1.1) or 26 Bytes (UVC1.0). Send it over EP0.
                     * Only UVC Frame Index 1 is valid. The actual settings returned depends on USB connection speed. */
                    ctrl_src = 0;
                    if (UvcFrameIndex == 1)
                    {
                        if (CyU3PUsbGetSpeed () == CY_U3P_HIGH_SPEED)
                            ctrl_src = (uint8_t *)glVga30ProbeCtrl;
                        else
                            ctrl_src = (uint8_t *)glQvga5ProbeCtrl;
                    }

                    if (ctrl_src != 0)
                    {
                        CyU3PMemCopy (glProbeCtrl, ctrl_src, UVC_MAX_PROBE_SETTING);

                        status = CyU3PUsbSendEP0Data (UVC_MAX_PROBE_SETTING, glProbeCtrl);
                        if (status != CY_U3P_SUCCESS)
                        {
                            CyU3PDebugPrint (4, "SendEp0Data failed, Code=%d\r\n", status);
                        }
                    }
                    else
                    {
                        CyU3PUsbStall (0, CyTrue, CyFalse);
                    }

                    isHandled = CyTrue;
                    break;

                case USB_UVC_SET_CUR_REQ:
                    UvcHandleSetCurRequest (wValue);
                    isHandled = CyTrue;
                    break;

                default:
                    isHandled = CyFalse;
                    break;
            }
        }

        /* Request addressed to the Video Control Interface */
        if ((wIndex & 0x00FF) == UVC_CONTROL_INTERFACE)
        {
            /* Respond to VC_REQUEST_ERROR_CODE_CONTROL and stall every other request as this example does
               not support any of the Video Control features */
            if ((wValue == UVC_VC_REQUEST_ERROR_CODE_CONTROL) && (wIndex == 0x00))
            {
                ep0Buf[0] = UVC_ERROR_INVALID_CONTROL;
                status = CyU3PUsbSendEP0Data (1, (uint8_t *)ep0Buf);
                if (status != CY_U3P_SUCCESS)
                {
                    CyU3PDebugPrint (4, "SendEp0Data failed, Code=%d\r\n", status);
                }

                isHandled = CyTrue;
            }
        }
    }

    return isHandled;
}

/* This function initialises the USB and DMA modules on the FX2G2 device as required
 * for this UVC application. */
static void
UvcApplnInit (
        void)
{
    CyU3PDmaChannelConfig_t dmaCfg;
    CyU3PEpConfig_t         epCfg;
    CyU3PReturnStatus_t     status = CY_U3P_SUCCESS;

    /* Start the USB functionality */
    status = CyU3PUsbStart ();
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "UsbStart failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }

    /* Register a callback for handling USB control (EP0) requests. We allow the FX3/FX2G2 driver to handle
     * standard requests, and only want to handle class specific requests here. */
    CyU3PUsbRegisterSetupCallback (UvcAppUsbSetupCB, CyTrue);

    /* Register a callback to obtain notification of USB events. */
    CyU3PUsbRegisterEventCallback (UvcAppUsbEventCB);

    /* There is no need to register an LPM callback, or to setup USB 3.0 specific descriptors; as this is a
     * USB 2.0 only application targeted at the FX2G2 controller.
     */

    /* Register the USB descriptors with the USB driver module. */

    /* High speed device descriptor. */
    status = CyU3PUsbSetDesc (CY_U3P_USB_SET_HS_DEVICE_DESCR, 0, (uint8_t *)CyFx2g2USB20DeviceDscr);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "SetDesc (HS_DEVICE) failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }

    /* Device qualifier descriptor */
    status = CyU3PUsbSetDesc (CY_U3P_USB_SET_DEVQUAL_DESCR, 0, (uint8_t *)CyFx2g2USBDeviceQualDscr);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "SetDesc (DEV_QUALIFIER) failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }

    /* High speed configuration descriptor */
    status = CyU3PUsbSetDesc (CY_U3P_USB_SET_HS_CONFIG_DESCR, 0, (uint8_t *)CyFx2g2USBHSConfigDscr);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "SetDesc (HS_CONFIG) failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }

    /* Full speed configuration descriptor */
    status = CyU3PUsbSetDesc (CY_U3P_USB_SET_FS_CONFIG_DESCR, 0, (uint8_t *)CyFx2g2USBFSConfigDscr);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "SetDesc (FS_CONFIG) failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }

    /* String descriptor 0 */
    status = CyU3PUsbSetDesc (CY_U3P_USB_SET_STRING_DESCR, 0, (uint8_t *)CyFx2g2USBLangIDString);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "SetDesc (STRING_0) failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }

    /* String descriptor 1 */
    status = CyU3PUsbSetDesc (CY_U3P_USB_SET_STRING_DESCR, 1, (uint8_t *)CyFx2g2USBManufacturerString);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "SetDesc (STRING_1) failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }

    /* String descriptor 2 */
    status = CyU3PUsbSetDesc (CY_U3P_USB_SET_STRING_DESCR, 2, (uint8_t *)CyFx2g2USBProductString);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "SetDesc (STRING_2) failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }
    /* String descriptor 3 */
    status = CyU3PUsbSetDesc (CY_U3P_USB_SET_STRING_DESCR, 3, (uint8_t *)CyFx2g2USBHSConfigString);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "SetDesc (STRING_3) failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }

    /* String descriptor 4 */
    status = CyU3PUsbSetDesc (CY_U3P_USB_SET_STRING_DESCR, 4, (uint8_t *)CyFx2g2USBFSConfigString);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "SetDesc (STRING_4) failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }

    /* Configure and enable the control/status endpoint. */
    epCfg.enable   = 1;
    epCfg.epType   = CY_U3P_USB_EP_INTR;
    epCfg.pcktSize = 64;
    epCfg.isoPkts  = 0;
    epCfg.burstLen = 1;
    status = CyU3PSetEpConfig (EP_CONTROL_STATUS, &epCfg);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "SetEpConfig failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }

    /* Flush the control/status endpoint. */
    CyU3PUsbFlushEp (EP_CONTROL_STATUS);

    /* Configure and enable the video streaming endpoint. Use HS parameters at all times. The driver will
     * automatically adjust this for FS. */
    epCfg.enable   = CyTrue;
    epCfg.streams  = 0;
    epCfg.burstLen = 1;
#ifdef USE_ISOCHRONOUS_ENDPOINT
    epCfg.epType   = CY_U3P_USB_EP_ISO;
    epCfg.isoPkts  = 1;                         /* Set this as 1 by default. We can update this as and when data is
                                                   being committed. */
    epCfg.pcktSize = UVC_ISO_EP_SIZE_HS;
#else
    epCfg.epType   = CY_U3P_USB_EP_BULK;
    epCfg.isoPkts  = 0;
    epCfg.pcktSize = UVC_BULK_EP_SIZE_HS;
#endif

    status = CyU3PSetEpConfig (EP_VIDEO_STREAM, &epCfg);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "SetEpConfig failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }

    /* Flush the endpoint memory */
    CyU3PUsbFlushEp (EP_VIDEO_STREAM);

    /* Create a DMA Manual OUT channel for streaming video data. On a MANUAL OUT channel, the demarcation between
     * header and data is maintained by the firmware. We create the channels without any pre-defined header or
     * footer areas. */
    dmaCfg.size           = UVC_STREAM_BUF_SIZE;
    dmaCfg.count          = UVC_STREAM_BUF_COUNT;
    dmaCfg.prodSckId      = CY_U3P_CPU_SOCKET_PROD;
    dmaCfg.consSckId      = EP_VIDEO_CONS_SOCKET;
    dmaCfg.dmaMode        = CY_U3P_DMA_MODE_BYTE;
    dmaCfg.notification   = CY_U3P_DMA_CB_CONS_EVENT;
    dmaCfg.cb             = UvcAppDmaCallback;
    dmaCfg.prodHeader     = 0;
    dmaCfg.prodFooter     = 0;
    dmaCfg.consHeader     = 0;
    dmaCfg.prodAvailCount = 0;
    status = CyU3PDmaChannelCreate (&UvcDmaHandle, CY_U3P_DMA_TYPE_MANUAL_OUT, &dmaCfg);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "ChannelCreate failed, Code=%d\r\n", status);
    }

#ifdef RESET_TIMER_ENABLE
    /* Create a timer for tracking and aborting stuck video frames. */
    CyU3PTimerCreate (&UvcTimer, UvcAppProgressTimer, 0x00, TIMER_PERIOD, 0, CYU3P_NO_ACTIVATE);
#endif

    /* Enable USB connection in USB 2.0 mode. USB 3.0 is not supported by FX2G2. */
    status = CyU3PConnectState (CyTrue, CyFalse);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (4, "ConnectState failed, Code=%d\r\n", status);
        AppErrorHandler (status);
    }
}

/* This function initializes the debug module for the UVC application */
static void
UvcAppDebugInit (
        void)
{
    CyU3PUartConfig_t   uartConfig;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    /* Errors in starting up the UART and enabling it for debug are not fatal errors.
     * Also, we cannot use DebugPrint until the debug module has been successfully initialized.
     */

    /* Initialize the UART for printing debug messages */
    status = CyU3PUartInit ();
    if (status != CY_U3P_SUCCESS)
    {
        return;
    }

    /* Set UART Configuration */
    uartConfig.baudRate = CY_U3P_UART_BAUDRATE_115200;
    uartConfig.stopBit  = CY_U3P_UART_ONE_STOP_BIT;
    uartConfig.parity   = CY_U3P_UART_NO_PARITY;
    uartConfig.txEnable = CyTrue;
    uartConfig.rxEnable = CyFalse;
    uartConfig.flowCtrl = CyFalse;
    uartConfig.isDma    = CyTrue;

    /* Set the UART configuration */
    status = CyU3PUartSetConfig (&uartConfig, NULL);
    if (status != CY_U3P_SUCCESS)
    {
        return;
    }

    /* Set the UART transfer */
    status = CyU3PUartTxSetBlockXfer (0xFFFFFFFF);
    if (status != CY_U3P_SUCCESS)
    {
        return;
    }

    /* Initialize the debug application */
    status = CyU3PDebugInit (CY_U3P_LPP_SOCKET_UART_CONS, 8);
    if (status != CY_U3P_SUCCESS)
    {
        return;
    }

    CyU3PDebugPreamble (CyFalse);
}

/* Entry function for the UVC application thread. */
static void
UvcAppThread_Entry (
        uint32_t input)
{
    uint16_t wakeReason;
    uint32_t eventFlag;
    CyU3PReturnStatus_t status;

    /* Initialize the Debug Module */
    UvcAppDebugInit ();

    /* Perform the rest of the required initialization. */
    UvcApplnInit ();

    /* Now, we keep waiting for the SUSPEND or FRAME ABORT events and handle them as and when they arise. */
    for (;;)
    {
        eventFlag = 0;
        CyU3PEventGet (&UvcAppEvent, USB_SUSP_EVENT_FLAG | DMA_RESET_EVENT, CYU3P_EVENT_OR_CLEAR,
                &eventFlag, CYU3P_WAIT_FOREVER);

        /* Frame timed out. Abort and start streaming again. */
        if (eventFlag & DMA_RESET_EVENT)
        {
            if (UvcStreamActive)
            {
                UvcApplnStop ();
            }

            UvcApplnStart ();
        }

        /* Handle Suspend Event*/
        if (eventFlag & USB_SUSP_EVENT_FLAG)
        {
            /* Place FX2G2 in Low Power Suspend mode, with USB bus activity as the wakeup source. */
            status = CyU3PSysEnterSuspendMode (CY_U3P_SYS_USB_BUS_ACTVTY_WAKEUP_SRC, 0, &wakeReason);
            CyU3PDebugPrint (4, "EnterSuspendMode returned %d, wakeReason=%d\r\n", status, wakeReason);
        }
    }
}

/* Application define function which creates the threads. This needs to be a public function, as it is called
 * from the FX3/FX2G2 firmware framework after the RTOS is started up.
 */
void
CyFxApplicationDefine (
        void)
{
    uint32_t  retThrdCreate = CY_U3P_SUCCESS;
    void     *ptr = NULL;

    /* Create application event group */
    retThrdCreate = CyU3PEventCreate (&UvcAppEvent);
    if (retThrdCreate != 0)
        goto StartupError;

    /* Allocate the memory for the thread and create the thread */
    ptr = CyU3PMemAlloc (UVC_APP_THREAD_STACK);
    if (ptr == 0)
        goto StartupError;

    retThrdCreate = CyU3PThreadCreate (
            &UvcAppThread,                              /* UVC Thread structure */
            "30:UVC_app_thread",                        /* Thread Id and name */
            UvcAppThread_Entry,                         /* UVC Application Thread Entry function */
            0,                                          /* No input parameter to thread */
            ptr,                                        /* Pointer to the allocated thread stack */
            UVC_APP_THREAD_STACK,                       /* UVC Application Thread stack size */
            UVC_APP_THREAD_PRIORITY,                    /* UVC Application Thread priority */
            UVC_APP_THREAD_PRIORITY,                    /* Pre-emption threshold */
            CYU3P_NO_TIME_SLICE,                        /* No time slice for the application thread */
            CYU3P_AUTO_START                            /* Start the Thread immediately */
            );

    /* Check the return code */
    if (retThrdCreate != 0)
        goto StartupError;

    return;

StartupError:
    {
        /* Failed to create threads and objects required for the application. This is a fatal error and we cannot
         * continue.
         */

        /* Add custom recovery or debug actions here */

        while (1);
    }
}

/*
 * Main function
 */
int
main (
        void)
{
    CyU3PIoMatrixConfig_t io_cfg;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    /* Initialize the device */
    status = CyU3PDeviceInit (NULL);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* Initialize the caches. Enable instruction cache and keep data cache disabled.
     * The data cache is useful only when there is a large amount of CPU based memory
     * accesses. When used in simple cases, it can decrease performance due to large
     * number of cache flushes and cleans and also it adds to the complexity of the
     * code. */
    status = CyU3PDeviceCacheControl (CyTrue, CyFalse, CyFalse);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* Configure the IO matrix for the device. Only UART is used, and no GPIOs are enabled. */
    CyU3PMemSet ((uint8_t *)&io_cfg, 0, sizeof (io_cfg));
    io_cfg.useUart = CyTrue;
    io_cfg.lppMode = CY_U3P_IO_MATRIX_LPP_DEFAULT;

    status = CyU3PDeviceConfigureIOMatrix (&io_cfg);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* This is a non returnable call for initializing the RTOS kernel */
    CyU3PKernelEntry ();

    /* Dummy return to make the compiler happy */
    return 0;

handle_fatal_error:
    /* Cannot recover from this error. */
    while (1);
}

/* [] */

