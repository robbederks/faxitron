
#include <stdbool.h>

#include "main.h"
#include <cyu3dma.h>
#include <cyu3error.h>
#include <cyu3os.h>
#include <cyu3system.h>
#include <cyu3usb.h>
#include <cyu3tx.h>
#include <cyu3pib.h>
#include <cyu3gpio.h>
#include <cyu3gpif.h>

// GPIF design
#include "gpif/faxitron.cydsn/cyfxgpif2config.h"

CyU3PThread applnThread;               /* Application thread structure */
static CyU3PDmaChannel glChHandleUtoP; /* DMA Channel handle for U2P transfer. */
static CyU3PDmaChannel glChHandlePtoU; /* DMA Channel handle for P2U transfer. */
bool glIsApplnActive = false;          /* Whether the application is active or not. */

void CyFxAppErrorHandler(CyU3PReturnStatus_t apiRetStatus) {
  while (true) {}
}

/* This function starts the logger application. This is called
 * when a SET_CONF event is received from the USB host. The endpoints
 * are configured and the DMA pipe is setup in this function. */
void CyFxApplnStart(void) {
  uint16_t size = 0;
  CyU3PEpConfig_t epCfg;
  CyU3PDmaChannelConfig_t dmaCfg;
  CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;
  CyU3PUSBSpeed_t usbSpeed = CyU3PUsbGetSpeed();

  // Identify USB speed
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
  epCfg.burstLen = 1;
  epCfg.streams = 0;
  epCfg.pcktSize = size;

  // Consumer endpoint configuration (debug)
  epCfg.epType = CY_U3P_USB_EP_INTR;
  CHECK_API_RET(CyU3PSetEpConfig(CY_FX_EP_DEBUG, &epCfg));
  CyU3PUsbFlushEp(CY_FX_EP_DEBUG);
  CHECK_API_RET(CyU3PDebugInit(CY_FX_EP_DEBUG_SOCKET, 8));

  // DMA endpoints
  epCfg.epType = CY_U3P_USB_EP_BULK;
  CHECK_API_RET(CyU3PSetEpConfig(CY_FX_EP_PRODUCER, &epCfg));
  CHECK_API_RET(CyU3PSetEpConfig(CY_FX_EP_CONSUMER, &epCfg));

  // DMA channels
  dmaCfg.size = size;
  dmaCfg.count = CY_FX_DMA_BUF_COUNT_U_2_P;
  dmaCfg.dmaMode = CY_U3P_DMA_MODE_BYTE;
  dmaCfg.notification = 0;
  dmaCfg.cb = NULL;
  dmaCfg.prodHeader = 0;
  dmaCfg.prodFooter = 0;
  dmaCfg.consHeader = 0;
  dmaCfg.prodAvailCount = 0;

  // Setup
  dmaCfg.prodSckId = CY_U3P_UIB_SOCKET_PROD_1;
  dmaCfg.consSckId = CY_U3P_PIB_SOCKET_1;
  CHECK_API_RET(CyU3PDmaChannelCreate(&glChHandleUtoP, CY_U3P_DMA_TYPE_AUTO, &dmaCfg));

  dmaCfg.prodSckId = CY_U3P_PIB_SOCKET_0;
  dmaCfg.consSckId = CY_U3P_UIB_SOCKET_CONS_1;
  CHECK_API_RET(CyU3PDmaChannelCreate(&glChHandlePtoU, CY_U3P_DMA_TYPE_AUTO, &dmaCfg));

  // Transfer size
  CHECK_API_RET(CyU3PDmaChannelSetXfer(&glChHandleUtoP, CY_FX_DMA_TX_SIZE));
  CHECK_API_RET(CyU3PDmaChannelSetXfer(&glChHandlePtoU, CY_FX_DMA_TX_SIZE));

  // Start GPIF state machine
  CHECK_API_RET(CyU3PGpifSMStart(START, ALPHA_START));

  // We're running!
  glIsApplnActive = true;
  CyU3PDebugPrint(4, "Up and running!");
}

/* This function stops the logger application. This shall be called whenever
 * a RESET or DISCONNECT event is received from the USB host. The endpoints are
 * disabled and the DMA pipe is destroyed by this function. */
