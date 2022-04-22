/*
 ## Cypress USB 3.0 Platform header file (cyfxmscdemo.h)
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

/* This file contains the constants and definitions for the MSC application example */

#ifndef _INCLUDED_CYFXMSCDEMO_H_
#define _INCLUDED_CYFXMSCDEMO_H_

#include <cyu3types.h>
#include <cyu3usbconst.h>
#include <cyu3externcstart.h>

/* Endpoint and socket definitions for the MSC application */

/* To change the Producer and Consumer EP enter the appropriate EP numbers for the #defines.
 * In the case of IN endpoints enter EP number along with the direction bit.
 * For eg. EP 6 IN endpoint is 0x86
 *     and EP 6 OUT endpoint is 0x06.
 * To change sockets mention the appropriate socket number in the #defines. */

/* Note: For USB 2.0 the endpoints and corresponding sockets are one-to-one mapped
         i.e. EP 1 is mapped to UIB socket 1 and EP 2 to socket 2 so on */

#define CY_FX_MSC_EP_BULK_OUT           (0x01)          /* EP 1 OUT */
#define CY_FX_MSC_EP_BULK_IN            (0x81)          /* EP 1 IN */

#define CY_FX_MSC_EP_BULK_OUT_SOCKET    (0x01)          /* Socket 1 is producer */
#define CY_FX_MSC_EP_BULK_IN_SOCKET     (0x01)          /* Socket 1 is consumer */

#define CY_FX_MSC_DMA_BUF_COUNT         (8)             /* MSC channel buffer count */
#define CY_FX_MSC_EP_BURST_SIZE         (4)             /* Burst setting for the endpoint. */

#define CY_FX_MSC_THREAD_STACK          (0x1000)        /* MSC application thread stack size */
#define CY_FX_MSC_THREAD_PRIORITY       (8)             /* MSC application thread priority */

#define CY_FX_SIB_PARTITIONS            (2)		/* 2 Partitions for this application per port */
#define CY_FX_SIB_PORTS                 (2)		/* Maximum ports for FX3S */
#define CY_FX_SIB_MAX_BLOCK_SIZE        (0x200)         /* Max allowed block size for SIB */

#define CY_FX_MSC_CLR_STALL_IN_EVENT_FLAG       (1 << 0)        /* MSC application Clear Stall IN Event Flag */
#define CY_FX_MSC_RESET_EVENT_FLAG              (1 << 1)        /* USB reset event. */
#define CY_FX_MSC_SETCONF_EVENT_FLAG            (1 << 2)        /* SET Config event */
#define CY_FX_MSC_CBW_EVENT_FLAG                (1 << 3)        /* MSC CBW received. */
#define CY_FX_MSC_DATASENT_EVENT_FLAG           (1 << 4)        /* Data has been sent to the host. */
#define CY_FX_MSC_SIBCB_EVENT_FLAG              (1 << 5)        /* SIB event callback flag. */
#define CY_FX_MSC_USB_SUSP_EVENT_FLAG           (1 << 6)        /* USB Suspend event flag. */
#define CY_FX_MSC_DATA_RECV_EVENT_FLAG          (1 << 7)        /* Data received on USB endpoint. */
#define CY_FX_MSC_WR_COMMIT_EVENT_FLAG          (1 << 8)        /* Write commit event flag. */

#define CY_FX_MSC_USB_REQ_MASK                  (0x60)          /* Request type mask */
#define CY_FX_MSC_USB_STANDARD_REQ              (0x00)          /* Standard request type */
#define CY_FX_MSC_USB_CLASS_REQ                 (0x20)          /* Class request type */
#define CY_FX_MSC_USB_VENDOR_REQ                (0x40)          /* Vendor request type */

#define CY_FX_MSC_GET_MAX_LUN_REQ               (0xFE)          /* MSC Get Max LUN request */
#define CY_FX_MSC_BOT_RESET_REQ                 (0xFF)          /* MSC BOT Reset request */
#define CY_FX_MSC_USB_REQ_WINDEX_MASK           (0x0000FFFF)    /* USB Request wIndex mask */

