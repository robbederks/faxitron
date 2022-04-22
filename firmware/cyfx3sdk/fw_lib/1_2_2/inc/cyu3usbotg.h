/*
 ## Cypress USB 3.0 Platform header file (cyu3usbotg.h)
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

#ifndef _INCLUDED_CYU3USBOTG_H_
#define _INCLUDED_CYU3USBOTG_H_

#include <cyu3types.h>
#include <cyu3externcstart.h>

/*@@USB Management
  The FX3 device has a single USB port which can function as a user programmable
  USB device port or single USB host port. The USB On-The-Go controller supports
  USB OTG 2.0 specification.
  
  Description
  FX3 USB port in device mode of operation, it can  function as per USB 3.0
  specification and function at super speed, high speed and full speed depending
  upon the remote host attached. When configured a USB host, the port supports
  USB 2.0 operations at high speed, full speed and low speed according to the
  speed supported by the peripheral attached. In host mode of operation, only
  a single peripheral can be attached at a time and HUB devices are not supported.
  The OTG port on FX3 is also capable of ACA charger detection based on Battery
  Charging specification 1.1 or Motorola EMU specification depending on the
  selected configuration. The peripheral type detection is based on the ID pin
  state. The OTG port supports D+ pulsing based SRP and also supports host
  negotiation protocol.
 */

/*@@USB OTG Management
   Summary
   The FX3 device supports programmable USB OTG 2.0 implementation which supports
   device mode operations at USB-SS, USB-HS and USB-FS speeds and host mode
   operations at USB-HS, USB-FS and USB-LS speeds. The single USB port can
   function as a device or as a host depending on the type of peripheral attached.
   It can also perform ACA or Motorola EMU charger detection based on the ID pin
   state.
  */

/*@@USB OTG Data Types
  This section documents the data types defined and used by the
  USB OTG mode APIs.
 */

/*@@USB OTG Mode Functions
   The USB OTG APIs are used to configure the single USB port functionality
   and peripheral detection.
 */

/**************************************************************************
 ******************************* Data types *******************************
 **************************************************************************/

/* Summary
   OTG modes of operation.

   Description
   FX3 device has a single USB port which can function in multiple modes.
   It can act as a USB 2.0 host (no hub support) or as a USB 3.0 device.
   It can also route the lines to UART for car-kit mode of operation.
   This enumeration lists the various modes of operation allowed for the
   device. The default mode of operation is CY_U3P_OTG_MODE_DEVICE_ONLY.

   See Also
   CyU3POtgConfig_t
 */
typedef enum CyU3POtgMode_t
{
    CY_U3P_OTG_MODE_DEVICE_ONLY = 0,    /* USB port acts in device only mode. The ID pin value is ignored. 
                                           Charger detection is disabled. */
    CY_U3P_OTG_MODE_HOST_ONLY,          /* USB port acts in host only mode. The ID pin value is ignored.
                                           Charger detection is disabled. */
    CY_U3P_OTG_MODE_OTG,                /* USB port acts in OTG mode and identifies the mode of operation based on ID pin.
                                           This mode also does charger detection based on the ID pin. */
    CY_U3P_OTG_MODE_CARKIT_PPORT,       /* The D+ / D- lines are routed to the p-port pads PIB_CTL11 / PIB_CTL12 pins.
                                           Charger detection is disabled. */
    CY_U3P_OTG_MODE_CARKIT_UART,        /* The D+ / D- lines are routed to the FX3 UART lines. FX3 UART will not function
                                           in this mode. Charger detection is disabled. */
    CY_U3P_OTG_NUM_MODES                /* Number of OTG modes. */
} CyU3POtgMode_t;

/* Summary
   Various charger detection mechanism.

   Description
   FX3 device can detect chargers based on standard ACA requirements
   as well as Motorola EMU (enhanced mini USB) requirements. This
   enumeration lists the various charger detect modes available.
   Charger detection is based on OTG ID pin and so is available only
   in CY_U3P_OTG_MODE_OTG mode of operation.

   See Also
   CyU3POtgConfig_t
 */