void CyFxApplnStop(
    void) {
  CyU3PEpConfig_t epCfg;
  CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

  // We fell :(
  glIsApplnActive = false;
  CyU3PDebugPrint(4, "Shut down!");

  // Disable debug
  CyU3PDebugDeInit();

  // Flush the endpoint memory
  CyU3PUsbFlushEp(CY_FX_EP_DEBUG);
  CyU3PUsbFlushEp(CY_FX_EP_PRODUCER);
  CyU3PUsbFlushEp(CY_FX_EP_CONSUMER);

  // Destroy the channel
  CyU3PDmaChannelDestroy(&glChHandleUtoP);
  CyU3PDmaChannelDestroy(&glChHandlePtoU);

  // Disable endpoints
  CyU3PMemSet((uint8_t *)&epCfg, 0, sizeof(epCfg));
  epCfg.enable = false;
  CHECK_API_RET(CyU3PSetEpConfig(CY_FX_EP_DEBUG, &epCfg));
  CHECK_API_RET(CyU3PSetEpConfig(CY_FX_EP_PRODUCER, &epCfg));
  CHECK_API_RET(CyU3PSetEpConfig(CY_FX_EP_CONSUMER, &epCfg));
}

// Callback to handle the USB setup requests.
CyBool_t CyFxApplnUSBSetupCB(uint32_t setupdat0, uint32_t setupdat1) {
  uint8_t bRequest, bReqType;
  uint8_t bType, bTarget;
  uint16_t wValue, wIndex;
  CyBool_t isHandled = false;

  // Decode the fields from the setup request
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
      if (glIsApplnActive) {
        if (wIndex == CY_FX_EP_DEBUG) {
          // TODO: do we want this?
          CyFxApplnStop();
          CyFxApplnStart();

          CyU3PUsbAckSetup();
        }

        if (wIndex == CY_FX_EP_PRODUCER) {
          CyU3PUsbSetEpNak(CY_FX_EP_PRODUCER, true);
          CyFx3BusyWait(125);

          CyU3PDmaChannelReset(&glChHandleUtoP);
          CyU3PUsbFlushEp(CY_FX_EP_PRODUCER);
          CyU3PUsbResetEp(CY_FX_EP_PRODUCER);
          CyU3PDmaChannelSetXfer(&glChHandleUtoP, CY_FX_DMA_TX_SIZE);

          CyU3PUsbSetEpNak(CY_FX_EP_PRODUCER, false);
          CyU3PUsbAckSetup();
        }

        if (wIndex == CY_FX_EP_CONSUMER) {
          CyU3PUsbSetEpNak(CY_FX_EP_CONSUMER, true);
          CyFx3BusyWait(125);

          CyU3PDmaChannelReset(&glChHandlePtoU);
          CyU3PUsbFlushEp(CY_FX_EP_CONSUMER);
          CyU3PUsbResetEp(CY_FX_EP_CONSUMER);
          CyU3PDmaChannelSetXfer(&glChHandlePtoU, CY_FX_DMA_RX_SIZE);

          CyU3PUsbSetEpNak(CY_FX_EP_CONSUMER, false);
          CyU3PUsbAckSetup();
        }

        CyU3PUsbStall(wIndex, false, true);
        isHandled = true;
      }
    }
  }

  if (bType == CY_U3P_USB_VENDOR_RQT) {
    if (bRequest == 0xE0) {
      // Device reset
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
      // Stop the application before re-starting
      if (glIsApplnActive) {
        CyFxApplnStop();
      }

      // Disable the low power entry to optimize USB throughput
      CyU3PUsbLPMDisable();

      // Start app
      CyFxApplnStart();
      break;

    case CY_U3P_USB_EVENT_RESET:
    case CY_U3P_USB_EVENT_DISCONNECT:
      // Stop app
      if (glIsApplnActive) {
        CyFxApplnStop();
      }
      break;

    default:
      break;
  }
}

// Return true to always allow LPM transitions
CyBool_t CyFxApplnLPMRqtCB(CyU3PUsbLinkPowerMode link_mode) {
  return true;
}

