
#include <stdbool.h>

#include "cyfxusbdebug.h"
#include "cyu3dma.h"
#include "cyu3error.h"
#include "cyu3os.h"
#include "cyu3system.h"
#include "cyu3usb.h"

CyU3PThread applnThread;          /* Application thread structure */
CyU3PDmaChannel glChHandleBulkLp; /* DMA Channel handle */
bool glIsApplnActive = false;     /* Whether the application is active or not. */

void CyFxAppErrorHandler(CyU3PReturnStatus_t apiRetStatus) {
  // Infinite loop
  while (true) {
    CyU3PThreadSleep(100);
  }
}

/* This function starts the logger application. This is called
 * when a SET_CONF event is received from the USB host. The endpoints
 * are configured and the DMA pipe is setup in this function. */
void CyFxApplnStart(void) {
  uint16_t size = 0;
  CyU3PEpConfig_t epCfg;
  CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;
  CyU3PUSBSpeed_t usbSpeed = CyU3PUsbGetSpeed();

  /* First identify the usb speed. Once that is identified,
   * create a DMA channel and start the transfer on this. */

  /* Based on the Bus Speed configure the endpoint packet size */
  switch (usbSpeed) {
    case CY_U3P_FULL_SPEED:
      size = 64;
      break;

    case CY_U3P_HIGH_SPEED:
      size = 128;
      break;

    case CY_U3P_SUPER_SPEED:
      size = 256;
      break;

    default:
      CyFxAppErrorHandler(CY_U3P_ERROR_FAILURE);
      break;
  }

  CyU3PMemSet((uint8_t *)&epCfg, 0, sizeof(epCfg));
  epCfg.enable = true;
  epCfg.epType = CY_U3P_USB_EP_INTR;
  epCfg.burstLen = 1;
  epCfg.streams = 0;
  epCfg.pcktSize = size;

  /* Consumer endpoint configuration */
  apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_DEBUG, &epCfg);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* Flush the Endpoint memory */
  CyU3PUsbFlushEp(CY_FX_EP_DEBUG);

  apiRetStatus = CyU3PDebugInit(CY_FX_EP_DEBUG_SOCKET, 8);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* Update the status flag. */
  glIsApplnActive = true;
}

/* This function stops the logger application. This shall be called whenever
 * a RESET or DISCONNECT event is received from the USB host. The endpoints are
 * disabled and the DMA pipe is destroyed by this function. */
void CyFxApplnStop(
    void) {
  CyU3PEpConfig_t epCfg;
  CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

  /* Update the flag. */
  glIsApplnActive = false;

  /* Disable the debug log mechanism. */
  CyU3PDebugDeInit();

  /* Flush the endpoint memory */
  CyU3PUsbFlushEp(CY_FX_EP_DEBUG);

  /* Disable endpoints. */
  CyU3PMemSet((uint8_t *)&epCfg, 0, sizeof(epCfg));
  epCfg.enable = false;

  /* Consumer endpoint configuration. */
  apiRetStatus = CyU3PSetEpConfig(CY_FX_EP_DEBUG, &epCfg);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }
}

