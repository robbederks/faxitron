/*
 ## Cypress FX2G2 Firmware Example Source (cyfx2g2_dscr.c)
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


/* This file defines the USB descriptors used by the FX2G2 UVC firmware example. The example assumes that
 * the Data Cache is not enabled; and therefore does not align these descriptors to cache lines.
 *
 * As FX2G2 is a USB 2.0 only device, we do not provide any USB 3.0 or BOS descriptors.
 */

#include <cyu3utils.h>
#include "cyfx2g2_uvc.h"

/* Standard Device Descriptor for USB 2.0 */
const uint8_t CyFx2g2USB20DeviceDscr[] =
{
    0x12,                               /* Descriptor size */
    CY_U3P_USB_DEVICE_DESCR,            /* Device descriptor type */
    0x00, 0x02,                         /* USB 2.1 */
    0xEF,                               /* Device class */
    0x02,                               /* Device sub-class */
    0x01,                               /* Device protocol */
    0x40,                               /* Maxpacket size for EP0 : 64 bytes */
    0xB4, 0x04,                         /* Vendor ID */
    0x22, 0x47,                         /* Product ID */
    0x00, 0x00,                         /* Device release number */
    0x01,                               /* Manufacture string index */
    0x02,                               /* Product string index */
    0x00,                               /* Serial number string index */
    0x01                                /* Number of configurations */
};

/* Standard Device Qualifier Descriptor */
const uint8_t CyFx2g2USBDeviceQualDscr[] =
{
    0x0A,                               /* descriptor size */
    CY_U3P_USB_DEVQUAL_DESCR,           /* Device qualifier descriptor type */
    0x00, 0x02,                         /* USB 2.0 */
    0xEF,                               /* Device class */
    0x02,                               /* Device sub-class */
    0x01,                               /* Device protocol */
    0x40,                               /* Maxpacket size for EP0 : 64 bytes */
    0x01,                               /* Number of configurations */
    0x00                                /* Reserved */
};

