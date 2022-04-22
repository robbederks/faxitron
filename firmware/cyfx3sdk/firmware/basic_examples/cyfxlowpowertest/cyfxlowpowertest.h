/*
 ## Cypress FX3 Example header file (cyfxlowpowertest.h)
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

/* This file contains the externants used by the low power mode test application */

#ifndef _INCLUDED_CYFXLOWPOWERTEST_H_
#define _INCLUDED_CYFXLOWPOWERTEST_H_

#include "cyu3types.h"
#include "cyu3usbconst.h"
#include "cyu3externcstart.h"

#define CY_FX_DMA_BUF_COUNT      (8)                       /* DMA channel buffer count */
#define CY_FX_DMA_TX_SIZE        (0)                       /* DMA transfer size is set to infinite */
#define CY_FX_THREAD_STACK       (0x1000)                  /* Application thread stack size */
#define CY_FX_THREAD_PRIORITY    (8)                       /* Application thread priority */

/* Endpoint and socket definitions for the application */

/* To change the producer and consumer EP enter the appropriate EP numbers for the #defines.
 * In the case of IN endpoints enter EP number along with the direction bit.
 * For eg. EP 6 IN endpoint is 0x86
 *     and EP 6 OUT endpoint is 0x06.
 * To change sockets mention the appropriate socket number in the #defines. */

/* Note: For USB 2.0 the endpoints and corresponding sockets are one-to-one mapped
         i.e. EP 1 is mapped to UIB socket 1 and EP 2 to socket 2 so on */

#define CY_FX_EP_PRODUCER               0x01    /* EP 1 OUT */
#define CY_FX_EP_CONSUMER               0x81    /* EP 1 IN */

#define CY_FX_EP_PRODUCER_SOCKET        CY_U3P_UIB_SOCKET_PROD_1    /* Socket 1 is producer */
#define CY_FX_EP_CONSUMER_SOCKET        CY_U3P_UIB_SOCKET_CONS_1    /* Socket 1 is consumer */

/* Vendor Request ID's for Low Power Test */
#define CY_FX_LP_UART_STANDBY_RQT       (0xE1)  /* Standby test with UART CTS as wakeup source */
#define CY_FX_LP_VBUS_STANDBY_RQT       (0xE2)  /* Standby test with VBUS as wakeup source */
#define CY_FX_LP_UART_SUSPEND_RQT       (0xE3)  /* Suspend test with UART CTS as wakeup source */
#define CY_FX_LP_VBUS_SUSPEND_RQT       (0xE4)  /* Suspend test with VBUS as wakeup source */
#define CY_FX_LP_USB_SUSPEND_RQT        (0xE5)  /* Suspend test with USB Bus as wakeup source */

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

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYFXLOWPOWERTEST_H_ */

/*[]*/