static void PibEventCallback(CyU3PPibIntrType cbType, uint16_t cbArg) {
  if (cbType == CYU3P_PIB_INTR_ERROR) {
    switch (CYU3P_GET_PIB_ERROR_TYPE(cbArg)) {
      case CYU3P_PIB_ERR_THR0_WR_OVERRUN:
        CyU3PDebugPrint(4, "CYU3P_PIB_ERR_THR0_WR_OVERRUN\r\n");
        break;
      case CYU3P_PIB_ERR_THR1_WR_OVERRUN:
        CyU3PDebugPrint(4, "CYU3P_PIB_ERR_THR1_WR_OVERRUN\r\n");
        break;
      case CYU3P_PIB_ERR_THR2_WR_OVERRUN:
        CyU3PDebugPrint(4, "CYU3P_PIB_ERR_THR2_WR_OVERRUN\r\n");
        break;
      case CYU3P_PIB_ERR_THR3_WR_OVERRUN:
        CyU3PDebugPrint(4, "CYU3P_PIB_ERR_THR3_WR_OVERRUN\r\n");
        break;
      case CYU3P_PIB_ERR_THR0_RD_UNDERRUN:
        CyU3PDebugPrint(4, "CYU3P_PIB_ERR_THR0_RD_UNDERRUN\r\n");
        break;
      case CYU3P_PIB_ERR_THR1_RD_UNDERRUN:
        CyU3PDebugPrint(4, "CYU3P_PIB_ERR_THR1_RD_UNDERRUN\r\n");
        break;
      case CYU3P_PIB_ERR_THR2_RD_UNDERRUN:
        CyU3PDebugPrint(4, "CYU3P_PIB_ERR_THR2_RD_UNDERRUN\r\n");
        break;
      case CYU3P_PIB_ERR_THR3_RD_UNDERRUN:
        CyU3PDebugPrint(4, "CYU3P_PIB_ERR_THR3_RD_UNDERRUN\r\n");
        break;
      default:
        CyU3PDebugPrint(4, "PID Error :%d\r\n", CYU3P_GET_PIB_ERROR_TYPE(cbArg));
        break;
    }
  }
}

void config_gpio(uint8_t gpio, bool input, bool drive_low, bool drive_high, bool initial_value) {
  CyU3PGpioSimpleConfig_t gpioConfig;
  CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

  CHECK_API_RET(CyU3PDeviceGpioOverride(gpio, true));
  gpioConfig.outValue = initial_value;
  gpioConfig.driveLowEn = drive_low;
  gpioConfig.driveHighEn = drive_high;
  gpioConfig.inputEn = input;
  gpioConfig.intrMode = CY_U3P_GPIO_NO_INTR;
  CHECK_API_RET(CyU3PGpioSetSimpleConfig(gpio, &gpioConfig));
}

// Init app when SET_CONF is received
void CyFxApplnInit(void) {
  CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;
  CyU3PPibClock_t pibClock;
  CyU3PGpioClock_t gpioClock;

  // Init P-port (GPIF)
  pibClock.clkDiv = 2;
  pibClock.clkSrc = CY_U3P_SYS_CLK_BY_16; // 15MHz
  pibClock.isHalfDiv = false;
  pibClock.isDllEnable = false;
  CHECK_API_RET(CyU3PPibInit(true, &pibClock));

  CHECK_API_RET(CyU3PGpifLoad(&CyFxGpifConfig));

  // Overflow callback
  CyU3PPibRegisterCallback(PibEventCallback, 0xFFFF);

  // Initialize GPIO
  gpioClock.fastClkDiv = 2;
  gpioClock.slowClkDiv = 0;
  gpioClock.simpleDiv = CY_U3P_GPIO_SIMPLE_DIV_BY_2;
  gpioClock.clkSrc = CY_U3P_SYS_CLK;
  gpioClock.halfDiv = 0;
  CHECK_API_RET(CyU3PGpioInit(&gpioClock, NULL));

  // GPIO configs
  config_gpio(GPIO_TXSCD, false, true, true, false);
  config_gpio(GPIO_TXHALT_N, false, true, true, true);
  config_gpio(GPIO_RESET_N, false, true, true, true);
  config_gpio(GPIO_DLB, false, true, true, true);
  config_gpio(GPIO_LED_N, false, true, false, true);

  // Start the USB functionality.
  CHECK_API_RET(CyU3PUsbStart());
  CyU3PUsbRegisterSetupCallback(CyFxApplnUSBSetupCB, true);
  CyU3PUsbRegisterEventCallback(CyFxApplnUSBEventCB);
  CyU3PUsbRegisterLPMRequestCallback(CyFxApplnLPMRqtCB);

  // Set the USB Enumeration descriptors
  CHECK_API_RET(CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB30DeviceDscr));
  CHECK_API_RET(CyU3PUsbSetDesc(CY_U3P_USB_SET_HS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB20DeviceDscr));
  CHECK_API_RET(CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_BOS_DESCR, 0, (uint8_t *)CyFxUSBBOSDscr));
  CHECK_API_RET(CyU3PUsbSetDesc(CY_U3P_USB_SET_DEVQUAL_DESCR, 0, (uint8_t *)CyFxUSBDeviceQualDscr));
  CHECK_API_RET(CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBSSConfigDscr));
  CHECK_API_RET(CyU3PUsbSetDesc(CY_U3P_USB_SET_HS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBHSConfigDscr));
  CHECK_API_RET(CyU3PUsbSetDesc(CY_U3P_USB_SET_FS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBFSConfigDscr));
  CHECK_API_RET(CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 0, (uint8_t *)CyFxUSBStringLangIDDscr));
  CHECK_API_RET(CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 1, (uint8_t *)CyFxUSBManufactureDscr));
  CHECK_API_RET(CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 2, (uint8_t *)CyFxUSBProductDscr));
  CHECK_API_RET(CyU3PConnectState(true, true));
}

