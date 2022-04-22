/*
 ## Cypress PIB firmware header file (cyfx3s_pib.h)
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

/* This file contains the constants and definitions for the PIB application example */

#ifndef _INCLUDED_CYFX3_PIB_H_
#define _INCLUDED_CYFX3_PIB_H_

#include <cyu3usbconst.h>
#include <cyu3error.h>
#include <cyu3socket.h>
#include <cyu3pib.h>
#include <cyu3mbox.h>
#include <cyu3types.h>
#include <cyu3uart.h>
#include <cyu3utils.h>
#include <cyu3usb.h>
#include <cyu3dma.h>
#include <cyu3sibpp.h>

/* Local header files. */
#include "cywbprotocol.h"
#include "cywberr.h"

#include <cyu3externcstart.h>

#define PIB_FW_VERS_MAJOR      (1)
#define PIB_FW_VERS_MINOR      (3)
#define PIB_FW_VERS_PATCH      (3)
#define PIB_FW_VERS_BUILD      (1)

#define PIB_MAX_VALID_CONTEXT  (3)
#define PIB_MAX_REQUEST_SIZE   (64)

/* Endpoint and socket definitions for the MSC application */

/* To change the Producer and Consumer EP enter the appropriate EP numbers for the #defines.
 * In the case of IN endpoints enter EP number along with the direction bit.
 * For eg. EP 6 IN endpoint is 0x86
 *     and EP 6 OUT endpoint is 0x06.
 * To change sockets mention the appropriate socket number in the #defines. */

/* Note: For USB 2.0 the endpoints and corresponding sockets are one-to-one mapped
         i.e. EP 1 is mapped to UIB socket 1 and EP 2 to socket 2 so on */

#define CY_FX_MSC_EP_BULK_OUT              0x01                /* EP 1 OUT */
#define CY_FX_MSC_EP_BULK_IN               0x81                /* EP 1 IN */

#define CY_FX_MSC_EP_BULK_OUT_SOCKET       0x01                /* Socket 1 is producer */
#define CY_FX_MSC_EP_BULK_IN_SOCKET        0x01                /* Socket 1 is consumer */

#define CY_FX_S2P_DMA_BUF_SIZE             (8192)              /* Using 8 KB buffers for better performance. */
#define CY_FX_S2P_DMA_BUF_COUNT            (8)                 /* Read channel buffer count */

#define CY_FX_P2S_DMA_BUF_SIZE             (8192)              /* Using 8 KB buffers for better performance. */
#define CY_FX_P2S_DMA_BUF_COUNT            (8)                 /* Write channel buffer count */

#define CY_FX_SIB_WR_CMT_SIZE              (16384)             /* SIB Write Commit Size */
#define CY_FX_SIB_WR_TIMER_PERIOD          (40000)             /* SIB Write Timer Period */

#define CY_FX_MSC_EP_BURST_SIZE            (4)                 /* Burst setting for the endpoint. */
#define CY_FX_MSC_EP_BUF_COUNT             (8)                 /* MSC channel buffer count */
#define CY_FX_MSC_LUN_COUNT                (4)                 /* MAX LUN COUNT */
#define CY_FX_MSC_DEFAULT_LUN_COUNT        (1)                 /* Default LUN Count */
#define CY_FX_MSC_PORT0                    (0)                 /* Port 0 */
#define CY_FX_MSC_PORT1                    (1)                 /* Port 1 */
#define CY_FX_MSC_DEFAULT_PORT             (2)                 /* Default Value */

#define CY_FX_MSC_FS_INTFDESC_LENGTH       (0x17)              /* Full Speed Interface Desc Length */
#define CY_FX_MSC_HS_INTFDESC_LENGTH       (0x17)              /* High Speed Interface Desc Length */
#define CY_FX_MSC_SS_INTFDESC_LENGTH       (0x23)              /* Super Speed Interface Desc Length */

#define CY_FX_MSC_CONFDESC_LENGTH_OFFSET   (0x02)              /* Length Offset in Config Desc */
#define CY_FX_MSC_DSCR_DEVICE_LEN          (0x12)              /* Device Dscr length */
#define CY_FX_MSC_DSCR_DEVQUAL_LEN         (0x0A)              /* Device Qualifier Dscr length */

#define CY_FX_APP_THREAD_STACK             (0x1000)            /* MSC application thread stack size */
#define CY_FX_APP_THREAD_PRIORITY          (8)                 /* MSC application thread priority */

#define CY_FX_SIB_P2S_WRITE_SOCKET         (0)                 /* P->S Write Xfer SIB Socket */
#define CY_FX_SIB_S2P_READ_SOCKET          (2)                 /* S->P Read Xfer SIB Socket */

#define CY_FX_SIB_U2S_WRITE_SOCKET         (3)                 /* U->S Write Xfer SIB Socket */
#define CY_FX_SIB_S2U_READ_SOCKET          (5)                 /* S->U Read Xfer SIB Socket */

#define CY_FX_SIB_PARTITIONS               (2)                 /* 2 Partitions for this application per port */
#define CY_FX_SIB_PORTS                    (2)                 /* Maximum ports for FX3S */
#define CY_FX_SIB_MAX_BLOCK_SIZE           (0x200)             /* Max allowed block size for SIB */

#define CY_FX_MSC_DEFAULT_UNIT_VAL         (5)                 /* MSC Interface Default Unit Value */
#define CY_FX_MSC_DEFAULT_PORT_VAL         (2)                 /* MSC Interface Default Port Value */

