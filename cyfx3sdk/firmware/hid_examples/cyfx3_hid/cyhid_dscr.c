/*
 ## Cypress FX3 Example Source File (cyhid_dscr.c)
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

/* This file contains the USB descriptors for the HID application example */
#include "cyfx3_hid.h"

/* Standard Device Descriptor for 2.0 */
const uint8_t CyFxUSB20DeviceDscr[] __attribute__ ((aligned (32))) =
{
    0x12,                           /* Descriptor Size */
    CY_U3P_USB_DEVICE_DESCR,        /* Device Descriptor Type */
    0x10,0x02,                      /* USB 2.1 */
    0x00,                           /* Device Class */
    0x00,                           /* Device Sub-class */
    0x00,                           /* Device protocol */
    0x40,                           /* Maxpacket size for EP0 : 64 bytes */
    0xB4,0x04,                      /* Vendor ID */
    0x25,0x60,                      /* Product ID */
    0x00,0x00,                      /* Device release number */
    0x01,                           /* Manufacture string index */
    0x02,                           /* Product string index */
    0x03,                           /* Serial number string index */
    0x01                            /* Number of configurations */
};

/* Standard Device Descriptor for USB 3.0 */
const uint8_t CyFxUSB30DeviceDscr[] __attribute__ ((aligned (32))) =
{
    0x12,                           /* Descriptor Size */
    CY_U3P_USB_DEVICE_DESCR,        /* Device Descriptor Type */
    0x10,0x03,                      /* USB 3.1 */
    0x00,                           /* Device Class */
    0x00,                           /* Device Sub-class */
    0x00,                           /* Device protocol */
    0x09,                           /* Maxpacket size for EP0 : 2^9 */
    0xB4,0x04,                      /* Vendor ID */
    0x25,0x60,                      /* Product ID */
    0x00,0x00,                      /* Device release number */
    0x01,                           /* Manufacture string index */
    0x02,                           /* Product string index */
    0x03,                           /* Serial number string index */
    0x01                            /* Number of configurations */
};

/* Standard Device Qualifier Descriptor */
const uint8_t CyFxUSBDeviceQualDscr[] __attribute__ ((aligned (32))) =
{
    0x0A,                           /* Descriptor Size */
    CY_U3P_USB_DEVQUAL_DESCR,       /* Device Qualifier Descriptor Type */
    0x01,0x02,                      /* USB 2.1 */
    0x00,                           /* Device Class */
    0x00,                           /* Device Sub-class */
    0x00,                           /* Device protocol */
    0x40,                           /* Maxpacket size for EP0 : 64 bytes */
    0x01,                           /* Number of configurations */
    0x00                            /* Reserved */
};

/* Standard Full Speed Configuration Descriptor */
const uint8_t CyFxUSBFSConfigDscr[] __attribute__ ((aligned (32))) =
{
    /* Configuration Descriptor Type */
    0x09,                           /* Descriptor Size */
    CY_U3P_USB_CONFIG_DESCR,        /* Configuration Descriptor Type */
    0x22,0x00,                      /* Length of this descriptor and all sub descriptors */
    0x01,                           /* Number of interfaces */
    0x01,                           /* Configuration number */
    0x00,                           /* Configuration string index */
    0xC0,                           /* Self powered device. */
    0x32,                           /* Max power consumption of device (in 2mA unit) : 100mA */

    /* Interface Descriptor */
    0x09,                           /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,        /* Interface Descriptor type */
    CY_FX_USB_HID_INTF,             /* Interface number */
    0x00,                           /* Alternate setting number */
    0x01,                           /* Number of end points */
    0x03,                           /* Interface class : HID Class */
    0x00,                           /* Interface sub class : None */
    0x02,                           /* Interface protocol code : Mouse */
    0x00,                           /* Interface descriptor string index */

    /* HID Descriptor (Mouse) */
    0x09,                           /* Descriptor size */
    CY_FX_USB_HID_DESC_TYPE,        /* Descriptor Type */
    0x10,0x11,                      /* HID Class Spec 11.1 */
    0x00,                           /* Target Country */
    0x01,                           /* Total HID Class Descriptors */
    0x22,                           /* Report Descriptor Type */
    0x1C,0x00,                      /* Total Length of Report Descriptor */

    /* Endpoint Descriptor (Mouse) */
    0x07,                           /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,        /* Endpoint Descriptor Type */
    CY_FX_HID_EP_INTR_IN,           /* Endpoint address and description */
    CY_U3P_USB_EP_INTR,             /* Interrupt Endpoint Type */
    0x02,0x00,                      /* Max packet size = 2 bytes */
    0x02                            /* Servicing interval for data transfers : 2 msec */
};