void ApplnThread_Entry(uint32_t input) {
  CyFxApplnInit();

  for (;;) {
    uint8_t state;
    CyU3PGpifGetSMState(&state);
    CyU3PDebugPrint(4, "State: 0x%x", state);

    if (glIsApplnActive) {
      CyU3PGpioSetValue(GPIO_LED_N, false);
      CyU3PThreadSleep(100);
      CyU3PGpioSetValue(GPIO_LED_N, true);
    } else {
      CyU3PGpioSetValue(GPIO_LED_N, true);
    }
    CyU3PThreadSleep(100);
  }
}

// Start threads here
void CyFxApplicationDefine() {
  void *ptr = NULL;
  uint32_t retThrdCreate = CY_U3P_SUCCESS;

  // Allocate memory
  ptr = CyU3PMemAlloc(CY_FX_APPLN_THREAD_STACK);

  // Create threads
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

  if (retThrdCreate != 0) {
    while (1) {}
  }
}

int main(void) {
  CyU3PIoMatrixConfig_t io_cfg;
  CyU3PSysClockConfig_t clkCfg;
  CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

  /* Initialize the device */
  clkCfg.setSysClk400 = true;     /* FX3 device's master clock is set to a frequency > 400 MHz */
  clkCfg.cpuClkDiv = 2;           /* CPU clock divider */
  clkCfg.dmaClkDiv = 2;           /* DMA clock divider */
  clkCfg.mmioClkDiv = 2;          /* MMIO clock divider */
  clkCfg.useStandbyClk = false;   /* device has no 32KHz clock supplied */
  clkCfg.clkSrc = CY_U3P_SYS_CLK; /* Clock source for a peripheral block  */
  status = CyU3PDeviceInit(&clkCfg);
  if (status != CY_U3P_SUCCESS) {
    goto handle_fatal_error;
  }

  /* Initialize the caches. */
  status = CyU3PDeviceCacheControl(true, false, false);
  if (status != CY_U3P_SUCCESS) {
    goto handle_fatal_error;
  }

  // IO config:
  // - 12: LFI_N
  // - 13: RXRST_N
  // - 14: RFEN
  // - 15: TXRST_N
  // - 17: RXEN_N
  // - 18: TXEN_N
  // - 19: TXSC/D
  // - 33: TXHALT_N
  // - 34: RESET_N
  // - 35: DLB
  // - 46-49: UART (TODO)
  io_cfg.isDQ32Bit = false;
  io_cfg.s0Mode = CY_U3P_SPORT_INACTIVE;
  io_cfg.s1Mode = CY_U3P_SPORT_INACTIVE;
  io_cfg.useUart = false;
  io_cfg.useI2C = false;
  io_cfg.useI2S = false;
  io_cfg.useSpi = false;
  io_cfg.lppMode = CY_U3P_IO_MATRIX_LPP_UART_ONLY;
  io_cfg.gpioSimpleEn[0] = 0;
  io_cfg.gpioSimpleEn[1] = (1 << 1) | (1 << 2) | (1 << 3) | (1 << 4);
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