#define CY_FX_MSC_RESET_EVENT_FLAG         (1 << 0)            /* USB reset event. */
#define CY_FX_MSC_SETCONF_EVENT_FLAG       (1 << 1)            /* SET Config event */
#define CY_FX_MSC_CBW_EVENT_FLAG           (1 << 2)            /* MSC CBW received. */
#define CY_FX_MSC_DATASENT_EVENT_FLAG      (1 << 3)            /* Data has been sent to the host. */
#define CY_FX_MSC_SIBCB_EVENT_FLAG         (1 << 4)            /* SIB event callback flag. */
#define CY_FX_MSC_USB_SUSP_EVENT_FLAG      (1 << 5)            /* USB Suspend event flag. */
#define CY_FX_MSC_DATA_RECV_EVENT_FLAG     (1 << 6)            /* Data received on USB endpoint. */
#define CY_FX_MSC_BMC_INIT_FLAG            (1 << 7)            /* BMC has connected to FX3S device. */
#define CY_FX_MSC_GEN_CTX_FLAG             (1 << 8)            /* Pending request on general context. */
#define CY_FX_MSC_USB_CTX_FLAG             (1 << 9)            /* Pending request on USB context. */
#define CY_FX_MSC_STR_CTX_FLAG             (1 << 10)           /* Pending request on Storage context. */

#define CY_FX_SIB_XFER_CPLT_EVENT          (1 << 0)            /* PIB Storage Xfer completion event. */

#define CY_FX_MSC_USB_CLASS_REQ            (0x20)              /* Class request type */
#define CY_FX_MSC_USB_VENDOR_REQ           (0x40)              /* Vendor request type */

#define CY_FX_MSC_GET_MAX_LUN_REQ          (0xFE)              /* MSC Get Max LUN request */
#define CY_FX_MSC_BOT_RESET_REQ            (0xFF)              /* MSC BOT Reset request */

/* Mass storage interface number */
#define CY_FX_USB_MSC_INTF                 (0x01)

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


#define CY_FX_MSC_CBW_MAX_COUNT             31
#define CY_FX_MSC_CSW_MAX_COUNT             13
#define CY_FX_MSC_REPONSE_DATA_MAX_COUNT    18

#define CY_FX_SCSI_INQUIRY_DATALEN              (96u)
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

#define CY_FX_SCSI_MODESENS_PC_OFFSET           (17u)
#define CY_FX_SCSI_MODESENS_PC_MASK             (0xC0u)
#define CY_FX_SCSI_MODESENS_PC_CURRENT          (0x00u)
#define CY_FX_SCSI_MODESENS_PC_CHANGE           (0x40u)
#define CY_FX_SCSI_MODESENS_PC_DEFAULT          (0x80u)

#define CY_FX_SCSI_MODESENS_PAGE_MASK           (0x3Fu)
#define CY_FX_SCSI_MODESENS_PAGE_ALL            (0x3Fu)
#define CY_FX_SCSI_MODESENS_PAGE_CACHE          (0x08u)

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
#define CY_FX_MSC_SENSE_OK                      (0x00u)
#define CY_FX_MSC_SENSE_CRC_ERROR               (0x01u)
#define CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW    (0x02u)
#define CY_FX_MSC_SENSE_NO_MEDIA                (0x03u)
#define CY_FX_MSC_SENSE_WRITE_FAULT             (0x04u)
#define CY_FX_MSC_SENSE_READ_ERROR              (0x05u)
#define CY_FX_MSC_SENSE_ADDR_NOT_FOUND          (0x06u)
#define CY_FX_MSC_SENSE_INVALID_OP_CODE         (0x07u)
#define CY_FX_MSC_SENSE_INVALID_LBA             (0x08u)
#define CY_FX_MSC_SENSE_INVALID_PARAMETER       (0x09u)
#define CY_FX_MSC_SENSE_CANT_EJECT              (0x0Au)
#define CY_FX_MSC_SENSE_MEDIA_CHANGED           (0x0Bu)
#define CY_FX_MSC_SENSE_DEVICE_RESET            (0x0Cu)
#define CY_FX_MSC_SENSE_WRITE_PROTECT           (0x0Du)
#define CY_FX_MSC_SENSE_INIT_REQUIRED           (0x0Eu)
#define CY_FX_MSC_SENSE_NO_SAVEPAGE_SUPPORT     (0x0Fu)

#define CY_FX_SIB_READ                          (CyTrue)        /* Read command to SIB driver. */
#define CY_FX_SIB_WRITE                         (CyFalse)       /* Write command to SIB driver. */

extern CyU3PEvent   glPibAppEvent;                      /* MSC application Event group */

#define CY_FX_SS_EP_COMPN_DSCR_TYPE     48

/* Extern definitions for the USB Enumeration descriptors */
extern uint8_t CyFxUSBFSBotIfaceDscr[];
extern uint8_t CyFxUSBHSBotIfaceDscr[];
extern uint8_t CyFxUSBSSBotIfaceDscr[];

extern uint16_t glPibRqtData[PIB_MAX_VALID_CONTEXT][PIB_MAX_REQUEST_SIZE];
extern CyBool_t glPibSendEvents;

extern void 
CyFxMscApplnDebugInit(
        void);

extern void
CyFxAppErrorHandler (
        CyU3PReturnStatus_t apiRetStatus);

#include <cyu3externcend.h>
#endif /*_INCLUDED_CYFX3_PIB_H_*/
/*[]*/

