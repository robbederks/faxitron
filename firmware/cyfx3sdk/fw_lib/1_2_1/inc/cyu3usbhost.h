/*
 ## Cypress USB 3.0 Platform header file (cyu3usbhost.h)
 ## ===========================
 ##
 ##  Copyright Cypress Semiconductor Corporation, 2010-2011,
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

#ifndef _INCLUDED_CYU3P_USB_HOST_H_
#define _INCLUDED_CYU3P_USB_HOST_H_

#include <cyu3types.h>
#include <cyu3usbconst.h>
#include <cyu3externcstart.h>

/*@@USB Host Management
  Summary
  The FX3 device supports programmable USB host implementation for a single 
  USB host port at USB-HS, USB-FS and USB-LS speeds. The control pipe as well
  as the data pipes to be used can be configured through a set of USB host mode
  APIs. The USB host mode APIs also provide the capability to manage the host port.
 */

/*@@USB Host Data Types
  This section documents the data types defined and used by the
  USB device mode APIs.
 */

/*@@USB Host Mode Functions
   The USB host APIs are used to configure the USB host functionality and to
   perform USB data transfers.
 */

/**************************************************************************
 ******************************* Macros ***********************************
 **************************************************************************/

/**************************************************************************
 ******************************* Data Types *******************************
 **************************************************************************/

/*Summary
  Mode of transfer.

  Description
  Depending upon the type of endpoint, the transfer can be normal or setup.
  A setup packet is send only on EP0. This is a parameter to the SetXfer call.

  See Also
  * CyU3PUsbHostEpSetXfer
  */
typedef enum CyU3PUsbHostEpXferType_t
{
    CY_U3P_USB_HOST_EPXFER_NORMAL = 0,          /* Normal transfer. All non-EP0 tranfers. */
    CY_U3P_USB_HOST_EPXFER_SETUP_OUT_DATA,      /* EP0 setup packet with OUT data phase. */
    CY_U3P_USB_HOST_EPXFER_SETUP_IN_DATA,       /* EP0 setup packet with IN data phase. */
    CY_U3P_USB_HOST_EPXFER_SETUP_NO_DATA        /* EP0 setup packet with no data phase. */

} CyU3PUsbHostEpXferType_t;

/*Summary
  Speed of operation for the usb host port.

  Description
  The usb host port can function in low, full and high speeds
  according to the capability of the device attached. This 
  enumeration gives the various speed of operation.

  See Also
  * CyU3PUsbHostGetPortStatus
  */
typedef enum CyU3PUsbHostOpSpeed_t
{
    CY_U3P_USB_HOST_LOW_SPEED = 0,      /* Host port is operating in low speed mode. */
    CY_U3P_USB_HOST_FULL_SPEED,         /* Host port is operating in full speed mode. */
    CY_U3P_USB_HOST_HIGH_SPEED          /* Host port is operating in high speed mode. */

} CyU3PUsbHostOpSpeed_t;

/* Summary
   Different host mode events.

   Description
   The different types of events reported to the application
   via the event callback function.

   See Also
   CyU3PUsbHostEventCb_t
 */
typedef enum CyU3PUsbHostEventType_t
{
    CY_U3P_USB_HOST_EVENT_CONNECT = 0,           /* USB Connect event. */
    CY_U3P_USB_HOST_EVENT_DISCONNECT             /* USB Disconnect event. */
} CyU3PUsbHostEventType_t;


/* Summary
   Host mode endpoint status.

   Description
   The endpoint status returned by the schduler is a 32 bit value
   with different fields listing the status of the transfer on the
   endpoint. The fields are described in the macros listed.

   See Also
   CyU3PUsbHostXferCb_t
 */
typedef uint32_t CyU3PUsbHostEpStatus_t;

