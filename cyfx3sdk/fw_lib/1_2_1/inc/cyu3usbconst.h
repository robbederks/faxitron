/*
## Cypress USB 3.0 Platform header file (cyu3usbconst.h)
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

#ifndef _INCLUDED_CYU3USBCONST_H_
#define _INCLUDED_CYU3USBCONST_H_

#include "cyu3externcstart.h"

/*@@USB Constants
   Summary
   These are USB constants that are derived from the USB protocol specification.
 */

/* USB Setup Commands Recipient */
#define CY_U3P_USB_TARGET_MASK                  (0x03)    /* The Target mask */
#define CY_U3P_USB_TARGET_DEVICE                (0x00)    /* The USB Target Device */
#define CY_U3P_USB_TARGET_INTF                  (0x01)    /* The USB Target Interface */
#define CY_U3P_USB_TARGET_ENDPT                 (0x02)    /* The USB Target Endpoint */
#define CY_U3P_USB_TARGET_OTHER                 (0x03)    /* The USB Target Other */

#define CY_U3P_USB_GS_DEVICE                    (0x80)    /* The standard device request, GET_STATUS device */
#define CY_U3P_USB_GS_INTERFACE                 (0x81)    /* The standard device request, GET_STATUS interface */
#define CY_U3P_USB_GS_ENDPOINT                  (0x82)    /* The standard device request, GET_STATUS endpoint */

/* USB Request Class types */
#define CY_U3P_USB_TYPE_MASK                    (0x60)     /* The request type mask */
#define CY_U3P_USB_STANDARD_RQT                 (0x00)     /* The USB standard request */
#define CY_U3P_USB_CLASS_RQT                    (0x20)     /* The USB class request */
#define CY_U3P_USB_VENDOR_RQT                   (0x40)     /* The USB vendor request */
#define CY_U3P_USB_RESERVED_RQT                 (0x60)     /* The USB reserved request */

/* Summary
   Standard device request codes.

   Description
   These are the various standard requests received from the USB host.
   The device is expected to respond to all these requests.
 */
typedef enum CyU3PUsbSetupCmds
{
    CY_U3P_USB_SC_GET_STATUS = 0x00,    /* Get status request. */
    CY_U3P_USB_SC_CLEAR_FEATURE,        /* Clear feature. */
    CY_U3P_USB_SC_RESERVED,             /* Reserved command. */
    CY_U3P_USB_SC_SET_FEATURE,          /* Set feature. */
    CY_U3P_USB_SC_SET_ADDRESS = 0x05,   /* Set address. */
    CY_U3P_USB_SC_GET_DESCRIPTOR,       /* Get descriptor. */
    CY_U3P_USB_SC_SET_DESCRIPTOR,       /* Set descriptor. */
    CY_U3P_USB_SC_GET_CONFIGURATION,    /* Get configuration. */
    CY_U3P_USB_SC_SET_CONFIGURATION,    /* Set configuration. */
    CY_U3P_USB_SC_GET_INTERFACE,        /* Get interface (alternate setting). */
    CY_U3P_USB_SC_SET_INTERFACE,        /* Set interface (alternate setting). */
    CY_U3P_USB_SC_SYNC_FRAME,           /* Synch frame. */
    CY_U3P_USB_SC_SET_SEL = 0x30,       /* Set system exit latency. */
    CY_U3P_USB_SC_SET_ISOC_DELAY        /* Set isochronous delay. */
} CyU3PUsbSetupCmds;

/* Summary
   Enumeration of the endpoint types.

   Description
   There are four types of endpoints. This defines the behaviour of
   the endpoint. The control endpoint is a compulsory for any device
   whereas the other endpoints are used as per requirement.
 */
typedef enum CyU3PUsbEpType_t
{
    CY_U3P_USB_EP_CONTROL = 0,          /* Control Endpoint Type */
    CY_U3P_USB_EP_ISO = 1,              /* Isochronous Endpoint Type */
    CY_U3P_USB_EP_BULK = 2,             /* Bulk Endpoint Type */
    CY_U3P_USB_EP_INTR = 3              /* Interrupt Endpoint Type */
} CyU3PUsbEpType_t;

