
#pragma once

#include "cyu3types.h"
#include "cyu3usbconst.h"
#include "cyu3externcstart.h"

#define CY_FX_APPLN_THREAD_STACK 0x1000
#define CY_FX_APPLN_THREAD_PRIORITY 8

#define DMA_IN_BUF_SIZE  1024 // Size of DMA buffer used for DEV to USB transfer.
#define DMA_OUT_BUF_SIZE 1024 // Size of DMA buffer used for USB to DEV transfer.

#define CY_FX_DMA_BUF_COUNT_P_2_U 1 // Number of DMA buffers used for DEV to USB transfer.
#define CY_FX_DMA_BUF_COUNT_U_2_P 1 // Number of DMA buffers used for USB to DEV transfer.

#define CY_FX_DMA_TX_SIZE 0 // DMA transfer size is set to infinite
#define CY_FX_DMA_RX_SIZE 0 // DMA transfer size is set to infinite

/* Endpoint and socket definitions for the application */

/* To change the producer and consumer EP enter the appropriate EP numbers for the #defines.
 * In the case of IN endpoints enter EP number along with the direction bit.
 * For eg. EP 6 IN endpoint is 0x86
 *     and EP 6 OUT endpoint is 0x06.
 * To change sockets mention the appropriate socket number in the #defines. */

/* Note: For USB 2.0 the endpoints and corresponding sockets are one-to-one mapped
         i.e. EP 1 is mapped to UIB socket 1 and EP 2 to socket 2 so on */

#define CY_FX_EP_PRODUCER 0x01 // Endpoint 1-OUT.
#define CY_FX_EP_CONSUMER 0x81 // Endpoint 1-IN.

#define CY_FX_EP_DEBUG 0x8A    /* EP 10 IN */
#define CY_FX_EP_DEBUG_SOCKET CY_U3P_UIB_SOCKET_CONS_10 // Needs to match the endpoint

#define LED_GPIO 54

/* Extern definitions for the USB Descriptors */
extern const uint8_t CyFxUSB20DeviceDscr[];
extern const uint8_t CyFxUSB30DeviceDscr[];
extern const uint8_t CyFxUSBDeviceQualDscr[];
extern const uint8_t CyFxUSBFSConfigDscr[];
extern const uint8_t CyFxUSBHSConfigDscr[];
extern const uint8_t CyFxUSBBOSDscr[];
extern const uint8_t CyFxUSBSSConfigDscr[];
extern const uint8_t CyFxUSBStringLangIDDscr[];
extern const uint8_t CyFxUSBManufactureDscr[];
extern const uint8_t CyFxUSBProductDscr[];

#define CHECK_API_RET(EXP) \
  apiRetStatus = EXP; \
  if (apiRetStatus != CY_U3P_SUCCESS) { \
    CyFxAppErrorHandler(apiRetStatus); \
  }

#include "cyu3externcend.h"