/* Macro definitions for the fields in CyU3PUsbHostEpStatus_t data type. */
#define CY_U3P_USB_HOST_EPS_EPNUM_POS              (0)          /* Position of EP number field. */
#define CY_U3P_USB_HOST_EPS_EPNUM_MASK             (0x0000000F) /* Mask for the EP number field. */
#define CY_U3P_USB_HOST_EPS_EPDIR                  (0x00000010) /* EP direction: 1 - OUT, 0 - IN. */
#define CY_U3P_USB_HOST_EPS_ACTIVE                 (0x00000020) /* Whether the EP is still active. */
#define CY_U3P_USB_HOST_EPS_HALT                   (0x00000040) /* Set if EP is halted (stalled). */
#define CY_U3P_USB_HOST_EPS_OVER_UNDER_RUN         (0x00000080) /* Set if EP buffer had an overrun or underrun happpened. */
#define CY_U3P_USB_HOST_EPS_BABBLE                 (0x00000100) /* Set if a babble was detected during transfer. */
#define CY_U3P_USB_HOST_EPS_XACT_ERROR             (0x00000200) /* Set if there was a transfer error of any kind. */
#define CY_U3P_USB_HOST_EPS_PING                   (0x00000400) /* Set if there was a ping. */
#define CY_U3P_USB_HOST_EPS_PHY_ERROR              (0x00000800) /* Set if there was a PHY error during transfer. */
#define CY_U3P_USB_HOST_EPS_PID_ERROR              (0x00001000) /* Set if there was a PID error during transfer. */
#define CY_U3P_USB_HOST_EPS_TIMEOUT_ERROR          (0x00002000) /* Set if there was a timeout error during transfer. */
#define CY_U3P_USB_HOST_EPS_ISO_ORUN_ERROR         (0x00004000) /* Set if there was an ISO overrun error. */
#define CY_U3P_USB_HOST_EPS_IOC_INT                (0x00008000) /* Set if there was an IOC interrupt (high speed only. */
#define CY_U3P_USB_HOST_EPS_BYTE_COUNT_POS         (16)         /* Position for remaining bytes to be transferred. */
#define CY_U3P_USB_HOST_EPS_BYTE_COUNT_MASK        (0xFFFF0000) /* Mask for remaining bytes to be transferred. */

/* Summary
   Host mode port status information.

   Description
   The port status returned by the library is a 16 bit value
   with different fields. The fields are described in
   the macros listed.

   See Also
   CyU3PUsbHostGetPortStatus
 */
typedef uint16_t CyU3PUsbHostPortStatus_t;

/* Macro definitions for the fields in CyU3PUsbHostPortStatus_t data type. */
#define CY_U3P_USB_HOST_PORT_STAT_CONNECTED        (0x0001)     /* Set if a down-stream peripheral is connected. */
#define CY_U3P_USB_HOST_PORT_STAT_ENABLED          (0x0002)     /* Set if the port is enable using CyU3PUsbHostPortEnable API. */
#define CY_U3P_USB_HOST_PORT_STAT_ACTIVE           (0x0003)     /* Mask to identify if the port is active. The port is active
                                                                   if port is enabled and connected but not suspended. */
#define CY_U3P_USB_HOST_PORT_STAT_SUSPENDED        (0x0004)     /* Set if the port has been suspended. */

/* Summary
   Host mode endpoint configuration structure.

   Description
   The structure holds the information for configuring the endpoint.

   See Also
   CyU3PUsbHostEpAdd
 */
typedef struct CyU3PUsbHostEpConfig_t
{
    CyU3PUsbEpType_t type;      /* Type of endpoint to be created. */
    uint8_t mult;               /* Number of packets per micro-frame. This should be 1
                                   for bulk and control and 1, 2, or 3 as ISO / interrupt
                                   requirements. The only valid values are 1, 2 and 3. */
    uint16_t maxPktSize;        /* The maximum packet size that can be received
                                   from the EP. Valid values are as per the USB 2.0 spec. */
    uint8_t pollingRate;        /* Rate at which the endpoint has to be polled in ms. A zero
                                   will indicate that polling will be done every micro-frame
                                   and any other value will poll at a rate specified. It should
                                   be noted that pollingRate is valid only when the request itself
                                   is larger than a single packet. For example if every packet
                                   to be received is queued as a seperate request, then the polling
                                   rate has to be manually maintained by the application. This is
                                   valid only for the synchronous EP. For asynchronous EPs this
                                   value should be zero. */
    uint16_t fullPktSize;       /* This is used for DMA packet boundary identification. If the DMA
                                   buffer allocated is larger than the maxPktSize specified, this
                                   field determines when a buffer is wrapped up. A DMA buffer is 
                                   wrapped up by hardware when it sees a SLP or ZLP. So as long as
                                   data received is a multiple of fullPktSize, the buffer is not 
                                   wrapped up. fullPktSize cannot be smaller than the maxPktSize. */
    CyBool_t isStreamMode;      /* Enable stream mode. This means that the EP is always active. This
                                   is valid only for an IN EP. It should be CyFalse for EP0 as well
                                   as for OUT EPs. When the flag is CyTrue, an IN EP will send IN token
                                   and collect data whenever there is free buffer. */
} CyU3PUsbHostEpConfig_t;