/* Summary
   Enumeration of descriptor types.

   Description
   A USB device is identified by the descriptors provided. The
   following are the standard defined descriptors. 
 */
typedef enum CyU3PUsbDescType
{
    CY_U3P_USB_DEVICE_DESCR = 0x01,      /* Super speed Device descr  */
    CY_U3P_USB_CONFIG_DESCR,             /* Configuration */
    CY_U3P_USB_STRING_DESCR,             /* String */
    CY_U3P_USB_INTRFC_DESCR,             /* Interface */
    CY_U3P_USB_ENDPNT_DESCR,             /* End Point */
    CY_U3P_USB_DEVQUAL_DESCR,            /* Device Qualifier */
    CY_U3P_USB_OTHERSPEED_DESCR,         /* Other Speed Configuration */
    CY_U3P_USB_INTRFC_POWER_DESCR,       /* Interface  power descriptor */
    CY_U3P_USB_OTG_DESCR,                /* OTG descriptor */
    CY_U3P_BOS_DESCR = 0x0F,             /* BOS descriptor*/
    CY_U3P_DEVICE_CAPB_DESCR,            /* Device Capability descriptor*/
    CY_U3P_USB_HID_DESCR = 0x21,         /* HID descriptor */
    CY_U3P_USB_REPORT_DESCR,             /* Report descriptor */
    CY_U3P_SS_EP_COMPN_DESCR = 0x30      /* End Point companion descriptor*/ 
} CyU3PUsbDescType;

/* Summary
   Device capability type codes.

   Description
   The following are the various extended
   capabilities of the USB device.
 */
typedef enum CyU3PUsbDevCapType
{
    CY_U3P_WIRELESS_USB_CAPB_TYPE = 0x01,       /* wireless USB specific device level capabilities. */
    CY_U3P_USB2_EXTN_CAPB_TYPE,                 /* USB 2.0 extension descriptor. */
    CY_U3P_SS_USB_CAPB_TYPE,                    /* super speed USB specific device level capabilities. */
    CY_U3P_CONTAINER_ID_CAPB_TYPE               /* instance unique ID used to identify the instance across
                                                   all operating modes. */
} CyU3PUsbDevCapType;

/* Position of device address field in USB 3.0 LMP and TP. */
#define CY_U3P_USB3_TP_DEVADDR_POS              (25)

/* Position of endpoint number field in USB 3.0 TP. */
#define CY_U3P_USB3_TP_EPNUM_POS                (8)

/* Summary
   USB 3.0 packet type codes.

   Description
   The following are the various USB 3.0 packets types.
 */
typedef enum CyU3PUsb3PacketType
{
    CY_U3P_USB3_PACK_TYPE_LMP = 0x00,           /* Link Management Packet. */
    CY_U3P_USB3_PACK_TYPE_TP  = 0x04,           /* Transaction Packet. */
    CY_U3P_USB3_PACK_TYPE_DPH = 0x08,           /* Data Packet Header. */
    CY_U3P_USB3_PACK_TYPE_ITP = 0x0C            /* Isochronous Timestamp Packet. */
} CyU3PUsb3PacketType;

/* Summary
   USB 3.0 transaction packet sub type codes.

   Description
   The following are the various packet sub-types transmitted
   on super speed operation.
 */