/* HID Report Descriptor */
const uint8_t CyFxUSBReportDscr[] __attribute__ ((aligned (32))) =
{
    0x05,0x01,                      /* Usage Page (Generic Desktop) */ 
    0x09,0x02,                      /* Usage (Mouse) */ 
    0xA1,0x01,                      /* Collection (Application) */
    0x09,0x01,                      /* Usage (Pointer) */
    0xA1,0x00,                      /* Collection (Physical) */
    0x05,0x01,                      /* Usage Page (Generic Desktop) */
    0x09,0x30,                      /* Usage (X) */
    0x09,0x31,                      /* Usage (Y) */    
    0x15,0x81,                      /* Logical Minimum (-127) */
    0x25,0x7F,                      /* Logical Maximum (127) */
    0x75,0x08,                      /* Report Size (8) */        
    0x95,0x02,                      /* Report Count (2) */
    0x81,0x06,                      /* Input (Data, Value, Relative, Bit Field) */        
    0xC0,                           /* End Collection */
    0xC0                            /* End Collection */
};

/* Standard High Speed Configuration Descriptor */
const uint8_t CyFxUSBHSConfigDscr[] __attribute__ ((aligned (32))) =
{
    /* Configuration Descriptor Type */
    0x09,                           /* Descriptor Size */
    CY_U3P_USB_CONFIG_DESCR,        /* Configuration Descriptor Type */
    0x22,0x00,                      /* Length of this descriptor and all sub descriptors */
    0x01,                           /* Number of interfaces */
    0x01,                           /* Configuration number */
    0x00,                           /* Configuration string index */
    0xC0,                           /* Self powered device. */
    0x32,                           /* Max power consumption of device (in 2mA unit) : 100mA */

    /* Interface Descriptor */
    0x09,                           /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,        /* Interface Descriptor type */
    CY_FX_USB_HID_INTF,             /* Interface number */
    0x00,                           /* Alternate setting number */
    0x01,                           /* Number of end points */
    0x03,                           /* Interface class : HID Class */
    0x01,                           /* Interface sub class : None */
    0x02,                           /* Interface protocol code : Mouse */
    0x00,                           /* Interface descriptor string index */

    /* HID Descriptor (Mouse) */
    0x09,                           /* Descriptor size */
    CY_FX_USB_HID_DESC_TYPE,        /* Descriptor Type */
    0x10,0x11,                      /* HID Class Spec 11.1 */
    0x00,                           /* Target Country */
    0x01,                           /* Total HID Class Descriptors */
    0x22,                           /* Report Descriptor Type */
    0x1C,0x00,                      /* Total Length of Report Descriptor */  

    /* Endpoint Descriptor (Mouse) */
    0x07,                           /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,        /* Endpoint Descriptor Type */
    CY_FX_HID_EP_INTR_IN,           /* Endpoint address and description */
    CY_U3P_USB_EP_INTR,             /* Interrupt Endpoint Type */
    0x02,0x00,                      /* Max packet size = 2 Bytes */
    0x05                            /* Polling interval : 2 ** (5-1) = 16 MicroFrames == 2 msec */    
};

/* Binary Device Object Store Descriptor */
const uint8_t CyFxUSBBOSDscr[] __attribute__ ((aligned (32))) =
{
    0x05,                           /* Descriptor Size */
    CY_FX_BOS_DSCR_TYPE,            /* Device Descriptor Type */
    0x16,0x00,                      /* Length of this descriptor and all sub descriptors */
    0x02,                           /* Number of device capability descriptors */

    /* USB 2.0 Extension */
    0x07,                           /* Descriptor Size */
    CY_FX_DEVICE_CAPB_DSCR_TYPE,    /* Device Capability Type descriptor */
    CY_U3P_USB2_EXTN_CAPB_TYPE,     /* USB 2.0 Extension Capability Type */
    0x1E,0x64,0x00,0x00,            /* Supported device level features: LPM support, BESL supported,
                                       Baseline BESL=400 us, Deep BESL=1000 us. */

    /* SuperSpeed Device Capability */
    0x0A,                           /* Descriptor Size */
    CY_FX_DEVICE_CAPB_DSCR_TYPE,    /* Device Capability Type descriptor */
    CY_U3P_SS_USB_CAPB_TYPE,        /* SuperSpeed Device Capability Type */
    0x00,                           /* Supported device level features  */
    0x0E,0x00,                      /* Speeds Supported by the device : SS, HS and FS */
    0x03,                           /* Functionality support */
    0x00,                           /* U1 Device Exit Latency */
    0x00,0x00                       /* U2 Device Exit Latency */
};