/* Summary
   Host mode event callback function.

   Description
   The event callback function is registered during start of
   the host stack. The function is invoked by the library on
   getting events like peripheral connect / disconnect.

   See Also
   CyU3PUsbHostStart
 */
typedef void (*CyU3PUsbHostEventCb_t)(
        CyU3PUsbHostEventType_t evType, /* The event type. */
        uint32_t                evData  /* Event specific data. */
        );

/* Summary
   Host mode endpoint transfer complete callback function.

   Description
   The transfer callback function is registered during start of
   the host stack. The function is invoked by the library on
   completion of transfer.

   See Also
   CyU3PUsbHostStart
   CyU3PUsbHostEpSetXfer
 */
typedef void (*CyU3PUsbHostXferCb_t)(
        uint8_t                ep,      /* The endpoint number. */
        CyU3PUsbHostEpStatus_t epStatus /* Endpoint status. */
        );

/* Summary
   Host mode configuration information.

   Description
   The host mode stack can be started with the following
   configurations. These options cannot be changed dynamically.
   The stack has to be stopped and started for changing any
   parameters.

   See Also
   CyU3PUsbHostStart
 */
typedef struct CyU3PUsbHostConfig_t
{
    CyBool_t ep0LowLevelControl;        /* Whether to enable EP0 low level control. If CyFalse, the EP0
                                           DMA is handled by firmware. Only CyU3PUsbHostSendSetupRqt
                                           need to be called. If CyTrue, all DMA paths need to be handled
                                           by application. This allows fine control over setup, data 
                                           and status phase. This should be used only if the EP0 data
                                           need to be routed to a different path. It is advised to leave
                                           this field as CyFalse. It should also be noted that the 
                                           maxPktSize and fullPktSize should be the same if the flag is
                                           CyTrue. */
    CyU3PUsbHostEventCb_t eventCb;      /* Event callback function for USB host stack. */
    CyU3PUsbHostXferCb_t  xferCb;       /* EP transfer completion callback for USB host stack. */   
} CyU3PUsbHostConfig_t;

/**************************************************************************
 ********************** Global variable declarations **********************
 **************************************************************************/

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/* Summary
   This function returns whether the USB host module has been started.

   Description
   Since there can be various modes of USB operations this API returns
   whether CyU3PUsbHostStart was invoked.

   Return value
   CyTrue - USB host module started
   CyFalse - USB host module stopped or not started

   See Also
   CyU3PUsbHostStart
   CyU3PUsbHostStop
 */
CyBool_t
CyU3PUsbHostIsStarted (void);

/* Summary
   This function initializes the USB 2.0 host stack.

   Description
   This function enables the USB block and configures it to
   function as USB host. The configuration parameters cannot
   be changed unless the stack is stopped and re-started.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_SUPPORTED - if the current FX3 device does not support the USB 2.0 host.
   CY_U3P_ERROR_NULL_POINTER - If NULL pointer is passed in as parameter.
   CY_U3P_ERROR_ALREADY_STARTED - The host stack is already running.
   CY_U3P_ERROR_INVALID_SEQUENCE - FX3 is in wrong OTG mode or USB device stack is running.

   See Also
   CyU3PUsbHostConfig_t
   CyU3PUsbHostStop
   CyU3PUsbHostIsStarted
 */
CyU3PReturnStatus_t
CyU3PUsbHostStart (
        CyU3PUsbHostConfig_t *hostCfg   /* Pointer to the host configuration information. */
        );

/* Summary
   This function de-inits the USB host stack.

   Description
   This function disables the USB host mode operation.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The host mode stack is not running.

   See Also
   CyU3PUsbHostStart
   CyU3PUsbHostIsStarted
 */
CyU3PReturnStatus_t
CyU3PUsbHostStop (
        void
        );

/* Summary
   This function enables the USB host port.

   Description
   When the host stack is started, the host mode port is not
   enabled completely. Only peripheral detection is turned on.
   USB 3.0 platform devices has only a single usb host port
   which can support only a single peripheral device. Hubs are
   not supported. This function is expected to be invoked 
   by the application when a downstream peripheral is detected.
   This function enables the port and sets it to the correct
   speed of operation. When a down-stream peripheral gets 
   disconnected, the port automatically gets disabled, and port
   needs to be explicitly enabled every time a connection is
   detected. The call can be made only when there is a
   downstream peripheral attached.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The host stack is not running.
   CY_U3P_ERROR_ALREADY_STARTED - The port is already enabled.
   CY_U3P_ERROR_FAILURE - No downstream peripheral attached.

   See Also
   CyU3PUsbHostPortDisable
   CyU3PUsbHostGetPortStatus
 */