typedef enum CyU3POtgChargerDetectMode_t
{
    CY_U3P_OTG_CHARGER_DETECT_ACA_MODE = 0, /* Charger detection is based on standard ACA charger mode.
                                               This is the default mode even when charger detection is not required. */
    CY_U3P_OTG_CHARGER_DETECT_MOT_EMU,      /* Charger detection is based on Motorola enhanced mini USB (EMU) requirements. */
    CY_U3P_OTG_CHARGER_DETECT_NUM_MODES     /* Number of charger detection modes. */
} CyU3POtgChargerDetectMode_t;

/* Summary
   OTG events.

   Description
   The enumeration lists the various OTG events. This is
   used to identify the type of callback invoked.

   See Also
   CyU3POtgEventCallback_t
 */
typedef enum CyU3POtgEvent_t
{
    CY_U3P_OTG_PERIPHERAL_CHANGE = 0,   /* The OTG peripheral attached to FX3 has changed / removed or a new
                                           peripheral got attached. The parameter to the event callback holds
                                           the type of peripheral attached. */
    CY_U3P_OTG_SRP_DETECT,              /* Remote device has initiated an SRP request. On receiving this
                                           request, the user is expected to turn on the VBUS. */
    CY_U3P_OTG_VBUS_VALID_CHANGE        /* Notifies that VBUS state has changed. The parameter is CyTrue if
                                           VBUS is valid and is CyFalse if VBUS is not valid. */

} CyU3POtgEvent_t;

/* Summary
   OTG peripheral types.

   Description
   The enumeration lists describes the various types of
   OTG peripherals that can be detected using FX3.

   See Also
   CyU3POtgEventCallback_t
 */
typedef enum CyU3POtgPeripheralType_t
{
    CY_U3P_OTG_TYPE_DISABLED = 0,       /* The OTG mode detection is disabled. */
    CY_U3P_OTG_TYPE_A_CABLE,            /* OTG A-type peripheral cable connected to FX3. FX3 is expected to behave
                                           as an OTG host. Since this is just detecting the  state of the ID-pin,
                                           it cannot be determined whether remote device has been attached. The FX3
                                           device can either enable the VBUS or wait for an SRP request depending
                                           upon the application use case. */
    CY_U3P_OTG_TYPE_B_CABLE,            /* OTG B-type peripheral cable connected to FX3. FX3 is expected to act as
                                           an OTG device by default. Since this is just detecting the state of the
                                           ID-pin, it cannot be determined whether a remote host is actually connected.
                                           The FX3 device is expected to wait for the VBUS to be valid. If this does
                                           not happen, then CyU3POtgSrpStart API should be invoked for initiating SRP. */
    CY_U3P_OTG_TYPE_ACA_A_CHG,          /* ACA RID_A_CHG charger. FX3 is expected to behave as OTG host.
                                           VBUS is already available from the charger. */
    CY_U3P_OTG_TYPE_ACA_B_CHG,          /* ACA RID_B_CHG charger. FX3 can charge and initiate SRP. The remote
                                           host is not asserting VBUS or is absent. */
    CY_U3P_OTG_TYPE_ACA_C_CHG,          /* ACA RID_C_CHG charger. FX3 device can charge and can connect but
                                           cannot initiate SRP as VBUS is already asserted by the remote host. */
    CY_U3P_OTG_TYPE_MOT_MPX200,         /* Motorola MPX.200 VPA */
    CY_U3P_OTG_TYPE_MOT_CHG,            /* Motorola non-intelligent charger. */
    CY_U3P_OTG_TYPE_MOT_MID,            /* Motorola mid rate charger. */
    CY_U3P_OTG_TYPE_MOT_FAST            /* Motorola fast charger. */

} CyU3POtgPeripheralType_t;

/* Summary
   OTG event callback function.

   Description
   The OTG event callback function returns various OTG events.
   This includes the OTG mode identification events and charger
   detect events.

   The input to the callback is dependant on the actual event.
   For CY_U3P_OTG_PERIPHERAL_CHANGE event, this returns the type of
   peripheral detected (CyU3POtgPeripheralType_t), for VBUS change
   interrupts it returns the status of VBUS and for all
   other events, the input is zero.

   See Also
   CyU3POtgEvent_t
   CyU3POtgConfig_t
 */
typedef void (*CyU3POtgEventCallback_t) (
        CyU3POtgEvent_t event,          /* OTG event type. */
        uint32_t input                  /* Input to the callback. */
        );