/* Standard High Speed Configuration Descriptor */
const uint8_t CyFx2g2USBHSConfigDscr[] =
{
    /* Configuration descriptor */
    0x09,                               /* Descriptor size */
    CY_U3P_USB_CONFIG_DESCR,            /* Configuration descriptor type */
#ifdef USE_ISOCHRONOUS_ENDPOINT
    0xD7,0x00,                          /* Length of this descriptor and all sub descriptors = 215 */
#else
    0xCE,0x00,                          /* Length of this descriptor and all sub descriptors = 206 */
#endif
    0x02,                               /* Number of interfaces */
    0x01,                               /* Configuration number */
    0x03,                               /* Configuration string index */
    0xC0,                               /* Config characteristics - self powered */
    0x32,                               /* Max power consumption of device (in 2mA unit) : 100mA */

    /* Interface Association Descriptor */
    0x08,                               /* Descriptor Size */
    INTRFC_ASSN_DESCR,                  /* Interface Association Descriptor Type */
    0x00,                               /* Interface number of the VideoControl interface
                                           that is associated with this function */
    0x02,                               /* Number of contiguous Video interfaces that are
                                           associated with this function */
    0x0E,                               /* Video Interface Class Code: CC_VIDEO */
    0x03,                               /* Subclass code: SC_VIDEO_INTERFACE_COLLECTION */
    0x00,                               /* Protocol: PC_PROTOCOL_UNDEFINED */
    0x00,                               /* String Descriptor index for interface */

    /* Standard Video Control Interface Descriptor (Interface 0, Alternate Setting 0) */
    0x09,                               /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,            /* Interface Descriptor type */
    0x00,                               /* Index of this Interface */
    0x00,                               /* Alternate setting number */
    0x01,                               /* Number of end points - 1 Interrupt Endpoint */
    0x0E,                               /* Video Interface Class Code: CC_VIDEO  */
    0x01,                               /* Interface sub class: SC_VIDEOCONTROL */
    0x00,                               /* Interface protocol code: PC_PROTOCOL_UNDEFINED */
    0x00,                               /* Interface descriptor string index */

    /* Class specific VC Interface Header Descriptor */
    0x0D,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class Specific Interface Descriptor type: CS_INTERFACE */
    0x01,                               /* Descriptor Sub type: VC_HEADER */
    0x10, 0x01,                         /* Revision of UVC class spec: 1.1 - Version required for USB Compliance. */
    0x51, 0x00,                         /* Total Size of class specific descriptors = 81 */
    0x00, 0x6C, 0xDC, 0x02,             /* Clock frequency : 48MHz (Deprecated) */
    0x01,                               /* Number of streaming interfaces */
    0x01,                               /* VideoStreaming interface 1 belongs to this VideoControl interface */

    /* Input (Camera) Terminal Descriptor */
    0x12,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* CS_INTERFACE */
    0x02,                               /* VC_INPUT_TERMINAL subtype */
    0x01,                               /* ID of this input terminal */
    0x01, 0x02,                         /* ITT_CAMERA type. This terminal is a camera terminal representing the
                                           CCD sensor */
    0x00,                               /* No association terminal */
    0x00,                               /* Unused */
    0x00, 0x00,                         /* No optical zoom supported */
    0x00, 0x00,                         /* No optical zoom supported */
    0x00, 0x00,                         /* No optical zoom supported */
    0x03,                               /* Size of controls field for this terminal : 3 bytes */
    0x00, 0x00, 0x00,                   /* No controls supported */

    /* Processing Unit Descriptor */
    0x0D,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class specific interface desc type */
    0x05,                               /* Processing Unit Descriptor type: VC_PROCESSING_UNIT */
    0x02,                               /* ID of this unit */
    0x01,                               /* Source ID: 1: Conencted to input terminal */
    0x00, 0x40,                         /* Digital multiplier */
    0x03,                               /* Size of controls field for this terminal: 3 bytes */
    0x00, 0x00, 0x00,                   /* No controls supported */
    0x00,                               /* String desc index: Not used */
    0x00,                               /* Analog Video Standards Supported: None */

    /* Extension Unit Descriptor */
    0x1C,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class specific interface desc type */
    0x06,                               /* Extension Unit Descriptor type */
    0x03,                               /* ID of this terminal */
    0xFF, 0xFF, 0xFF, 0xFF,             /* 16 byte GUID */
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0x00,                               /* Number of controls in this terminal */
    0x01,                               /* Number of input pins in this terminal */
    0x02,                               /* Source ID : 2 : Connected to Proc Unit */
    0x03,                               /* Size of controls field for this terminal : 3 bytes */
    0x00, 0x00, 0x00,                   /* No controls supported */
    0x00,                               /* String descriptor index : Not used */

    /* Output Terminal Descriptor */
    0x09,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class specific interface desc type */
    0x03,                               /* Output Terminal Descriptor type */
    0x04,                               /* ID of this terminal */
    0x01, 0x01,                         /* USB Streaming terminal type */
    0x00,                               /* No association terminal */
    0x03,                               /* Source ID : 3 : Connected to Extn Unit */
    0x00,                               /* String desc index : Not used */

    /* Video Control Status Interrupt Endpoint Descriptor */
    0x07,                               /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,            /* Endpoint Descriptor Type */
    EP_CONTROL_STATUS,                  /* Endpoint address and description: EP-2 IN */
    CY_U3P_USB_EP_INTR,                 /* Interrupt End point Type */
    0x40, 0x00,                         /* Max packet size: 64 bytes */
    0x01,                               /* Servicing interval */

    /* Class Specific Interrupt Endpoint Descriptor */
    0x05,                               /* Descriptor size */
    0x25,                               /* Class specific endpoint descriptor type */
    CY_U3P_USB_EP_INTR,                 /* End point sub type */
    0x40,0x00,                          /* Max packet size = 64 bytes */

    /* Standard Video Streaming Interface Descriptor (Interface 1, Alternate Setting 0) */
    0x09,                               /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,            /* Interface descriptor type */
    0x01,                               /* Interface number: 1 */
    0x00,                               /* Alternate setting number: 0 */
#ifdef USE_ISOCHRONOUS_ENDPOINT
    0x00,                               /* No endpoints. */
#else
    0x01,                               /* 1 Bulk endpoint */
#endif
    0x0E,                               /* Interface class : CC_VIDEO */
    0x02,                               /* Interface sub class : SC_VIDEOSTREAMING */
    0x00,                               /* Interface protocol code : PC_PROTOCOL_UNDEFINED */
    0x00,                               /* Interface descriptor string index */

    /* Class-specific Video Streaming Input Header Descriptor */
    0x0E,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class-specific VS interface type */
    0x01,                               /* Descriptotor subtype : Input Header */
    0x01,                               /* 1 format desciptor follows */
    0x47, 0x00,                         /* Total size of class specific VS descr = 71 */
    EP_VIDEO_STREAM,                    /* EP address for BULK video data: EP 3 IN  */
    0x00,                               /* No dynamic format change supported */
    0x04,                               /* Output terminal ID : 4 */
    0x00,                               /* No Still image capture method supported */
    0x00,                               /* Hardware trigger not supported */
    0x00,                               /* Hardware to initiate still image capture not supported */
    0x01,                               /* Size of controls field : 1 byte */
    0x00,                               /* D2 : Compression quality supported - No compression */

    /* Class specific VS format descriptor */
    0x1B,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class-specific VS interface Type */
    0x04,                               /* Subtype : VS_FORMAT_UNCOMPRESSED  */
    0x01,                               /* Format desciptor index */
    0x01,                               /* Number of Frame Descriptors that follow this descriptor: 1 */
    0x59, 0x55, 0x59, 0x32,             /* MEDIASUBTYPE_YUY2 GUID: 32595559-0000-0010-8000-00AA00389B71 */
    0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0xAA,
    0x00, 0x38, 0x9B, 0x71,
    0x10,                               /* Number of bits per pixel: 16 */
    0x01,                               /* Optimum Frame Index for this stream: 1 (640x480) */
    0x00,                               /* X dimension of the picture aspect ratio; Non-interlaced */
    0x00,                               /* Y dimension of the pictuer aspect ratio: Non-interlaced */
    0x00,                               /* Interlace Flags: Progressive scanning, no interlace */
    0x00,                               /* duplication of the video stream restriction: 0 - no restriction */

    /* Class specific Uncompressed VS frame descriptor 1 - 640 x 480 @ 30 fps */
    0x1E,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Descriptor type */
    0x05,                               /* Subtype: Uncompressed frame interface */
    0x01,                               /* Frame Descriptor Index: 1 */
    0x00,                               /* No Still image capture method supported */
    0x80, 0x02,                         /* Width in pixel:  640 */
    0xE0, 0x01,                         /* Height in pixel: 480 */
    0x00, 0x00, 0xCA, 0x08,             /* Min bit rate (bits/s): 640 x 480 x 2 x 30 x 8 = 147456000 */
    0x00, 0x00, 0xCA, 0x08,             /* Max bit rate (bits/s): Fixed rate so same as Min */
    0x00, 0x60, 0x09, 0x00,             /* Maximum video or still frame size in bytes (Deprecated): 640 x 480 x 2 */
    0x15, 0x16, 0x05, 0x00,             /* Default frame interval (in 100ns units): 333333 */
    0x01,                               /* Frame interval type : No of discrete intervals */
    0x15, 0x16, 0x05, 0x00,             /* Frame interval 3: Same as Default frame interval */

#ifdef USE_ISOCHRONOUS_ENDPOINT
    /* Standard Video Streaming Interface Descriptor (Interface 1, Alternate Setting 1) */
    0x09,                               /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,            /* Interface descriptor type */
    0x01,                               /* Interface number: 1 */
    0x01,                               /* Alternate setting number: 1 */
    0x01,                               /* 1 Isochronous endpoint */
    0x0E,                               /* Interface class : CC_VIDEO */
    0x02,                               /* Interface sub class : SC_VIDEOSTREAMING */
    0x00,                               /* Interface protocol code : PC_PROTOCOL_UNDEFINED */
    0x00,                               /* Interface descriptor string index */
#endif

    /* Endpoint descriptor for Bulk streaming video data */
    0x07,                               /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,            /* Endpoint Descriptor Type */
    EP_VIDEO_STREAM,                    /* Endpoint address and description: EP 3 IN */

#ifdef USE_ISOCHRONOUS_ENDPOINT
                                        /* Isochronous (Asynchronous Data EP) endpoint transferring three
                                         * 1024 byte packets per micro-frame. */
    (CY_U3P_USB_EP_ISO | UVC_ISO_EP_ASYNC_MODE),
    CY_U3P_GET_LSB(UVC_ISO_EP_SIZE_HS),
    (CY_U3P_GET_MSB(UVC_ISO_EP_SIZE_HS) | UVC_ISO_EP_HS_MULT),
    0x01
#else
                                        /* Bulk endpoint with a maximum packet size of 512 bytes. */
    CY_U3P_USB_EP_BULK,
    CY_U3P_GET_LSB(UVC_BULK_EP_SIZE_HS),
    CY_U3P_GET_MSB(UVC_BULK_EP_SIZE_HS),
    0x00
#endif
};