CyU3PReturnStatus_t
CyU3PUsbHostPortEnable (
        void
        );

/* Summary
   Disable the USB host port.

   Description
   The function will disable the host port activity. The port will
   no-longer be able to function or do data transfers. The port
   will still be able to do peripheral detection.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The port is not enabled.

   See Also
   CyU3PUsbHostPortEnable
   CyU3PUsbHostGetPortStatus
 */
CyU3PReturnStatus_t
CyU3PUsbHostPortDisable (
        void
        );

/* Summary
   This function retreives the current port status.

   Description
   This function will just retrive the current state of the port
   and the current speed of operation for the port.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The host stack is not running.

   See Also
   CyU3PUsbHostPortEnable
   CyU3PUsbHostPortDisable
 */
CyU3PReturnStatus_t
CyU3PUsbHostGetPortStatus (
        CyU3PUsbHostPortStatus_t *portStatus, /* Current port status. */
        CyU3PUsbHostOpSpeed_t    *portSpeed   /* Current port speed.  */
        );

/* Summary
   This function resets the USB host port.

   Description
   A port reset is nothing but a CyU3PUsbHostPortDisable () call
   followed by a CyU3PUsbHostPortEnable () call. The port reset
   shall reset the host port's state machine and re-enumeration
   can take place.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The host stack is not running.

   See Also
   CyU3PUsbHostPortEnable
   CyU3PUsbHostPortDisable
   CyU3PUsbHostGetPortStatus
 */
CyU3PReturnStatus_t
CyU3PUsbHostPortReset (
        void
        );

/* Summary
   This function suspends the USB host port.

   Description
   The port shall be suspended and will resume on 
   subsequent resume call. It should be noted that
   remote wakeup is not supported and the host should
   wakeup the device whenever required.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The port is not active.
   CY_U3P_ERROR_INVALID_SEQUENCE - There is active on-going transfer..

   See Also
   CyU3PUsbHostPortResume
   CyU3PUsbHostGetPortStatus
 */
CyU3PReturnStatus_t
CyU3PUsbHostPortSuspend (
        void
        );

/* Summary
   This function resusmes the previously suspended USB host port.

   Description
   The port resumes operation at this point. The function will
   succeed only if the bus is suspended.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The port is not enabled or device got disconnected.
   CY_U3P_ERROR_INVALID_SEQUENCE - The port is not suspended.
   CY_U3P_ERROR_NOT_SUPPORTED - TODO Not implemented.

   See Also
   CyU3PUsbHostPortSuspend
   CyU3PUsbHostGetPortStatus
 */
CyU3PReturnStatus_t
CyU3PUsbHostPortResume (
        void
        );

/* Summary
   This function gets the current frame number to be used.

   Description
   This function call is not synchronized with the scheduler
   and will return the frame number at the time of call. This
   can be used to time the synchronous transfers.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NULL_POINTER - The input pointer was NULL.
   CY_U3P_ERROR_NOT_STARTED - The host port is not enabled.

   See Also
   CyU3PUsbHostStart
   CyU3PUsbHostPortEnable
 */
CyU3PReturnStatus_t
CyU3PUsbHostGetFrameNumber (
        uint32_t *frameNumber   /* Pointer to load the frame number. */
        );

/* Summary
   This function gets the current downstream peripheral address.

   Description
   The function returns the device address used by the host
   controller to address the down-stream peripheral. This call
   simply returns the address previously set. The address gets
   reset to zero if there is a CyU3PUsbHostPortDisable call or
   if there is a disconnect.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NULL_POINTER - The pointer provided is NULL.
   CY_U3P_ERROR_NOT_STARTED - The host port is not enabled.

   See Also
   CyU3PUsbHostSetDeviceAddress
 */
CyU3PReturnStatus_t
CyU3PUsbHostGetDeviceAddress (
        uint8_t *devAddr        /* Pointer to load the current
                                   device address. */
        );