typedef enum CyU3PUsb3TpSubType
{
    CY_U3P_USB3_TP_SUBTYPE_RES = 0,             /* Reserved. */
    CY_U3P_USB3_TP_SUBTYPE_ACK,                 /* ACK TP. */
    CY_U3P_USB3_TP_SUBTYPE_NRDY,                /* NRDY TP. */
    CY_U3P_USB3_TP_SUBTYPE_ERDY,                /* ERDY TP. */
    CY_U3P_USB3_TP_SUBTYPE_STATUS,              /* STATUS TP. */
    CY_U3P_USB3_TP_SUBTYPE_STALL,               /* STALL TP. */
    CY_U3P_USB3_TP_SUBTYPE_NOTICE,              /* DEV_NOTIFICATION TP. */
    CY_U3P_USB3_TP_SUBTYPE_PING,                /* PING TP. */
    CY_U3P_USB3_TP_SUBTYPE_PINGRSP              /* PING RESPONSE TP. */
} CyU3PUsb3TpSubType;

/* Index field of the setup request indicating OTG GET_STATUS request. */
#define CY_U3P_USB_OTG_STATUS_SELECTOR          (0xF000)

/* Summary
   List of USB Feature selector codes.

   Description
   The following are the various features that can be selected
   using the SetFeature request or cleared using ClearFeature
   setup request. Refer to the USB specification for more
   information.
 */
typedef enum CyU3PUsbFeatureSelector
{
    CY_U3P_USBX_FS_EP_HALT     = 0,             /* USB Endpoint HALT feature. Sets or clears EP stall.  */
    CY_U3P_USB2_FS_REMOTE_WAKE = 1,             /* USB 2.0 Remote Wakeup. */
    CY_U3P_USB2_FS_TEST_MODE   = 2,             /* USB 2.0 Test mode. */
    CY_U3P_USB2_OTG_B_HNP_ENABLE  = 3,          /* USB 2.0 OTG HNP enable signal to B-device. */
    CY_U3P_USB2_OTG_A_HNP_SUPPORT = 4,          /* USB 2.0 OTG HNP supported indication to B-device. */
    CY_U3P_USB3_FS_U1_ENABLE   = 48,            /* USB 3.0 U1 Enable. */
    CY_U3P_USB3_FS_U2_ENABLE   = 49,            /* USB 3.0 U2 Enable. */
    CY_U3P_USB3_FS_LTM_ENABLE  = 50             /* USB 3.0 LTM Enable. */
} CyU3PUsbFeatureSelector;

/* Summary
   Link state machine states.
   
   Description
   These are the following link states of interest to firmware.
 */
typedef enum CyU3PUsbLinkState_t
{
    CY_U3P_UIB_LNK_STATE_SSDISABLED = 0x00,     /* SS.Disabled */
    CY_U3P_UIB_LNK_STATE_RXDETECT_RES = 0x01,   /* Rx.Detect.Reset */
    CY_U3P_UIB_LNK_STATE_RXDETECT_ACT = 0x02,   /* Rx.Detect.Active */
    CY_U3P_UIB_LNK_STATE_POLLING_LFPS = 0x08,   /* Polling.LFPS */
    CY_U3P_UIB_LNK_STATE_POLLING_RxEQ = 0x09,   /* Polling.RxEq */
    CY_U3P_UIB_LNK_STATE_POLLING_ACT = 0x0A,    /* Polling.Active */
    CY_U3P_UIB_LNK_STATE_POLLING_IDLE = 0x0C,   /* Polling.Idle */
    CY_U3P_UIB_LNK_STATE_U0 = 0x10,             /* U0 - Active state */
    CY_U3P_UIB_LNK_STATE_U1 = 0x11,             /* U1 */
    CY_U3P_UIB_LNK_STATE_U2 = 0x12,             /* U2 */
    CY_U3P_UIB_LNK_STATE_U3 = 0x13,             /* U3 - Suspend state */
    CY_U3P_UIB_LNK_STATE_COMP = 0x17,           /* Compliance */
    CY_U3P_UIB_LNK_STATE_RECOV_ACT = 0x18,      /* Recovery.Active */
    CY_U3P_UIB_LNK_STATE_RECOV_CNFG = 0x19,     /* Recovery.Configuration */
    CY_U3P_UIB_LNK_STATE_RECOV_IDLE = 0x1A      /* Recovery.Idle */
} CyU3PUsbLinkState_t;

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3USBCONST_H_ */

/*[]*/

