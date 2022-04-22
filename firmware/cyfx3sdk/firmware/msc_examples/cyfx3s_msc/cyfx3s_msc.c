/*
## Cypress FX3S Example Application Source File (cyfx3s_msc.c)
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

/* This file illustrates the Mass Storage Class Driver example.
   The example makes use of the the internal device memory to provide the storage space.
   A minimum of 32KB is required for the device to be formatted as FAT on the windows host */

#include <cyu3system.h>
#include <cyu3os.h>
#include <cyu3dma.h>
#include <cyu3socket.h>
#include <cyu3error.h>
#include <cyu3usb.h>
#include <cyu3usbconst.h>
#include <cyu3uart.h>
#include <cyu3sib.h>
#include <cyu3gpio.h>
#include <cyu3utils.h>

#include "cyfx3s_msc.h"

/* This example demonstrates the implementation of a USB Mass Storage Class (Bulk Only Transport) function
 * using the FX3S APIs.
 * The implementation enumerates CY_FX_SIB_PARTITIONS on each of the storage ports as separate mass storage
 * drives.
 */

#ifdef LOG_ENABLE
#define AppDebugPrint CyU3PDebugPrint
#else
#define AppDebugPrint(...)
#endif

#define CY_FX_LUN_COUNT  (CY_FX_SIB_PARTITIONS * CY_FX_SIB_PORTS)

static CyBool_t glLunState[CY_FX_LUN_COUNT];                   /* Whether the LUN has valid storage. */
static uint8_t  glLunUnit[CY_FX_LUN_COUNT];                    /* The unit number corresponding to this LUN. */
static CyBool_t glLunWriteable[CY_FX_LUN_COUNT];               /* Whether the LUN is writeable. */
static uint16_t glLunBlkSize[CY_FX_LUN_COUNT];                 /* Sector size for each of the LUNs. */
static uint32_t glLunNumBlks[CY_FX_LUN_COUNT];                 /* Capacity of the LUN in sectors. */
static CyBool_t glLunStopped[CY_FX_LUN_COUNT];                 /* Whether each LUN has been stopped by the user. */

static uint8_t  glSensePtr[CY_FX_LUN_COUNT] = {CY_FX_MSC_SENSE_DEVICE_RESET};

static CyBool_t         glDevConfigured = CyFalse;              /* Whether the device has been configured. */
static CyBool_t         glInPhaseError  = CyFalse;              /* Whether a phase error condition has been detected. */
static CyFxMscFuncState glMscState      = CY_FX_MSC_STATE_INACTIVE;

static CyU3PSibDevInfo_t   glDevInfo[CY_FX_SIB_PORTS];          /* Structure to hold device info */
static CyU3PDmaChannel     glChHandleMscOut;                    /* DMA channel for OUT endpoint. */
static CyU3PDmaChannel     glChHandleMscIn;                     /* DMA channel for IN endpoint. */

static CyU3PThread mscAppThread;	                        /* MSC application thread structure */
static CyU3PEvent  glMscAppEvent;                               /* MSC application Event group */
static CyU3PTimer  glMscAppTimer;                               /* Application idle timer. */

uint8_t *glMscCbwBuffer  = 0;                                   /* Scratch buffer used for CBW. */
uint8_t *glMscCswBuffer  = 0;                                   /* Scratch buffer used for CSW. */
uint8_t *glMscDataBuffer = 0;                                   /* Scratch buffer used for query commands. */

static          uint8_t  glCmdDirection  = 0;                   /* SCSI Command Direction */
static          uint8_t  glMscCmdStatus  = 0;                   /* MSC command status. */
static          uint8_t  glMscCmdLun     = 0;                   /* Current MSC LUN. */
static          uint32_t glMscResidue    = 0;                   /* Residue length for CSW */
static          CyBool_t glMscDriverBusy = CyFalse;             /* Status variable used for Link Power Management. */
static volatile uint32_t glMscIdleTime   = 0;                   /* MSC application idle time in seconds. */

/* Request Sense Lookup Table */
static const uint8_t glReqSenseCode[][3] =
{
    /* SK,  ASC,  ASCQ */
    {0x00, 0x00, 0x00},    /* senseOk                     0    */
    {0x0b, 0x08, 0x03},    /* senseCRCError               1    */
    {0x05, 0x24, 0x00},    /* senseInvalidFieldInCDB      2    */
    {0x02, 0x3a, 0x00},    /* senseNoMedia                3    */
    {0x03, 0x03, 0x00},    /* senseWriteFault             4    */
    {0x03, 0x11, 0x00},    /* senseReadError              5    */
    {0x03, 0x12, 0x00},    /* senseAddrNotFound           6    */
    {0x05, 0x20, 0x00},    /* senseInvalidOpcode          7    */
    {0x05, 0x21, 0x00},    /* senseInvalidLBA             8    */
    {0x05, 0x26, 0x00},    /* senseInvalidParameter       9    */
    {0x05, 0x53, 0x02},    /* senseCantEject              0xa  */
    {0x06, 0x28, 0x00},    /* senseMediaChanged           0xb  */
    {0x06, 0x29, 0x00},    /* senseDeviceReset            0xc  */
    {0x07, 0x27, 0x00},    /* senseWriteProtected         0xd  */
    {0x02, 0x04, 0x02},    /* senseInitRequired           0xe  */
    {0x05, 0x39, 0x00}     /* senseSavePageNotSupported   0xf  */
};

/* Standard Inquiry Data */
static const uint8_t CyFxMscScsiInquiryData[CY_FX_SCSI_INQUIRY_DATALEN] __attribute__ ((aligned (32))) = {
    0x00,       /* PQ and PDT */
    0x80,       /* Removable device. */
    0x06,       /* Version */
    0x02,       /* Response data format */
    0x5B,       /* Addnl Length */
    0x00,
    0x00,
    0x00,

    'C',        /* Vendor Id */
    'y',
    'p',
    'r',
    'e',
    's',
    's',
    0x00,

    'F',        /* Product Id */
    'X',
    '3',
    'S',
    0x20,
    'M',
    'S',
    'C',
    0x20,
    'D',
    'E',
    'M',
    'O',
    0x20,
    0x00,
    0x00,

    '0',        /* Revision */
    '0',
    '0',
    '1',

    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0, /* 20 bytes of vendor specific info: not used. */

    0, 0,       /* Reserved fields for SCSI over USB. */

    0x00, 0x80, /* SAM-4 spec compliant. */
    0x17, 0x30, /* BOT spec compliant. */
    0x04, 0x60, /* SPC-4 spec compliant. */
    0x04, 0xC0, /* SBC-3 spec compliant. */
    0x00, 0x00, /* No more specs. */
    0x00, 0x00, /* No more specs. */
    0x00, 0x00, /* No more specs. */
    0x00, 0x00, /* No more specs. */
    0x00, 0x00, /* Reserved. */

    0, 0, 0, 0, /* Reserved. */
    0, 0, 0, 0, /* Reserved. */
    0, 0, 0, 0, /* Reserved. */
    0, 0, 0, 0, /* Reserved. */
    0, 0, 0, 0  /* Reserved. */
};

static const uint8_t CyFxMscSupportedPageList[] __attribute__ ((aligned (32))) = {
    0x00,                                       /* PDT = 0. */
    CY_FX_SCSI_INQCMD_PCODE_LIST,               /* Page code = 0. */
    0x00,                                       /* Reserved. */
    0x04,                                       /* Max Index - 3. */
    CY_FX_SCSI_INQCMD_PCODE_LIST,
    CY_FX_SCSI_INQCMD_PCODE_SERNUM,
    CY_FX_SCSI_INQCMD_PCODE_DEVIDENT,
    CY_FX_SCSI_INQCMD_PCODE_BLKLIMITS
};

static const uint8_t CyFxMscScsiSerialNumber[] __attribute__ ((aligned (32))) = {
    0x00,                                       /* PDT = 0. */
    CY_FX_SCSI_INQCMD_PCODE_SERNUM,             /* Page code = 0x80. */
    0x00,                                       /* Reserved. */
    0x0C,                                       /* Max Index (15) - 3. */
    0x30, 0x31, 0x32, 0x33,                     /* "0123" */
    0x34, 0x35, 0x36, 0x37,                     /* "4567" */
    0x38, 0x39, 0x30, 0x31                      /* "8901" */
};

static uint8_t CyFxMscScsiDevIdent[] __attribute__ ((aligned (32))) = {
    0x00,                                       /* PDT = 0. */
    CY_FX_SCSI_INQCMD_PCODE_DEVIDENT,           /* Page code = 0x83. */
    0x00,                                       /* MSB (PAGE LENGTH) = 0x00. */
    0x0C,                                       /* LSB (PAGE LENGTH) = 15 - 3 = 12. */

    0x01, 0x02, 0x00, 0x08,
    0x00, 0xA0, 0x50, 0x46, 0x58, 0x33, 0x53, 0x20
};

static const uint8_t CyFxMscScsiBlkLimits[] __attribute__ ((aligned (32))) = {
    0x00,                                       /* PDT = 0. */
    CY_FX_SCSI_INQCMD_PCODE_BLKLIMITS,          /* Page code = 0xB0. */
    0x00,                                       /* Reserved. */
    0x3C,                                       /* Max Index (63) - 3. */
    0x00, 0x00, 0x08, 0x00,                     /* Optimal size granularity = 1 MB. */
    0x00, 0x00, 0x40, 0x00,                     /* Maximum size = 8 MB. */
    0x00, 0x00, 0x40, 0x00,                     /* Optimum size = 8 MB. */
    0x00, 0x00, 0x40, 0x00,                     /* Max. Prefetch size = 8 MB. */
    0x00, 0x00, 0x00, 0x00,                     /* No unmap support. */
    0x00, 0x00, 0x00, 0x00,                     /* No unmap support. */
    0x00, 0x00, 0x00, 0x00,                     /* No unmap support. */
    0x00, 0x00, 0x00, 0x00,                     /* No unmap support. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00                      /* Reserved. */
};