/* Summary
   This function sets the current downstream peripheral address.

   Description
   The function sets the device address to be used by the host
   controller to address the down-stream peripheral. On enabling
   the port, the device address set to zero by default. The
   address should be changed by the application accordingly after
   a successful SET_ADDRESS setup request to the device.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_BAD_ARGUMENT - The address parameter is invalid.
   CY_U3P_ERROR_NOT_STARTED - The host port is not enabled.

   See Also
   CyU3PUsbHostGetDeviceAddress
 */
CyU3PReturnStatus_t
CyU3PUsbHostSetDeviceAddress (
        uint8_t devAddr         /* Device address to be set. */
        );

/* Summary
   This function adds an endpoint to the scheduler.

   Description
   Depending upon the endpoints on the downstream peripheral,
   the corresponding enpoint must be added to the scheduler for
   enabling data transfer. The endpoint configuration parameter
   depends on the endpoint on the down-stream peripheral.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NULL_POINTER - The input pointer was NULL.
   CY_U3P_ERROR_NOT_STARTED - The port is not enabled.
   CY_U3P_ERROR_ALREADY_STARTED - The endpoint is already added.
   CY_U3P_ERROR_BAD_ARGUMENT - One or more of the input parameter field is invalid.

   See Also
   CyU3PUsbHostEpRemove
 */
CyU3PReturnStatus_t
CyU3PUsbHostEpAdd (
        uint8_t ep,                 /* Endpoint to be created. */
        CyU3PUsbHostEpConfig_t *cfg /* Endpoint configuration
                                       information. */
        );

/* Summary
   This function removes and endpoint from the scheduler.

   Description
   This function must be called when the host application is done
   with using an endpoint or if the type of endpoint has to be 
   changed.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_BAD_ARGUMENT - The endpoint number is invalid.
   CY_U3P_ERROR_NOT_STARTED - The port is not enabled.
   CY_U3P_ERROR_NOT_CONFIGURED - The endpoint is not yet added.

   See Also
   CyU3PUsbHostEpAdd
 */
CyU3PReturnStatus_t
CyU3PUsbHostEpRemove (
        uint8_t ep             /* Endpoint to be removed from scheduler. */
        );

/* Summary
    This function resets an endpoint state.

   Description
   The function flushes the internal buffers and resets the data toggle.
   It should be called only when there is no active transfer on the endpoint.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The port is not enabled.
   CY_U3P_ERROR_BAD_ARGUMENT - The endpoint number is invalid.
   CY_U3P_ERROR_NOT_CONFIGURED - The endpoint is not added.
   CY_U3P_ERROR_INVALID_SEQUENCE - The endpoint is active. Abort before invoking this call.

   See Also
   CyU3PUsbHostEpAdd
 */
CyU3PReturnStatus_t
CyU3PUsbHostEpReset (
        uint8_t ep             /* Endpoint to be reset. */
        );

/* Summary
   This function does EP0 setup transfer handling.

   Description
   This API is valid only if ep0LowLevelControl is CyFalse.
   The API initiates the transfer of setup packet, but does
   not wait. The setup, data and status phases of the transfer
   will be handled by the library and the transfer complete
   callback shall be invoked for EP0 in the end.

   The data required for the transfer (incase of OUT data)
   and buffer required for the transfer (incase of IN data)
   should be available before starting the transfer.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_SUPPORTED - EP0 Low level control enabled.
   CY_U3P_ERROR_NOT_STARTED - The port is not enabled.
   CY_U3P_ERROR_INVALID_SEQUENCE - The endpoint is already active.
   CY_U3P_ERROR_NOT_CONFIGURED - The endpoint is not added.
   CY_U3P_ERROR_DMA_FAILURE - Error in setting internal DMA channels.

   See Also
   CyU3PUsbHostStart
   CyU3PUsbHostEpAbort
 */
CyU3PReturnStatus_t
CyU3PUsbHostSendSetupRqt (
        uint8_t *setupPkt,     /* Pointer to 8 byte setup packet. */
        uint8_t *buf_p         /* Buffer for data phase. The buffer must be
                                  capable of doing DMA transfers and should be
                                  pre-allocated to size as per the setup packet. */
        );