/* Standard Full Speed Configuration Descriptor */
const uint8_t CyFx2g2USBFSConfigDscr[] =
{
    /* Configuration descriptor */
    0x09,                               /* Descriptor size */
    CY_U3P_USB_CONFIG_DESCR,            /* Configuration descriptor type */
#ifdef USE_ISOCHRONOUS_ENDPOINT
    0xD7,0x00,                          /* Length of this descriptor and all sub descriptors = 215 */
#else
    0xCE,0x00,                          /* Length of this descriptor and all sub descriptors = 206 */
#endif
    0x02,                               /* Number of interfaces */
    0x01,                               /* Configuration number */
    0x04,                               /* Configuration string index */
    0xC0,                               /* Config characteristics - self powered */
    0x32,                               /* Max power consumption of device (in 2mA unit) : 100mA */

    /* Interface Association Descriptor */
    0x08,                               /* Descriptor Size */
    INTRFC_ASSN_DESCR,                  /* Interface Association Descriptor Type */
    0x00,                               /* Interface number of the VideoControl interface
                                           that is associated with this function */
    0x02,                               /* Number of contiguous Video interfaces that are
                                           associated with this function */
    0x0E,                               /* Video Interface Class Code: CC_VIDEO */
    0x03,                               /* Subclass code: SC_VIDEO_INTERFACE_COLLECTION */
    0x00,                               /* Protocol: PC_PROTOCOL_UNDEFINED */
    0x00,                               /* String Descriptor index for interface */

    /* Standard Video Control Interface Descriptor (Interface 0, Alternate Setting 0) */
    0x09,                               /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,            /* Interface Descriptor type */
    0x00,                               /* Index of this Interface */
    0x00,                               /* Alternate setting number */
    0x01,                               /* Number of end points - 1 Interrupt Endpoint */
    0x0E,                               /* Video Interface Class Code: CC_VIDEO  */
    0x01,                               /* Interface sub class: SC_VIDEOCONTROL */
    0x00,                               /* Interface protocol code: PC_PROTOCOL_UNDEFINED. */
    0x00,                               /* Interface descriptor string index */

    /* Class specific VC Interface Header Descriptor */
    0x0D,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class Specific Interface Descriptor type: CS_INTERFACE */
    0x01,                               /* Descriptor Sub type: VC_HEADER */
    0x10, 0x01,                         /* Revision of UVC class spec: 1.1 */
    0x51, 0x00,                         /* Total Size of class specific descriptors = 81 */
    0x00, 0x6C, 0xDC, 0x02,             /* Clock frequency : 48MHz(Deprecated) */
    0x01,                               /* Number of streaming interfaces */
    0x01,                               /* VideoStreaming interface 1 belongs to this VideoControl interface */

    /* Input (Camera) Terminal Descriptor */
    0x12,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* CS_INTERFACE */
    0x02,                               /* VC_INPUT_TERMINAL subtype */
    0x01,                               /* ID of this input terminal */
    0x01, 0x02,                         /* ITT_CAMERA type. */
    0x00,                               /* No association terminal */
    0x00,                               /* Unused */
    0x00, 0x00,                         /* No optical zoom supported */
    0x00, 0x00,                         /* No optical zoom supported */
    0x00, 0x00,                         /* No optical zoom supported */
    0x03,                               /* Size of controls field for this terminal : 3 bytes */
    0x00, 0x00, 0x00,                   /* No controls supported */

    /* Processing Unit Descriptor */
    0x0D,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class specific interface desc type */
    0x05,                               /* Processing Unit Descriptor type: VC_PROCESSING_UNIT */
    0x02,                               /* ID of this unit */
    0x01,                               /* Source ID: 1: Conencted to input terminal */
    0x00, 0x40,                         /* Digital multiplier */
    0x03,                               /* Size of controls field for this terminal: 3 bytes */
    0x00, 0x00, 0x00,                   /* No controls supported */
    0x00,                               /* String desc index: Not used */
    0x00,                               /* Analog Video Standards Supported: None */

    /* Extension Unit Descriptor */
    0x1C,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class specific interface desc type */
    0x06,                               /* Extension Unit Descriptor type */
    0x03,                               /* ID of this terminal */
    0xFF, 0xFF, 0xFF, 0xFF,             /* 16 byte GUID */
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0x00,                               /* Number of controls in this terminal */
    0x01,                               /* Number of input pins in this terminal */
    0x02,                               /* Source ID : 2 : Connected to Proc Unit */
    0x03,                               /* Size of controls field for this terminal : 3 bytes */
    0x00, 0x00, 0x00,                   /* No controls supported */
    0x00,                               /* String descriptor index : Not used */

    /* Output Terminal Descriptor */
    0x09,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class specific interface desc type */
    0x03,                               /* Output Terminal Descriptor type */
    0x04,                               /* ID of this terminal */
    0x01, 0x01,                         /* USB Streaming terminal type */
    0x00,                               /* No association terminal */
    0x03,                               /* Source ID : 3 : Connected to Extn Unit */
    0x00,                               /* String desc index : Not used */

    /* Video Control Status Interrupt Endpoint Descriptor */
    0x07,                               /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,            /* Endpoint Descriptor Type */
    EP_CONTROL_STATUS,                  /* Endpoint address and description: EP-2 IN */
    CY_U3P_USB_EP_INTR,                 /* Interrupt End point Type */
    0x40, 0x00,                         /* Max packet size: 64 bytes */
    0x01,                               /* Servicing interval */

    /* Class Specific Interrupt Endpoint Descriptor */
    0x05,                               /* Descriptor size */
    0x25,                               /* Class specific endpoint descriptor type */
    CY_U3P_USB_EP_INTR,                 /* End point sub type */
    0x40,0x00,                          /* Max packet size = 64 bytes */

    /* Standard Video Streaming Interface Descriptor (Interface 1, Alternate Setting 0) */
    0x09,                               /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,            /* Interface descriptor type */
    0x01,                               /* Interface number: 1 */
    0x00,                               /* Alternate setting number: 0 */
#ifdef USE_ISOCHRONOUS_ENDPOINT
    0x00,                               /* No endpoints. */
#else
    0x01,                               /* 1 Bulk endpoint */
#endif
    0x0E,                               /* Interface class : CC_VIDEO */
    0x02,                               /* Interface sub class : SC_VIDEOSTREAMING */
    0x00,                               /* Interface protocol code : PC_PROTOCOL_UNDEFINED */
    0x00,                               /* Interface descriptor string index */

    /* Class-specific Video Streaming Input Header Descriptor */
    0x0E,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class-specific VS interface type */
    0x01,                               /* Descriptotor subtype : Input Header */
    0x01,                               /* 1 format desciptor follows */
    0x47, 0x00,                         /* Total size of class specific VS descr = 71 */
    EP_VIDEO_STREAM,                    /* EP address for BULK video data: EP 3 IN  */
    0x00,                               /* No dynamic format change supported */
    0x04,                               /* Output terminal ID : 4 */
    0x00,                               /* No Still image capture method supported */
    0x00,                               /* Hardware trigger not supported */
    0x00,                               /* Hardware to initiate still image capture not supported */
    0x01,                               /* Size of controls field : 1 byte */
    0x00,                               /* D2 : Compression quality supported - No compression */

    /* Class specific VS format descriptor */
    0x1B,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Class-specific VS interface Type */
    0x04,                               /* Subtype : VS_FORMAT_UNCOMPRESSED  */
    0x01,                               /* Format desciptor index */
    0x01,                               /* Number of Frame Descriptors that follow this descriptor: 1 */
    0x59, 0x55, 0x59, 0x32,             /* MEDIASUBTYPE_YUY2 GUID: 32595559-0000-0010-8000-00AA00389B71 */
    0x00, 0x00, 0x10, 0x00,
    0x80, 0x00, 0x00, 0xAA,
    0x00, 0x38, 0x9B, 0x71,
    0x10,                               /* Number of bits per pixel: 16 */
    0x01,                               /* Optimum Frame Index for this stream: 1 (320x240) */
    0x00,                               /* X dimension of the picture aspect ratio; Non-interlaced */
    0x00,                               /* Y dimension of the pictuer aspect ratio: Non-interlaced */
    0x00,                               /* Interlace Flags: Progressive scanning, no interlace */
    0x00,                               /* duplication of the video stream restriction: 0 - no restriction */

    /* Class specific Uncompressed VS Frame Descriptor 1 - 320 * 240 @ 5 fps */
    0x1E,                               /* Descriptor size */
    CS_INTRFC_DESCR,                    /* Descriptor type */
    0x05,                               /* Subtype: Uncompressed frame interface */
    0x01,                               /* Frame Descriptor Index: 1 */
    0x00,                               /* No Still image capture method supported */
    0x40, 0x01,                         /* Width in pixel:  320 */
    0xF0, 0x00,                         /* Height in pixel: 240 */
    0x00, 0xC0, 0x5D, 0x00,             /* Min bit rate (bits/s): 320 x 240 x 2 x 5 x 8 = 6144000 */
    0x00, 0xC0, 0x5D, 0x00,             /* Max bit rate (bits/s): Fixed rate so same as Min  */
    0x00, 0x58, 0x02, 0x00,             /* Maximum video or still frame size in bytes(Deprecated): 320 x 240 x 2 */
    0x80, 0x84, 0x1E, 0x00,             /* Default frame interval (in 100ns units): 200 ms */
    0x01,                               /* Frame interval type : No of discrete intervals */
    0x80, 0x84, 0x1E, 0x00,             /* Frame interval 3: Same as Default frame interval */

#ifdef USE_ISOCHRONOUS_ENDPOINT
    /* Standard Video Streaming Interface Descriptor (Interface 1, Alternate Setting 1) */
    0x09,                               /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,            /* Interface descriptor type */
    0x01,                               /* Interface number: 1 */
    0x01,                               /* Alternate setting number: 1 */
    0x01,                               /* 1 Isochronous endpoint */
    0x0E,                               /* Interface class : CC_VIDEO */
    0x02,                               /* Interface sub class : SC_VIDEOSTREAMING */
    0x00,                               /* Interface protocol code : PC_PROTOCOL_UNDEFINED */
    0x00,                               /* Interface descriptor string index */
#endif

    /* Endpoint descriptor for Bulk streaming video data */
    0x07,                               /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,            /* Endpoint Descriptor Type */
    EP_VIDEO_STREAM,                    /* Endpoint address and description: EP 3 IN */

#ifdef USE_ISOCHRONOUS_ENDPOINT
    CY_U3P_USB_EP_ISO,                  /* Isochronous endpoint transferring 1023 bytes per frame. */
    CY_U3P_GET_LSB(UVC_ISO_EP_SIZE_FS),
    CY_U3P_GET_MSB(UVC_ISO_EP_SIZE_FS),
    0x01
#else
    CY_U3P_USB_EP_BULK,                 /* Bulk endpoint with a max. packet size of 64 bytes. */
    CY_U3P_GET_LSB(UVC_BULK_EP_SIZE_FS),
    CY_U3P_GET_MSB(UVC_BULK_EP_SIZE_FS),
    0x00
#endif
};

