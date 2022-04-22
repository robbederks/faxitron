/*
 ## Cypress FX2G2 Firmware Example Header (cyfx2g2_uvc.h)
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

#ifndef _INCLUDED_CYFX2G2_UVC_H_
#define _INCLUDED_CYFX2G2_UVC_H_

#include "cyu3types.h"
#include "cyu3usbconst.h"
#include "cyu3os.h"
#include "cyu3utils.h"

#include "cyu3externcstart.h"

/* This header file defines the UVC application constants and the video frame configurations */

/* Uncomment this to enable a timer that aborts an ongoing frame and restarts streaming when the transfer is
 * stalled. Default setting is timer enabled. */
#define RESET_TIMER_ENABLE                      1

/* Uncomment this to select UVC over Isochronous Endpoint mode. By default, a BULK endpoint is used for video
 * streaming. */
/*
#define USE_ISOCHRONOUS_ENDPOINT                1
 */

#define UVC_APP_THREAD_STACK                    (0x1000)        /* Thread stack size */
#define UVC_APP_THREAD_PRIORITY                 (8)             /* Thread priority */

/* Endpoint definition for UVC application */
#define EP_VIDEO_STREAM                         (0x83)          /* Video streaming endpoint: 3-IN. */
#define EP_CONTROL_STATUS                       (0x82)          /* Control and Status endpoint: 2-IN. */

#define EP_VIDEO_CONS_SOCKET                    (CY_U3P_UIB_SOCKET_CONS_3)      /* Consumer socket 3 */

/* UVC descriptor types */
#define INTRFC_ASSN_DESCR                       (11)    /* Interface association descriptor type. */
#define CS_INTRFC_DESCR                         (0x24)  /* Class Specific Interface Descriptor type: CS_INTERFACE */

/* DMA buffer size used: 3 KB. */
#define UVC_STREAM_BUF_SIZE                     (3072)

/* UVC header size in bytes. */
#define UVC_HEADER_SIZE                         (12)

/* Valid data size in each DMA buffer at HS: 2 HS lines = 640 * 2 * 2 */
#define UVC_VALID_DATA_SIZE_HS                  (2560)

/* Number of DMA buffers per video frame at HS: 480 / 2 */
#define UVC_BUF_PER_FRAME_HS                    (240)

/* Valid data size in each DMA buffer at FS: 1.5 FS lines = 320 * 2 * 1.5 */
#define UVC_VALID_DATA_SIZE_FS                  (960)

/* Number of DMA buffers per video frame at FS: 240 / 1.5 */
#define UVC_BUF_PER_FRAME_FS                    (160)

/* Number of DMA buffers used. We are using a value that does not divide the number of buffers from frame,
   to obtain a changing video effect. This should be set to as high a value as possible when implementing
   a complete sensor -> USB streaming application. */
#define UVC_STREAM_BUF_COUNT                    (14)

/* Bulk endpoint packet size for high speed: 512 bytes. */
#define UVC_BULK_EP_SIZE_HS                     (512)

/* Isochronous endpoint packet size for high speed: 1024 bytes. */
#define UVC_ISO_EP_SIZE_HS                      (1024)

/* Bulk endpoint packet size for high speed: 64 bytes. */
#define UVC_BULK_EP_SIZE_FS                     (64)

/* Isochronous endpoint packet size for full speed: 1023 bytes. */
#define UVC_ISO_EP_SIZE_FS                      (1023)

/* Isochronous Endpoint: Asynchronous Data transfer Mode. */
#define UVC_ISO_EP_ASYNC_MODE                   (0x04)

/* High speed Isochronous Endpoint: MULT setting. */
#define UVC_ISO_EP_HS_MULT                      (2 << 3)

/* Micro-frame duration in micro-seconds. */
#define USB_MICROFRAME_DURATION                 (125)