/* Mass storage interface number */
#define CY_FX_USB_MSC_INTF                      (0x00)

/* Descriptor Types */
#define CY_FX_BOS_DSCR_TYPE                     (15)
#define CY_FX_DEVICE_CAPB_DSCR_TYPE             (16)
#define CY_FX_SS_EP_COMPN_DSCR_TYPE             (48)

/* Device Capability Type Codes */
#define CY_FX_WIRELESS_USB_CAPB_TYPE            (1)
#define CY_FX_USB2_EXTN_CAPB_TYPE               (2)
#define CY_FX_SS_USB_CAPB_TYPE                  (3)
#define CY_FX_CONTAINER_ID_CAPBD_TYPE           (4)

typedef enum
{
    CY_FX_CBW_CMD_PASSED = 0,
    CY_FX_CBW_CMD_FAILED,
    CY_FX_CBW_CMD_PHASE_ERROR,
    CY_FX_CBW_CMD_MSC_RESET
} CyFxMscCswReturnStatus_t;

/* Current state of the MSC function state machine. */
typedef enum
{
    CY_FX_MSC_STATE_INACTIVE = 0,               /* Inactive state, waiting for SET_CONFIG. */
    CY_FX_MSC_STATE_CBW,                        /* Waiting to queue a CBW command. */
    CY_FX_MSC_STATE_WAITING,                    /* Waiting to receive a CBW command. */
    CY_FX_MSC_STATE_DATA,                       /* Waiting to complete data transfer for a command. */
    CY_FX_MSC_STATE_STATUS,                     /* Waiting to send CSW for a command. */
    CY_FX_MSC_STATE_CSW                         /* Waiting for host to read out CSW packet. */
} CyFxMscFuncState;

#define CY_FX_MSC_CBW_MAX_COUNT                 (31u)
#define CY_FX_MSC_CSW_MAX_COUNT                 (13u)
#define CY_FX_MSC_REPONSE_DATA_MAX_COUNT        (18u)

#define CY_FX_SCSI_INQUIRY_DATALEN              (96)
#define CY_FX_SCSI_INQCMD_EVPD_OFFSET           (16u)
#define CY_FX_SCSI_INQCMD_EVPD_MASK             (0x01u)
#define CY_FX_SCSI_INQCMD_PCODE_OFFSET          (17u)

#define CY_FX_SCSI_INQCMD_PCODE_LIST            (0x00u)
#define CY_FX_SCSI_INQCMD_PCODE_SERNUM          (0x80u)
#define CY_FX_SCSI_INQCMD_PCODE_DEVIDENT        (0x83u)
#define CY_FX_SCSI_INQCMD_PCODE_BLKLIMITS       (0xB0u)

#define CY_FX_SCSI_RQSCMD_DESC_OFFSET           (16u)
#define CY_FX_SCSI_REQSENSE_DESC                (0x01u)

#define CY_FX_SCSI_STOPCMD_LOEJ_OFFSET          (19u)
#define CY_FX_SCSI_STOPCMD_LOEJ_VAL             (0x02u)
#define CY_FX_SCSI_STOPCMD_START_VAL            (0x01u)

#define CY_FX_SCSI_READCAP16_SERVACT_OFFSET     (16u)
#define CY_FX_SCSI_READCAP16_SERVACT_VALUE      (0x10u)

#define CY_FX_SCSI_MODESENS_DBD_OFFSET          (16u)
#define CY_FX_SCSI_MODESENS_DBD_VALUE           (0x08u)
#define CY_FX_SCSI_MODESENS_PC_OFFSET           (17u)
#define CY_FX_SCSI_MODESENS_PC_MASK             (0xC0)
#define CY_FX_SCSI_MODESENS_PC_CURRENT          (0x00)
#define CY_FX_SCSI_MODESENS_PC_CHANGE           (0x40)
#define CY_FX_SCSI_MODESENS_PC_DEFAULT          (0x80)
#define CY_FX_SCSI_MODESENS_PC_SAVED            (0xC0)