/* Standard Super Speed Configuration Descriptor */
const uint8_t CyFxUSBSSConfigDscr[] __attribute__ ((aligned (32))) =
{
    /* Configuration Descriptor Type */
    0x09,                           /* Descriptor Size */
    CY_U3P_USB_CONFIG_DESCR,        /* Configuration Descriptor Type */
    0x28,0x00,                      /* Length of this descriptor and all sub descriptors */
    0x01,                           /* Number of interfaces */
    0x01,                           /* Configuration number */
    0x00,                           /* Configuration string index */
    0xC0,                           /* Self powered device. */
    0x0C,                           /* Max power consumption of device (in 8mA unit) : 96 mA */

    /* Interface Descriptor */
    0x09,                           /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,        /* Interface Descriptor type */
    CY_FX_USB_HID_INTF,             /* Interface number */
    0x00,                           /* Alternate setting number */
    0x01,                           /* Number of end points */
    0x03,                           /* Interface class */
    0x00,                           /* Interface sub class : None */
    0x02,                           /* Interface protocol code */
    0x00,                           /* Interface descriptor string index */

    /* HID Descriptor (Mouse) */
    0x09,                           /* Descriptor size */
    CY_FX_USB_HID_DESC_TYPE,        /* Descriptor Type */
    0x10,0x11,                      /* HID Class Spec 11.1 */
    0x00,                           /* Target Country */
    0x01,                           /* Total HID Class Descriptors */
    0x22,                           /* Report Descriptor Type */
    0x1C,0x00,                      /* Total Length of Report Descriptor */      

    /* Endpoint Descriptor (Mouse) */
    0x07,                           /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,        /* Endpoint Descriptor Type */
    CY_FX_HID_EP_INTR_IN,           /* Endpoint address and description */
    CY_U3P_USB_EP_INTR,             /* Bulk End point Type */
    0x02,0x00,                      /* Max packet size = 2 bytes */
    0x05,                           /* Servicing interval is 2 ** (5 - 1) = 16 Intervals = 2 ms. */

    /* Super Speed Endpoint Companion Descriptor (Mouse) */
    0x06,                           /* Descriptor size */
    CY_FX_SS_EP_COMPN_DSCR_TYPE,    /* SS Endpoint Companion Descriptor Type */
    0x00,                           /* Max no. of packets in a Burst. */
    0x00,                           /* No streaming for Interrupt Endpoints. */
    0x02,0x00                       /* Number of bytes per interval = 2. */
};


/* Standard Language ID String Descriptor */
const uint8_t CyFxUSBStringLangIDDscr[] __attribute__ ((aligned (32))) =
{
    0x04,                           /* Descriptor Size */
    CY_U3P_USB_STRING_DESCR,        /* String Descriptor Type */
    0x09,0x04                       /* Language ID supported: US English */
};

/* Standard Manufacturer String Descriptor */
const uint8_t CyFxUSBManufactureDscr[] __attribute__ ((aligned (32))) =
{
    0x10,                           /* Descriptor Size */
    CY_U3P_USB_STRING_DESCR,        /* String Descriptor Type */
    'C',0x00,
    'y',0x00,
    'p',0x00,
    'r',0x00,
    'e',0x00,
    's',0x00,
    's',0x00
};


/* Standard Product String Descriptor */
const uint8_t CyFxUSBProductDscr[] __attribute__ ((aligned (32))) =
{
    0x08,                           /* Descriptor Size */
    CY_U3P_USB_STRING_DESCR,        /* String Descriptor Type */
    'F',0x00,
    'X',0x00,
    '3',0x00
};

/* Product Serial Number String Descriptor */
const uint8_t CyFxUSBSerialNumberDscr[] __attribute__ ((aligned (32))) =
{
    0x1A,                           /* Descriptor Size */
    CY_U3P_USB_STRING_DESCR,        /* String Descriptor Type */
    0x30, 0x00, 0x31, 0x00, 
    0x32, 0x00, 0x33, 0x00, 
    0x34, 0x00, 0x35, 0x00, 
    0x36, 0x00, 0x37, 0x00, 
    0x38, 0x00, 0x39, 0x00, 
    0x30, 0x00, 0x31, 0x00
};

/*[]*/