/* Standard language ID string descriptor */
const uint8_t CyFx2g2USBLangIDString[] =
{
    0x04,                               /* Descriptor size */
    CY_U3P_USB_STRING_DESCR,            /* Device descriptor type */
    0x09,0x04                           /* Language ID supported */
};

/* Standard manufacturer string descriptor */
const uint8_t CyFx2g2USBManufacturerString[] =
{
    0x10,                               /* Descriptor size */
    CY_U3P_USB_STRING_DESCR,            /* Device descriptor type */
    'C',0x00,                           /* Manufacturer String */
    'y',0x00,
    'p',0x00,
    'r',0x00,
    'e',0x00,
    's',0x00,
    's',0x00
};

/* Standard product string descriptor */
const uint8_t CyFx2g2USBProductString[] =
{
    0x14,                               /* Descriptor Size */
    CY_U3P_USB_STRING_DESCR,            /* Device descriptor type */
    'F', 0x00,                          /* Product Descriptor String */
    'X', 0x00,
    '2', 0x00,
    'G', 0x00,
    '2', 0x00,
    '-', 0x00,
    'U', 0x00,
    'V', 0x00,
    'C', 0x00
};

/* Standard product string descriptor */
const uint8_t CyFx2g2USBHSConfigString[] =
{
    0x10,                               /* Descriptor Size */
    CY_U3P_USB_STRING_DESCR,            /* Device descriptor type */
    'U', 0x00,                          /* High-Speed Configuration Descriptor */
    'S', 0x00,
    'B', 0x00,
    '-', 0x00,
    '2', 0x00,
    '.', 0x00,
    '0', 0x00
};
/* Standard product string descriptor */
const uint8_t CyFx2g2USBFSConfigString[] =
{
    0x10,                               /* Descriptor Size */
    CY_U3P_USB_STRING_DESCR,            /* Device descriptor type */
    'U', 0x00,                          /* Full-Speed Configuration Descriptor */
    'S', 0x00,
    'B', 0x00,
    '-', 0x00,
    '1', 0x00,
    '.', 0x00,
    '1', 0x00
};