/* Callback to handle the USB setup requests. */
CyBool_t CyFxApplnUSBSetupCB(uint32_t setupdat0, uint32_t setupdat1) {
  /* Fast enumeration is used. Only requests addressed to the interface, class,
   * vendor and unknown control requests are received by this function.
   * This application does not support any class or vendor requests. */

  uint8_t bRequest, bReqType;
  uint8_t bType, bTarget;
  uint16_t wValue, wIndex;
  CyBool_t isHandled = false;

  /* Decode the fields from the setup request. */
  bReqType = (setupdat0 & CY_U3P_USB_REQUEST_TYPE_MASK);
  bType = (bReqType & CY_U3P_USB_TYPE_MASK);
  bTarget = (bReqType & CY_U3P_USB_TARGET_MASK);
  bRequest = ((setupdat0 & CY_U3P_USB_REQUEST_MASK) >> CY_U3P_USB_REQUEST_POS);
  wValue = ((setupdat0 & CY_U3P_USB_VALUE_MASK) >> CY_U3P_USB_VALUE_POS);
  wIndex = ((setupdat1 & CY_U3P_USB_INDEX_MASK) >> CY_U3P_USB_INDEX_POS);

  if (bType == CY_U3P_USB_STANDARD_RQT) {
    /* Handle SET_FEATURE(FUNCTION_SUSPEND) and CLEAR_FEATURE(FUNCTION_SUSPEND)
     * requests here. It should be allowed to pass if the device is in configured
     * state and failed otherwise. */
    if ((bTarget == CY_U3P_USB_TARGET_INTF) && ((bRequest == CY_U3P_USB_SC_SET_FEATURE) || (bRequest == CY_U3P_USB_SC_CLEAR_FEATURE)) && (wValue == 0)) {
      if (glIsApplnActive)
        CyU3PUsbAckSetup();
      else
        CyU3PUsbStall(0, true, false);

      isHandled = true;
    }

    /* CLEAR_FEATURE request for endpoint is always passed to the setup callback
     * regardless of the enumeration model used. When a clear feature is received,
     * the previous transfer has to be flushed and cleaned up. This is done at the
     * protocol level. Since this is just a debug log operation, there is no higher
     * level protocol and there are two DMA channels associated with the function,
     * it is easier to stop and restart the application. If there are more than one
     * EP associated with the channel reset both the EPs. The endpoint stall and toggle
     * / sequence number is also expected to be reset. Return false to make the
     * library clear the stall and reset the endpoint toggle. Or invoke the
     * CyU3PUsbStall (ep, false, true) and return true. Here we are clearing
     * the stall. */
    if ((bTarget == CY_U3P_USB_TARGET_ENDPT) && (bRequest == CY_U3P_USB_SC_CLEAR_FEATURE) && (wValue == CY_U3P_USBX_FS_EP_HALT)) {
      if ((wIndex == CY_FX_EP_DEBUG) && (glIsApplnActive)) {
        CyFxApplnStop();
        CyFxApplnStart();
        CyU3PUsbStall(wIndex, false, true);

        CyU3PUsbAckSetup();
        isHandled = true;
      }
    }
  }

  if (bType == CY_U3P_USB_VENDOR_RQT) {
    if (bRequest == 0xE0) {
      isHandled = true;
      CyU3PUsbAckSetup();
      CyU3PThreadSleep(100);
      CyU3PDeviceReset(false);
    }
  }

  return isHandled;
}

/* This is the callback function to handle the USB events. */
void CyFxApplnUSBEventCB(CyU3PUsbEventType_t evtype, uint16_t evdata) {
  switch (evtype) {
    case CY_U3P_USB_EVENT_SETCONF:
      /* Disable the low power entry to optimize USB throughput */
      CyU3PUsbLPMDisable();
      /* Stop the application before re-starting. */
      if (glIsApplnActive) {
        CyFxApplnStop();
      }
      /* Start the logger function. */
      CyFxApplnStart();
      break;

    case CY_U3P_USB_EVENT_RESET:
    case CY_U3P_USB_EVENT_DISCONNECT:
      /* Stop the logger function. */
      if (glIsApplnActive) {
        CyFxApplnStop();
      }
      break;

    default:
      break;
  }
}

/* Callback function to handle LPM requests from the USB 3.0 host. This function is invoked by the API
   whenever a state change from U0 -> U1 or U0 -> U2 happens. If we return true from this function, the
   FX3 device is retained in the low power state. If we return false, the FX3 device immediately tries
   to trigger an exit back to U0.

   This application does not have any state in which we should not allow U1/U2 transitions; and therefore
   the function always return true.
 */
CyBool_t CyFxApplnLPMRqtCB(CyU3PUsbLinkPowerMode link_mode) {
  return true;
}

/* This function initializes the USB Module, sets the enumeration descriptors.
 * This function does not start the logger and this is done only when
 * SET_CONF event is received. */
void CyFxApplnInit(void) {
  CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

  /* Start the USB functionality. */
  apiRetStatus = CyU3PUsbStart();
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* The fast enumeration is the easiest way to setup a USB connection,
   * where all enumeration phase is handled by the library. Only the
   * class / vendor requests need to be handled by the application. */
  CyU3PUsbRegisterSetupCallback(CyFxApplnUSBSetupCB, true);

  /* Setup the callback to handle the USB events. */
  CyU3PUsbRegisterEventCallback(CyFxApplnUSBEventCB);

  /* Register a callback to handle LPM requests from the USB 3.0 host. */
  CyU3PUsbRegisterLPMRequestCallback(CyFxApplnLPMRqtCB);

  /* Set the USB Enumeration descriptors */

  /* Super speed device descriptor. */
  apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB30DeviceDscr);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* High speed device descriptor. */
  apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_HS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB20DeviceDscr);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* BOS descriptor */
  apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_BOS_DESCR, 0, (uint8_t *)CyFxUSBBOSDscr);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* Device qualifier descriptor */
  apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_DEVQUAL_DESCR, 0, (uint8_t *)CyFxUSBDeviceQualDscr);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* Super speed configuration descriptor */
  apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBSSConfigDscr);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* High speed configuration descriptor */
  apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_HS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBHSConfigDscr);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* Full speed configuration descriptor */
  apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_FS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBFSConfigDscr);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* String descriptor 0 */
  apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 0, (uint8_t *)CyFxUSBStringLangIDDscr);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* String descriptor 1 */
  apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 1, (uint8_t *)CyFxUSBManufactureDscr);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* String descriptor 2 */
  apiRetStatus = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 2, (uint8_t *)CyFxUSBProductDscr);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }

  /* Connect the USB Pins with super speed operation enabled. */
  apiRetStatus = CyU3PConnectState(true, true);
  if (apiRetStatus != CY_U3P_SUCCESS) {
    CyFxAppErrorHandler(apiRetStatus);
  }
}