/* Summary
   OTG configuration information.

   Description
   The structure is given as parameter to the CyU3POtgStart function.
   The configuration of the USB port is done based on this.

   See Also
   CyU3POtgMode_t
   CyU3POtgEventCallback_t
   CyU3POtgStart
 */
typedef struct CyU3POtgConfig_t
{
    CyU3POtgMode_t otgMode;     /* USB port mode of operation. */
    CyU3POtgChargerDetectMode_t chargerMode; /* Charger detect mode. */
    CyU3POtgEventCallback_t cb; /* OTG event callback function. */

} CyU3POtgConfig_t;

/**************************************************************************
 ******************************* Macros ***********************************
 **************************************************************************/

/**************************************************************************
 ******************************* Global Variables *************************
 **************************************************************************/

/**************************************************************************
 ******************************* Functions ********************************
 **************************************************************************/

/* Summary
   This function returns whether the OTG module has been started.

   Description
   Since there can be various modes of USB operations this API returns
   whether CyU3POtgStart was invoked.

   Return value
   CyTrue - OTG module started
   CyFalse - OTG module stopped or not started

   See Also
   CyU3POtgStart
   CyU3POtgStop
 */
CyBool_t
CyU3POtgIsStarted (void);

/* Summary
   This function returns the type of attached USB peripheral.

   Description
   This function returns the type of USB peripheral attached. The call
   will return correct value only if there is a valid VBUS or VBATT.

   Return value
   CyU3POtgPeripheralType_t - Type of peripheral attached.

   See Also
   CyU3POtgPeripheralType_t
 */
CyU3POtgPeripheralType_t
CyU3POtgGetPeripheralType (void);

/* Summary
   This function returns the current selected OTG mode.

   Description
   This function returns the current configuration as done
   using the CyU3POtgStart call.

   Return value
   CyU3POtgMode_t - OTG mode selected.

   See Also
   CyU3POtgMode_t
   CyU3POtgStart
 */
CyU3POtgMode_t
CyU3POtgGetMode (void);

/* Summary
   This function returns whether the device mode of operation is allowed.

   Description
   The function determines the mode of operation and checks if the 
   device mode of operation can be initiated.

   Return value
   CyTrue - Device mode of operation allowed.
   CyFalse - Device mode of operation not allowed.

   See Also
   CyU3POtgStart
   CyU3PUsbStart
 */
CyBool_t
CyU3POtgIsDeviceMode (void);

/* Summary
   This function returns whether the host mode of operation is allowed.

   Description
   The function determines the mode of operation and checks if the 
   host mode of operation can be initiated.

   Return value
   CyTrue - Host mode of operation allowed.
   CyFalse - Host mode of operation not allowed.

   See Also
   CyU3POtgStart
   CyU3PUsbHostStart
 */
CyBool_t
CyU3POtgIsHostMode (void);

/* Summary
   This function initializes the OTG module.

   Description
   The function initializes the USB block to enable OTG detection. It does not
   actually start the module operation. Once the mode of operation is identified,
   the corresponding start API needs to be invoked. Carkit mode allows the USB 2.0
   D+ and D- lines to be routed to either p-port IOs or UART TX/RX IO lines.
   These IOs cannot be used for the normal function when the carkit mode is active.
   Also USB connection cannot be enabled when the carkit mode is active.
   For CY_U3P_OTG_MODE_CARKIT_PPORT to be used, the PIB_CTL11 / PIB_CTL12 pins 
   should not be used for GPIF configuration, For CY_U3P_OTG_MODE_CARKIT_UART to be
   used, the LPP UART cannot be used and IO matrix should be configured with useUart
   as CyFalse. The carkit mode can be disabled by invoking the CyU3POtgStop API.

   Return value
   CY_U3P_SUCCESS - When the configuration was successful.
   CY_U3P_ERROR_NOT_SUPPORTED - if the FX3 device in use does not support the OTG feature.
   CY_U3P_ERROR_NULL_POINTER - If any of the input parameter is NULL.
   CY_U3P_ERROR_ALREADY_STARTED - The module was already started.
   CY_U3P_ERROR_INVALID_SEQUENCE - The CY_U3P_OTG_MODE_DEVICE_ONLY mode was already started.
   CY_U3P_ERROR_BAD_ARGUMENT - Some input parameter(s) are invalid.

   See Also
   CyU3POtgConfig_t
   CyU3PUsbStart
   CyU3PUsbHostStart
   CyU3POtgStop
   CyU3POtgIsStarted
 */