static uint8_t CyFxMscCachingDefault[] __attribute__ ((aligned (32))) = {
    0x08,                                       /* PS=0, SPF=0, PC=08. */
    0x12,                                       /* Page length. */
    00, 00,                                     /* No caching supported. */
    00, 00, 00, 00,
    00, 00, 00, 00,
    00, 00, 00, 00,
    00, 00, 00, 00
};

static uint8_t CyFxMscCachingCurrent[] __attribute__ ((aligned (32))) = {
    0x08,                                       /* PS=0, SPF=0, PC=08. */
    0x12,                                       /* Page length. */
    00, 00,                                     /* No caching supported. */
    00, 00, 00, 00,
    00, 00, 00, 00,
    00, 00, 00, 00,
    00, 00, 00, 00
};

static const uint8_t CyFxMscCachingChangeable[] __attribute__ ((aligned (32))) = {
    0x08,                                       /* PS=0, SPF=0, PC=08. */
    0x12,                                       /* Page length. */
    0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xE1, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00
};

/* Data used by Cypress proprietary mass storage driver to identify the device. */
uint8_t A0VendBuf[8] __attribute__ ((aligned (32))) =
{
    0x02, 0x0E, 0, 0, 0, 0, 0, 0
};

/*
 * Main function
 */
int
main (void)
{
    CyU3PReturnStatus_t status;
    CyU3PIoMatrixConfig_t io_cfg;

    /* Initialize the device */
    status = CyU3PDeviceInit (0);
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

    /* Configure the IO matrix for the device.
     * S0 port is enabled in 8 bit mode.
     * S1 port is enabled in 4 bit mode.
     * UART is enabled on remaining pins of the S1 port.
     */
    io_cfg.isDQ32Bit        = CyFalse;
    io_cfg.s0Mode           = CY_U3P_SPORT_8BIT;

#ifdef LOG_ENABLE
    /* If the UART is used for logging, we can only configure the S1 port as 4 bits wide. */
    io_cfg.s1Mode           = CY_U3P_SPORT_4BIT;
    io_cfg.lppMode          = CY_U3P_IO_MATRIX_LPP_UART_ONLY;
    io_cfg.useUart          = CyTrue;
#else
    io_cfg.s1Mode           = CY_U3P_SPORT_8BIT;
    io_cfg.lppMode          = CY_U3P_IO_MATRIX_LPP_NONE;
    io_cfg.useUart          = CyFalse;
#endif

    io_cfg.gpioSimpleEn[0]  = 0;
    io_cfg.gpioSimpleEn[1]  = 0x02102800;                       /* IOs 43, 45, 52 and 57 are chosen as GPIO. */
    io_cfg.gpioComplexEn[0] = 0;
    io_cfg.gpioComplexEn[1] = 0;
    io_cfg.useI2C           = CyFalse;
    io_cfg.useI2S           = CyFalse;
    io_cfg.useSpi           = CyFalse;

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

/* MSC application error handler */
void
CyFxAppErrorHandler (
        CyU3PReturnStatus_t apiRetStatus)
{

    /* Application failed with the error code apiRetStatus */

    /* Add custom debug or recovery actions here */

    /* Loop Indefinitely */
    for (;;)
    {
        /* Thread Sleep : 100 ms */
        CyU3PThreadSleep (100);
    }
}

#ifdef LOG_ENABLE
/* This function initializes the Debug Module for the MSC Application */
void
CyFxMscApplnDebugInit()
{

    CyU3PUartConfig_t uartConfig;
    CyU3PReturnStatus_t apiRetStatus;

    /* Initialize the UART for printing debug messages */
    apiRetStatus = CyU3PUartInit ();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error Handling */
        CyFxAppErrorHandler(apiRetStatus);
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
    apiRetStatus = CyU3PUartSetConfig (&uartConfig, NULL);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error Handling */
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Set the UART transfer */
    apiRetStatus = CyU3PUartTxSetBlockXfer (0xFFFFFFFF);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error Handling */
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Initialize the Debug application */
    apiRetStatus = CyU3PDebugInit (CY_U3P_LPP_SOCKET_UART_CONS, 8);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error Handling */
        CyFxAppErrorHandler(apiRetStatus);
    }

    CyU3PDebugPreamble (CyFalse);
}
#endif

static void
CyFxMscApplnResetDatapath (
        void)
{
    int8_t i = 0;
    CyU3PUsbSetEpNak (CY_FX_MSC_EP_BULK_OUT, CyTrue);
    CyU3PUsbSetEpNak (CY_FX_MSC_EP_BULK_IN, CyTrue);
    CyU3PBusyWait (120);

    CyU3PDmaChannelReset(&glChHandleMscOut);
    CyU3PDmaChannelReset(&glChHandleMscIn);

    CyU3PUsbFlushEp (CY_FX_MSC_EP_BULK_OUT);
    CyU3PUsbStall (CY_FX_MSC_EP_BULK_OUT, CyFalse, CyTrue);

    CyU3PUsbFlushEp (CY_FX_MSC_EP_BULK_IN);
    CyU3PUsbStall (CY_FX_MSC_EP_BULK_IN, CyFalse, CyTrue);

    /* Request Sense Index */
    for (i = 0; i < CY_FX_LUN_COUNT; i++)
        glSensePtr[i] = CY_FX_MSC_SENSE_DEVICE_RESET;

    CyU3PUsbSetEpNak (CY_FX_MSC_EP_BULK_OUT, CyFalse);
    CyU3PUsbSetEpNak (CY_FX_MSC_EP_BULK_IN, CyFalse);

    glInPhaseError  = CyFalse;
    glMscDriverBusy = CyFalse;
}

/* Callback to handle the USB Setup Requests and Mass Storage Class requests */
CyBool_t
CyFxMscApplnUSBSetupCB (
        uint32_t setupdat0, /* SETUP Data 0 */
        uint32_t setupdat1  /* SETUP Data 1 */
    )
{
    CyBool_t isHandled = CyFalse;
    uint8_t  ep0Buf[2];

    uint8_t  bRequest, bReqType;
    uint8_t  bType, bTarget;
    uint16_t wValue, wIndex, wLength;

    /* Decode the fields from the setup request. */
    bReqType = (setupdat0 & CY_U3P_USB_REQUEST_TYPE_MASK);
    bType    = (bReqType & CY_U3P_USB_TYPE_MASK);
    bTarget  = (bReqType & CY_U3P_USB_TARGET_MASK);
    bRequest = ((setupdat0 & CY_U3P_USB_REQUEST_MASK) >> CY_U3P_USB_REQUEST_POS);
    wValue   = ((setupdat0 & CY_U3P_USB_VALUE_MASK)   >> CY_U3P_USB_VALUE_POS);
    wIndex   = ((setupdat1 & CY_U3P_USB_INDEX_MASK)   >> CY_U3P_USB_INDEX_POS);
    wLength  = ((setupdat1 & CY_U3P_USB_LENGTH_MASK)  >> CY_U3P_USB_LENGTH_POS);

    /* Some setup requests have to be handled in a non-standard way for this device. */
    if (bType == CY_U3P_USB_STANDARD_RQT)
    {
        switch (bRequest)
        {
            case CY_U3P_USB_SC_CLEAR_FEATURE:
                /* Handle EP RESET requests based on interface state. */
                if ((bTarget == CY_U3P_USB_TARGET_ENDPT) && (
                            (wIndex == CY_FX_MSC_EP_BULK_IN) || (wIndex == CY_FX_MSC_EP_BULK_OUT)))
                {
                    /* In case of phase error, the stall condition can only be cleared by bus reset or
                       mass storage reset. */
                    if (!glInPhaseError)
                        CyU3PUsbStall (wIndex, CyFalse, CyTrue);
                    else
                    {
                        /* Even though we cannot clear the STALL condition, the sequence number should be cleared. */
                        if (CyU3PUsbGetSpeed () == CY_U3P_SUPER_SPEED)
                            CyU3PUsbSetEpSeqNum (wIndex, 0);
                    }

                    CyU3PUsbAckSetup ();
                    isHandled = CyTrue;
                }
                /* No break */
            case CY_U3P_USB_SC_SET_FEATURE:
                /* Handle SET_FEATURE(FUNCTION_SUSPEND) and CLEAR_FEATURE(FUNCTION_SUSPEND)
                 * requests here. It should be allowed to pass if the device is in configured
                 * state and failed otherwise. */
                if ((bTarget == CY_U3P_USB_TARGET_INTF) && (wValue == 0))
                {
                    if (glDevConfigured)
                        CyU3PUsbAckSetup ();
                    else
                        CyU3PUsbStall (0, CyTrue, CyFalse);

                    isHandled = CyTrue;
                }
                break;

                /* We need to handle all Interface specific requests here. */
            case CY_U3P_USB_SC_GET_STATUS:
                if (bTarget == CY_U3P_USB_TARGET_INTF)
                {
                    /* We support only interface 0. */
                    if (wIndex == 0)
                    {
                        ep0Buf[0] = 0;
                        ep0Buf[1] = 0;
                        CyU3PUsbSendEP0Data (0x02, ep0Buf);
                    }
                    else
                        CyU3PUsbStall (0, CyTrue, CyFalse);

                    isHandled = CyTrue;
                }
                break;

            case CY_U3P_USB_SC_SET_INTERFACE:
                isHandled = CyTrue;
                if ((wValue == 0) && (wIndex == 0))
                    CyU3PUsbAckSetup ();
                else
                    CyU3PUsbStall (0, CyTrue, CyFalse);
                break;

            case CY_U3P_USB_SC_GET_INTERFACE:
                isHandled = CyTrue;
                if (wIndex == 0)
                {
                    ep0Buf[0] = 0;
                    CyU3PUsbSendEP0Data (0x01, ep0Buf);
                }
                else
                    CyU3PUsbStall (0, CyTrue, CyFalse);
                break;

            default:
                break;
        }
    }

    /* MSC class specific request handling. */
    if (bType == CY_FX_MSC_USB_CLASS_REQ)
    {
        if ((bTarget == CY_U3P_USB_TARGET_INTF) &&
                (wIndex == CY_FX_USB_MSC_INTF) && (wValue == 0))
        {
            /* Get MAX LUN Request */
            if (bRequest == CY_FX_MSC_GET_MAX_LUN_REQ)
            {
                if (wLength == 1)
                {
                    isHandled = CyTrue;
                    ep0Buf[0] = CY_FX_LUN_COUNT - 1;
                    CyU3PUsbSendEP0Data (0x01, ep0Buf);
                }
            }

            /* BOT Reset Request */
            if (bRequest == CY_FX_MSC_BOT_RESET_REQ)
            {
                isHandled      = CyTrue;
                glInPhaseError = CyFalse;

                if (wLength == 0)
                {
                    CyU3PUsbAckSetup ();
                    CyFxMscApplnResetDatapath ();

                    /* Inform the thread to start waiting for USB CBWs again. */
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SETCONF_EVENT_FLAG, CYU3P_EVENT_OR);
                }
                else
                {
                    CyU3PUsbStall (0x00, CyTrue, CyFalse);
                }
            }
        }
    }

    /* This request is not needed when using the standard Mass Storage class driver on any OS.
       It is used by the Cypress proprietary Mass Storage Class Driver for device identification.
       */
    if (bType == CY_FX_MSC_USB_VENDOR_REQ)
    {
        if ((bReqType & 0x80) && (bRequest == 0xA0))
        {
            if (wLength < 8)
                CyU3PUsbSendEP0Data ((uint8_t)wLength, A0VendBuf);
            else
                CyU3PUsbSendEP0Data (8, A0VendBuf);
            isHandled = CyTrue;
        }
    } 

    return isHandled;
}