/* Summary
   This function starts a transfer for all non-EP0 endpoints
   and sets the transfer parameters for EP0. This function is
   relavant for EP0 only if the ep0LowLevelControl is set to CyTrue
   while starting the host stack. If the API is invoked for EP0
   when the ep0LowLevelControl is CyFalse, then the behaviour is
   undefined as this API will be used by the library for setting up
   the EP0 transfers.

   Description
   This function will setup the transfer and enable the endpoint
   for all EPs except for EP0. When EP0 low level control is enabled,
   the setup packet must be queued on the DMA pipe before it can be
   enabled. EP0 has two data pipes: one EP0_INGRESS and one EP0_EGRESS.
   The setup packet needs to be queued on the egress pipe. There is only
   one request required for both the ingress and egress pipes. The type
   of transfer is decided by the type parameter. Once the setup packet
   is queued on the DMA pipe EP0 needs to be explicitly turned on by
   invoking CyU3PUsbHostEp0BeginXfer.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The port is not enabled.
   CY_U3P_ERROR_BAD_ARGUMENT - One or more input parameters is invalid.
   CY_U3P_ERROR_NOT_CONFIGURED - The endpoint is not added.
   CY_U3P_ERROR_INVALID_SEQUENCE - The endpoint is already active.

   See Also
   CyU3PUsbHostEp0BeginXfer
   CyU3PUsbHostEpAbort
 */
CyU3PReturnStatus_t
CyU3PUsbHostEpSetXfer (
        uint8_t ep,            /* Endpoint to configure. */
        CyU3PUsbHostEpXferType_t type,
                               /* Type of transfer. This is meaningful only
                                * for EP0. */
        uint32_t count         /* Size of data to be transferred. */
        );

/* Summary
   This function enables and starts transfer on EP0. The function is
   relavant only if the ep0LowLevelControl is set to CyTrue
   while starting the host stack. If the API is invoked for EP0
   when the ep0LowLevelControl is CyFalse, then the behaviour is
   undefined as this API will be used by the library for setting up
   the EP0 transfers.

   Description
   This function and enable the endpoint and start the transfer
   for EP0. The setup packet must be queued on EP0 egress socket
   before invoking this function. Also CyU3PUsbHostEpSetXfer must
   be invoked prior to this to setup the transfer paramters.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The port is not enabled.
   CY_U3P_ERROR_INVALID_SEQUENCE - The endpoint is not configured or the transfer is not setup.

   See Also
   CyU3PUsbHostEpSetXfer
   CyU3PUsbHostEpAbort
 */
CyU3PReturnStatus_t
CyU3PUsbHostEp0BeginXfer (
        void);

/* Summary
   This function aborts pending transfers on selected endpoint.

   Description
   The function deactivates the endpoint for the ongoing data
   transfer. This does not abort the DMA channels and it has to be
   done explictly.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The port is not enabled.
   CY_U3P_ERROR_BAD_ARGUMENT - The endpoint number is invalid.
   CY_U3P_ERROR_NOT_CONFIGURED - The endpoint is not added.

   See Also
   CyU3PUsbHostEpSetXfer
   CyU3PUsbHostEp0BeginXfer
 */
CyU3PReturnStatus_t
CyU3PUsbHostEpAbort (
        uint8_t ep             /* Endpoint to abort. */
        );

/* Summary
   This function waits for the current endpoint transfer to complete.

   Description
   The function shall wait for the transfer to complete of get aborted.
   If this does not happen within the timeout specified, it returns
   with a timeout error. This does not mean that the transfer failed.
   It just means that the transfer did not complete within the specified
   timeout. The function can be called again or the user can wait for
   the transfer complete callback.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The port is not enabled.
   CY_U3P_ERROR_BAD_ARGUMENT - The endpoint number is invalid.
   CY_U3P_ERROR_NOT_CONFIGURED - The endpoint is not added.
   CY_U3P_ERROR_INVALID_SEQUENCE - No active transfer.
   CY_U3P_ERROR_TIMEOUT - The transfer is not completed in the given timeout.
   CY_U3P_ERROR_STALLED - The transfer was stalled by the remote device.

   See Also
   CyU3PUsbHostEpSetXfer
   CyU3PUsbHostEp0BeginXfer
 */
CyU3PReturnStatus_t
CyU3PUsbHostEpWaitForCompletion (
        uint8_t ep,             /* Endpoint to wait on. */
        CyU3PUsbHostEpStatus_t *epStatus,
                                /* Endpoint status after transfer.
                                   If this is NULL, no status is returned. */
        uint32_t waitOption     /* Wait option for the function. */
        );

#include <cyu3externcend.h>

#endif /* _INCLUDED_CYU3P_USB_HOST_H_ */

/*[]*/