extern CyU3PReturnStatus_t
CyU3POtgStart (
        CyU3POtgConfig_t *cfg   /* OTG configuration information. */
        );

/* Summary
   This function disables the OTG module.

   Description
   The function disables the USB block for OTG detection. It does not
   actually stop the module operation. The corresponding stop API has
   to be invoked before invoking this.

   Return value
   CY_U3P_SUCCESS - The API call was successful.
   CY_U3P_ERROR_NOT_STARTED - The module has not been started yet.
   CY_U3P_ERROR_INVALID_SEQUENCE - The device or host stack is running.
   
   See Also
   CyU3PUsbStop
   CyU3PUsbHostStop
   CyU3POtgStart
   CyU3POtgIsStarted
 */
extern CyU3PReturnStatus_t
CyU3POtgStop (void);

/* Maximum value in ms that can be used for the SRP repeat interval. */
#define CY_U3P_OTG_SRP_MAX_REPEAT_INTERVAL              (10000)

/* Summary
   Initiates an SRP request.

   Description
   This API is valid when FX3 is in device mode operation. This call will
   start the SRP request. If there is a remote host, it will respond by
   enabling the VBUS. When a valid VBUS is detected, the
   CY_U3P_OTG_VBUS_VALID_CHANGE event is generated. If there is no remote
   device, SRP will to be repeated periodically until a valid VBUS is
   detected or the peripheral type changes or CyU3POtgStpAbort API is
   called. The valid values of repeat period is in range of 500ms to 10s.
   SRP can be done only when the USB port is in device mode of operation
   and both device and host stack are disabled.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The module is not yet started.
   CY_U3P_ERROR_BAD_ARGUMENT - Input parameter is invalid.
   CY_U3P_ERROR_INVALID_SEQUENCE - Host / device stack is still active or not in the correct mode.

   See Also
   CyU3POtgStart
   CyU3POtgSrpAbort
 */
extern CyU3PReturnStatus_t
CyU3POtgSrpStart (
        uint32_t repeatInterval /* Repeat interval in ms. The valid range is from
                                   500ms to 10s. */
        );

/* Summary
   Aborts SRP request.

   Description
   Since SRP need to be repeated until a valid VBUS is detected, this call
   will abort the periodic SRP requests. This might be because no host is
   detected in the given time period.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_STARTED - The module is not started.
   CY_U3P_ERROR_INVALID_SEQUENCE - Wrong mode or host stack is still running.

   See Also
   CyU3POtgStart
   CyU3POtgSrpStart
 */
extern CyU3PReturnStatus_t
CyU3POtgSrpAbort (void);

/* Summary
   Requests remote host for the HNP process.

   Description
   This API is valid only in device mode of operation. To initiate a
   HNP, the device need to set the session request bit in the GetStatus
   bit. When using normal enumeration model, this needs to be done by
   the user. Since this is a role change request, the flag is not cleared
   unless CyU3PUsbStop or CyU3POtgRequestHnp (CyFalse) is invoked or if
   there is an OTG peripheral change. In case of a change in the OTG
   peripheral attached, the library will automatically clear the session
   request flag.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_SUPPORTED - Not in OTG mode.
   CY_U3P_ERROR_NOT_STARTED - Device stack is not started.

   See Also
   CyU3POtgStart
   CyU3POtgHnpEnable
 */
extern CyU3PReturnStatus_t
CyU3POtgRequestHnp (
        CyBool_t isEnable       /* Whether to set or clear the host request flag. */
        );