/* Function to initiate sending of data to the USB host. */
CyU3PReturnStatus_t
CyFxMscApplnSendDataToHost (
    uint8_t  *data,
    uint32_t length)
{
    CyU3PDmaBuffer_t    dmaBuf;
    CyU3PReturnStatus_t status;

    /* Prepare the DMA Buffer */
    dmaBuf.buffer = data;
    dmaBuf.status = 0;
    dmaBuf.size   = (length + 15) & 0xFFF0;      /* Round up to a multiple of 16.  */
    dmaBuf.count  = length;

    status = CyU3PDmaChannelSetupSendBuffer (&glChHandleMscIn, &dmaBuf);
    return status;
}

/* Function to initiate sending of data to the USB host. */
CyU3PReturnStatus_t
CyFxMscApplnReceiveUsbData (
    uint8_t  *data,
    uint32_t length)
{
    CyU3PDmaBuffer_t    dmaBuf;
    CyU3PReturnStatus_t status;

    /* Prepare the DMA Buffer */
    dmaBuf.buffer = data;
    dmaBuf.status = 0;
    dmaBuf.size   = (length + 15) & 0xFFF0;      /* Round up to a multiple of 16.  */
    dmaBuf.count  = length;

    status = CyU3PDmaChannelSetupRecvBuffer (&glChHandleMscOut, &dmaBuf);
    return status;
}

/* Common function to check all CBW parameters against expected values. */
CyBool_t
CyFxMscApplnCheckCbwParams (
        uint8_t   lun,                  /* Lun to which the command is addressed. */
        CyBool_t  lunState,             /* Whether the LUN should be active or don't care (for INQUIRY/REQ_SENSE) */
        CyBool_t  isReadExpected,       /* Whether the op-code corresponds to a read (IN) data transfer. */
        CyBool_t  isDataCmd,            /* Whether this is a read or write command. Exact data length match is
                                           expected in data commands. */
        uint32_t  expLength,            /* Default data transfer length for this opcode. */
        uint32_t *actLength_p           /* Return parameter to pass the actual data transfer length. */
        )
{
    /* Verify that:
        1. Direction of data transfer matches the expected value.
        2. That the transfer length matches the expected value for read/write commands.
        3. That a non-zero transfer length is provided for commands that want to return data.
        4. That zero transfer length is provided for commands that have no data.
     */
    if ((isReadExpected != (glCmdDirection == 0x80)) || ((isDataCmd) && (expLength != glMscResidue)) ||
            ((expLength != 0) && (glMscResidue == 0)) || ((expLength == 0) && (glMscResidue != 0)))
    {
        glMscCmdStatus  = 1;
        glSensePtr[lun] = CY_FX_MSC_SENSE_INVALID_PARAMETER;
        CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
        return CyFalse;
    }

    /* Verify that the LUN is ready for commands that report the LUN state or do data transfers. */
    if (lunState)
    {
        if (glSensePtr[lun] != CY_FX_MSC_SENSE_MEDIA_CHANGED)
        {
            if ((!glLunState[lun]) || (glLunStopped[lun]))
            {
                glMscCmdStatus  = 1;
                glSensePtr[lun] = CY_FX_MSC_SENSE_NO_MEDIA;
                CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                return CyFalse;
            }
            else
            {
                glSensePtr[lun] = CY_FX_MSC_SENSE_OK;
            }
        }
        else
        {
            glMscCmdStatus = 1;
            CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
            return CyFalse;
        }
    }

    /* Calculate the size of data to be transferred in the case of non-data commands. */
    if (!isDataCmd)
    {
        if (expLength > glMscResidue)
        {
            *actLength_p = glMscResidue;
            glMscResidue = 0;
        }
        else
        {
            *actLength_p  = expLength;
            glMscResidue -= expLength;
        }
    }

    return CyTrue;
}

static void
HandleModeSelectData (
        void)
{
    CyU3PReturnStatus_t status;
    CyU3PDmaBuffer_t    dmaBuf;

    /* Verify the received data and copy into the current sense parameter structure. */
    status = CyU3PDmaChannelWaitForRecvBuffer (&glChHandleMscOut, &dmaBuf, CYU3P_NO_WAIT);
    if (status == CY_U3P_SUCCESS)
    {
        if ((dmaBuf.count >= (4 + sizeof (CyFxMscCachingDefault))) &&
                (glMscDataBuffer[4] == CY_FX_SCSI_MODESENS_PAGE_CACHE))
        {
            CyU3PMemCopy ((uint8_t *)&CyFxMscCachingCurrent[2], (uint8_t *)&glMscDataBuffer[6],
                    sizeof (CyFxMscCachingDefault) - 2);
            glMscCmdStatus = 0;
        }
        else
        {
            glMscCmdStatus = 1;
            glSensePtr[glMscCmdLun] = CY_FX_MSC_SENSE_INVALID_PARAMETER;
        }
    }
    else
    {
        glMscCmdStatus = 1;
        glSensePtr[glMscCmdLun] = CY_FX_MSC_SENSE_INVALID_PARAMETER;
    }

    /* Signal the thread to send status data out. */
    glMscResidue = 0;
    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_DATASENT_EVENT_FLAG, CYU3P_EVENT_OR);
}

static void
HandleModeSenseCommand (
        uint8_t lun)
{
    CyU3PReturnStatus_t status;
    uint32_t            dataLength;
    uint8_t             mode_page, mode_pc;
    uint8_t            *cache_p = 0;

    /* We only support the Caching mode page. */
    mode_page = glMscCbwBuffer[CY_FX_SCSI_MODESENS_PC_OFFSET] & CY_FX_SCSI_MODESENS_PAGE_MASK;
    if ((mode_page != CY_FX_SCSI_MODESENS_PAGE_CACHE) && (mode_page != CY_FX_SCSI_MODESENS_PAGE_ALL))
    {
        glMscCmdStatus  = 1;
        glSensePtr[lun] = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
        CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
        return;
    }

    mode_pc = glMscCbwBuffer[CY_FX_SCSI_MODESENS_PC_OFFSET] & CY_FX_SCSI_MODESENS_PC_MASK;
    switch (mode_pc)
    {
        case CY_FX_SCSI_MODESENS_PC_CHANGE:
            cache_p = (uint8_t *)CyFxMscCachingChangeable;
            break;

        case CY_FX_SCSI_MODESENS_PC_CURRENT:
            cache_p = CyFxMscCachingCurrent;
            break;

        case CY_FX_SCSI_MODESENS_PC_DEFAULT:
            cache_p = CyFxMscCachingDefault;
            break;

        default:
            /* No saved page support. */
            glMscCmdStatus  = 1;
            glSensePtr[lun] = CY_FX_MSC_SENSE_NO_SAVEPAGE_SUPPORT;
            CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
            return;
    }

    glMscDataBuffer[0] = 0x03;
    glMscDataBuffer[1] = 0x00;
    glMscDataBuffer[2] = (glLunWriteable[lun]) ? 0x00 : 0x80;
    glMscDataBuffer[3] = 0x00;

    CyU3PMemCopy ((uint8_t *)&glMscDataBuffer[4], cache_p, sizeof (CyFxMscCachingDefault));
    dataLength = 4 + sizeof (CyFxMscCachingDefault);
    glMscDataBuffer[0] = dataLength - 1;

    if (!CyFxMscApplnCheckCbwParams (lun, CyTrue, CyTrue, CyFalse, dataLength, &dataLength))
        return;

    glMscState = CY_FX_MSC_STATE_DATA;
    status = CyFxMscApplnSendDataToHost (glMscDataBuffer, dataLength);
    if (status != CY_U3P_SUCCESS)
    {
        glMscCmdStatus  = 1;
        glSensePtr[lun] = CY_FX_MSC_SENSE_CRC_ERROR;
        CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
    }
}