/* UVC Probe Control Settings */
uint8_t glProbeCtrl[UVC_MAX_PROBE_SETTING] = {
    0x00, 0x00,                         /* bmHint : No fixed parameters */
    0x01,                               /* Use 1st Video format index */
    0x01,                               /* Use 1st Video frame index */
    0x15, 0x16, 0x05, 0x00,             /* Desired frame interval in 100ns = 33.33 ms */
    0x00, 0x00,                         /* Key frame rate in key frame/video frame units */
    0x00, 0x00,                         /* PFrame rate in PFrame / key frame units */
    0x00, 0x00,                         /* Compression quality control */
    0x00, 0x00,                         /* Window size for average bit rate */
    0x00, 0x00,                         /* Internal video streaming i/f latency in ms */
    0x00, 0x60, 0x09, 0x00,             /* Max video frame size in bytes */
    0x00, 0x0C, 0x00, 0x00,             /* No. of bytes device can rx in single payload: 3 KB */
    0x00, 0x60, 0xE3, 0x16,             /* Device Clock */
    0x00,                               /* Framing Information - Ignored for uncompressed format */
    0x00,                               /* Preferred payload format version */
    0x00,                               /* Minimum payload format version */
    0x00                                /* Maximum payload format version */
};

/* UVC Probe Control Setting - VGA @30fps (High Speed) */
uint8_t const glVga30ProbeCtrl[UVC_MAX_PROBE_SETTING] = {
    0x00, 0x00,                         /* bmHint : No fixed parameters */
    0x01,                               /* Use 1st Video format index */
    0x01,                               /* Use 1st Video frame index */
    0x15, 0x16, 0x05, 0x00,             /* Desired frame interval in 100ns = 33.33 ms */
    0x00, 0x00,                         /* Key frame rate in key frame/video frame units */
    0x00, 0x00,                         /* PFrame rate in PFrame / key frame units */
    0x00, 0x00,                         /* Compression quality control */
    0x00, 0x00,                         /* Window size for average bit rate */
    0x00, 0x00,                         /* Internal video streaming i/f latency in ms */
    0x00, 0x60, 0x09, 0x00,             /* Max video frame size in bytes: 640 x 480 x 2 */
    0x00, 0x0C, 0x00, 0x00,             /* No. of bytes device can rx in single payload: 3 KB */
    0x00, 0x60, 0xE3, 0x16,             /* Device Clock */
    0x00,                               /* Framing Information - Ignored for uncompressed format */
    0x00,                               /* Preferred payload format version */
    0x00,                               /* Minimum payload format version */
    0x00                                /* Maximum payload format version */
};