/* Entry function for the AppThread. */
void ApplnThread_Entry(uint32_t input) {
  uint8_t logPriority;
  uint16_t logId;
  uint32_t logParam;

  /* Initialize the logger application */
  CyFxApplnInit();

  logPriority = 1;
  logId = 0x1111;
  logParam = 0x12345678;

  for (;;) {
    if (glIsApplnActive) {
      CyU3PDebugLog(logPriority, logId, logParam);
      logParam += 0x10000000;
      if (logParam == 0x12345678) {
        logId += 0x1000;
        if (logId == 0x1111) {
          logPriority++;
          if (logPriority == 5) {
            logPriority = 1;
            CyU3PDebugPrint(2, "USB Debug logger: time from start in ticks: %d\n", CyU3PGetTime());
          }
        }
      }
    }

    CyU3PThreadSleep(100);
  }
}

/* Application define function which creates the threads. */
void CyFxApplicationDefine() {
  void *ptr = NULL;
  uint32_t retThrdCreate = CY_U3P_SUCCESS;

  /* Allocate the memory for the threads */
  ptr = CyU3PMemAlloc(CY_FX_APPLN_THREAD_STACK);

  /* Create the thread for the application */
  retThrdCreate = CyU3PThreadCreate(&applnThread,                /* App Thread structure */
                                    "21:USB_DEBUG",              /* Thread ID and Thread name */
                                    ApplnThread_Entry,           /* App Thread Entry function */
                                    0,                           /* No input parameter to thread */
                                    ptr,                         /* Pointer to the allocated thread stack */
                                    CY_FX_APPLN_THREAD_STACK,    /* App Thread stack size */
                                    CY_FX_APPLN_THREAD_PRIORITY, /* App Thread priority */
                                    CY_FX_APPLN_THREAD_PRIORITY, /* App Thread pre-emption threshold */
                                    CYU3P_NO_TIME_SLICE,         /* No time slice for the application thread */
                                    CYU3P_AUTO_START             /* Start the Thread immediately */
  );

  /* Check the return code */
  if (retThrdCreate != 0) {
    /* Thread Creation failed with the error code retThrdCreate */

    /* Add custom recovery or debug actions here */

    /* Application cannot continue */
    while (1) {
    }
  }
}

int main(void) {
  CyU3PIoMatrixConfig_t io_cfg;
  CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

  /* Initialize the device */
  status = CyU3PDeviceInit(NULL);
  if (status != CY_U3P_SUCCESS) {
    goto handle_fatal_error;
  }

  /* Initialize the caches. Enable both Instruction and Data Caches. */
  status = CyU3PDeviceCacheControl(true, true, true);
  if (status != CY_U3P_SUCCESS) {
    goto handle_fatal_error;
  }

  /* No LPP or GPIO is being used. */
  io_cfg.isDQ32Bit = false;
  io_cfg.s0Mode = CY_U3P_SPORT_INACTIVE;
  io_cfg.s1Mode = CY_U3P_SPORT_INACTIVE;
  io_cfg.useUart = false;
  io_cfg.useI2C = false;
  io_cfg.useI2S = false;
  io_cfg.useSpi = false;
  io_cfg.lppMode = CY_U3P_IO_MATRIX_LPP_DEFAULT;
  /* No GPIOs are enabled. */
  io_cfg.gpioSimpleEn[0] = 0;
  io_cfg.gpioSimpleEn[1] = 0;
  io_cfg.gpioComplexEn[0] = 0;
  io_cfg.gpioComplexEn[1] = 0;
  status = CyU3PDeviceConfigureIOMatrix(&io_cfg);
  if (status != CY_U3P_SUCCESS) {
    goto handle_fatal_error;
  }

  /* This is a non returnable call for initializing the RTOS kernel */
  CyU3PKernelEntry();

  /* Dummy return to make the compiler happy */
  return 0;

handle_fatal_error:
  /* Cannot recover from this error. */
  while (1) {
  }
}