/* This example only supports UVC Spec Version 1.1, and will not work on Windows XP. */
#define UVC_MAX_PROBE_SETTING                   (34)    /* UVC 1.1 Maximum number of bytes in Probe Control */
#define UVC_MAX_PROBE_SETTING_ALIGNED           (64)    /* Maximum Probe Control size aligned to 32 bytes */

#define UVC_HEADER_DEFAULT_BFH                  (0x8C)  /* Default BFH (Bit Field Header) for the UVC Header */

#define UVC_HEADER_FRAME                        (0)                     /* Normal frame indication */
#define UVC_HEADER_EOF                          (uint8_t)(1 << 1)       /* End of frame indication */
#define UVC_HEADER_FRAME_ID                     (uint8_t)(1 << 0)       /* Frame ID toggle bit */

#define USB_UVC_SET_REQ_TYPE                    (uint8_t)(0x21)         /* UVC interface SET request type */
#define USB_UVC_GET_REQ_TYPE                    (uint8_t)(0xA1)         /* UVC Interface GET request type */
#define USB_UVC_GET_CUR_REQ                     (uint8_t)(0x81)         /* UVC GET_CUR request */
#define USB_UVC_SET_CUR_REQ                     (uint8_t)(0x01)         /* UVC SET_CUR request */
#define USB_UVC_GET_MIN_REQ                     (uint8_t)(0x82)         /* UVC GET_MIN Request */
#define USB_UVC_GET_MAX_REQ                     (uint8_t)(0x83)         /* UVC GET_MAX Request */
#define USB_UVC_GET_RES_REQ                     (uint8_t)(0x84)         /* UVC GET_RES Request */
#define USB_UVC_GET_LEN_REQ                     (uint8_t)(0x85)         /* UVC GET_LEN Request */
#define USB_UVC_GET_INFO_REQ                    (uint8_t)(0x86)         /* UVC GET_INFO Request */
#define USB_UVC_GET_DEF_REQ                     (uint8_t)(0x87)         /* UVC GET_DEF Request */

#define UVC_VS_PROBE_CONTROL                    (0x0100)                /* Video Stream Probe Control Request */
#define UVC_VS_COMMIT_CONTROL                   (0x0200)                /* Video Stream Commit Control Request */
#define UVC_VC_REQUEST_ERROR_CODE_CONTROL       (0x0200)                /* Request Control Error Code */
#define UVC_ERROR_INVALID_CONTROL               (0x06)                  /* Error indicating invalid control */
#define UVC_STREAM_INTERFACE                    (0x01)                  /* Streaming Interface : Alternate setting 1 */
#define UVC_CONTROL_INTERFACE                   (0x00)                  /* Control Interface: Alternate Setting 0 */

/* Event to restart a video frame that has taken too long. */
#define DMA_RESET_EVENT                         (1<<4)

/* Event generated on a USB Suspend Request*/
#define USB_SUSP_EVENT_FLAG                     (1<<5)

/* Extern definitions of the USB Enumeration constant arrays used for the Application */
extern const uint8_t CyFx2g2USB20DeviceDscr[];
extern const uint8_t CyFx2g2USBDeviceQualDscr[];
extern const uint8_t CyFx2g2USBHSConfigDscr[];
extern const uint8_t CyFx2g2USBFSConfigDscr[];
extern const uint8_t CyFx2g2USBLangIDString[];
extern const uint8_t CyFx2g2USBManufacturerString[];
extern const uint8_t CyFx2g2USBProductString[];
extern const uint8_t CyFx2g2USBHSConfigString[];
extern const uint8_t CyFx2g2USBFSConfigString[];

/* UVC Probe Control Settings */
extern uint8_t       glProbeCtrl[UVC_MAX_PROBE_SETTING];            /* Scratch buffer. */
extern const uint8_t glVga30ProbeCtrl[UVC_MAX_PROBE_SETTING];       /* 640*480 (VGA) x30 settings. */
extern const uint8_t glQvga5ProbeCtrl[UVC_MAX_PROBE_SETTING];       /* 320*240 (QVGA) x5 settings. */

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYFX2G2_UVC_H_ */