#define CY_FX_SCSI_MODESENS_PAGE_MASK           (0x3F)
#define CY_FX_SCSI_MODESENS_PAGE_ALL            (0x3F)
#define CY_FX_SCSI_MODESENS_PAGE_EXCP           (0x1C)
#define CY_FX_SCSI_MODESENS_PAGE_CACHE          (0x08)

#define CY_FX_SCSI_MODESELECT_SP_OFFSET         (16u)
#define CY_FX_SCSI_MODESELECT_SP_MASK           (0x01u)

#define CY_FX_SCSI_SENDDIAG_SFTEST_OFFSET       (16u)
#define CY_FX_SCSI_SENDDIAG_SFTEST_MASK         (0x04u)

/* SCSI Commands */
#define CY_FX_MSC_SCSI_TEST_UNIT_READY          (0x00u)
#define CY_FX_MSC_SCSI_REQUEST_SENSE            (0x03u)
#define CY_FX_MSC_SCSI_FORMAT_UNIT              (0x04u)
#define CY_FX_MSC_SCSI_INQUIRY                  (0x12u)
#define CY_FX_MSC_SCSI_MODE_SELECT_6            (0x15u)
#define CY_FX_MSC_SCSI_MODE_SENSE_6             (0x1Au)
#define CY_FX_MSC_SCSI_START_STOP_UNIT          (0x1Bu)
#define CY_FX_MSC_SCSI_SEND_DIAG                (0x1Du)
#define CY_FX_MSC_SCSI_PREVENT_ALLOW_MEDIUM     (0x1Eu)
#define CY_FX_MSC_SCSI_READ_FORMAT_CAPACITY     (0x23u)
#define CY_FX_MSC_SCSI_READ_CAPACITY            (0x25u)
#define CY_FX_MSC_SCSI_READ_10                  (0x28u)
#define CY_FX_MSC_SCSI_WRITE_10                 (0x2Au)
#define CY_FX_MSC_SCSI_VERIFY_10                (0x2Fu)
#define CY_FX_MSC_SCSI_SYNCCACHE_10             (0x35u)
#define CY_FX_MSC_SCSI_READCAP_16               (0x9Eu)

/* Sense Codes */
#define CY_FX_MSC_SENSE_OK                      (0x00)
#define CY_FX_MSC_SENSE_CRC_ERROR               (0x01)
#define CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW    (0x02)
#define CY_FX_MSC_SENSE_NO_MEDIA                (0x03)
#define CY_FX_MSC_SENSE_WRITE_FAULT             (0x04)
#define CY_FX_MSC_SENSE_READ_ERROR              (0x05)
#define CY_FX_MSC_SENSE_ADDR_NOT_FOUND          (0x06)
#define CY_FX_MSC_SENSE_INVALID_OP_CODE         (0x07)
#define CY_FX_MSC_SENSE_INVALID_LBA             (0x08)
#define CY_FX_MSC_SENSE_INVALID_PARAMETER       (0x09)
#define CY_FX_MSC_SENSE_CANT_EJECT              (0x0A)
#define CY_FX_MSC_SENSE_MEDIA_CHANGED           (0x0B)
#define CY_FX_MSC_SENSE_DEVICE_RESET            (0x0C)
#define CY_FX_MSC_SENSE_WRITE_PROTECT           (0x0D)
#define CY_FX_MSC_SENSE_INIT_REQUIRED           (0x0E)
#define CY_FX_MSC_SENSE_NO_SAVEPAGE_SUPPORT     (0x0F)

#define CY_FX_USB_SETUP_REQ_TYPE_MASK   (uint32_t)(0x000000FF)  /* Setup Request Type Mask */
#define CY_FX_USB_SETUP_REQ_MASK        (uint32_t)(0x0000FF00)  /* Setup Request Mask */

#define CY_FX_SIB_READ                          (CyTrue)        /* Read command to SIB driver. */
#define CY_FX_SIB_WRITE                         (CyFalse)       /* Write command to SIB driver. */

#define CY_FX_MSC_APP_MAX_COMMIT_DELAY          (3)             /* Max. write commit delay in seconds. */

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

#include <cyu3externcend.h>

#endif /* _INCLUDED_CYFXMSCDEMO_H_ */

/*[]*/