/* Parse the received CBW and handle the command. */
void
CyFxMscApplnParseCbw (
    void)
{
    CyU3PReturnStatus_t status;
    uint8_t cmd = glMscCbwBuffer[15];
    uint8_t lun = glMscCbwBuffer[13] & 0x0F;
    uint32_t startAddr, dataLength;
    uint16_t numBlks;

    uint8_t *inqData_p = 0;
    uint32_t inqDataLen = 0;

    /* Direction of data transfer. */
    glCmdDirection = glMscCbwBuffer[12] & 0x80;

    /* Expected transfer length. */
    glMscResidue   = *((uint32_t *)(glMscCbwBuffer + 8));
    glMscCmdStatus = 0;

    /* Invalid LUN. */
    if (lun >= CY_FX_LUN_COUNT)
    {
        glMscCmdStatus  = 1;
        CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
        return;
    }

    glMscCmdLun = lun;

    /* Execute the command */
    switch (cmd)
    {
        case CY_FX_MSC_SCSI_INQUIRY:
            {
                if ((glMscCbwBuffer[CY_FX_SCSI_INQCMD_EVPD_OFFSET] & CY_FX_SCSI_INQCMD_EVPD_MASK) != 0)
                {
                    /* Request for Vital Product Data. */
                    switch (glMscCbwBuffer[CY_FX_SCSI_INQCMD_PCODE_OFFSET])
                    {
                        case CY_FX_SCSI_INQCMD_PCODE_LIST:
                            /* List of supported Vital Product Data Pages. */
                            inqData_p  = (uint8_t *)CyFxMscSupportedPageList;
                            inqDataLen = sizeof (CyFxMscSupportedPageList);
                            break;

                        case CY_FX_SCSI_INQCMD_PCODE_SERNUM:
                            /* Unit Serial Number Page. */
                            inqData_p  = (uint8_t *)CyFxMscScsiSerialNumber;
                            inqDataLen = sizeof (CyFxMscScsiSerialNumber);
                            break;

                        case CY_FX_SCSI_INQCMD_PCODE_DEVIDENT:
                            /* Device Identification Page. */
                            inqData_p  = (uint8_t *)CyFxMscScsiDevIdent;
                            inqDataLen = sizeof (CyFxMscScsiDevIdent);

                            /* Update the last byte based on the LUN. */
                            CyFxMscScsiDevIdent[inqDataLen - 1] = '0' + lun;
                            break;

                        case CY_FX_SCSI_INQCMD_PCODE_BLKLIMITS:
                            /* Block Limits Page. */
                            inqData_p  = (uint8_t *)CyFxMscScsiBlkLimits;
                            inqDataLen = sizeof (CyFxMscScsiBlkLimits);
                            break;

                        default:
                            /* No other VPD pages supported. */
                            glMscCmdStatus  = 1;
                            glSensePtr[lun] = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
                            CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                            break;
                    }

                    if (glMscCmdStatus != 0)
                        break;
                }
                else
                {
                    /* Page code must be zero if Vital Product Data is not being requested. */
                    if (glMscCbwBuffer[CY_FX_SCSI_INQCMD_PCODE_OFFSET] != 0)
                    {
                        glMscCmdStatus  = 1;
                        glSensePtr[lun] = CY_FX_MSC_SENSE_INVALID_PARAMETER;
                        CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                        break;
                    }

                    inqDataLen = CY_FX_SCSI_INQUIRY_DATALEN;
                    inqData_p  = (uint8_t *)CyFxMscScsiInquiryData;
                }

                if (!CyFxMscApplnCheckCbwParams (lun, CyFalse, CyTrue, CyFalse,
                            inqDataLen, &dataLength))
                    break;

                glMscState = CY_FX_MSC_STATE_DATA;
                status = CyFxMscApplnSendDataToHost (inqData_p, dataLength);
                
                if (status != CY_U3P_SUCCESS)
                {
                    glMscCmdStatus  = 1;
                    glSensePtr[lun] = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        case CY_FX_MSC_SCSI_READ_CAPACITY:
        case CY_FX_MSC_SCSI_READ_FORMAT_CAPACITY:
            {
                uint8_t i = 0;

                if (!CyFxMscApplnCheckCbwParams (lun, CyTrue, CyTrue, CyFalse,
                            ((cmd == CY_FX_MSC_SCSI_READ_CAPACITY) ? 8 : 12), &dataLength))
                    break;

                CyU3PMemSet (glMscDataBuffer, 0, 12);
                if (cmd == CY_FX_MSC_SCSI_READ_FORMAT_CAPACITY)
                {
                    glMscDataBuffer[3] = 0x08;
                    glMscDataBuffer[8] = 0x02;
                    i = 4;
                }

                glMscDataBuffer[i++] = (uint8_t)(glLunNumBlks[lun] >> 24);
                glMscDataBuffer[i++] = (uint8_t)(glLunNumBlks[lun] >> 16);
                glMscDataBuffer[i++] = (uint8_t)(glLunNumBlks[lun] >> 8);
                glMscDataBuffer[i++] = (uint8_t)(glLunNumBlks[lun]);

                i += 2;
                glMscDataBuffer[i++] = (uint8_t)(glLunBlkSize[lun] >> 8);
                glMscDataBuffer[i++] = (uint8_t)(glLunBlkSize[lun]);

                glMscState = CY_FX_MSC_STATE_DATA;
                status = CyFxMscApplnSendDataToHost (glMscDataBuffer, dataLength);
                if (status != CY_U3P_SUCCESS) 
                {
                    glMscCmdStatus  = 1;
                    glSensePtr[lun] = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        case CY_FX_MSC_SCSI_REQUEST_SENSE:
            {
                if ((glMscCbwBuffer[CY_FX_SCSI_RQSCMD_DESC_OFFSET] & CY_FX_SCSI_REQSENSE_DESC) != 0)
                {
                    glMscCmdStatus  = 1;
                    glSensePtr[lun] = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }

                if (!CyFxMscApplnCheckCbwParams (lun, CyFalse, CyTrue, CyFalse, 18, &dataLength))
                    break;

                CyU3PMemSet (glMscDataBuffer, 0, 18);
                glMscDataBuffer[0]  = 0x70;
                glMscDataBuffer[2]  = glReqSenseCode[glSensePtr[lun]][0];
                glMscDataBuffer[7]  = 0x0A;
                glMscDataBuffer[12] = glReqSenseCode[glSensePtr[lun]][1];
                glMscDataBuffer[13] = glReqSenseCode[glSensePtr[lun]][2];

                glMscState = CY_FX_MSC_STATE_DATA;
                if (glSensePtr[lun] == CY_FX_MSC_SENSE_MEDIA_CHANGED)
                    glSensePtr[lun] = CY_FX_MSC_SENSE_OK;

                status = CyFxMscApplnSendDataToHost (glMscDataBuffer, dataLength);
                if (status != CY_U3P_SUCCESS)
                {
                    glMscCmdStatus  = 1;
                    glSensePtr[lun] = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

            /* No real support for Verify, Start/stop unit and Prevent/Allow Medium Removal commands. */
        case CY_FX_MSC_SCSI_VERIFY_10:
        case CY_FX_MSC_SCSI_PREVENT_ALLOW_MEDIUM:
        case CY_FX_MSC_SCSI_TEST_UNIT_READY:
            {
                if (!CyFxMscApplnCheckCbwParams (lun, CyTrue, CyFalse, CyFalse, 0, &dataLength))
                    break;

                glMscState = CY_FX_MSC_STATE_STATUS;
            }
            break;

        case CY_FX_MSC_SCSI_START_STOP_UNIT:
            {
                if (!CyFxMscApplnCheckCbwParams (lun, CyFalse, CyFalse, CyFalse, 0, &dataLength))
                    break;

                /* Start/Stop the unit as desired by the user. */
                if ((glMscCbwBuffer[CY_FX_SCSI_STOPCMD_LOEJ_OFFSET] & CY_FX_SCSI_STOPCMD_LOEJ_VAL) != 0)
                {
                    if ((glMscCbwBuffer[CY_FX_SCSI_STOPCMD_LOEJ_OFFSET] & CY_FX_SCSI_STOPCMD_START_VAL) == 0)
                        glLunStopped[lun] = CyTrue;
                    else
                        glLunStopped[lun] = CyFalse;
                }

                glMscState = CY_FX_MSC_STATE_STATUS;
            }
            break;

        case CY_FX_MSC_SCSI_SEND_DIAG:
            {
                /* In case of self-test, report success. In other cases, report no support. */
                if ((glMscCbwBuffer[CY_FX_SCSI_SENDDIAG_SFTEST_OFFSET] & CY_FX_SCSI_SENDDIAG_SFTEST_MASK) == 0)
                {
                    glMscCmdStatus  = 1;
                    glSensePtr[lun] = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }

                if (!CyFxMscApplnCheckCbwParams (lun, CyTrue, CyFalse, CyFalse, 0, &dataLength))
                    break;

                glMscState = CY_FX_MSC_STATE_STATUS;
            }
            break;

        case CY_FX_MSC_SCSI_MODE_SELECT_6:
            {
                if ((glMscCbwBuffer[CY_FX_SCSI_MODESELECT_SP_OFFSET] & CY_FX_SCSI_MODESELECT_SP_MASK) != 0)
                {
                    /* Saving changes is not supported. */
                    glMscCmdStatus  = 1;
                    glSensePtr[lun] = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }

                if (!CyFxMscApplnCheckCbwParams (lun, CyTrue, CyFalse, CyFalse, glMscResidue, &dataLength))
                    break;

                if (dataLength != 0)
                {
                    glMscState = CY_FX_MSC_STATE_DATA;
                    status = CyFxMscApplnReceiveUsbData (glMscDataBuffer, dataLength);
                    if (status != CY_U3P_SUCCESS)
                    {
                        glMscCmdStatus  = 1;
                        glSensePtr[lun] = CY_FX_MSC_SENSE_CRC_ERROR;
                        CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    }
                }
                else
                {
                    glMscState = CY_FX_MSC_STATE_STATUS;
                }
            }
            break;

        case CY_FX_MSC_SCSI_MODE_SENSE_6:
            {
                HandleModeSenseCommand (lun);
            }
            break;

        case CY_FX_MSC_SCSI_READ_10:
            {
                startAddr = (((uint32_t)glMscCbwBuffer[17] << 24) | ((uint32_t)glMscCbwBuffer[18] << 16) |
                        ((uint32_t)glMscCbwBuffer[19] << 8) | ((uint32_t)glMscCbwBuffer[20]));
                numBlks   = (((uint16_t)glMscCbwBuffer[22] << 8) | ((uint16_t)glMscCbwBuffer[23]));

                if (!CyFxMscApplnCheckCbwParams (lun, CyTrue, CyTrue, CyTrue, (numBlks * glLunBlkSize[lun]),
                            &dataLength))
                    break;

                glMscState = CY_FX_MSC_STATE_DATA;
                status = CyU3PDmaChannelSetXfer (&glChHandleMscIn, (numBlks * CY_FX_SIB_MAX_BLOCK_SIZE));
                if (status == CY_U3P_SUCCESS)
                {
                    status = CyU3PSibReadWriteRequest (CY_FX_SIB_READ, ((lun >= CY_FX_SIB_PARTITIONS) ? 1 : 0),
                            glLunUnit[lun], numBlks, startAddr, 1);
                    if (status != CY_U3P_SUCCESS)
                    {
                        /* Abort the DMA Channel */
                        CyU3PDmaChannelReset (&glChHandleMscIn);
                    }
                }	

                if (status != CY_U3P_SUCCESS)
                {
                    glMscCmdStatus  = 1;
                    glSensePtr[lun] = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        case CY_FX_MSC_SCSI_WRITE_10:
            {
                startAddr = (((uint32_t)glMscCbwBuffer[17] << 24) | ((uint32_t)glMscCbwBuffer[18] << 16) |
                        ((uint32_t)glMscCbwBuffer[19] << 8) | ((uint32_t)glMscCbwBuffer[20]));
                numBlks   = (((uint16_t)glMscCbwBuffer[22] << 8) | ((uint16_t)glMscCbwBuffer[23]));

                if (!CyFxMscApplnCheckCbwParams (lun, CyTrue, CyFalse, CyTrue, (numBlks * glLunBlkSize[lun]),
                            &dataLength))
                    break;

                if (!glLunWriteable[lun])
                {
                    glMscCmdStatus  = 1;
                    glSensePtr[lun] = CY_FX_MSC_SENSE_WRITE_PROTECT;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }

                glMscState = CY_FX_MSC_STATE_DATA;
                status = CyU3PDmaChannelSetXfer (&glChHandleMscOut, (numBlks * CY_FX_SIB_MAX_BLOCK_SIZE));
                if (status == CY_U3P_SUCCESS)
                {
                    status = CyU3PSibReadWriteRequest (CY_FX_SIB_WRITE, ((lun >= CY_FX_SIB_PARTITIONS) ? 1 : 0),
                            glLunUnit[lun], numBlks, startAddr, 0);
                    if (status != CY_U3P_SUCCESS)
                    {
                        /* Abort the DMA Channel */
                        CyU3PDmaChannelReset (&glChHandleMscIn);
                    }
                    else
                    {
                        /* Reset idle timer when initiating a new write operation. */
                        glMscIdleTime = 0;
                    }
                }

                if (status != CY_U3P_SUCCESS)
                {
                    glMscCmdStatus  = 1;
                    glSensePtr[lun] = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        case CY_FX_MSC_SCSI_SYNCCACHE_10:
            {
                if (!CyFxMscApplnCheckCbwParams (lun, CyTrue, CyFalse, CyFalse, 0, &dataLength))
                    break;

                /* Make sure that all data is committed to the Storage device. */
                CyU3PSibCommitReadWrite ((lun >= CY_FX_SIB_PARTITIONS) ? 1 : 0);
                glMscState = CY_FX_MSC_STATE_STATUS;
            }
            break;

        case CY_FX_MSC_SCSI_READCAP_16:
            {
                /* Verify that the SERVICE ACTION field is set to the expected value. */
                if (glMscCbwBuffer[CY_FX_SCSI_READCAP16_SERVACT_OFFSET] != CY_FX_SCSI_READCAP16_SERVACT_VALUE)
                {
                    glMscCmdStatus  = 1;
                    glSensePtr[lun] = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }

                if (!CyFxMscApplnCheckCbwParams (lun, CyTrue, CyTrue, CyFalse, 32, &dataLength))
                    break;

                CyU3PMemSet (glMscDataBuffer, 0, 32);

                /* First four bytes are always zero. */
                glMscDataBuffer[4] = (uint8_t)(glLunNumBlks[lun] >> 24);
                glMscDataBuffer[5] = (uint8_t)(glLunNumBlks[lun] >> 16);
                glMscDataBuffer[6] = (uint8_t)(glLunNumBlks[lun] >> 8);
                glMscDataBuffer[7] = (uint8_t)(glLunNumBlks[lun]);

                glMscDataBuffer[10] = (uint8_t)(glLunBlkSize[lun] >> 8);
                glMscDataBuffer[11] = (uint8_t)(glLunBlkSize[lun]);

                glMscDataBuffer[13] = 3;        /* Report 8 LBs per physical block. */

                glMscState = CY_FX_MSC_STATE_DATA;
                status = CyFxMscApplnSendDataToHost (glMscDataBuffer, dataLength);
                if (status != CY_U3P_SUCCESS) 
                {
                    glMscCmdStatus  = 1;
                    glSensePtr[lun] = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        default:
            {
                /* Unsupported command. */
                glMscCmdStatus  = 1;
                glSensePtr[lun] = CY_FX_MSC_SENSE_INVALID_OP_CODE;
                CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
            }
            break;
    }
}

/* This is the Callback function to handle the USB Events */
void
CyFxMscApplnUSBEventCB (
    CyU3PUsbEventType_t evtype, /* Event type */
    uint16_t            evdata  /* Event data */
    )
{
    uint16_t epPktSize = 64;    /* Full speed setting by default. */
    uint32_t i;

    switch (evtype)
    {
        case CY_U3P_USB_EVENT_SUSPEND:
            CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_USB_SUSP_EVENT_FLAG, CYU3P_EVENT_OR);
            break;

        case CY_U3P_USB_EVENT_DISCONNECT:
        case CY_U3P_USB_EVENT_RESET:
        case CY_U3P_USB_EVENT_CONNECT:
            {
                AppDebugPrint (4, "USB event %d received\r\n", evtype);
                CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_RESET_EVENT_FLAG, CYU3P_EVENT_OR);

                CyFxMscApplnResetDatapath ();
                glDevConfigured = CyFalse;

                /* Restore all LUNs to an active state. */
                for (i = 0; i < CY_FX_LUN_COUNT; i++)
                    glLunStopped[i] = CyFalse;
            }
            break;

        case CY_U3P_USB_EVENT_SETCONF:
            {
                if (evdata == 0)
                {
                    glDevConfigured = CyFalse;
                    break;
                }

                AppDebugPrint (4, "USB Set Config completed\r\n");

                /* Check if Set Config event is handled */
                if (glDevConfigured == CyFalse )
                {
                    glDevConfigured = CyTrue;

                    /* The max. packet size depends on connection speed. */
                    switch (CyU3PUsbGetSpeed ())
                    {
                        case CY_U3P_FULL_SPEED:
                            epPktSize = 64;
                            break;
                        case CY_U3P_HIGH_SPEED:
                            epPktSize = 512;
                            break;
                        case CY_U3P_SUPER_SPEED:
                        default:
                            epPktSize = 1024;
                            break;
                    }

                    /* Update the packet size for the endpoints based on connection speed. */
                    CyU3PSetEpPacketSize (CY_FX_MSC_EP_BULK_OUT, epPktSize);
                    CyU3PSetEpPacketSize (CY_FX_MSC_EP_BULK_IN, epPktSize);

                    /* Clear stall on the endpoints and reset the DMA channels. */
                    CyU3PUsbStall (CY_FX_MSC_EP_BULK_OUT, CyFalse, CyTrue);
                    CyU3PUsbStall (CY_FX_MSC_EP_BULK_IN, CyFalse, CyTrue);

                    CyU3PDmaChannelReset (&glChHandleMscIn);
                    CyU3PDmaChannelReset (&glChHandleMscOut);

                    /* Restore all LUNs to an active state. */
                    for (i = 0; i < CY_FX_LUN_COUNT; i++)
                        glLunStopped[i] = CyFalse;

                    /* Notify the thread to start waiting for a mass storage CBW. */
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SETCONF_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        default:
            break;
    }
}

/* Callback function to handle LPM requests from the USB 3.0 host. This function is invoked by the API
   whenever a state change from U0 -> U1 or U0 -> U2 happens. If we return CyTrue from this function, the
   FX3S device is retained in the low power state. If we return CyFalse, the FX3S device immediately tries
   to trigger an exit back to U0.
 */
CyBool_t
CyFxMscApplnLPMRqtCB (
        CyU3PUsbLinkPowerMode link_mode)
{
    /* If we are in the middle of processing a mass storage command, keep reverting to U0 state. */
    return !glMscDriverBusy;
}

void
CyFxMscApplnQueryDevStatus (
        void)
{
    uint32_t partsize[CY_FX_SIB_PARTITIONS] = {0};
    uint8_t  parttype[CY_FX_SIB_PARTITIONS] = {0};
    CyU3PReturnStatus_t status;
    CyU3PSibLunInfo_t   unitInfo;
    uint8_t i, j;
    uint8_t unitsFound;

    for (i = 0; i < CY_FX_SIB_PORTS; i++)
    {
        unitsFound = 0;

        /* Initialize LUN data structures with default values. */
        for (j = 0; j < CY_FX_SIB_PARTITIONS; j++)
        {
            glLunState[i * CY_FX_SIB_PARTITIONS + j]     = CyFalse;
            glLunUnit[i * CY_FX_SIB_PARTITIONS + j]      = 10;          /* Invalid value. */
            glLunWriteable[i * CY_FX_SIB_PARTITIONS + j] = CyTrue;
            glLunBlkSize[i * CY_FX_SIB_PARTITIONS + j]   = 0;
            glLunNumBlks[i * CY_FX_SIB_PARTITIONS + j]   = 0;
        }

        /* Query each of the storage ports to identify whether a device has been detected. */
        status = CyU3PSibQueryDevice (i, &glDevInfo[i]);
        if (status == CY_U3P_SUCCESS)
        {
            AppDebugPrint (8, "Found a device on port %d\r\n", i);
            AppDebugPrint (6, "\tType=%d, numBlks=%d, eraseSize=%d, clkRate=%d\r\n",
                    glDevInfo[i].cardType, glDevInfo[i].numBlks, glDevInfo[i].eraseSize, glDevInfo[i].clkRate);
            AppDebugPrint (6, "\tblkLen=%d removable=%d, writeable=%d, locked=%d\r\n",
                    glDevInfo[i].blkLen, glDevInfo[i].removable, glDevInfo[i].writeable, glDevInfo[i].locked);
            AppDebugPrint (6, "\tddrMode=%d, opVoltage=%d, busWidth=%d, numUnits=%d\r\n",
                    glDevInfo[i].ddrMode, glDevInfo[i].opVoltage, glDevInfo[i].busWidth, glDevInfo[i].numUnits);
            AppDebugPrint (6, "\tcardCmdClass=%d\r\n", glDevInfo[i].ccc);

            /* Run through each of the units present and check how many data partitions are present.
               We skip all boot partitions in this application.
             */
            for (j = 0, unitsFound = 0; j < glDevInfo[i].numUnits; j++)
            {
                status = CyU3PSibQueryUnit (i, j, &unitInfo);
                if (status != CY_U3P_SUCCESS)
                {
                    AppDebugPrint (2, "Error: Failed to query partition %d on port %d\r\n", j, i);
                    break;
                }

                AppDebugPrint (6, "Dev %d, Unit %d: location=%d numBlocks=%d\r\n",
                        i, j, unitInfo.location, unitInfo.numBlocks);
                if (unitInfo.location == CY_U3P_SIB_LOCATION_USER)
                {
                    AppDebugPrint (4, "Dev %d: Found user partition\r\n", i);
                    unitsFound++;
                }
            }

            if (unitsFound > CY_FX_SIB_PARTITIONS)
            {
                CyU3PSibRemovePartitions (i);
            }

            if ((unitsFound < CY_FX_SIB_PARTITIONS) && (glDevInfo[i].writeable))
            {
                /* Make sure any existing partitioning is removed. */
                CyU3PSibRemovePartitions (i);

                /* Makes sure that each storage device is partitioned into the required number of units. */
                for (j = 0; j < CY_FX_SIB_PARTITIONS; j++)
                {
                    partsize[j] = (glDevInfo[i].numBlks / CY_FX_SIB_PARTITIONS);
                    parttype[j] = CY_U3P_SIB_LUN_DATA;
                }

                AppDebugPrint (4, "Port %d: Creating partitions as required\r\n", i);
                status = CyU3PSibPartitionStorage (i, CY_FX_SIB_PARTITIONS, partsize, parttype);
                if (status != CY_U3P_SUCCESS)
                {
                    AppDebugPrint (2, "Error: Failed to partition storage device %d, status=%d\r\n", i, status);
                    continue;
                }
            }

            /* Query the number of partitions again, and then fill the required information into the local
               data structures.
               */
            status = CyU3PSibQueryDevice (i, &glDevInfo[i]);
            if (status != CY_U3P_SUCCESS)
            {
                AppDebugPrint (2, "StorageQueryDev (%d) failed with code %d\r\n", i, status);
                continue;
            }

            for (j = 0, unitsFound = 0; ((j < glDevInfo[i].numUnits) && (unitsFound < CY_FX_SIB_PARTITIONS)); j++)
            {
                status = CyU3PSibQueryUnit (i, j, &unitInfo);
                if (status == CY_U3P_SUCCESS)
                {
                    if (unitInfo.location == CY_U3P_SIB_LOCATION_USER)
                    {
                        AppDebugPrint (6, "Dev %d, Unit %d: blkSize=%d numBlocks=%d\r\n", i, j,
                                unitInfo.blockSize, unitInfo.numBlocks);
                        glLunState[i * CY_FX_SIB_PARTITIONS + unitsFound]     = CyTrue;
                        glLunUnit[i * CY_FX_SIB_PARTITIONS + unitsFound]      = j;
                        glLunBlkSize[i * CY_FX_SIB_PARTITIONS + unitsFound]   = unitInfo.blockSize;
                        glLunNumBlks[i * CY_FX_SIB_PARTITIONS + unitsFound]   = unitInfo.numBlocks - 1;
                        glLunWriteable[i * CY_FX_SIB_PARTITIONS + unitsFound] = glDevInfo[i].writeable;
                        unitsFound++;
                    }
                    else
                    {
                        AppDebugPrint (6, "Port %d: Skipping boot partition %d\r\n", i, j);
                    }
                }
                else
                {
                    AppDebugPrint (2, "Error: Failed to query partition %d on port %d\r\n", j, i);
                    break;
                }
            }

            /* Commit data in 8MB chunks. */
            CyU3PSibSetWriteCommitSize (i, 16384);
        }
        else
        {
            AppDebugPrint (2, "StorageQueryDev (%d) failed with code %d\r\n", i, status);
        }
    }
}

void
CyFxMscApplnSibCB (
        uint8_t             portId,
        CyU3PSibEventType   evt,
        CyU3PReturnStatus_t status)
{
    CyU3PDmaSocketConfig_t sockConf;

    if (evt == CY_U3P_SIB_EVENT_XFER_CPLT)
    {
        if (status != CY_U3P_SUCCESS)
        {
            glMscCmdStatus     = 1;
            glSensePtr[portId] = CY_FX_MSC_SENSE_CRC_ERROR;

            /* Transfer has failed. Reset the DMA channel. */
            if (glCmdDirection)
            {
                CyU3PDmaSocketGetConfig ((uint16_t)(CY_U3P_UIB_SOCKET_CONS_0 | CY_FX_MSC_EP_BULK_IN_SOCKET),
                        &sockConf);
                glMscResidue -= sockConf.xferCount;
                CyU3PDmaChannelReset (&glChHandleMscIn);
            }
            else
            {
                CyU3PDmaSocketGetConfig ((uint16_t)(CY_U3P_UIB_SOCKET_PROD_0 + CY_FX_MSC_EP_BULK_OUT_SOCKET),
                        &sockConf);
                glMscResidue -= sockConf.xferCount;
                CyU3PDmaChannelReset (&glChHandleMscOut);
            }
        }
        else
        {
            glMscCmdStatus = 0;
            glMscResidue   = 0;
            glSensePtr[portId] = CY_FX_MSC_SENSE_OK;
        }

        CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
    }

    if (evt == CY_U3P_SIB_EVENT_INSERT)
    {
        uint8_t i = 0;

        AppDebugPrint (2, "Insert event on port %d\r\n", portId);
        CyFxMscApplnQueryDevStatus ();

        for (i = 0; i < CY_FX_SIB_PARTITIONS; i++)
        {
            glSensePtr[portId * CY_FX_SIB_PARTITIONS + i] = CY_FX_MSC_SENSE_MEDIA_CHANGED;
        }
    }

    if (evt == CY_U3P_SIB_EVENT_REMOVE)
    {
        uint8_t i = 0;

        AppDebugPrint (2, "Remove event on port %d\r\n", portId);
        CyFxMscApplnQueryDevStatus ();

        for (i = 0; i < CY_FX_SIB_PARTITIONS; i++)
        {
            glSensePtr[portId * CY_FX_SIB_PARTITIONS + i] = CY_FX_MSC_SENSE_MEDIA_CHANGED;
        }
    }

    if ((evt == CY_U3P_SIB_EVENT_DATA_ERROR) || (evt == CY_U3P_SIB_EVENT_ABORT))
    {
        /* Transfer has failed. Reset the DMA channel. */
        if (glCmdDirection)
        {
            CyU3PDmaChannelReset ((CyU3PDmaChannel *) &glChHandleMscOut);
        }
        else
        {
            CyU3PDmaChannelReset ((CyU3PDmaChannel *) &glChHandleMscIn);
        }

        /* Make sure the request is aborted and that the controller is reset. */
        CyU3PSibAbortRequest (portId);
    }
}

void
CyFxMscApplnSibInit (
        void)
{
    CyU3PReturnStatus_t  status;
    CyU3PSibIntfParams_t intfParams;
    CyU3PGpioClock_t     gpioClock;

    /* GPIO module needs to be initialized before SIB is initialized. This is required because
       GPIOs are used in the SIB code.
     */
    gpioClock.fastClkDiv = 2;
    gpioClock.slowClkDiv = 16;
    gpioClock.simpleDiv  = CY_U3P_GPIO_SIMPLE_DIV_BY_2;
    gpioClock.clkSrc     = CY_U3P_SYS_CLK;
    gpioClock.halfDiv    = 0;
    status = CyU3PGpioInit (&gpioClock, NULL);
    if (status != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "GPIO Init failed, code=%d\r\n", status);
        CyFxAppErrorHandler (status);
    }

    intfParams.resetGpio       = 0xFF;                          /* No GPIO control on SD/MMC power. */
    intfParams.rstActHigh      = CyTrue;                        /* Don't care as no GPIO is selected. */
    intfParams.cardDetType     = CY_U3P_SIB_DETECT_DAT_3;       /* Card detect based on SD_DAT[3]. */
    intfParams.voltageSwGpio   = 45;                            /* Use GPIO_45 for voltage switch on S0 port. */
    intfParams.lvGpioState     = CyFalse;                       /* Driving GPIO low selects 1.8 V on SxVDDQ. */
    intfParams.writeProtEnable = CyFalse;                       /* Write protect handling enabled. */
    intfParams.lowVoltage      = CyTrue;                        /* Low voltage operation enabled. */
    intfParams.useDdr          = CyTrue;                        /* DDR clocking enabled. */
    intfParams.maxFreq         = CY_U3P_SIB_FREQ_104MHZ;        /* No S port clock limitation. */
    intfParams.cardInitDelay   = 0;                             /* No delay required between SD card insertion
                                                                   before initialization. */
    status = CyU3PSibSetIntfParams (0, &intfParams);
    if (status == CY_U3P_SUCCESS)
    {
        intfParams.voltageSwGpio = 57;
        status = CyU3PSibSetIntfParams (1, &intfParams);
    }

    if (status != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "Set SIB interface parameters failed, code=%d\r\n", status);
        CyFxAppErrorHandler (status);
    }

    status = CyU3PSibStart ();
    if (status != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "SIB start failed, code=%d\r\n", status);
        CyFxAppErrorHandler (status);
    }

    /* Register a callback for SIB events. */
    CyU3PSibRegisterCbk (CyFxMscApplnSibCB);

    /* Query the current device status. */
    CyFxMscApplnQueryDevStatus ();
}

/* Configure the endpoints required for the mass storage device operation. */
void
CyFxMscApplnConfigEndpoints (
        CyBool_t enable)
{
    CyU3PEpConfig_t epConf = {0};
    CyU3PReturnStatus_t status;

    if (enable)
    {
        epConf.enable   = CyTrue;
        epConf.epType   = CY_U3P_USB_EP_BULK;
        epConf.streams  = 0;
        epConf.pcktSize = 1024;                     /* Configuring for super speed by default. */
        epConf.burstLen = CY_FX_MSC_EP_BURST_SIZE;
        epConf.isoPkts  = 0;
    }
    else
    {
        epConf.enable = CyFalse;
    }

    status = CyU3PSetEpConfig (CY_FX_MSC_EP_BULK_OUT, &epConf);
    if (status != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set EP config failed, code=%d\r\n", status);
        CyFxAppErrorHandler (status);
    } 

    status = CyU3PSetEpConfig (CY_FX_MSC_EP_BULK_IN, &epConf);
    if (status != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set EP config failed, code=%d\r\n", status);
        CyFxAppErrorHandler (status);
    } 
}

void
CyFxMscApplnDmaCb (
        CyU3PDmaChannel   *handle,
        CyU3PDmaCbType_t   evtype,
        CyU3PDmaCBInput_t *input)
{
    CyU3PDmaBuffer_t dmaBuf = input->buffer_p;

    switch (evtype)
    {
        case CY_U3P_DMA_CB_RECV_CPLT:
            {
                if ((glMscState == CY_FX_MSC_STATE_CBW) || (glMscState == CY_FX_MSC_STATE_WAITING))
                {
                    if ((dmaBuf.count != 31) || (dmaBuf.buffer[0] != 'U') || (dmaBuf.buffer[1] != 'S') ||
                            (dmaBuf.buffer[2] != 'B') || (dmaBuf.buffer[3] != 'C'))
                        glInPhaseError = CyTrue;
                    else
                        glInPhaseError = CyFalse;
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_CBW_EVENT_FLAG, CYU3P_EVENT_OR);
                }
                else
                {
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_DATA_RECV_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        case CY_U3P_DMA_CB_SEND_CPLT:
            CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_DATASENT_EVENT_FLAG, CYU3P_EVENT_OR);
            break;

        default:
            break;
    }
}

/* Create the DMA channels required for the mass storage device operation. */
void
CyFxMscApplnDmaInit (
        void)
{
    CyU3PDmaChannelConfig_t dmaConfig;
    CyU3PReturnStatus_t status;

    glMscCbwBuffer  = (uint8_t *)CyU3PDmaBufferAlloc (1024);
    glMscCswBuffer  = (uint8_t *)CyU3PDmaBufferAlloc (1024);
    glMscDataBuffer = (uint8_t *)CyU3PDmaBufferAlloc (1024);
    if ((glMscCbwBuffer == 0) || (glMscCswBuffer == 0) || (glMscDataBuffer == 0))
    {
        AppDebugPrint (4, "Failed to allocate scratch buffer\r\n");
        CyFxAppErrorHandler (CY_U3P_ERROR_MEMORY_ERROR);
    }

    /* Both DMA channels are created with SuperSpeed parameters. The CyU3PSetEpPacketSize () API is used
       to reconfigure the endpoints to work with DMA channels with large buffers. */
    dmaConfig.size           = 1024 * CY_FX_MSC_EP_BURST_SIZE;
    dmaConfig.count          = CY_FX_MSC_DMA_BUF_COUNT;
    dmaConfig.prodSckId      = (CyU3PDmaSocketId_t)(CY_U3P_UIB_SOCKET_PROD_0 | CY_FX_MSC_EP_BULK_OUT_SOCKET);
    dmaConfig.consSckId      = CY_U3P_SIB_SOCKET_0;
    dmaConfig.dmaMode        = CY_U3P_DMA_MODE_BYTE;
    dmaConfig.notification   = CY_U3P_DMA_CB_RECV_CPLT;
    dmaConfig.cb             = CyFxMscApplnDmaCb;
    dmaConfig.prodHeader     = 0;
    dmaConfig.prodFooter     = 0;
    dmaConfig.consHeader     = 0;
    dmaConfig.prodAvailCount = 0;

    status = CyU3PDmaChannelCreate (&glChHandleMscOut, CY_U3P_DMA_TYPE_AUTO, &dmaConfig);
    if (status != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "DMA channel create failed, code=%d\r\n", status);
        CyFxAppErrorHandler (status);
    }

    dmaConfig.prodSckId     = CY_U3P_SIB_SOCKET_1;
    dmaConfig.consSckId     = (CyU3PDmaSocketId_t)(CY_U3P_UIB_SOCKET_CONS_0 | CY_FX_MSC_EP_BULK_IN_SOCKET);
    dmaConfig.notification  = CY_U3P_DMA_CB_SEND_CPLT;
    dmaConfig.cb            = CyFxMscApplnDmaCb;

    status = CyU3PDmaChannelCreate (&glChHandleMscIn, CY_U3P_DMA_TYPE_AUTO, &dmaConfig);
    if (status != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "DMA channel create failed, code=%d\r\n", status);
        CyFxAppErrorHandler (status);
    }
}

static void
CyFxMscAppInitVars (
		void)
{
    uint8_t i;

    for (i = 0; i < (CY_FX_LUN_COUNT); i++)
    {
        glLunState[i]     = CyFalse;
        glLunUnit[i]      = 10;
        glLunWriteable[i] = CyTrue;
        glLunBlkSize[i]   = 0;
        glLunNumBlks[i]   = 0;
        glSensePtr[i]     = CY_FX_MSC_SENSE_DEVICE_RESET;
        glLunStopped[i]   = CyFalse;
    }
}

void
CyFxMscApplnInit (
        void)
{
    CyU3PReturnStatus_t apiRetStatus;

    /* Start the USB functionality */
    apiRetStatus = CyU3PUsbStart ();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error Handling */
        AppDebugPrint (4, "USB Function Failed to Start, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Setup the Callback to Handle the USB Setup Requests */
    CyU3PUsbRegisterSetupCallback (CyFxMscApplnUSBSetupCB, CyTrue);

    /* Setup the Callback to Handle the USB Events */
    CyU3PUsbRegisterEventCallback (CyFxMscApplnUSBEventCB);

    /* Register a callback to handle LPM requests from the USB 3.0 host. */
    CyU3PUsbRegisterLPMRequestCallback (CyFxMscApplnLPMRqtCB);

    /* Register the USB descriptors. */
    apiRetStatus = CyU3PUsbSetDesc (CY_U3P_USB_SET_HS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB20DeviceDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set Device Descriptor failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB30DeviceDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set Device Descriptor failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_DEVQUAL_DESCR, 0, (uint8_t *)CyFxUSBDeviceQualDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set Device Qualifier Descriptor failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_HS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBHSConfigDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set HS Descriptor failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_FS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBFSConfigDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set FS Configuration Descriptor failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBSSConfigDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set SS Configuration Descriptor failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_BOS_DESCR, 0, (uint8_t *)CyFxUSBBOSDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set BOS Descriptor failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 0, (uint8_t *)CyFxUSBStringLangIDDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set String Descriptor failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 1, (uint8_t *)CyFxUSBManufactureDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set String Descriptor failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 2, (uint8_t *)CyFxUSBProductDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set String Descriptor failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 3, (uint8_t *)CyFxUSBSerialNumberDscr);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Set String Descriptor failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Configure the USB endpoints as required. We configure for SuperSpeed and update at runtime as required. */
    CyFxMscApplnConfigEndpoints (CyTrue);

    /* Create the required DMA channels. */
    CyFxMscApplnDmaInit ();

    AppDebugPrint (4, "Calling USB Connect\r\n");

    /* Enable USB connection to host. SuperSpeed is not supported. */
    apiRetStatus = CyU3PConnectState (CyTrue, CyTrue);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "USB Connect failed, Error Code = %d\n",apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }
}

void
CyFxMscApplnSendCsw (
        void)
{
    CyU3PDmaBuffer_t dmaBuf;
    CyU3PReturnStatus_t status;

    /* Compute the CSW information. */
    glMscCswBuffer[0] = 'U';
    glMscCswBuffer[1] = 'S';
    glMscCswBuffer[2] = 'B';
    glMscCswBuffer[3] = 'S';
    CyU3PMemCopy (glMscCswBuffer + 4, glMscCbwBuffer + 4, 4);       /* Copy the tag. */
    *((uint32_t *)(glMscCswBuffer + 8)) = glMscResidue;
    glMscCswBuffer[12] = glMscCmdStatus;

    dmaBuf.buffer = glMscCswBuffer;
    dmaBuf.status = 0;
    dmaBuf.size   = 32;
    dmaBuf.count  = 13;
    status = CyU3PDmaChannelSetupSendBuffer (&glChHandleMscIn, &dmaBuf);
    if (status == CY_U3P_SUCCESS)
        glMscState = CY_FX_MSC_STATE_CSW;
    else
        CyU3PDmaChannelReset (&glChHandleMscIn);
}

static void
CyFxMscAppHandleMscTask (
        void)
{
    CyU3PReturnStatus_t status;
    CyU3PDmaBuffer_t    dmaBuf;

    uint32_t evMask = CY_FX_MSC_SETCONF_EVENT_FLAG | CY_FX_MSC_RESET_EVENT_FLAG | CY_FX_MSC_CBW_EVENT_FLAG |
        CY_FX_MSC_DATASENT_EVENT_FLAG | CY_FX_MSC_SIBCB_EVENT_FLAG | CY_FX_MSC_USB_SUSP_EVENT_FLAG |
        CY_FX_MSC_DATA_RECV_EVENT_FLAG | CY_FX_MSC_WR_COMMIT_EVENT_FLAG;
    uint32_t evStat;
    uint32_t cardStat;
    uint16_t wakeReason;
    uint32_t i;

    for (;;)
    {
        status = CyU3PEventGet (&glMscAppEvent, evMask, CYU3P_EVENT_OR_CLEAR, &evStat, CYU3P_WAIT_FOREVER);
        if (status == CY_U3P_SUCCESS)
        {
            if (evStat & CY_FX_MSC_RESET_EVENT_FLAG)
            {
                if (glMscState == CY_FX_MSC_STATE_DATA)
                {
                    /* Make sure any ongoing storage requests are aborted, and then verify that the cards can be
                       accessed. */
                    CyU3PSibAbortRequest (0);
                    status = CyU3PSibGetCardStatus (0, &cardStat);
                    AppDebugPrint (4, "S0 card status check: status=%d value=%x\r\n", status, cardStat);
                    CyU3PSibAbortRequest (1);
                    status = CyU3PSibGetCardStatus (1, &cardStat);
                    AppDebugPrint (4, "S1 card status check: status=%d value=%x\r\n", status, cardStat);
                }

                glMscState = CY_FX_MSC_STATE_INACTIVE;
            }

            if (evStat & CY_FX_MSC_SETCONF_EVENT_FLAG)
                glMscState = CY_FX_MSC_STATE_CBW;

            if (evStat & CY_FX_MSC_USB_SUSP_EVENT_FLAG)
            {
                /* Place FX3S in Low Power Suspend mode, with USB bus activity as the wakeup source. */
                status = CyU3PSysEnterSuspendMode (CY_U3P_SYS_USB_BUS_ACTVTY_WAKEUP_SRC, 0, &wakeReason);
                AppDebugPrint (4, "EnterSuspendMode returned %d, wakeup reason=%d\r\n", status, wakeReason);
            }

            if (evStat & CY_FX_MSC_WR_COMMIT_EVENT_FLAG)
            {
                /* Commit can only be done when the MSC state machine is idle. */
                if ((glMscState != CY_FX_MSC_STATE_DATA) && (glMscState != CY_FX_MSC_STATE_STATUS))
                {
                    for (i = 0; i < CY_FX_SIB_PORTS; i++)
                    {
                        CyU3PSibCommitReadWrite (i);
                    }

                    /* Clear the idle timer. */
                    glMscIdleTime = 0;
                }
                else
                {
                    /* Queue the commit action again. */
                    CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_WR_COMMIT_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }

            if (evStat & CY_FX_MSC_CBW_EVENT_FLAG)
            {
                if (glInPhaseError)
                {
                    /* Phase error: Send an error CSW and then stall both endpoints. */
                    glMscCmdStatus = 2;
                    CyU3PUsbStall (CY_FX_MSC_EP_BULK_IN, CyTrue, CyFalse);
                    CyU3PUsbStall (CY_FX_MSC_EP_BULK_OUT, CyTrue, CyFalse);
                }
                else
                {
                    glMscDriverBusy = CyTrue;
                    CyU3PUsbLPMDisable ();

                    /* Make sure the link is brought back to active state at this stage. */
                    if (CyU3PUsbGetSpeed () == CY_U3P_SUPER_SPEED)
                        CyU3PUsbSetLinkPowerState (CyU3PUsbLPM_U0);
                    else
                        CyU3PUsb2Resume ();

                    /* Parse CBW here. */
                    glMscState      = CY_FX_MSC_STATE_DATA;
                    CyFxMscApplnParseCbw ();
                }
            }
 
            if (evStat & CY_FX_MSC_DATA_RECV_EVENT_FLAG)
            {
                /* Data received for Mode Select command. */
                HandleModeSelectData ();
            }

            if (evStat & CY_FX_MSC_DATASENT_EVENT_FLAG)
            {
                /* If CSW has been sent, go to CBW state. */
                if (glMscState == CY_FX_MSC_STATE_CSW)
                    glMscState = CY_FX_MSC_STATE_CBW;

                /* If data has been sent, go to CSW state. */
                if (glMscState == CY_FX_MSC_STATE_DATA)
                {
                    /* If we sent less data than the host wanted, stall the endpoint afterwards. */
                    if ((glMscResidue != 0) && (glMscCmdStatus == 0))
                        CyU3PUsbStall (CY_FX_MSC_EP_BULK_IN, CyTrue, CyFalse);

                    glMscState = CY_FX_MSC_STATE_STATUS;
                }
            }

            if (evStat & CY_FX_MSC_SIBCB_EVENT_FLAG)
            {
                /* Stall the endpoint if any data transfer is expected. */
                if ((glMscCmdStatus) && (glMscResidue))
                {
                    /* The DMA channel has already been reset. */
                    if (glCmdDirection)
                        CyU3PUsbStall (CY_FX_MSC_EP_BULK_IN, CyTrue, CyFalse);
                    else
                        CyU3PUsbStall (CY_FX_MSC_EP_BULK_OUT, CyTrue, CyFalse);
                }
                else
                {
                    /* Wait for all of the data to be transferred to the host in the case of a read. */
                    if (glCmdDirection)
                        CyU3PDmaChannelWaitForCompletion (&glChHandleMscIn, CYU3P_WAIT_FOREVER);
                }

                /* Now move to the STATUS state to try and send the CSW. */
                glMscState = CY_FX_MSC_STATE_STATUS;
            }

            if (glMscState == CY_FX_MSC_STATE_CBW)
            {
                /* Re-enable Link power management now that the command has been handled. */
                glMscDriverBusy = CyFalse;
                CyU3PUsbLPMEnable ();

                dmaBuf.buffer = glMscCbwBuffer;
                dmaBuf.status = 0;
                dmaBuf.count  = 0;
                dmaBuf.size   = 64;     /* CBW is expected to be smaller than 64 bytes. */
                status = CyU3PDmaChannelSetupRecvBuffer (&glChHandleMscOut, &dmaBuf);
                if (status == CY_U3P_SUCCESS)
                {
                    glMscState = CY_FX_MSC_STATE_WAITING;

                    /* Make sure the link is brought back to active state at this stage. */
                    if (CyU3PUsbGetSpeed () == CY_U3P_SUPER_SPEED)
                        CyU3PUsbSetLinkPowerState (CyU3PUsbLPM_U0);
                    else
                        CyU3PUsb2Resume ();
                }
                else
                    CyU3PDmaChannelReset (&glChHandleMscOut);
            }

            if (glMscState == CY_FX_MSC_STATE_STATUS)
            {
                CyFxMscApplnSendCsw ();
            }
        }
    }
}

/* Callback that checks for MSC idle status and initiates write commit operation. */
static void
CyFxMscAppTimerCallback(
        uint32_t param)
{
    glMscIdleTime++;
    if (glMscIdleTime > CY_FX_MSC_APP_MAX_COMMIT_DELAY)
    {
        CyU3PEventSet (&glMscAppEvent, CY_FX_MSC_WR_COMMIT_EVENT_FLAG, CYU3P_EVENT_OR);
    }
}

/*
 * Entry function for the mscAppThread
 */
void
MscAppThread_Entry (
        uint32_t input)
{
    uint32_t txApiRetStatus;

    /* Create FX MSC events */
    txApiRetStatus = CyU3PEventCreate (&glMscAppEvent);
    if (txApiRetStatus != 0)
    {
        /* Error Handling */
        AppDebugPrint (4, "MSC Appln Create Event Failed, Error Code = %d\n", txApiRetStatus);
        CyFxAppErrorHandler (CY_U3P_ERROR_MEMORY_ERROR);
    }

    /* Create timer object used to commit writes when firmware is idle. */
    txApiRetStatus = CyU3PTimerCreate (&glMscAppTimer, CyFxMscAppTimerCallback, 0, 1000, 1000, CYU3P_AUTO_ACTIVATE);
    if (txApiRetStatus != 0)
    {
        /* Error Handling */
        AppDebugPrint (4, "MSC Appln Create Timer Failed, Error Code = %d\n", txApiRetStatus);
        CyFxAppErrorHandler (CY_U3P_ERROR_MEMORY_ERROR);
    }

    /* Initialize global variables. */
    CyFxMscAppInitVars ();

#ifdef LOG_ENABLE
    /* Initialize the Debug Module */
    CyFxMscApplnDebugInit ();
#endif

    CyFxMscApplnSibInit ();
    CyFxMscApplnInit ();

    /* Do the MSC application main work. */
    CyFxMscAppHandleMscTask ();
}

/*
 * Application define function which creates the threads. This is called from
 * the tx_application _define function.
 */
void
CyFxApplicationDefine (
        void)
{
    uint32_t retThrdCreate = CY_U3P_ERROR_MEMORY_ERROR;
    void *ptr;

    /* Allocate the memory for the threads */
    ptr = CyU3PMemAlloc (CY_FX_MSC_THREAD_STACK);

    if (ptr != 0)
    {
        /* Create the thread for the application */
        retThrdCreate = CyU3PThreadCreate (&mscAppThread,       /* MSC App Thread structure */
                "25:MSC Application",                           /* Thread ID and Thread name */
                MscAppThread_Entry,                             /* MSC App Thread Entry function */
                0,                                              /* No input parameter to thread */
                ptr,                                            /* Pointer to the allocated thread stack */
                CY_FX_MSC_THREAD_STACK,                         /* MSC App Thread stack size */
                CY_FX_MSC_THREAD_PRIORITY,                      /* MSC App Thread priority */
                CY_FX_MSC_THREAD_PRIORITY,                      /* MSC App Thread priority */
                CYU3P_NO_TIME_SLICE,                            /* No time slice for the application thread */
                CYU3P_AUTO_START                                /* Start the Thread immediately */
                );
    }

    /* Check the return code */
    if (retThrdCreate != 0)
    {
        /* Thread Creation failed with the error code retThrdCreate */

        /* Add custom recovery or debug actions here */

        /* Application cannot continue */
        /* Loop indefinitely */
        while(1);
    }
}

/*[]*/