/* Summary
   Initiates a role change.

   Description
   This API must be called only when the USB bus is suspended. The input
   parameter determines whether to enable or disable HNP. This call should
   be called only when both host and device mode stacks are disabled.

   isEnable = CyTrue:
   If this API is called when in 'A' session, the API assumes that the
   remote device wants to get host role and will allow subsequent
   CyU3PUsbStart call to go through. If this API is called when in 'B'
   session, the API assumes that the remote host wants to relinqush control
   and will allow subsequent CyU3PUsbHostStart call to go through. It
   should be noted that the previous configuration has to be stopped
   before invoking this call.

   isEnable = CyFalse:
   This API allows the original role to be restored. The previous 
   configuration has to be stopped before invoking this call.

   The sequence to be followed when FX3 is default USB host:
   1. The FX3 hosts sends down the SetFeature request for a_hnp_support
      indicating to the remote device that the host can do a role change.
      This is required for only legacy peripherals.
   2. Remote devices requests for an HNP via the session request bit.
   3. Finish / abort all on-going transfers.
   4. FX3 host sends down the SetFeature request for b_hnp_enable.
      This is to indicate to the remote device that the host is ready
      to initiate a role change.
   5. FX3 should then invoke CyU3PUsbHostPortSuspend () to suspend the port.
   6. Once the port is suspended, FX3 should wait for the remote device
      to get disconnected (CY_U3P_USB_HOST_EVENT_DISCONNECT host event).
      If this does not happen within the spec specified time, then the
      host can either resume host mode operation or it can end the session.
   7. If the remote device got disconnected, FX3 should then invoke
      CyU3PUsbHostStop () to stop the host mode of operation.
   8. Enable role change by calling CyU3POtgHnpEnable (CyTrue).
   9. Invoke CyU3PUsbStart () to start the device mode stack.

   The sequence to be followed when FX3 is default USB device:
   1. When FX3 requires a role change, it should first respond to a 
      OTG GetStatus request with the session request flag set.
      When using fast enumeration mode, this can be done using 
      CyU3POtgRequestHnp (CyTrue) call.
   2. FX3 should wait until the remote host enables HNP by issuing
      SetFeature request for b_hnp_enable.
   3. Once the SetFeature request is received, FX3 should wait for
      the bus to be suspended within the spec specified time. If this
      does not happen, then the device can either disconnect and end
      session or it can remain connected until the remote host resumes
      operation.
   4. If the bus is suspended, then disconnect from the USB bus by
      invoking CyU3PConnectState (CyFalse, CyFalse).
   5. Now FX3 should stop the device mode stack by cleaning up all DMA
      channels and then finally invoking CyU3PUsbStop ().
   6. FX3 should then start the role change by invoking
      CyU3POtgHnpEnable (CyTrue).
   7. FX3 should then start the host mode operation by invoking
      CyU3PUsbHostStart ().
   
   It should be noted that HNP is role reversal and it has to
   be explicitly disabled. It will get disabled by only on 
   CyU3POtgStop or CyU3POtgHnpEnable (CyFalse) calls or or if there is an
   OTG peripheral change. In case of a change in the OTG peripheral attached,
   the library will automatically clear the session request flag.

   Return value
   CY_U3P_SUCCESS - The call was successful.
   CY_U3P_ERROR_NOT_SUPPORTED - Not in OTG mode.
   CY_U3P_ERROR_NOT_STARTED - The module is not stated.
   CY_U3P_ERROR_INVALID_SEQUENCE - Device or host stack is still active.

   See Also
   CyU3POtgStart
   CyU3PUsbStart
   CyU3PUsbStop
   CyU3PUsbHostStart
   CyU3PUsbHostStop
   CyU3POtgRequestHnp
 */
extern CyU3PReturnStatus_t
CyU3POtgHnpEnable (CyBool_t isEnable);

/* Summary
   The function checks if a role reversal mode is active or not.

   Description
   The API just returns the current state of HNP. This call just
   retrieves the previously set state.

   Return value
   CyTrue - HNP role change is active.
   CyFalse - HNP role change is not active.

   See Also
   CyU3POtgStart
   CyU3POtgEventCallback_t
 */
extern CyBool_t
CyU3POtgIsHnpEnabled (
        void);

/* Summary
   The function checks if a valid VBUS is available.

   Description
   The API can be used to determine the state of the VBUS. Since
   USB module can function properly only with the VBUS enabled,
   this can be used to determine when to start the device / host
   stacks. The VBUS state change can be received through the
   registered OTG event callback function (CyU3POtgEventCallback_t)
   as well.

   Return value
   CyTrue - VBUS is valid.
   CyFalse - A valid VBUS is not available.

   See Also
   CyU3POtgStart
   CyU3POtgEventCallback_t
 */
extern CyBool_t
CyU3POtgIsVBusValid (
        void);

#include <cyu3externcend.h>

#endif /* _INCLUDED_CYU3USBOTG_H_ */

/*[]*/
