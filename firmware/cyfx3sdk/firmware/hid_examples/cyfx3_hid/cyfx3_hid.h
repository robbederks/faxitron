/*
 ## Cypress FX3 example header file (cyfx3hid.h)
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

/* This file contains the macros and definitions for the HID application example */

#ifndef _INCLUDED_CYFX3_HID_H_
#define _INCLUDED_CYFX3_HID_H_

#include <cyu3types.h>
#include <cyu3usbconst.h>
#include <cyu3externcstart.h>

/* Endpoint and socket definitions for the HID application */

/* To change the Producer and Consumer EP enter the appropriate EP numbers for the #defines.
 * In the case of IN endpoints enter EP number along with the direction bit.
 * For eg. EP 6 IN endpoint is 0x86
 *     and EP 6 OUT endpoint is 0x06.
 * To change sockets mention the appropriate socket number in the #defines. */

/* Note: For USB 2.0 the endpoints and corresponding sockets are one-to-one mapped
         i.e. EP 1 is mapped to UIB socket 1 and EP 2 to socket 2 so on */

#define CY_FX_HID_EP_INTR_IN                  (0x81)          /* EP 1 IN */

#define CY_FX_HID_THREAD_STACK                (0x1000)        /* HID application thread stack size */
#define CY_FX_HID_THREAD_PRIORITY             (8)             /* HID application thread priority */

#define CY_FX_HID_DMA_BUF_COUNT               (20)            /* DMA Buffer Count */

#define CY_FX_APP_GPIO_INTR_CB_EVENT_FLAG     (1 << 1)        /* MSC application Clear Stall IN Event Flag */

#define CY_FX_HID_SET_IDLE                    (0x0A)          /* HID Class Specific Setup Request */
#define CY_FX_GET_REPORT_DESC                 (0x22)          /* HID Standard Request */

#define CY_FX_USB_HID_INTF                    (0x00)          /* HID interface number */

/* Descriptor Types */
#define CY_FX_BOS_DSCR_TYPE                   (15)            /* BOS Descriptor */
#define CY_FX_DEVICE_CAPB_DSCR_TYPE           (16)            /* Device Capability */
#define CY_FX_SS_EP_COMPN_DSCR_TYPE           (48)            /* EP Companion */
#define CY_FX_USB_HID_DESC_TYPE               (0x21)          /* HID Descriptor */

/* Interrupt GPIO ID. */
#define CY_FX_BUTTON_GPIO                       (45)

/* Extern definitions for the USB Enumeration descriptors */
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
extern const uint8_t CyFxUSBSerialNumberDscr[];
extern const uint8_t CyFxUSBReportDscr[]; 

#include <cyu3externcend.h>

#endif /* _INCLUDED_CYFX3_HID_H_ */

/*[]*/

