/*
 ## Cypress FX3 Boot Firmware Example Header File (defines.h)
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

#include "cyfx3usb.h"
#include "cyfx3gpio.h"
#include "cyfx3uart.h"
#include "cyfx3device.h"
#include "cyfx3utils.h"
#include "cyfx3pib.h"

#ifdef CYMEM_256K

/* DMA buffer addresses for various modules. */
#define UART_DMA_BUF_ADDRESS    (0x40037000)
#define USB_DMA_BUF_ADDRESS     (0x40037800)

/* Base address, size and number of DMA buffers used for the GPIF to USB DMA channel. */
#define DMA_BUF_BASE       (0x4002F000)
#define DMA_BUF_SIZE       (0x2000)
#define DMA_BUF_COUNT      (4)

#else

/* DMA buffer addresses for various modules. */
#define UART_DMA_BUF_ADDRESS    (0x40077000)
#define USB_DMA_BUF_ADDRESS     (0x40077800)

/* Base address, size and number of DMA buffers used for the GPIF to USB DMA channel. */
#define DMA_BUF_BASE       (0x40067000)
#define DMA_BUF_SIZE       (0x4000)
#define DMA_BUF_COUNT      (4)

#endif

/* Result of a USB control request. */
typedef enum
{
    eStall = 0,     /* Send STALL */
    eDataIn,        /* Data IN Stage */
    eDataOut,       /* Data Out Stage */
    eStatus         /* Status Stage */
} eUsbStage;

#define USB_CTRL_IN_EP                  (0x80)
#define USB_CTRL_OUT_EP                 (0x00)

#define USB_SETUP_DIR                   (0x80) /* 0x80 = To Host */

#define USB_RQT_TYPE_MASK               (0x60) /* Mask to get request type. */
#define USB_STANDARD_REQUEST            (0x00) /* Standard Request */
#define USB_CLASS_REQUEST               (0x20) /* Class Request */
#define USB_VENDOR_REQUEST              (0x40) /* Vendor Request */

#define USB_RQT_TARGET_MASK             (0x03) /* USB Request mask */
#define USB_TARGET_DEVICE               (0x00) /* Device Request */
#define USB_TARGET_INTERFACE            (0x01) /* Interface Request */
#define USB_TARGET_ENDPOINT             (0x02) /* Endpoint Request */

#define USB_SC_GET_STATUS               (0x00)
#define USB_SC_CLEAR_FEATURE            (0x01)
#define USB_SC_SET_FEATURE              (0x03)
#define USB_SC_SET_ADDRESS              (0x05)
#define USB_SC_GET_DESCRIPTOR           (0x06)
#define USB_SC_GET_CONFIGURATION        (0x08)
#define USB_SC_SET_CONFIGURATION        (0x09)
#define USB_SC_GET_INTERFACE            (0x0A)
#define USB_SC_SET_INTERFACE            (0x0B)
#define USB_SC_SYNC_FRAME               (0x0C)
#define USB_SC_SET_SEL                  (0x30)
#define USB_SC_SET_ISOC_DELAY           (0x31)

#define USB_FS_REMOTE_WAKE              (1)
#define USB_FS_U1_ENABLE                (48)
#define USB_FS_U2_ENABLE                (49)
#define USB_FS_LTM_ENABLE               (50)

#define STAT_SELF_POWERED               (0x01)
#define STAT_REMOTE_WAKEUP              (0x02)
#define STAT_U1_ENABLE                  (0x04)
#define STAT_U2_ENABLE                  (0x08)
#define STAT_LTM_ENABLE                 (0x10)

#define USB_MAX_CONFIG                  (1)             /* We have only one configuration. */
#define DATA_ENDPOINT                   (0x81)          /* Using EP 1-IN for data transfer. */

#define GET_MSB(w)                      ((uint8_t)((w) >> 8))
#define GET_LSB(w)                      ((uint8_t)((w) & 0xFF))
#define MAKE_WORD(mb,lb)                (((uint16_t)(mb) << 8) | ((uint16_t)(lb)))

/* Function prototypes. */

/* Initialize the PIB and GPIF-II State Machine. */
extern int
GpifInit (
        void);

/* De-initialize the GPIF-II and PIB blocks. */
extern void
GpifDeinit (
        void);

/*[]*/