/* UVC Probe Control Setting - QVGA @5fps (Full Speed) */
uint8_t const glQvga5ProbeCtrl[UVC_MAX_PROBE_SETTING] = {
    0x00, 0x00,                         /* bmHint : No fixed parameters */
    0x01,                               /* Use 1st Video format index */
    0x01,                               /* Use 1st Video frame index */
    0x80, 0x84, 0x1E, 0x00,             /* Desired frame interval in 100ns = 200 ms */
    0x00, 0x00,                         /* Key frame rate in key frame/video frame units */
    0x00, 0x00,                         /* PFrame rate in PFrame / key frame units */
    0x00, 0x00,                         /* Compression quality control */
    0x00, 0x00,                         /* Window size for average bit rate */
    0x00, 0x00,                         /* Internal video streaming i/f latency in ms */
    0x00, 0x58, 0x02, 0x00,             /* Max video frame size in bytes: 320 x 240 x 2 */
    0xFF, 0x03, 0x00, 0x00,             /* No. of bytes device can rx in single payload: 1023 bytes */
    0x00, 0x60, 0xE3, 0x16,             /* Device Clock */
    0x00,                               /* Framing Information - Ignored for uncompressed format */
    0x00,                               /* Preferred payload format version */
    0x00,                               /* Minimum payload format version */
    0x00                                /* Maximum payload format version */
};

/* [ ] */

