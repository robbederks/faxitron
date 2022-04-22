/*
 ## Cypress PIB Firmware Source (cyfx3s_pib.c)
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

/* This file contains code for establishing a communication model between Application Processor
 * or AP using P-Port API and FX3S device through a PMMC Interface. USB, Storage and General
 * requests issued by AP side are handled in this file.
 *
 * Appropriate response as well as real time events will be transmitted to the external AP. FX3S
 * device will enumerate as a specific class of device as specified by external AP if MSC Interface
 * is not enable once configuration is finished successfully.
 * */

#include <cyu3system.h>
#include <cyu3os.h>
#include <cyu3sib.h>
#include <cyu3gpio.h>
#include <cyu3pib.h>
#include <pib_regs.h>

#include "cyfx3_pib.h"
#include "cyfx3_pibctxt.h"

#if 0
#define AppDebugPrint CyU3PDebugPrint
#else
#define AppDebugPrint(...)
#endif

static CyU3PThread   mscAppThread;                      /* MSC application thread structure */
static CyU3PEvent    glXferEvent;                       /* Transfer completion event group. */
static CyBool_t      glInPhaseError     = CyFalse;      /* Whether a phase error condition has been detected. */
static uint8_t       glMscCmdStatus     = 0;            /* MSC command status. */
static uint32_t      glMscResidue       = 0;            /* Residue length for CSW */
       CyU3PEvent    glPibAppEvent;                     /* PIB application Event group */       
static uint8_t       *glPibDataBuffer   = 0;            /* Scratch buffer used for PIB. */
static uint8_t       *glMscCbwBuffer    = 0;            /* Scratch buffer used for CBW. */
static uint8_t       *glMscCswBuffer    = 0;            /* Scratch buffer used for CSW. */
static uint8_t       *glMscDataBuffer   = 0;            /* Scratch buffer used for CSW. */
static CyBool_t      glDevConfigured    = CyFalse;      /* Whether the device has been configured. */
static CyBool_t      glMscDriverBusy    = CyFalse;      /* Whether the MSC driver has pending work. */
static uint8_t       glPibMaxLun        = CY_FX_MSC_DEFAULT_LUN_COUNT;            /* Max LUN Count */

static CyU3PDmaChannel      glChWrHandleP2S;            /* DMA channel for P->S write operations. */
static CyU3PDmaChannel      glChWrHandleU2S;            /* DMA channel for U->S write operations. */
static CyU3PDmaChannel      glChRdHandleS2P;            /* DMA channel for S->P read operations. */
static CyU3PDmaChannel      glChRdHandleS2U;            /* DMA channel for S->U read operations. */
static CyU3PDmaChannel      glChRdHandleP2EP0;          /* DMA channel used to send control transfer data. */
static CyU3PDmaChannel      glChWrHandleEP02P;          /* DMA channel used to recieve control transfer data. */
static CyU3PDmaChannel      *glChHandleU2P  = 0;        /* DMA channel Handle for U->P operations (AP specific Interface). */
static CyU3PDmaChannel      *glChHandleP2U  = 0;        /* DMA channel Handle for P->U operations (AP specific Interface). */

static uint8_t       glU2PChHandleOffset = 0;           /* U->P DMA Channel Handle Offset Min:0 Max: 14 */   
static uint8_t       glP2UChHandleOffset = 0;           /* P->U DMA Channel Handle Offset Min:0 Max: 14 */   

static uint8_t       glMscIfaceIndex = 0;               /* MSC Interface Index */
static uint8_t       glDescType      = 0;               /* Descriptor Type */
static CyBool_t      glIsDescRqt     = CyFalse;         /* Is current setup request GET_DESCRIPTOR. */
static uint16_t      glRqtLen        = 0;               /* Requested descriptor Length */

static CyBool_t      glUsbOperation     = CyFalse;      /* Whether the current transfer is a USB operation. */
static uint16_t      glRecvBufCount     = 0;            /* Number of buffers of data received during P2S write. */
static uint8_t       glMscPartitions[2] = {0};          /* LUN Visibilty Tracker */
static uint8_t       glMscWritable[2]   = {0};          /* LUN Writeable Tracker */
static uint8_t       glMscPartOwner[2]  = {0};          /* Ownership for all ports. */
static CyBool_t      glPIBUsbConnected  = CyFalse;      /* Whether USB connect has been called. */
static CyBool_t      glPIBSSEnable      = CyFalse;      /* Whether USB 3.0 is enabled. */
static CyBool_t      glIsMscEnbl        = CyFalse;      /* MSC Operation Enable/Disable */
static uint8_t       glCmdDirection     = 0;            /* SCSI Command Direction */
static uint8_t       glCurrentCmd       = 0xFF;         /* Current SCSI Command */
static uint8_t       glMscCmdLun        = 0;            /* Current LUN in Operation */

typedef struct MscLunInfo {
    uint32_t    capacity;               /* Capacity (in blocks) of the LUN. */
    CyBool_t    state;                  /* Whether LUN is ready. */
    CyBool_t    released;               /* Whether the LUN has been released by AP. */
    CyBool_t    writeable;              /* Whether the LUN is writeable. */
    CyBool_t    stopped;                /* Whether the LUN has been stopped. */
    CyBool_t    changed;                /* Whether the LUN state has changed since last command. */
    uint8_t     port;                   /* Port corresponding to the LUN. */
    uint8_t     unit;                   /* Unit corresponding to the LUN. */
    uint8_t     sensePtr;               /* Active sense data pointer. */
} MscLunInfo;

static MscLunInfo glLunDatabase[CY_FX_MSC_LUN_COUNT] = {
    {0, CyFalse, CyFalse, CyTrue, CyFalse, CyFalse, 0xFF, 0xFF, CY_FX_MSC_SENSE_OK},  
    {0, CyFalse, CyFalse, CyTrue, CyFalse, CyFalse, 0xFF, 0xFF, CY_FX_MSC_SENSE_OK},    
    {0, CyFalse, CyFalse, CyTrue, CyFalse, CyFalse, 0xFF, 0xFF, CY_FX_MSC_SENSE_OK},    
    {0, CyFalse, CyFalse, CyTrue, CyFalse, CyFalse, 0xFF, 0xFF, CY_FX_MSC_SENSE_OK}
};

static CyU3PReturnStatus_t glXferStatus = CY_U3P_SUCCESS; /* BMS SIB Xfer Status */
static CyU3PSibDevInfo_t   glDevInfo[CY_FX_SIB_PORTS];    /* Structure to hold device info */
static CyFxMscFuncState    glMscState   = CY_FX_MSC_STATE_INACTIVE; /* Current MSC State */

static const uint8_t CyFxMscScsiInquiryData[CY_FX_SCSI_INQUIRY_DATALEN] __attribute__ ((aligned (32))) = {
    0x00,       /* PQ and PDT */
    0x80,       /* Removable device. */
    0x06,       /* Version */
    0x02,       /* Response data format */
    0x5B,       /* Addnl Length */
    0x00,
    0x00,
    0x00,

    'C',        /* Vendor Id */
    'y',
    'p',
    'r',
    'e',
    's',
    's',
    0x00,

    'F',        /* Product Id */
    'X',
    '3',
    'S',
    0x20,
    'M',
    'S',
    'C',
    0x20,
    'D',
    'E',
    'M',
    'O',
    0x20,
    0x00,
    0x00,

    '0',        /* Revision */
    '0',
    '0',
    '1',

    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0, /* 20 bytes of vendor specific info: not used. */
    0, 0,       /* Reserved fields for SCSI over USB. */

    0x00, 0x80, /* SAM-4 spec compliant. */
    0x17, 0x30, /* BOT spec compliant. */
    0x04, 0x60, /* SPC-4 spec compliant. */
    0x04, 0xC0, /* SBC-3 spec compliant. */
    0x00, 0x00, /* No more specs. */
    0x00, 0x00, /* No more specs. */
    0x00, 0x00, /* No more specs. */
    0x00, 0x00, /* No more specs. */
    0x00, 0x00, /* Reserved. */

    0, 0, 0, 0, /* Reserved. */
    0, 0, 0, 0, /* Reserved. */
    0, 0, 0, 0, /* Reserved. */
    0, 0, 0, 0, /* Reserved. */
    0, 0, 0, 0  /* Reserved. */
};

static const uint8_t CyFxMscSupportedPageList[] __attribute__ ((aligned (32))) = {
    0x00,                                       /* PDT = 0. */
    CY_FX_SCSI_INQCMD_PCODE_LIST,               /* Page code = 0. */
    0x00,                                       /* Reserved. */
    0x04,                                       /* Max Index - 3. */
    CY_FX_SCSI_INQCMD_PCODE_LIST,
    CY_FX_SCSI_INQCMD_PCODE_SERNUM,
    CY_FX_SCSI_INQCMD_PCODE_DEVIDENT,
    CY_FX_SCSI_INQCMD_PCODE_BLKLIMITS
};

static uint8_t CyFxMscScsiSerialNumber[] __attribute__ ((aligned (32))) = {
    0x00,                                       /* PDT = 0. */
    CY_FX_SCSI_INQCMD_PCODE_SERNUM,             /* Page code = 0x80. */
    0x00,                                       /* Reserved. */
    0x0C,                                       /* Max Index (15) - 3. */
    0x30, 0x31, 0x32, 0x33,                     /* "0123" */
    0x34, 0x35, 0x36, 0x37,                     /* "4567" */
    0x38, 0x39, 0x30, 0x31                      /* "8901" */
};

static uint8_t CyFxMscScsiDevIdent[] __attribute__ ((aligned (32))) = {
    0x00,                                       /* PDT = 0. */
    CY_FX_SCSI_INQCMD_PCODE_DEVIDENT,           /* Page code = 0x83. */
    0x00,                                       /* MSB (PAGE LENGTH) = 0x00. */
    0x0C,                                       /* LSB (PAGE LENGTH) = 15 - 3 = 12. */

    0x01, 0x02, 0x00, 0x08,
    0x00, 0xA0, 0x50, 0x46, 0x58, 0x33, 0x53, 0x20
};

static const uint8_t CyFxMscScsiBlkLimits[] __attribute__ ((aligned (32))) = {
    0x00,                                       /* PDT = 0. */
    CY_FX_SCSI_INQCMD_PCODE_BLKLIMITS,          /* Page code = 0xB0. */
    0x00,                                       /* Reserved. */
    0x3C,                                       /* Max Index (63) - 3. */
    0x00, 0x00, 0x08, 0x00,                     /* Optimal size granularity = 1 MB. */
    0x00, 0x00, 0x40, 0x00,                     /* Maximum size = 8 MB. */
    0x00, 0x00, 0x40, 0x00,                     /* Optimum size = 8 MB. */
    0x00, 0x00, 0x40, 0x00,                     /* Max. Prefetch size = 8 MB. */
    0x00, 0x00, 0x00, 0x00,                     /* No unmap support. */
    0x00, 0x00, 0x00, 0x00,                     /* No unmap support. */
    0x00, 0x00, 0x00, 0x00,                     /* No unmap support. */
    0x00, 0x00, 0x00, 0x00,                     /* No unmap support. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00,                     /* Reserved. */
    0x00, 0x00, 0x00, 0x00                      /* Reserved. */
};

static const uint8_t CyFxMscCachingDefault[] __attribute__ ((aligned (32))) = {
    0x08,                                       /* PS=0, SPF=0, PC=08. */
    0x12,                                       /* Page length. */
    00, 00,                                     /* No caching supported. */
    00, 00, 00, 00,
    00, 00, 00, 00,
    00, 00, 00, 00,
    00, 00, 00, 00
};

static const uint8_t CyFxMscCachingChangeable[] __attribute__ ((aligned (32))) = {
    0x08,                                       /* PS=0, SPF=0, PC=08. */
    0x12,                                       /* Page length. */
    0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF,
    0xE1, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00
};

static uint8_t CyFxMscCachingCurrent[] __attribute__ ((aligned (32))) = {
    0x08,                                       /* PS=0, SPF=0, PC=08. */
    0x12,                                       /* Page length. */
    00, 00,                                     /* No caching supported. */
    00, 00, 00, 00,
    00, 00, 00, 00,
    00, 00, 00, 00,
    00, 00, 00, 00
};

static const uint16_t glWbErrMap[] = {
    CY_WB_ERROR_SUCCESS,                // CY_U3P_SUCCESS = 0,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_DELETED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_POOL,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_POINTER,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_INVALID_WAIT,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_SIZE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_EVENT_GRP,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_NO_EVENTS,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_OPTION,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_QUEUE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_QUEUE_EMPTY,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_QUEUE_FULL,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_SEMAPHORE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_SEMGET_FAILED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_THREAD,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_PRIORITY,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_MEMORY_ERROR,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_DELETE_FAILED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_RESUME_FAILED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_INVALID_CALLER,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_SUSPEND_FAILED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_TIMER,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_TICK,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_ACTIVATE_FAILED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_THRESHOLD,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_SUSPEND_LIFTED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_WAIT_ABORTED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_WAIT_ABORT_FAILED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_MUTEX,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_MUTEX_FAILURE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_MUTEX_PUT_FAILED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_INHERIT_FAILED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_NOT_IDLE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_INVALID_PARAMETER,      // CY_U3P_ERROR_BAD_ARGUMENT = 0x40,
    CY_WB_ERROR_INVALID_PARAMETER,      // CY_U3P_ERROR_NULL_POINTER,
    CY_WB_ERROR_NOT_RUNNING,            // CY_U3P_ERROR_NOT_STARTED,
    CY_WB_ERROR_ALREADY_RUNNING,        // CY_U3P_ERROR_ALREADY_STARTED,
    CY_WB_ERROR_NOT_RUNNING,            // CY_U3P_ERROR_NOT_CONFIGURED,
    CY_WB_ERROR_TIMEOUT,                // CY_U3P_ERROR_TIMEOUT,
    CY_WB_ERROR_NOT_SUPPORTED,          // CY_U3P_ERROR_NOT_SUPPORTED,
    CY_WB_ERROR_INVALID_CONFIGURATION,  // CY_U3P_ERROR_INVALID_SEQUENCE,
    CY_WB_ERROR_CANCELED,               // CY_U3P_ERROR_ABORTED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_DMA_FAILURE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_FAILURE,
    CY_WB_ERROR_INVALID_PARAMETER,      // CY_U3P_ERROR_BAD_INDEX,
    CY_WB_ERROR_INVALID_PARAMETER,      // CY_U3P_ERROR_BAD_ENUM_METHOD,
    CY_WB_ERROR_INVALID_CONFIGURATION,  // CY_U3P_ERROR_INVALID_CONFIGURATION,
    CY_WB_ERROR_OUT_OF_MEMORY,          // CY_U3P_ERROR_CHANNEL_CREATE_FAILED,
    CY_WB_ERROR_OUT_OF_MEMORY,          // CY_U3P_ERROR_CHANNEL_DESTROY_FAILED,
    CY_WB_ERROR_INVALID_PARAMETER,      // CY_U3P_ERROR_BAD_DESCRIPTOR_TYPE,
    CY_WB_ERROR_CANCELED,               // CY_U3P_ERROR_XFER_CANCELLED,
    CY_WB_ERROR_NOT_SUPPORTED,          // CY_U3P_ERROR_FEATURE_NOT_ENABLED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_STALLED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BLOCK_FAILURE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_LOST_ARBITRATION,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_STANDBY_FAILED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_NO_SUCH_DEVICE,         // CY_U3P_ERROR_INVALID_VOLTAGE_RANGE = 0x60,
    CY_WB_ERROR_MEDIA_ACCESS_FAILURE,   // CY_U3P_ERROR_CARD_WRONG_RESPONSE,
    CY_WB_ERROR_NO_SUCH_DEVICE,         // CY_U3P_ERROR_UNSUPPORTED_CARD,
    CY_WB_ERROR_NO_SUCH_DEVICE,         // CY_U3P_ERROR_CARD_WRONG_STATE,
    CY_WB_ERROR_NOT_SUPPORTED,          // CY_U3P_ERROR_CMD_NOT_SUPPORTED,
    CY_WB_ERROR_MEDIA_ACCESS_FAILURE,   // CY_U3P_ERROR_CRC,
    CY_WB_ERROR_NO_SUCH_DEVICE,         // CY_U3P_ERROR_INVALID_ADDR,
    CY_WB_ERROR_NO_SUCH_UNIT,           // CY_U3P_ERROR_INVALID_UNIT,
    CY_WB_ERROR_NO_SUCH_DEVICE,         // CY_U3P_ERROR_INVALID_DEV,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_ALREADY_PARTITIONED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_NOT_PARTITIONED,
    CY_WB_ERROR_INVALID_PARAMETER,      // CY_U3P_ERROR_BAD_PARTITION,
    CY_WB_ERROR_CANCELED,               // CY_U3P_ERROR_READ_WRITE_ABORTED,
    CY_WB_ERROR_MEDIA_WRITE_PROTECTED,  // CY_U3P_ERROR_WRITE_PROTECTED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_SIB_INIT,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_CARD_LOCKED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_CARD_LOCK_FAILURE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_CARD_FORCE_ERASE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_INVALID_BLOCKSIZE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_INVALID_FUNCTION,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_TUPLE_NOT_FOUND,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_IO_ABORTED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_IO_SUSPENDED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_ILLEGAL_CMD,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_SDIO_UNKNOWN,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_BAD_CMD_ARG,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_UNINITIALIZED_FUNCTION,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_CARD_NOT_ACTIVE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // CY_U3P_ERROR_DEVICE_BUSY,
    CY_WB_ERROR_MEDIA_ACCESS_FAILURE,   // CY_U3P_ERROR_MEDIA_FAILURE,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE,     // RESERVED,
    CY_WB_ERROR_GENERAL_FW_FAILURE      // RESERVED
};

/* Request Sense Table */
uint8_t glReqSenseCode[][3] __attribute__ ((aligned (32))) =
{
    /* SK,  ASC,  ASCQ */
    {0x00, 0x00, 0x00},    /* senseOk                     0    */
    {0x0b, 0x08, 0x03},    /* senseCRCError               1    */
    {0x05, 0x24, 0x00},    /* senseInvalidFieldInCDB      2    */
    {0x02, 0x3a, 0x00},    /* senseNoMedia                3    */
    {0x03, 0x03, 0x00},    /* senseWriteFault             4    */
    {0x03, 0x11, 0x00},    /* senseReadError              5    */
    {0x03, 0x12, 0x00},    /* senseAddrNotFound           6    */
    {0x05, 0x20, 0x00},    /* senseInvalidOpcode          7    */
    {0x05, 0x21, 0x00},    /* senseInvalidLBA             8    */
    {0x05, 0x26, 0x00},    /* senseInvalidParameter       9    */
    {0x05, 0x53, 0x02},    /* senseCantEject              0xa  */
    {0x06, 0x28, 0x00},    /* senseMediaChanged           0xb  */
    {0x06, 0x29, 0x00},    /* senseDeviceReset            0xc  */
    {0x07, 0x27, 0x00},    /* senseWriteProtected         0xd  */
    {0x02, 0x04, 0x02},    /* senseInitRequired           0xe  */
    {0x05, 0x39, 0x00}     /* senseSavePageNotSupported   0xf  */
};

/* Data used by Cypress proprietary mass storage driver to identify the device. */
uint8_t A0VendBuf[8] __attribute__ ((aligned (32))) =
{
    0x02, 0x0E, 0, 0, 0, 0, 0, 0
};

void
CyFxAppQueryDevStatus (
        void);

extern void
CyU3PDeviceOverridePart (
        CyU3PPartNumber_t partNum);

/*
 * Main function
 */
int
main (void)
{
    CyU3PReturnStatus_t status;
    CyU3PIoMatrixConfig_t io_cfg;

    CyU3PDeviceOverridePart (CYPART_USB3035);

    /* Initialize the device */
    status = CyU3PDeviceInit (0);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* Initialize the caches. Enable instruction cache and keep data cache disabled.
     * The data cache is useful only when there is a large amount of CPU based memory
     * accesses. When used in simple cases, it can decrease performance due to large
     * number of cache flushes and cleans and also it adds to the complexity of the
     * code. */
    status = CyU3PDeviceCacheControl (CyTrue, CyFalse, CyFalse);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* Configure the IO matrix for the device.
     * S0 port is enabled in 4 bit mode.
     * S1 port is enabled in 4 bit mode.
     * UART is enabled on remaining pins of the S1 port.
     */
    io_cfg.isDQ32Bit        = CyFalse;
    io_cfg.s0Mode           = CY_U3P_SPORT_4BIT;
    io_cfg.s1Mode           = CY_U3P_SPORT_4BIT;
    io_cfg.gpioSimpleEn[0]  = 0;
    io_cfg.gpioSimpleEn[1]  = 0x02102800;                       /* IOs 43, 45, 52 and 57 are chosen as GPIO. */
    io_cfg.gpioComplexEn[0] = 0;
    io_cfg.gpioComplexEn[1] = 0;
    io_cfg.useUart          = CyTrue;
    io_cfg.useI2C           = CyFalse;
    io_cfg.useI2S           = CyFalse;
    io_cfg.useSpi           = CyFalse;
    io_cfg.lppMode          = CY_U3P_IO_MATRIX_LPP_UART_ONLY;
    status = CyU3PDeviceConfigureIOMatrix (&io_cfg);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* This is a non returnable call for initializing the RTOS kernel */
    CyU3PKernelEntry ();

    /* Dummy return to make the compiler happy */
    return 0;

handle_fatal_error:
    /* Cannot recover from this error. */
    while (1);
}

/* MSC application error handler */
void
CyFxAppErrorHandler (
        CyU3PReturnStatus_t apiRetStatus)
{

    /* Application failed with the error code apiRetStatus */

    /* Add custom debug or recovery actions here */

    /* Loop Indefinitely */
    for (;;)
    {
        /* Thread Sleep : 100 ms */
        CyU3PThreadSleep (100);
    }
}

/* This function initializes the Debug Module for the PIB Application */
void
CyFxMscApplnDebugInit()
{

    CyU3PUartConfig_t uartConfig;
    CyU3PReturnStatus_t apiRetStatus;

    /* Initialize the UART for printing debug messages */
    apiRetStatus = CyU3PUartInit ();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error Handling */
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Set UART Configuration */
    uartConfig.baudRate = CY_U3P_UART_BAUDRATE_115200;
    uartConfig.stopBit  = CY_U3P_UART_ONE_STOP_BIT;
    uartConfig.parity   = CY_U3P_UART_NO_PARITY;
    uartConfig.txEnable = CyTrue;
    uartConfig.rxEnable = CyFalse;
    uartConfig.flowCtrl = CyFalse;
    uartConfig.isDma    = CyTrue;

    /* Set the UART configuration */
    apiRetStatus = CyU3PUartSetConfig (&uartConfig, NULL);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error Handling */
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Set the UART transfer */
    apiRetStatus = CyU3PUartTxSetBlockXfer (0xFFFFFFFF);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error Handling */
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Initialize the Debug application */
    apiRetStatus = CyU3PDebugInit (CY_U3P_LPP_SOCKET_UART_CONS, 8);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error Handling */
        CyFxAppErrorHandler(apiRetStatus);
    }

    CyU3PDebugPreamble (CyFalse);
}


/* PMMC event callback. */
static void
CyFxAppPmmcCallback (
        CyU3PPmmcEventType cbType,
        uint32_t           cbArg)
{
    /* Once the driver has selected FX3S, we can send the init complete notification. */
    if (cbType == CYU3P_PMMC_CMD7_SELECT)
    {
        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_BMC_INIT_FLAG, CYU3P_EVENT_OR);
        glPibSendEvents = CyTrue;
    }
}

static void
CyFxMscSetLunDefaultParams (
        void)
{
    uint32_t i;

    for (i = 0; i < (CY_FX_MSC_LUN_COUNT); i++)
    {
        glLunDatabase[i].capacity  = 0;
        glLunDatabase[i].state     = CyFalse;
        glLunDatabase[i].released  = CyFalse;
        glLunDatabase[i].writeable = CyTrue;
        glLunDatabase[i].stopped   = CyFalse;
        glLunDatabase[i].changed   = CyFalse;
        glLunDatabase[i].port      = 0xFF;
        glLunDatabase[i].unit      = 0xFF;
        glLunDatabase[i].sensePtr  = CY_FX_MSC_SENSE_DEVICE_RESET;
    }
}

/* Initialize MSC specific variables to default values */
static void
CyFxMscPibInitVars (
		void)
{
    glMscPartitions[CY_FX_MSC_PORT0] = 0;
    glMscPartitions[CY_FX_MSC_PORT1] = 0;    
    glMscWritable[CY_FX_MSC_PORT0]   = 0xFF;
    glMscWritable[CY_FX_MSC_PORT1]   = 0xFF;    
    glMscPartOwner[CY_FX_MSC_PORT0]  = 0;
    glMscPartOwner[CY_FX_MSC_PORT1]  = 0;    

    CyFxMscSetLunDefaultParams ();
}


/* Initialize PIB Application */
void
CyFxPibApplnInit (
        void)
{
    uint32_t tdat = 0xFFFEFFFF;    
    CyU3PReturnStatus_t status;
    CyU3PGpioClock_t    gpioClock;
    CyU3PPibClock_t     pibClock;

    uint32_t txApiRetStatus;

    /* Default value assigner to global variables */
    CyFxMscPibInitVars ();

    /* Create PIB Application events */
    txApiRetStatus = CyU3PEventCreate (&glPibAppEvent);
    if (txApiRetStatus != 0)
    {
        /* Error Handling */
        AppDebugPrint (4, "MSC Appln Create Event Failed, Error Code = %d\r\n", txApiRetStatus);
        CyFxAppErrorHandler (txApiRetStatus);
    }

    /* Create P<->S Xfer Event Group */
    txApiRetStatus = CyU3PEventCreate (&glXferEvent);
    if (txApiRetStatus != 0)
    {
        /* Error Handling */
        AppDebugPrint (4, "MSC Appln Create Event Failed, Error Code = %d\r\n", txApiRetStatus);
        CyFxAppErrorHandler (txApiRetStatus);
    }

    /* GPIO module needs to be initialized before SIB is initialized. This is required because
       GPIOs are used in the SIB code.
     */
    gpioClock.fastClkDiv = 2;
    gpioClock.slowClkDiv = 16;
    gpioClock.simpleDiv  = CY_U3P_GPIO_SIMPLE_DIV_BY_2;
    gpioClock.clkSrc     = CY_U3P_SYS_CLK;
    gpioClock.halfDiv    = 0;
    status = CyU3PGpioInit (&gpioClock, NULL);
    if (status != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "GPIO Init failed, code=%d\r\n", status);
        CyFxAppErrorHandler (status);
    }

    CyU3PPibSelectMmcSlaveMode ();

    /* Initialize PIB and load the GPIF waveform. */
    pibClock.clkDiv      = 2;
    pibClock.clkSrc      = CY_U3P_SYS_CLK_BY_4;
    pibClock.isHalfDiv   = CyFalse;
    pibClock.isDllEnable = CyFalse;

    /* Initialize PIB Block */
    status = CyU3PPibInit (CyTrue, &pibClock);
    if (status != CY_U3P_SUCCESS)
    {
        AppDebugPrint (2, "Error: PIB Init failed with code %d\r\n", status);
        CyFxAppErrorHandler (status);
    }
   
    CyU3PWriteDeviceRegisters ((uvint32_t *)0xe0010024, 1, &tdat);
    
    /* Signal busy for as long as possible. */
    tdat = 0x000000FF;
    CyU3PWriteDeviceRegisters ((uvint32_t *)0xe0011018, 1, &tdat);

    /* Buffer for Application Usage */
    glPibDataBuffer = (uint8_t *) CyU3PDmaBufferAlloc (1024);
    if (glPibDataBuffer == 0)
    {
        CyFxAppErrorHandler (CY_U3P_ERROR_MEMORY_ERROR);
    }

    /* Register PMMC Callback */
    CyU3PPmmcRegisterCallback (CyFxAppPmmcCallback);

    /* Register a callback that will receive notifications of incoming requests. */
    CyU3PMboxInit (CyFxAppMboxCallback);    
}

/* P->S DMA Write Callback */
static void
CyFxPIBP2SDmaCallback (
        CyU3PDmaChannel *chHandle,
        CyU3PDmaCbType_t      type,
        CyU3PDmaCBInput_t    *input)
{
    if (type == CY_U3P_DMA_CB_PROD_EVENT)
    {
        CyU3PDmaChannelCommitBuffer (chHandle, input->buffer_p.count, 0);
        glRecvBufCount++;       
    }
}


/* U->S DMA Write Callback */
static void
CyFxPIBU2SDmaCallback (
        CyU3PDmaChannel *chHandle,
        CyU3PDmaCbType_t      type,
        CyU3PDmaCBInput_t    *input)
{
    CyU3PDmaBuffer_t dmaBuf = input->buffer_p;

    switch (type)
    {
        case CY_U3P_DMA_CB_PROD_EVENT:
            CyU3PDmaChannelCommitBuffer (chHandle, input->buffer_p.count, 0);
            break;

        case CY_U3P_DMA_CB_RECV_CPLT:
            {
                if ((glMscState == CY_FX_MSC_STATE_CBW) || (glMscState == CY_FX_MSC_STATE_WAITING))
                {
                    /* Check for valid CBW. */
                    if ((dmaBuf.count != CY_FX_MSC_CBW_MAX_COUNT) || (dmaBuf.buffer[0] != 'U') || (dmaBuf.buffer[1] != 'S') ||
                            (dmaBuf.buffer[2] != 'B') || (dmaBuf.buffer[3] != 'C'))
                    {
                        glInPhaseError = CyTrue;
                    }
                    else
                    {
                        glInPhaseError = CyFalse;
                    }
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_CBW_EVENT_FLAG, CYU3P_EVENT_OR);
                }
                else
                {
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_DATA_RECV_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        default:
            break;
    }
}

/* S->U DMA Read Callback */
static void
CyFxPIBS2UDmaCallback (
        CyU3PDmaChannel   *chHandle,
        CyU3PDmaCbType_t   type,
        CyU3PDmaCBInput_t *input)
{
    if (type == CY_U3P_DMA_CB_SEND_CPLT)
    {
        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_DATASENT_EVENT_FLAG, CYU3P_EVENT_OR);
    }
}

/* DMA Channel Creator Function between P<->S & U<->S */
static CyU3PReturnStatus_t
CyFxPibApplnDmaInit (
        uint8_t rdSock,
        uint8_t wrSock)
{
    CyU3PDmaChannelConfig_t      dmaConfig;
    CyU3PReturnStatus_t          status;

    /* Allocate scratch buffers required by the application. */
    glMscCbwBuffer   = (uint8_t *)CyU3PDmaBufferAlloc (1024);
    glMscCswBuffer   = (uint8_t *)CyU3PDmaBufferAlloc (1024);
    glMscDataBuffer  = (uint8_t *)CyU3PDmaBufferAlloc ((1024 * 2));
    if ((glMscCbwBuffer == 0) || (glMscCswBuffer == 0) || (glMscDataBuffer == 0))
    {
        goto destroy;
    }

    /* Allocate memory for max possible P<->U DMA Channels */
    glChHandleP2U   = (CyU3PDmaChannel *)CyU3PDmaBufferAlloc (15 * sizeof(CyU3PDmaChannel));
    glChHandleU2P   = (CyU3PDmaChannel *)CyU3PDmaBufferAlloc (15 * sizeof(CyU3PDmaChannel));
    if ((glChHandleP2U == 0) || (glChHandleU2P == 0))
    {
        goto destroy;
    }

    /* Create P2S write channel. */
    dmaConfig.size           = CY_FX_P2S_DMA_BUF_SIZE;
    dmaConfig.count          = CY_FX_P2S_DMA_BUF_COUNT;
    dmaConfig.prodSckId      = (CyU3PDmaSocketId_t)(CY_U3P_PIB_SOCKET_0 | wrSock);
    dmaConfig.consSckId      = (CyU3PDmaSocketId_t)(CY_U3P_SIB_SOCKET_0 | CY_FX_SIB_P2S_WRITE_SOCKET);
    dmaConfig.dmaMode        = CY_U3P_DMA_MODE_BYTE;
    dmaConfig.notification   = CY_U3P_DMA_CB_PROD_EVENT;
    dmaConfig.cb             = CyFxPIBP2SDmaCallback;
    dmaConfig.prodHeader     = 0;
    dmaConfig.prodFooter     = 0;
    dmaConfig.consHeader     = 0;
    dmaConfig.prodAvailCount = 0;

    status = CyU3PDmaChannelCreate (&glChWrHandleP2S, CY_U3P_DMA_TYPE_AUTO, &dmaConfig);
    if (status != CY_U3P_SUCCESS)
    {
        goto destroy;
    }


    /* Create U2S write channel. */
    dmaConfig.size         = 1024 * CY_FX_MSC_EP_BURST_SIZE;
    dmaConfig.count        = CY_FX_MSC_EP_BUF_COUNT;
    dmaConfig.prodSckId    = (CyU3PDmaSocketId_t)(CY_U3P_UIB_SOCKET_PROD_0 | CY_FX_MSC_EP_BULK_OUT_SOCKET);
    dmaConfig.consSckId    = (CyU3PDmaSocketId_t)(CY_U3P_SIB_SOCKET_0 | CY_FX_SIB_U2S_WRITE_SOCKET);
    dmaConfig.notification = CY_U3P_DMA_CB_PROD_EVENT | CY_U3P_DMA_CB_RECV_CPLT;
    dmaConfig.cb           = CyFxPIBU2SDmaCallback;

    status = CyU3PDmaChannelCreate (&glChWrHandleU2S, CY_U3P_DMA_TYPE_AUTO, &dmaConfig);
    if (status != CY_U3P_SUCCESS)
    {
        goto destroy;
    }

    /* Create S2P read channel. */
    dmaConfig.size           = CY_FX_S2P_DMA_BUF_SIZE;
    dmaConfig.count          = CY_FX_S2P_DMA_BUF_COUNT;    
    dmaConfig.prodSckId      = (CyU3PDmaSocketId_t)(CY_U3P_SIB_SOCKET_0 | CY_FX_SIB_S2P_READ_SOCKET);
    dmaConfig.consSckId      = (CyU3PDmaSocketId_t)(CY_U3P_PIB_SOCKET_0 | rdSock);
    dmaConfig.notification   = 0;
    dmaConfig.cb             = 0;

    status = CyU3PDmaChannelCreate (&glChRdHandleS2P, CY_U3P_DMA_TYPE_AUTO, &dmaConfig);
    if (status != CY_U3P_SUCCESS)
    {
        goto destroy;
    }

    /* Create S2U read channel. */
    dmaConfig.size           = 1024 * CY_FX_MSC_EP_BURST_SIZE;
    dmaConfig.count          = CY_FX_MSC_EP_BUF_COUNT;
    dmaConfig.prodSckId      = (CyU3PDmaSocketId_t)(CY_U3P_SIB_SOCKET_0 | CY_FX_SIB_S2U_READ_SOCKET);
    dmaConfig.consSckId      = (CyU3PDmaSocketId_t)(CY_U3P_UIB_SOCKET_CONS_0 | CY_FX_MSC_EP_BULK_IN_SOCKET);
    dmaConfig.notification   = CY_U3P_DMA_CB_SEND_CPLT;
    dmaConfig.cb             = CyFxPIBS2UDmaCallback;

    status = CyU3PDmaChannelCreate (&glChRdHandleS2U, CY_U3P_DMA_TYPE_AUTO, &dmaConfig);
    if (status != CY_U3P_SUCCESS)
    {
        goto destroy;
    }

    return status;

destroy:
  
    /* Free any memory that might have been allocated. */
    if (glMscCbwBuffer)
        CyU3PDmaBufferFree (glMscCbwBuffer);
    if (glMscCswBuffer)
        CyU3PDmaBufferFree (glMscCswBuffer);
    if (glMscDataBuffer)
        CyU3PDmaBufferFree (glMscDataBuffer);
    if (glPibDataBuffer)
        CyU3PDmaBufferFree (glPibDataBuffer);
    if (glChHandleP2U)
        CyU3PDmaBufferFree (glChHandleP2U);
    if (glChHandleU2P)
        CyU3PDmaBufferFree (glChHandleU2P);

    glMscCbwBuffer   = 0;
    glMscCswBuffer   = 0;
    glMscDataBuffer  = 0;
    glPibDataBuffer  = 0;
    glChHandleP2U    = 0;
    glChHandleU2P    = 0;

    /* Tear down any channels that got created. */    
    CyU3PDmaChannelDestroy (&glChWrHandleP2S);
    CyU3PDmaChannelDestroy (&glChWrHandleU2S);
    CyU3PDmaChannelDestroy (&glChRdHandleS2P);
    CyU3PDmaChannelDestroy (&glChRdHandleS2U);

    return (CY_U3P_ERROR_MEMORY_ERROR);
}

/* SIB Interface Configuration Request Handler */
static uint16_t
CyFxAppSDIntfConfig (
        uint8_t gpioS0,
        uint8_t gpioS1,
        uint16_t intfParam)
{
    uint16_t                ret = CY_WB_ERROR_SUCCESS;
    CyU3PReturnStatus_t     status;   
    CyU3PSibIntfParams_t    intfParams;

    intfParams.resetGpio       = 0xFF;                          /* No GPIO control on SD/MMC power. */
    intfParams.rstActHigh      = CyTrue;                        /* Don't care as no GPIO is selected. */
    intfParams.cardDetType     = (intfParam & 1) ? (CY_U3P_SIB_DETECT_DAT_3)
                                            : (CY_U3P_SIB_DETECT_NONE);       /* Card detect based on SD_DAT[3]. */
    intfParams.voltageSwGpio   = gpioS0;                        /* Use GPIO_45 for voltage switch on S0 port. */
    intfParams.lvGpioState     = CyFalse;                       /* Driving GPIO low selects 1.8 V on SxVDDQ. */
    intfParams.writeProtEnable = (CyBool_t) (intfParam & 2);    /* Write protect handling enabled. */
    intfParams.lowVoltage      = CyTrue;                        /* Low voltage operation enabled. */
    
    intfParams.useDdr          = CyTrue;                        /* DDR clocking enabled. */
    intfParams.maxFreq         = CY_U3P_SIB_FREQ_104MHZ;        /* No S port clock limitation. */
    intfParams.cardInitDelay   = 0;                             /* No delay required between SD card insertion
                                                                   before initialization. */

    /* Set Interface Params for Port 0 */
    status = CyU3PSibSetIntfParams (0, &intfParams);
    if (status == CY_U3P_SUCCESS)
    {
        intfParams.voltageSwGpio   = gpioS1;                       /* Use GPIO_57 for voltage switch on S1 port. */
        intfParams.writeProtEnable = (intfParam & (1 << 9)) ? (CyTrue) : (CyFalse);  /* Write protect handling enabled. */        
        intfParams.cardDetType     = (intfParam & (1 << 8)) ? (CY_U3P_SIB_DETECT_DAT_3)
                                                : (CY_U3P_SIB_DETECT_NONE);       /* Card detect based on SD_DAT[3]. */

        /* Set Interface Params for Port 1 */        
        status = CyU3PSibSetIntfParams (1, &intfParams);
    }

    if (status != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "Set SIB interface parameters failed, code=%d\r\n", status);
        ret = CY_WB_ERROR_NOT_SUPPORTED;
    }

    return ret;
}


/* SIB Related events callback */
void
CyFxSibCallback (
        uint8_t             portId,
        CyU3PSibEventType   evt,
        CyU3PReturnStatus_t status)
{
    CyU3PDmaSocketConfig_t sockConf;

    if (evt == CY_U3P_SIB_EVENT_XFER_CPLT)
    {        
        /* U<->S Operation Handler */
        if (glUsbOperation)
        {
            if (status != CY_U3P_SUCCESS)
            {
                glMscCmdStatus     = 1;
                glLunDatabase[glMscCmdLun].sensePtr = CY_FX_MSC_SENSE_CRC_ERROR;
                
                /* Transfer has failed. Reset the DMA channel. */
                /* Residue Data Computation */
                if (glCmdDirection)
                {
                    CyU3PDmaSocketGetConfig ((uint16_t)(CY_U3P_UIB_SOCKET_CONS_0 | CY_FX_MSC_EP_BULK_IN_SOCKET),
                            &sockConf);
                    glMscResidue -= sockConf.xferCount;
                    CyU3PDmaChannelReset (&glChRdHandleS2U);
                }
                else
                {
                    CyU3PDmaSocketGetConfig ((uint16_t)(CY_U3P_UIB_SOCKET_PROD_0 + CY_FX_MSC_EP_BULK_OUT_SOCKET),
                            &sockConf);
                    glMscResidue -= sockConf.xferCount;
                    CyU3PDmaChannelReset (&glChWrHandleU2S);
                }
            }
            else
            {
                glMscCmdStatus = 0;
                glMscResidue   = 0;
                glLunDatabase[glMscCmdLun].sensePtr = CY_FX_MSC_SENSE_OK;
            }
            
            CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
        }
        else
        {
            /* P<->S Operation Handler */
            glXferStatus = status;
            CyU3PEventSet (&glXferEvent, CY_FX_SIB_XFER_CPLT_EVENT, CYU3P_EVENT_OR);
        }
    }

    /* Storage Device Insertion Event */
    if (evt == CY_U3P_SIB_EVENT_INSERT)
    {
        uint8_t i = 0;

        AppDebugPrint (2, "Insert event on port %d\r\n", portId);
        CyFxAppQueryDevStatus ();

        for (i = 0; i < CY_FX_MSC_LUN_COUNT; i++)
        {
            /* Mark any LUNs that are mapped to this port as active and changed. */
            if (glLunDatabase[i].port == portId)
        {
                glLunDatabase[i].state   = CyTrue;
                glLunDatabase[i].changed = glLunDatabase[i].released;
            }
        }

        /* Report to AP */        
        CyFxAppSendHotPlugEvent (portId, CyFalse);
    }

    /* Storage Device Removal Event */    
    if (evt == CY_U3P_SIB_EVENT_REMOVE)
    {	      
        uint8_t i = 0;

        AppDebugPrint (2, "Remove event on port %d\r\n", portId);
        CyFxAppQueryDevStatus ();

        for (i = 0; i < CY_FX_MSC_LUN_COUNT; i++)
        {
            /* Mark any LUNs that are mapped to this port as inactive and changed. */
            if (glLunDatabase[i].port == portId)
        {
                glLunDatabase[i].state   = CyFalse;
                glLunDatabase[i].changed = glLunDatabase[i].released;
            }
        }

        /* Report to AP */
        CyFxAppSendHotPlugEvent (portId, CyTrue);
    }

    /* Storage Operation Failure Event */
    if ((evt == CY_U3P_SIB_EVENT_DATA_ERROR) || (evt == CY_U3P_SIB_EVENT_ABORT))
    {
        /* Transfer has failed. Reset the DMA channel. */
        if (glCmdDirection)
        {
            CyU3PDmaChannelReset ((CyU3PDmaChannel *) &glChRdHandleS2U);
        }
        else
        {
            CyU3PDmaChannelReset ((CyU3PDmaChannel *) &glChWrHandleU2S);
        }

        /* Make sure the request is aborted and that the controller is reset. */
        CyU3PSibAbortRequest (portId);
    }
}

/* SIB Start Request Handler */
static uint16_t
CyFxAppStartStorage (
        uint8_t rdSock,
        uint8_t wrSock)
{
    uint16_t                ret = CY_WB_ERROR_SUCCESS;
    CyU3PReturnStatus_t     status;

    /* Start Storage Module */
    status = CyU3PSibStart ();
    if (status != CY_U3P_SUCCESS)
    {
        AppDebugPrint (4, "SIB start failed, code=%d\r\n", status);
        CyFxAppErrorHandler (status);
    }

    CyU3PSibRegisterCbk (CyFxSibCallback);

    /* Writes to the SD cards are to be committed on 8 MB intervals. */
    CyU3PSibSetWriteCommitSize (CY_FX_MSC_PORT0, CY_FX_SIB_WR_CMT_SIZE);
    CyU3PSibSetWriteCommitSize (CY_FX_MSC_PORT1, CY_FX_SIB_WR_CMT_SIZE); 

    /* SIB Write Timer Update */
    CyU3PSibWriteTimerModify (CY_FX_SIB_WR_TIMER_PERIOD);    

    /* Start USB Module to enable U<->S DMA Channel Creation */
    status = CyU3PUsbStart ();
    if (status != CY_U3P_SUCCESS)
    {
        return glWbErrMap[status];
    }   

    /* Create the required DMA channels and register with the PIB block. */
    status = CyFxPibApplnDmaInit (rdSock, wrSock);
    if (status != CY_U3P_SUCCESS)
    {
        ret = CY_WB_ERROR_MEDIA_ACCESS_FAILURE;
    }

    /* Default Partition Ownership : AP */
    glMscPartOwner[CY_FX_MSC_PORT0] = 0;
    glMscPartOwner[CY_FX_MSC_PORT1] = 0;    

    return ret;
}


/* Destroy Existing DMA Channels */
static void
CyFxPibApplnDmaDeInit (
        void)
{
    CyU3PDmaChannelDestroy (&glChWrHandleP2S);
    CyU3PDmaChannelDestroy (&glChWrHandleU2S);
    CyU3PDmaChannelDestroy (&glChRdHandleS2P);
    CyU3PDmaChannelDestroy (&glChRdHandleS2U);

    CyU3PDmaBufferFree (glMscCbwBuffer);
    CyU3PDmaBufferFree (glMscCswBuffer);
    CyU3PDmaBufferFree (glMscDataBuffer);
    CyU3PDmaBufferFree (glPibDataBuffer);    

    glMscDataBuffer  = 0;
    glMscCswBuffer   = 0;
    glMscCbwBuffer   = 0;
    glPibDataBuffer  = 0;
}

/* SIB Stop Request Handler */
static uint16_t
CyFxAppStopStorage (
        void)
{
    /* Destroy all DMA channels and free up scratch buffers. */
    CyFxPibApplnDmaDeInit ();

    /* De-init the PIB module. */
    CyU3PSibStop ();

    /* Stop the USB stack. */
    CyU3PUsbStop ();    

    /* Default Partition Ownership : AP */
    glMscPartOwner[CY_FX_MSC_PORT0] = 0;
    glMscPartOwner[CY_FX_MSC_PORT1] = 0;    

    return CY_WB_ERROR_SUCCESS;
}


/* SIB->AP Read Command Handler */
static uint16_t
CyFxAppProcReadHandler (
        uint8_t addr)
{
    CyU3PReturnStatus_t apiStat;
    uint32_t blkAddr, evStat;
    uint16_t numBlks;
    uint8_t  port, unit;

    /* Port ID & Unit ID Computer */
    if (addr >= CY_RQT_STOR_READ_P1_U0)
    {
        port = 1;
        unit = addr - CY_RQT_STOR_READ_P1_U0;
    }
    else
    {
        port = 0;
        unit = addr - CY_RQT_STOR_READ_P0_U0;
    }

    /* Ownership Checker */
    if ((glMscPartOwner[port] & (1 << unit)) != 0)
    {
        return CY_WB_ERROR_MEDIA_NOT_CLAIMED;
    }

    /* SIB Arguments Fetcher */
    numBlks = glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1];
    if (numBlks == 0)
    {
        return CY_WB_ERROR_SUCCESS;
    }

    blkAddr = (glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][3] << 16) | glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][2];

    /* Clear any stale events. */
    glUsbOperation = CyFalse;
    CyU3PEventGet (&glXferEvent, CY_FX_SIB_XFER_CPLT_EVENT, CYU3P_EVENT_OR_CLEAR, &evStat, CYU3P_NO_WAIT);

    /* DMA Channel Setup & SIB Read Request */
    apiStat = CyU3PDmaChannelSetXfer (&glChRdHandleS2P, (numBlks * CY_FX_SIB_MAX_BLOCK_SIZE));
    if (apiStat == CY_U3P_SUCCESS)
    {
        apiStat = CyU3PSibReadWriteRequest (CyTrue, port, unit, numBlks, blkAddr, (uint8_t) (CY_U3P_SIB_SOCKET_0 | CY_FX_SIB_S2P_READ_SOCKET));
    }
    
    if (apiStat != CY_U3P_SUCCESS)
    {
        CyU3PDmaChannelReset (&glChRdHandleS2P);
        return (glWbErrMap[apiStat]);
    }

    /* SIB Read Completion Event Wait for Time Period based on number of data blocks involved in transaction */
    apiStat = CyU3PEventGet (&glXferEvent, CY_FX_SIB_XFER_CPLT_EVENT, CYU3P_EVENT_OR_CLEAR, &evStat,
            (CY_FX_SIB_WR_TIMER_PERIOD + (numBlks * 100)));
    if (apiStat != CY_U3P_SUCCESS)
    {
        CyFxAppSendDebugMessage (0x34E0, apiStat, 0);
        CyU3PSibAbortRequest (port);
        CyU3PDmaChannelReset (&glChRdHandleS2P);
        return (glWbErrMap[apiStat]);
    }
    else
    {
        if (glXferStatus != CY_U3P_SUCCESS)
        {
            CyFxAppSendDebugMessage (0x34E1, CY_U3P_GET_MSW (glXferStatus), CY_U3P_GET_LSW (glXferStatus));
            CyU3PSibAbortRequest (port);
            CyU3PDmaChannelReset (&glChRdHandleS2P);            
            return CY_WB_ERROR_MEDIA_ACCESS_FAILURE;
        }

        CyU3PDmaChannelWaitForCompletion (&glChRdHandleS2P, (CY_FX_SIB_WR_TIMER_PERIOD + (numBlks * 100)));
    }

    return CY_WB_ERROR_SUCCESS;
}


/* AP->SIB Write Command Handler */
static uint16_t
CyFxAppProcWriteHandler (
        uint8_t addr)
{
    CyU3PReturnStatus_t apiStat;
    uint32_t blkAddr, evStat;
    uint16_t numBlks;
    uint8_t  port, unit;

    /* Port ID & Unit ID Computer */    
    if (addr >= CY_RQT_STOR_WRITE_P1_U0)
    {
        port = 1;
        unit = addr - CY_RQT_STOR_WRITE_P1_U0;
    }
    else
    {
        port = 0;
        unit = addr - CY_RQT_STOR_WRITE_P0_U0;
    }

    /* Ownership Checker */    
    if ((glMscPartOwner[port] & (1 << unit)) != 0)
    {
        return CY_WB_ERROR_MEDIA_NOT_CLAIMED;
    }

    /* SIB Arguments Fetcher */    
    numBlks = glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1];
    if (numBlks == 0)
    {
        return CY_WB_ERROR_SUCCESS;
    }
    
    blkAddr = (glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][3] << 16) | glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][2];

    /* Clear any stale events. */
    glUsbOperation = CyFalse;
    glRecvBufCount = 0;
    CyU3PEventGet (&glXferEvent, CY_FX_SIB_XFER_CPLT_EVENT, CYU3P_EVENT_OR_CLEAR, &evStat, CYU3P_NO_WAIT);

    /* DMA Channel Setup & SIB Write Request */    
    apiStat = CyU3PDmaChannelSetXfer (&glChWrHandleP2S, (numBlks * CY_FX_SIB_MAX_BLOCK_SIZE));
    if (apiStat == CY_U3P_SUCCESS)
    {        
        apiStat = CyU3PSibReadWriteRequest (CyFalse, port, unit, numBlks, blkAddr, (uint8_t) (CY_U3P_SIB_SOCKET_0 | CY_FX_SIB_P2S_WRITE_SOCKET));
    }

    if (apiStat != CY_U3P_SUCCESS)
    {
        CyFxAppSendDebugMessage (0x34E2, apiStat, glRecvBufCount);
        CyU3PDmaChannelReset (&glChWrHandleP2S);
        return (glWbErrMap[apiStat]);
    }    

    /* SIB Write Completion Event */    
    apiStat = CyU3PEventGet (&glXferEvent, CY_FX_SIB_XFER_CPLT_EVENT, CYU3P_EVENT_OR_CLEAR,
            &evStat, (CY_FX_SIB_WR_TIMER_PERIOD + (numBlks * 100)));
    if (apiStat != CY_U3P_SUCCESS)
    {
        CyFxAppSendDebugMessage (0x34E3, apiStat, glRecvBufCount);
        CyU3PSibAbortRequest (port);
        CyU3PDmaChannelReset (&glChWrHandleP2S);
        return (glWbErrMap[apiStat]);
    }
    else
    {
        if (glXferStatus != CY_U3P_SUCCESS)
        {
            CyFxAppSendDebugMessage (0x34E4, CY_U3P_GET_MSW (glXferStatus), CY_U3P_GET_LSW (glXferStatus));
            CyU3PSibAbortRequest (port);
            CyU3PDmaChannelReset (&glChWrHandleP2S);            
            return CY_WB_ERROR_MEDIA_ACCESS_FAILURE;
        }        
    }

    return CY_WB_ERROR_SUCCESS;
}

/* Update ownership for LUNs based on current ownership bitmaps. */
static void
CyFxMscUpdateLunOwners (
        void)
{
    uint8_t i, released[CY_FX_SIB_PORTS][CY_FX_SIB_PARTITIONS] = {{0}};

    /* Get the current ownership for each partition. */
    for (i = 0; i < CY_FX_SIB_PARTITIONS; i++)
    {
        if ((glMscPartOwner[CY_FX_MSC_PORT0] & (1 << i)) != 0)
            released[CY_FX_MSC_PORT0][i] = CyTrue;
        if ((glMscPartOwner[CY_FX_MSC_PORT1] & (1 << i)) != 0)
            released[CY_FX_MSC_PORT1][i] = CyTrue;
    }

    /* Now, for each partition; check whether ownership has changed and update LUN state accordingly. */
    for (i = 0; i < CY_FX_MSC_LUN_COUNT; i++)
    {
        if ((glLunDatabase[i].port < CY_FX_SIB_PORTS) && (glLunDatabase[i].unit < CY_FX_SIB_PARTITIONS))
        {
            if (glLunDatabase[i].released != released[glLunDatabase[i].port][glLunDatabase[i].unit])
            {
                glLunDatabase[i].changed  = glLunDatabase[i].state;
            }

            glLunDatabase[i].released = released[glLunDatabase[i].port][glLunDatabase[i].unit];
        }
    }
}

/* AP Storage related request Handler Function */
static void
CyFxAppHandleStorageRqt (
        void)
{
    CyU3PSibDevInfo_t   devInfo;    
    CyU3PSibLunInfo_t   unitInfo;
    CyU3PReturnStatus_t apiStat;
    uint16_t status;
    uint8_t  rqt, tag;
    uint8_t  val, port, unit;

    /* MBOX Request & Tag ID fetcher */
    rqt = (uint8_t)(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][0] & 0xFF);
    tag = (uint8_t)((glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][0] & 0x3800) >> 11);

    switch (rqt)
    {
        case CY_RQT_START_STORAGE:
            {
                uint8_t wrSock, rdSock;

                wrSock = CY_U3P_GET_MSB(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1]);
                rdSock = CY_U3P_GET_LSB(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1]);
                
                status = CyFxAppStartStorage (rdSock, wrSock);
                CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, status);
            }
            break;

        case CY_RQT_STOP_STORAGE:
            {
                status = CyFxAppStopStorage ();
                CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, status);
            }
            break;

        case CY_RQT_SD_INTERFACE_CONTROL:
            {
                uint8_t gpioS0, gpioS1;
                uint16_t intfParams;
                
                gpioS1     = CY_U3P_GET_MSB(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1]);
                gpioS0     = CY_U3P_GET_LSB(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1]);
                intfParams = glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][2];
                
                status = CyFxAppSDIntfConfig (gpioS0, gpioS1, intfParams);          
                CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, status);                
            }
            break;

        case CY_RQT_STOR_SET_OWNER:
            {
                glMscPartOwner[CY_FX_MSC_PORT0] = CY_U3P_GET_LSB (glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1]);
                glMscPartOwner[CY_FX_MSC_PORT1] = CY_U3P_GET_MSB (glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1]);
                CyFxMscUpdateLunOwners ();
                CyFxAppSendSetOwnerResp (tag, glMscPartOwner[CY_FX_MSC_PORT0], glMscPartOwner[CY_FX_MSC_PORT1]);            
            }
            break;

        case CY_RQT_STOR_GET_OWNER:
            {
                CyFxAppSendSetOwnerResp (tag, glMscPartOwner[CY_FX_MSC_PORT0], glMscPartOwner[CY_FX_MSC_PORT1]);
            }
            break;
          
        case CY_RQT_QUERY_PORT:
            {
                port = (uint8_t)(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1] >> 12);
                
                apiStat = CyU3PSibQueryDevice (port, &devInfo);
                if ((apiStat != CY_U3P_SUCCESS) || (devInfo.cardType == CY_U3P_SIB_DEV_NONE))
                    val = 0;
                else
                    val = 1;
                CyFxAppSendQueryPortResp (tag, port, val);
            }
            break;

        case CY_RQT_QUERY_DEVICE:
            {
                port = (uint8_t)(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1] >> 12);
                
                apiStat = CyU3PSibQueryDevice (port, &devInfo);
                if ((apiStat != CY_U3P_SUCCESS) || (devInfo.cardType == CY_U3P_SIB_DEV_NONE))
                    CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, CY_WB_ERROR_NO_SUCH_DEVICE);
                else
                {
                    CyFxAppSendQueryMediaResp (tag, port, devInfo.cardType, devInfo.blkLen, devInfo.locked,
                            devInfo.writeable, devInfo.numUnits, devInfo.eraseSize);
                }
            }
            break;

        case CY_RQT_QUERY_UNIT:
            {
                port = (uint8_t)(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1] >> 12);
                unit = (uint8_t)(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1] & 0x0F);
                
                apiStat = CyU3PSibQueryUnit (port, unit, &unitInfo);
                if (apiStat == CY_U3P_SUCCESS)
                {
                    CyFxAppSendQueryUnitResp (tag, port, unit, unitInfo.blockSize, unitInfo.startAddr,
                            unitInfo.numBlocks);
                }
                else
                {
                    CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, glWbErrMap[apiStat]);                    
                }
            }
            break;

        case CY_RQT_STOR_READ_P0_U0:
        case CY_RQT_STOR_READ_P0_U1:
        case CY_RQT_STOR_READ_P1_U0:
        case CY_RQT_STOR_READ_P1_U1:
            {
                status = CyFxAppProcReadHandler (rqt);
                CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, status);
            }
            break;

        case CY_RQT_STOR_WRITE_P0_U0:
        case CY_RQT_STOR_WRITE_P0_U1:
        case CY_RQT_STOR_WRITE_P1_U0:
        case CY_RQT_STOR_WRITE_P1_U1:
            {
                status = CyFxAppProcWriteHandler (rqt);
                CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, status);
            }
            break;
           
        case CY_RQT_SD_REGISTER_READ:
            {
                port = (uint8_t)(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1] >> 12);
                unit = (uint8_t)(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1] & 0x0F);

                apiStat = CyU3PSibReadRegister (port, (CyU3PSibDevRegType)unit, glPibDataBuffer, &val);
                if (apiStat != CY_U3P_SUCCESS)
                    CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, CY_WB_ERROR_NO_SUCH_DEVICE);
                else
                    CyFxAppSendReadRegResponse (tag, glPibDataBuffer, val);
            }
            break;           
           
        case CY_RQT_PARTITION_STORAGE:
            {
                uint8_t partType[2] = {CY_U3P_SIB_LUN_DATA, CY_U3P_SIB_LUN_DATA}; 
                uint32_t partSize;
                
                port = (uint8_t)(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1] >> 12);
                partSize = (uint32_t)((glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][2] << 16) | glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][3]);
                
                status = CyU3PSibPartitionStorage (port, 2, &partSize, partType);
                CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, status);
            }
            break;
          
        case CY_RQT_PARTITION_ERASE:
            {
                port = (uint8_t)(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1] >> 12);
                
                if (((glMscPartOwner[port] & 1) != 0) || (glMscPartOwner[port] & 2) != 0)
                {
                    CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, CY_WB_ERROR_NOT_SUPPORTED);
                    break;
                }
                
                status = CyU3PSibRemovePartitions (port);
                CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, status);
            }
            break;

        case CY_RQT_ERASE_BLOCKS:
            {
                uint16_t eraseUnits;
                uint32_t eraseAddr;

                port = (uint8_t)(glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][1] >> 12);
                eraseAddr = (uint32_t) (glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][2] << 16 | glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][3]);
                eraseUnits = glPibRqtData[CY_RQT_STORAGE_RQT_CONTEXT][4];
                
                status = CyU3PSibEraseBlocks (port, eraseUnits, eraseAddr, CY_U3P_SIB_ERASE_STANDARD);
                CyFxAppSendStatusResponse (CY_RQT_STORAGE_RQT_CONTEXT, tag, status);
            }
            break;

        case CY_RQT_ABORT_P2S_XFER:
            {
                CyU3PSibAbortRequest (CY_FX_MSC_PORT0);
                CyU3PSibAbortRequest (CY_FX_MSC_PORT1);
                CyU3PDmaChannelReset (&glChWrHandleP2S);
                CyU3PDmaChannelReset (&glChRdHandleS2P);            
            }
            break;

        default:
            CyFxAppSendDebugMessage (0xFFFF, 0xFFFF, 0xFFFF);
            break;
    }
}


/* U<->S DMA path Refresher */
static void
CyFxAppResetMscDataPath (
        void)
{
    uint8_t i;
    
    CyU3PUsbSetEpNak (CY_FX_MSC_EP_BULK_OUT, CyTrue);
    CyU3PUsbSetEpNak (CY_FX_MSC_EP_BULK_IN, CyTrue);

    CyU3PBusyWait (120);    
    
    CyU3PDmaChannelReset (&glChWrHandleU2S);
    CyU3PDmaChannelReset (&glChRdHandleS2U);       

    CyU3PUsbFlushEp (CY_FX_MSC_EP_BULK_OUT);
    CyU3PUsbStall (CY_FX_MSC_EP_BULK_OUT, CyFalse, CyTrue);               
    
    CyU3PUsbFlushEp (CY_FX_MSC_EP_BULK_IN);
    CyU3PUsbStall (CY_FX_MSC_EP_BULK_IN, CyFalse, CyTrue);
       
    /* Clear any MSC session specific state. */
    for (i = 0; i < CY_FX_MSC_LUN_COUNT; i++)
    {
        glLunDatabase[i].sensePtr = CY_FX_MSC_SENSE_OK;
        glLunDatabase[i].stopped  = CyFalse;
        glLunDatabase[i].changed  = CyFalse;
    }
    
    CyU3PUsbSetEpNak (CY_FX_MSC_EP_BULK_OUT, CyFalse);
    CyU3PUsbSetEpNak (CY_FX_MSC_EP_BULK_IN, CyFalse);

    glInPhaseError  = CyFalse;
    glMscDriverBusy = CyFalse;
}


/* Function to initiate sending of data to the USB host. */
CyU3PReturnStatus_t
CyFxSendMscDataToHost (
    uint8_t  *data,
    uint32_t length)
{
    CyU3PDmaBuffer_t    dmaBuf;
    CyU3PReturnStatus_t status;

    /* Prepare the DMA Buffer */
    dmaBuf.buffer = data;
    dmaBuf.status = 0;
    dmaBuf.size   = (length + 15) & 0xFFF0;      /* Round up to a multiple of 16.  */
    dmaBuf.count  = length;

    status = CyU3PDmaChannelSetupSendBuffer (&glChRdHandleS2U, &dmaBuf);
    return status;
}

/* Function to initiate reception of data from the USB host. */
CyU3PReturnStatus_t
CyFxReceiveMscDataFromHost (
    uint8_t  *data,
    uint32_t length)
{
    CyU3PDmaBuffer_t    dmaBuf;
    CyU3PReturnStatus_t status;

    /* Prepare the DMA Buffer */
    dmaBuf.buffer = data;
    dmaBuf.status = 0;
    dmaBuf.size   = (length + 15) & 0xFFF0;      /* Round up to a multiple of 16.  */
    dmaBuf.count  = length;

    status = CyU3PDmaChannelSetupRecvBuffer (&glChWrHandleU2S, &dmaBuf);
    return status;
}

/* Common function to check all CBW parameters against expected values. */
CyBool_t
CyFxCheckMscCbwParams (
        uint8_t   lun,                  /* Lun to which the command is addressed. */
        CyBool_t  lunState,             /* Whether the LUN should be active or don't care (for INQUIRY/REQ_SENSE) */
        CyBool_t  isReadExpected,       /* Whether the op-code corresponds to a read (IN) data transfer. */
        CyBool_t  isDataCmd,            /* Whether this is a read or write command. Exact data length match is
                                           expected in data commands. */
        uint32_t  expLength,            /* Default data transfer length for this opcode. */
        uint32_t *actLength_p           /* Return parameter to pass the actual data transfer length. */
        )
{
    /* Verify that:
        1. Direction of data transfer matches the expected value.
        2. That the transfer length matches the expected value for read/write commands.
        3. That a non-zero transfer length is provided for commands that want to return data.
        4. That zero transfer length is provided for commands that have no data.
     */
    if ((isReadExpected != (glCmdDirection == 0x80)) || ((isDataCmd) && (expLength != glMscResidue)) ||
            ((expLength != 0) && (glMscResidue == 0)) || ((expLength == 0) && (glMscResidue != 0)))
    {
        glMscCmdStatus  = 1;
        glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_INVALID_PARAMETER;
        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
        return CyFalse;
    }

    /* Verify that the LUN is ready for commands that report the LUN state or do data transfers. */
    if (lunState)
    {
        if (!glLunDatabase[lun].changed)
        {
            if ((!glLunDatabase[lun].state) || (!glLunDatabase[lun].released) || (glLunDatabase[lun].stopped))
            {
                glMscCmdStatus  = 1;
                glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_NO_MEDIA;
                CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                return CyFalse;
            }
            else
            {
                glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_OK;
            }
        }
        else
        {
            /* Make sure we return a MEDIA CHANGE status at least one time. */
            glMscCmdStatus = 1;
            glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_MEDIA_CHANGED;
            glLunDatabase[lun].changed  = CyFalse;
            CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
            return CyFalse;
        }
    }

    /* Calculate the size of data to be transferred in the case of non-data commands. */
    if (!isDataCmd)
    {
        if (expLength > glMscResidue)
        {
            *actLength_p = glMscResidue;
            glMscResidue = 0;
        }
        else
        {
            *actLength_p  = expLength;
            glMscResidue -= expLength;
        }
    }

    return CyTrue;
}

/* SCSI Mode Select Command Handler */
static void
CyFxHandleModeSelectData (
        void)
{
    CyU3PReturnStatus_t status;
    CyU3PDmaBuffer_t    dmaBuf;

    /* Verify the received data and copy into the current sense parameter structure. */
    status = CyU3PDmaChannelWaitForRecvBuffer (&glChWrHandleU2S, &dmaBuf, CYU3P_NO_WAIT);
    if (status == CY_U3P_SUCCESS)
    {
        if ((dmaBuf.count >= (4 + sizeof (CyFxMscCachingDefault))) &&
                (glMscDataBuffer[4] == CY_FX_SCSI_MODESENS_PAGE_CACHE))
        {
            CyU3PMemCopy ((uint8_t *)&CyFxMscCachingCurrent[2], (uint8_t *)&glMscDataBuffer[6],
                    sizeof (CyFxMscCachingDefault) - 2);
            glMscCmdStatus = 0;
        }
        else
        {
            glMscCmdStatus = 1;
            glLunDatabase[glMscCmdLun].sensePtr = CY_FX_MSC_SENSE_INVALID_PARAMETER;
        }
    }
    else
    {
        glMscCmdStatus = 1;
        glLunDatabase[glMscCmdLun].sensePtr = CY_FX_MSC_SENSE_INVALID_PARAMETER;
    }

    /* Signal the thread to send status data out. */
    glMscResidue = 0;
    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_DATASENT_EVENT_FLAG, CYU3P_EVENT_OR);
}


/* SCSI Mode Sense Command Handler */
static void
CyFxHandleModeSenseCommand (
        uint8_t lun)
{
    CyU3PReturnStatus_t status;
    uint32_t            dataLength;
    uint8_t             mode_page, mode_pc;
    uint8_t            *cache_p = 0;

    /* We only support the Caching mode page. */
    mode_page = glMscCbwBuffer[CY_FX_SCSI_MODESENS_PC_OFFSET] & CY_FX_SCSI_MODESENS_PAGE_MASK;
    if ((mode_page != CY_FX_SCSI_MODESENS_PAGE_CACHE) && (mode_page != CY_FX_SCSI_MODESENS_PAGE_ALL))
    {
        glMscCmdStatus  = 1;
        glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
        return;
    }

    mode_pc = glMscCbwBuffer[CY_FX_SCSI_MODESENS_PC_OFFSET] & CY_FX_SCSI_MODESENS_PC_MASK;
    switch (mode_pc)
    {
        case CY_FX_SCSI_MODESENS_PC_CHANGE:
            cache_p = (uint8_t *)CyFxMscCachingChangeable;
            break;

        case CY_FX_SCSI_MODESENS_PC_CURRENT:
            cache_p = (uint8_t *)CyFxMscCachingCurrent;
            break;

        case CY_FX_SCSI_MODESENS_PC_DEFAULT:
            cache_p = (uint8_t *)CyFxMscCachingDefault;
            break;

        default:
            /* No saved page support. */
            glMscCmdStatus  = 1;
            glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_NO_SAVEPAGE_SUPPORT;
            CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
            return;
    }

    glMscDataBuffer[0] = 0x03;
    glMscDataBuffer[1] = 0x00;
    glMscDataBuffer[2] = (glLunDatabase[lun].writeable) ? 0x00 : 0x80;
    glMscDataBuffer[3] = 0x00;

    CyU3PMemCopy ((uint8_t *)&glMscDataBuffer[4], cache_p, sizeof (CyFxMscCachingDefault));
    dataLength = 4 + sizeof (CyFxMscCachingDefault);
    glMscDataBuffer[0] = dataLength - 1;

    if (!CyFxCheckMscCbwParams (lun, CyTrue, CyTrue, CyFalse, dataLength, &dataLength))
        return;

    glMscState = CY_FX_MSC_STATE_DATA;
    status = CyFxSendMscDataToHost (glMscDataBuffer, dataLength);
    if (status != CY_U3P_SUCCESS)
    {
        glMscCmdStatus  = 1;
        glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_CRC_ERROR;
        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
    }
}


/* Parse the received CBW and handle the command. */
void
CyFxAppParseScsiCommand (
    void)
{
    CyU3PReturnStatus_t status;
    uint8_t cmd = glMscCbwBuffer[15];
    uint8_t lun = glMscCbwBuffer[13] & 0x0F;
    uint32_t startAddr, dataLength;
    uint16_t numBlks;

    uint8_t *inqData_p = 0;
    uint32_t inqDataLen = 0;

    /* Direction of data transfer. */
    glCmdDirection = glMscCbwBuffer[12] & 0x80;

    /* Expected transfer length. */
    glMscResidue   = *((uint32_t *)(glMscCbwBuffer + 8));
    glMscCmdStatus = 0;

    /* Invalid LUN. */
    if (lun >= CY_FX_MSC_LUN_COUNT)
    {
        glMscCmdStatus  = 1;
        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
        return;
    }

    glMscCmdLun  = lun;
    glCurrentCmd = cmd;

    /* Execute the command */
    switch (cmd)
    {
        case CY_FX_MSC_SCSI_INQUIRY:
            {
                if ((glMscCbwBuffer[CY_FX_SCSI_INQCMD_EVPD_OFFSET] & CY_FX_SCSI_INQCMD_EVPD_MASK) != 0)
                {
                    /* Request for Vital Product Data. */
                    switch (glMscCbwBuffer[CY_FX_SCSI_INQCMD_PCODE_OFFSET])
                    {
                        case CY_FX_SCSI_INQCMD_PCODE_LIST:
                            /* List of supported Vital Product Data Pages. */
                            inqData_p  = (uint8_t *)CyFxMscSupportedPageList;
                            inqDataLen = sizeof (CyFxMscSupportedPageList);
                            break;

                        case CY_FX_SCSI_INQCMD_PCODE_SERNUM:
                            /* Unit Serial Number Page. */
                            inqData_p  = (uint8_t *)CyFxMscScsiSerialNumber;
                            inqDataLen = sizeof (CyFxMscScsiSerialNumber);
                            break;

                        case CY_FX_SCSI_INQCMD_PCODE_DEVIDENT:
                            /* Device Identification Page. */
                            inqData_p  = (uint8_t *)CyFxMscScsiDevIdent;
                            inqDataLen = sizeof (CyFxMscScsiDevIdent);

                            /* Update the last byte based on the LUN. */
                            CyFxMscScsiDevIdent[inqDataLen - 1] = '0' + lun;
                            break;

                        case CY_FX_SCSI_INQCMD_PCODE_BLKLIMITS:
                            /* Block Limits Page. */
                            inqData_p  = (uint8_t *)CyFxMscScsiBlkLimits;
                            inqDataLen = sizeof (CyFxMscScsiBlkLimits);
                            break;

                        default:
                            /* No other VPD pages supported. */
                            glMscCmdStatus  = 1;
                                glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
                            CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                            break;
                    }

                    if (glMscCmdStatus != 0)
                        break;
                }
                else
                {
                    /* Page code must be zero if Vital Product Data is not being requested. */
                    if (glMscCbwBuffer[CY_FX_SCSI_INQCMD_PCODE_OFFSET] != 0)
                    {
                        glMscCmdStatus  = 1;
                        glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_INVALID_PARAMETER;
                        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                        break;
                    }

                    inqDataLen = CY_FX_SCSI_INQUIRY_DATALEN;
                    inqData_p  = (uint8_t *)CyFxMscScsiInquiryData;
                }

                if (!CyFxCheckMscCbwParams (lun, CyFalse, CyTrue, CyFalse,
                            inqDataLen, &dataLength))
                    break;

                glMscState = CY_FX_MSC_STATE_DATA;
                status = CyFxSendMscDataToHost (inqData_p, dataLength);
                
                if (status != CY_U3P_SUCCESS)
                {
                    glMscCmdStatus  = 1;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        case CY_FX_MSC_SCSI_READ_CAPACITY:
        case CY_FX_MSC_SCSI_READ_FORMAT_CAPACITY:
            {
                uint8_t i = 0;

                if (!CyFxCheckMscCbwParams (lun, CyTrue, CyTrue, CyFalse,
                            ((cmd == CY_FX_MSC_SCSI_READ_CAPACITY) ? 8 : 12), &dataLength))
                    break;

                CyU3PMemSet (glMscDataBuffer, 0, 12);
                if (cmd == CY_FX_MSC_SCSI_READ_FORMAT_CAPACITY)
                {
                    glMscDataBuffer[3] = 0x08;
                    glMscDataBuffer[8] = 0x02;
                    i = 4;
                }

                glMscDataBuffer[i++] = (uint8_t)(glLunDatabase[lun].capacity >> 24);
                glMscDataBuffer[i++] = (uint8_t)(glLunDatabase[lun].capacity >> 16);
                glMscDataBuffer[i++] = (uint8_t)(glLunDatabase[lun].capacity >> 8);
                glMscDataBuffer[i++] = (uint8_t)(glLunDatabase[lun].capacity);

                i += 2;
                glMscDataBuffer[i++] = 0x02;
                glMscDataBuffer[i++] = 0x00;

                glMscState = CY_FX_MSC_STATE_DATA;
                status = CyFxSendMscDataToHost (glMscDataBuffer, dataLength);
                if (status != CY_U3P_SUCCESS) 
                {
                    glMscCmdStatus  = 1;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        case CY_FX_MSC_SCSI_REQUEST_SENSE:
            {
                if ((glMscCbwBuffer[CY_FX_SCSI_RQSCMD_DESC_OFFSET] & CY_FX_SCSI_REQSENSE_DESC) != 0)
                {
                    glMscCmdStatus  = 1;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }

                if (!CyFxCheckMscCbwParams (lun, CyFalse, CyTrue, CyFalse, 
                            CY_FX_MSC_REPONSE_DATA_MAX_COUNT, &dataLength))
                    break;

                CyU3PMemSet (glMscDataBuffer, 0, 18);
                glMscDataBuffer[0]  = 0x70;
                glMscDataBuffer[2]  = glReqSenseCode[glLunDatabase[lun].sensePtr][0];
                glMscDataBuffer[7]  = 0x0A;
                glMscDataBuffer[12] = glReqSenseCode[glLunDatabase[lun].sensePtr][1];
                glMscDataBuffer[13] = glReqSenseCode[glLunDatabase[lun].sensePtr][2];

                glMscState = CY_FX_MSC_STATE_DATA;

                status = CyFxSendMscDataToHost (glMscDataBuffer, dataLength);
                if (status != CY_U3P_SUCCESS)
                {
                    glMscCmdStatus  = 1;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

            /* No real support for Verify, Start/stop unit and Prevent/Allow Medium Removal commands. */
        case CY_FX_MSC_SCSI_VERIFY_10:
        case CY_FX_MSC_SCSI_PREVENT_ALLOW_MEDIUM:
        case CY_FX_MSC_SCSI_TEST_UNIT_READY:
            {
                if (!CyFxCheckMscCbwParams (lun, CyTrue, CyFalse, CyFalse, 0, &dataLength))
                    break;

                glMscState = CY_FX_MSC_STATE_STATUS;
            }
            break;

        case CY_FX_MSC_SCSI_START_STOP_UNIT:
            {
                if (!CyFxCheckMscCbwParams (lun, CyFalse, CyFalse, CyFalse, 0, &dataLength))
                    break;

                /* Start/Stop the unit as desired by the user. */
                if ((glMscCbwBuffer[CY_FX_SCSI_STOPCMD_LOEJ_OFFSET] & CY_FX_SCSI_STOPCMD_LOEJ_VAL) != 0)
                {
                    if ((glMscCbwBuffer[CY_FX_SCSI_STOPCMD_LOEJ_OFFSET] & CY_FX_SCSI_STOPCMD_START_VAL) == 0)
                        glLunDatabase[lun].stopped = CyTrue;
                    else
                        glLunDatabase[lun].stopped = CyFalse;
                }

                glMscState = CY_FX_MSC_STATE_STATUS;
            }
            break;

        case CY_FX_MSC_SCSI_SEND_DIAG:
            {
                /* In case of self-test, report success. In other cases, report no support. */
                if ((glMscCbwBuffer[CY_FX_SCSI_SENDDIAG_SFTEST_OFFSET] & CY_FX_SCSI_SENDDIAG_SFTEST_MASK) == 0)
                {
                    glMscCmdStatus  = 1;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }

                if (!CyFxCheckMscCbwParams (lun, CyTrue, CyFalse, CyFalse, 0, &dataLength))
                    break;

                glMscState = CY_FX_MSC_STATE_STATUS;
            }
            break;

        case CY_FX_MSC_SCSI_MODE_SELECT_6:
            {
                if ((glMscCbwBuffer[CY_FX_SCSI_MODESELECT_SP_OFFSET] & CY_FX_SCSI_MODESELECT_SP_MASK) != 0)
                {
                    /* Saving changes is not supported. */
                    glMscCmdStatus  = 1;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }

                if (!CyFxCheckMscCbwParams (lun, CyTrue, CyFalse, CyFalse, glMscResidue, &dataLength))
                    break;

                if (dataLength != 0)
                {
                    glMscState = CY_FX_MSC_STATE_DATA;
                    status = CyFxReceiveMscDataFromHost (glMscDataBuffer, dataLength);
                    if (status != CY_U3P_SUCCESS)
                    {
                        glMscCmdStatus  = 1;
                        glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_CRC_ERROR;
                        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    }
                }
                else
                {
                    glMscState = CY_FX_MSC_STATE_STATUS;
                }
            }
            break;

        case CY_FX_MSC_SCSI_MODE_SENSE_6:
            {
                CyFxHandleModeSenseCommand (lun);
            }
            break;

        case CY_FX_MSC_SCSI_READ_10:
            {
                startAddr = (((uint32_t)glMscCbwBuffer[17] << 24) | ((uint32_t)glMscCbwBuffer[18] << 16) |
                        ((uint32_t)glMscCbwBuffer[19] << 8) | ((uint32_t)glMscCbwBuffer[20]));
                numBlks   = (((uint16_t)glMscCbwBuffer[22] << 8) | ((uint16_t)glMscCbwBuffer[23]));

                if (!CyFxCheckMscCbwParams (lun, CyTrue, CyTrue, CyTrue, (numBlks * 512),
                            &dataLength))
                    break;
                
                if (numBlks == 0)
                {
                    glMscCmdStatus = 0;
                    glMscResidue   = 0;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_OK;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }                
                
                glUsbOperation = CyTrue;
                glMscState = CY_FX_MSC_STATE_DATA;
                status = CyU3PDmaChannelSetXfer (&glChRdHandleS2U, (numBlks * CY_FX_SIB_MAX_BLOCK_SIZE));
                if (status == CY_U3P_SUCCESS)
                {
                    status = CyU3PSibReadWriteRequest (CY_FX_SIB_READ, glLunDatabase[lun].port, glLunDatabase[lun].unit,
                            numBlks, startAddr, (CyU3PDmaSocketId_t)(CY_FX_SIB_S2U_READ_SOCKET));
                    if (status != CY_U3P_SUCCESS)
                    {
                        /* Abort the DMA Channel */
                        CyU3PDmaChannelReset (&glChRdHandleS2U);
                    }
                }	

                if (status != CY_U3P_SUCCESS)
                {
                    glMscCmdStatus  = 1;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        case CY_FX_MSC_SCSI_WRITE_10:
            {
                startAddr = (((uint32_t)glMscCbwBuffer[17] << 24) | ((uint32_t)glMscCbwBuffer[18] << 16) |
                        ((uint32_t)glMscCbwBuffer[19] << 8) | ((uint32_t)glMscCbwBuffer[20]));
                numBlks   = (((uint16_t)glMscCbwBuffer[22] << 8) | ((uint16_t)glMscCbwBuffer[23]));

                if (!CyFxCheckMscCbwParams (lun, CyTrue, CyFalse, CyTrue, (numBlks * 512),
                            &dataLength))
                    break;

                if (numBlks == 0)
                {
                    glMscCmdStatus = 0;
                    glMscResidue   = 0;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_OK;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }           

                /* LUN Writeable */
                if (!glLunDatabase[lun].writeable)
                {
                    glMscCmdStatus  = 1;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_WRITE_PROTECT;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }

                glUsbOperation = CyTrue;
                glMscState = CY_FX_MSC_STATE_DATA;
                status = CyU3PDmaChannelSetXfer (&glChWrHandleU2S, (numBlks * CY_FX_SIB_MAX_BLOCK_SIZE));
                if (status == CY_U3P_SUCCESS)
                {
                    status = CyU3PSibReadWriteRequest (CY_FX_SIB_WRITE, glLunDatabase[lun].port, glLunDatabase[lun].unit,
                            numBlks, startAddr, (CyU3PDmaSocketId_t)(CY_FX_SIB_U2S_WRITE_SOCKET));
                    if (status != CY_U3P_SUCCESS)
                    {
                        /* Abort the DMA Channel */
                        CyU3PDmaChannelReset (&glChWrHandleU2S);
                    }
                }

                if (status != CY_U3P_SUCCESS)
                {
                    glMscCmdStatus  = 1;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        case CY_FX_MSC_SCSI_SYNCCACHE_10:
            {
                if (!CyFxCheckMscCbwParams (lun, CyTrue, CyFalse, CyFalse, 0, &dataLength))
                    break;

                /* Make sure that all data is committed to the Storage device. */
                CyU3PSibCommitReadWrite ((lun >= CY_FX_SIB_PARTITIONS) ? 1 : 0);
                glMscState = CY_FX_MSC_STATE_STATUS;
            }
            break;

        case CY_FX_MSC_SCSI_READCAP_16:
            {
                /* Verify that the SERVICE ACTION field is set to the expected value. */
                if (glMscCbwBuffer[CY_FX_SCSI_READCAP16_SERVACT_OFFSET] != CY_FX_SCSI_READCAP16_SERVACT_VALUE)
                {
                    glMscCmdStatus  = 1;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_INVALID_FIELD_IN_CBW;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                    break;
                }

                if (!CyFxCheckMscCbwParams (lun, CyTrue, CyTrue, CyFalse, 32, &dataLength))
                    break;

                CyU3PMemSet (glMscDataBuffer, 0, 32);

                /* First four bytes are always zero. */
                glMscDataBuffer[4] = (uint8_t)(glLunDatabase[lun].capacity >> 24);
                glMscDataBuffer[5] = (uint8_t)(glLunDatabase[lun].capacity >> 16);
                glMscDataBuffer[6] = (uint8_t)(glLunDatabase[lun].capacity >> 8);
                glMscDataBuffer[7] = (uint8_t)(glLunDatabase[lun].capacity);

                glMscDataBuffer[10] = 0x02;
                glMscDataBuffer[11] = 0x00;

                glMscDataBuffer[13] = 3;        /* Report 8 LBs per physical block. */

                glMscState = CY_FX_MSC_STATE_DATA;
                status = CyFxSendMscDataToHost (glMscDataBuffer, dataLength);
                if (status != CY_U3P_SUCCESS) 
                {
                    glMscCmdStatus  = 1;
                    glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_CRC_ERROR;
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
                }
            }
            break;

        default:
            {
                /* Unsupported command. */
                glMscCmdStatus  = 1;
                glLunDatabase[lun].sensePtr = CY_FX_MSC_SENSE_INVALID_OP_CODE;
                CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SIBCB_EVENT_FLAG, CYU3P_EVENT_OR);
            }
            break;
    }
}

/* Callback to handle the USB Setup Requests and Mass Storage Class requests */
CyBool_t
CyFxAppUsbSetupCallback (
        uint32_t setupdat0, /* SETUP Data 0 */
        uint32_t setupdat1  /* SETUP Data 1 */
        )
{
    CyBool_t isHandled = CyFalse;
    CyU3PReturnStatus_t status;
    uint8_t  bRequest, bReqType;
    uint8_t  bType, bTarget;
    uint8_t  tmp, ep0Buf[2] ={0};    
    uint16_t wValue, wIndex, wLength;
    uint16_t epPktSize = 64;    /* Full speed setting by default. */    

    /* Decode the fields from the setup request. */
    bReqType = (setupdat0 & CY_U3P_USB_REQUEST_TYPE_MASK);
    bType    = (bReqType & CY_U3P_USB_TYPE_MASK);
    bTarget  = (bReqType & CY_U3P_USB_TARGET_MASK);
    bRequest = ((setupdat0 & CY_U3P_USB_REQUEST_MASK) >> CY_U3P_USB_REQUEST_POS);
    wValue   = ((setupdat0 & CY_U3P_USB_VALUE_MASK)   >> CY_U3P_USB_VALUE_POS);
    wIndex   = ((setupdat1 & CY_U3P_USB_INDEX_MASK)   >> CY_U3P_USB_INDEX_POS);
    wLength  = ((setupdat1 & CY_U3P_USB_LENGTH_MASK)  >> CY_U3P_USB_LENGTH_POS);

    glIsDescRqt = CyFalse;
    glRqtLen    = wLength;
   
    /* Application Supported MSC Interface Setup Request Handler */
    if ((bTarget == CY_U3P_USB_TARGET_INTF) && (wIndex == glMscIfaceIndex))
    {
        /* MSC class specific request handling. */
        if (bType == CY_FX_MSC_USB_CLASS_REQ)
        {
            /* Get MAX LUN Request */
            if (bRequest == CY_FX_MSC_GET_MAX_LUN_REQ)
            {
                if (wLength == 1)
                {
                    tmp       = glPibMaxLun - 1;
                    isHandled = CyTrue;
                    status = CyU3PUsbSendEP0Data (0x01, (uint8_t *)&tmp);
                    if (status != CY_U3P_SUCCESS)
                    {
                        CyU3PUsbStall (0, CyTrue, CyFalse);
                    }
                }
            }  
            
            /* BOT Reset Request */
            if (bRequest == CY_FX_MSC_BOT_RESET_REQ)
            {
                isHandled      = CyTrue;
                glInPhaseError = CyFalse;      
                
                if (wLength == 0)
                {
                    CyU3PUsbAckSetup ();
                    CyFxAppResetMscDataPath ();                     
                    
                    /* Inform the thread to start waiting for USB CBWs again. */
                    CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SETCONF_EVENT_FLAG, CYU3P_EVENT_OR);
                }
                else
                {
                    CyU3PUsbStall (0x00, CyTrue, CyFalse);
                }
            }
        }
        
        if (bType == CY_U3P_USB_STANDARD_RQT)
        {
            switch (bRequest)
            {
                case CY_U3P_USB_SC_CLEAR_FEATURE:                
                case CY_U3P_USB_SC_SET_FEATURE :
                    if (wValue == 0)
                    {
                        if (glDevConfigured)
                            CyU3PUsbAckSetup ();
                        else
                            CyU3PUsbStall (0, CyTrue, CyFalse);
                        isHandled = CyTrue;
                    }
                    break;
                                
                case CY_U3P_USB_SC_GET_STATUS:
                    if (wIndex ==  glMscIfaceIndex)
                    {
                        ep0Buf[0] = 0;
                        ep0Buf[1] = 0;
                        CyU3PUsbSendEP0Data (0x02, ep0Buf);
                    }
                    else
                    {
                        CyU3PUsbStall (0, CyTrue, CyFalse);                  
                    }
                    isHandled = CyTrue;
                    break;
                
                case CY_U3P_USB_SC_SET_INTERFACE:
                    if (wValue == 0)
                        CyU3PUsbAckSetup ();
                    else
                        CyU3PUsbStall (0, CyTrue, CyFalse);
                    isHandled = CyTrue;
                    break;
                
                case CY_U3P_USB_SC_GET_INTERFACE:
                    if (wIndex == glMscIfaceIndex)
                    {
                        ep0Buf[0] = 0;
                        CyU3PUsbSendEP0Data (0x01, ep0Buf);
                    }
                    else
                    {
                        CyU3PUsbStall (0, CyTrue, CyFalse);
                    }
                    isHandled = CyTrue;
                    break;
                
                default:
                    break;
            }
            return isHandled;
        }
    }

    /* Application specific MSC Interface EP Setup Request Handler */
    if (bTarget == CY_U3P_USB_TARGET_ENDPT && ((wIndex == CY_FX_MSC_EP_BULK_IN)
                || (wIndex == CY_FX_MSC_EP_BULK_OUT)))
    {
        switch (bRequest)
        {
            case CY_U3P_USB_SC_CLEAR_FEATURE:
                {
                    if (!glInPhaseError)
                    {
                        CyU3PUsbStall (wIndex, CyFalse, CyTrue);
                    }
                    else
                    {
                        /* Even though we cannot clear the STALL condition, the sequence number should be cleared. */
                        if (CyU3PUsbGetSpeed() == CY_U3P_SUPER_SPEED)
                            CyU3PUsbSetEpSeqNum (wIndex, 0);
                    }          
                    CyU3PUsbAckSetup ();
                    isHandled = CyTrue;                
                }
                break;

            case CY_U3P_USB_SC_SYNC_FRAME:
                {
                    isHandled = CyTrue;
                    CyU3PUsbAckSetup ();
                }
                break;                

            case CY_U3P_USB_SC_SET_FEATURE:
                {                
                    if (glDevConfigured)
                        CyU3PUsbAckSetup ();
                    else
                        CyU3PUsbStall (0, CyTrue, CyFalse);

                    isHandled = CyTrue;
                }
                break;

            case CY_U3P_USB_SC_GET_STATUS:
                {
                    /* We support only interface 0. */
                    if (wIndex == 0)
                    {
                        ep0Buf[0] = 0;
                        ep0Buf[1] = 0;
                        CyU3PUsbSendEP0Data (0x02, ep0Buf);
                    }
                    else
                        CyU3PUsbStall (0, CyTrue, CyFalse);

                    isHandled = CyTrue;
                }
                break;

            default:
                break;               
        }
        return isHandled;
    }
   
    /* Device specific setup request handler */
    if (bTarget == CY_U3P_USB_TARGET_DEVICE)            
    {
        switch (bRequest)
        {
            case CY_U3P_USB_SC_GET_DESCRIPTOR:                    
                {
                    glIsDescRqt  = CyTrue;
                    glDescType   = wValue >> 8;
                    isHandled    = CyTrue;
                }
                break;                   
            
            case CY_U3P_USB_SC_SET_CONFIGURATION:
                {
                    if (wValue == 0)
                    {
                        glDevConfigured = CyFalse;
                        break;
                    }
                    
                    /* Check if Set Config event is handled */
                    if (glDevConfigured == CyFalse )
                    {
                        glDevConfigured = CyTrue;
                        
                        /* The max. packet size depends on connection speed. */
                        switch (CyU3PUsbGetSpeed ())
                        {
                            case CY_U3P_FULL_SPEED:
                                epPktSize = 64;
                                break;
                            case CY_U3P_HIGH_SPEED:
                                epPktSize = 512;
                                break;
                            case CY_U3P_SUPER_SPEED:
                            default:
                                epPktSize = 1024;
                                break;
                        }
                        
                        /* Update the packet size for the endpoints based on connection speed. */
                        CyU3PSetEpPacketSize (CY_FX_MSC_EP_BULK_OUT, epPktSize);
                        CyU3PSetEpPacketSize (CY_FX_MSC_EP_BULK_IN, epPktSize);
                        
                        /* Clear stall on the endpoints and reset the DMA channels. */
                        CyU3PUsbStall (CY_FX_MSC_EP_BULK_OUT, CyFalse, CyTrue);
                        CyU3PUsbStall (CY_FX_MSC_EP_BULK_IN, CyFalse, CyTrue);
                        
                        CyU3PDmaChannelReset (&glChRdHandleS2U);
                        CyU3PDmaChannelReset (&glChWrHandleU2S);
                        
                        /* Notify the thread to start waiting for a mass storage CBW. */
                        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SETCONF_EVENT_FLAG, CYU3P_EVENT_OR);
                    }
                }        
                break; 
            
            default:
                break;                    
        }
    }

    /* This request is not needed when using the standard Mass Storage class driver on any OS.
       It is used by the Cypress proprietary Mass Storage Class Driver for device identification.
       */
    if (bType == CY_FX_MSC_USB_VENDOR_REQ)
    {
        if ((bReqType & 0x80) && (bRequest == 0xA0))
        {
            if (wLength < 8)
                CyU3PUsbSendEP0Data ((uint8_t)wLength, A0VendBuf);
            else
                CyU3PUsbSendEP0Data (8, A0VendBuf);
            return CyTrue;
        }
    }
   
    /* Forward AP specific setup requests to AP */
    status = CyFxAppSendSetupRqtEvent (setupdat0, setupdat1);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PUsbStall (0, CyTrue, CyFalse);
    }      

    /* P-Port Enumeration */
    if (wIndex != glMscIfaceIndex)
    {
        isHandled = CyTrue;
    }

    return isHandled;
}

/* Callback function to handle LPM requests from the USB 3.0 host. This function is invoked by the API
   whenever a state change from U0 -> U1 or U0 -> U2 happens. If we return CyTrue from this function, the
   FX3S device is retained in the low power state. If we return CyFalse, the FX3S device immediately tries
   to trigger an exit back to U0.
   */
CyBool_t
CyFxAppUsbLpmCallback (
        CyU3PUsbLinkPowerMode link_mode)
{
    /* If we are in the middle of processing a mass storage command, keep reverting to U0 state. */
    return !glMscDriverBusy;
}

/* Not all events in firmware need to be forwarded to the Apps Processor. This map
 * enables the events that need to be forwarded.
 */
static const uint8_t UsbEventForwardList[] = {
    CyTrue,  /* CY_U3P_USB_EVENT_CONNECT */
    CyTrue,  /* CY_U3P_USB_EVENT_DISCONNECT */
    CyTrue,  /* CY_U3P_USB_EVENT_SUSPEND */
    CyTrue,  /* CY_U3P_USB_EVENT_RESUME */
    CyTrue,  /* CY_U3P_USB_EVENT_RESET */
    CyTrue,  /* CY_U3P_USB_EVENT_SETCONF */
    CyTrue,  /* CY_U3P_USB_EVENT_SPEED */
    CyTrue,  /* CY_U3P_USB_EVENT_SETINTF */
    CyFalse, /* CY_U3P_USB_EVENT_SET_SEL */
    CyFalse, /* CY_U3P_USB_EVENT_SOF_ITP */
    CyFalse, /* CY_U3P_USB_EVENT_EP0_STAT_CPLT */
    CyTrue,  /* CY_U3P_USB_EVENT_VBUS_VALID */
    CyTrue,  /* CY_U3P_USB_EVENT_VBUS_REMOVED */
    CyFalse, /* CY_U3P_USB_EVENT_HOST_CONNECT */
    CyFalse, /* CY_U3P_USB_EVENT_HOST_DISCONNECT */
    CyFalse, /* CY_U3P_USB_EVENT_OTG_CHANGE */
    CyFalse, /* CY_U3P_USB_EVENT_OTG_VBUS_CHG */
    CyFalse, /* CY_U3P_USB_EVENT_OTG_SRP */
    CyFalse, /* CY_U3P_USB_EVENT_EP_UNDERRUN */
    CyFalse, /* CY_U3P_USB_EVENT_LNK_RECOVERY */
    CyFalse, /* CY_U3P_USB_EVENT_USB3_LNKFAIL */
    CyFalse, /* CY_U3P_USB_EVENT_SS_COMP_ENTRY */
    CyFalse  /* CY_U3P_USB_EVENT_SS_COMP_EXIT */
};

/* This is the Callback function to handle the USB Events */
void
CyFxAppUsbEventCallback (
        CyU3PUsbEventType_t evtype, /* Event type */
        uint16_t            evdata  /* Event data */
        )
{
#ifdef DEBUG
    if (evtype != CY_U3P_USB_EVENT_EP0_STAT_CPLT)
        CyFxAppSendDebugMessage (0x1500, evtype, evdata);
#endif

    switch (evtype)
    {
        case CY_U3P_USB_EVENT_CONNECT:
            {
                CyFxAppSendUSBRqtEvent ((uint16_t) evtype, CyU3PUsbGetSpeed());
                CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_RESET_EVENT_FLAG, CYU3P_EVENT_OR);
            }
            break;

        case CY_U3P_USB_EVENT_SETCONF:
            {
                CyFxAppResetMscDataPath ();
                CyFxAppSendUSBRqtEvent ((uint16_t) evtype, evdata);

                /* Notify the thread to start waiting for a mass storage CBW. */
                CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_SETCONF_EVENT_FLAG, CYU3P_EVENT_OR);
            }
            break;

        case CY_U3P_USB_EVENT_SPEED:
            {
                CyFxAppSendUSBRqtEvent ((uint16_t) evtype, CyU3PUsbGetSpeed());
            }
            break;

        default:
            if (UsbEventForwardList[evtype] != CyFalse)
            CyFxAppSendUSBRqtEvent ((uint16_t) evtype, 0);
            break;
    }
}

/* Configure the endpoints required for application supported MSC Interface. */
static CyU3PReturnStatus_t
CyFxAppEndPointControl (
        CyBool_t enable)
{
    CyU3PEpConfig_t     epConf = {0};
    CyU3PReturnStatus_t status;

    epConf.enable   = enable;
    epConf.epType   = CY_U3P_USB_EP_BULK;
    epConf.streams  = 0;
    epConf.pcktSize = 1024;                     /* Configuring for super speed by default. */
    epConf.burstLen = CY_FX_MSC_EP_BURST_SIZE;
    epConf.isoPkts  = 0;

    status = CyU3PSetEpConfig (CY_FX_MSC_EP_BULK_OUT, &epConf);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    status = CyU3PSetEpConfig (CY_FX_MSC_EP_BULK_IN, &epConf);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    return status;
}

/* Query storage device information */
void
CyFxAppQueryDevStatus (
        void)
{
    CyU3PReturnStatus_t status;
    CyU3PSibLunInfo_t   unitInfo;
    uint8_t i, j, k;
    uint8_t unitsFound;

    /* Initialize LUN data structures with default values. */
    for (k = 0; k < CY_FX_MSC_LUN_COUNT; k++)
    {
        glLunDatabase[k].state     = CyFalse;
        glLunDatabase[k].capacity  = 0;
        glLunDatabase[k].writeable = CyTrue;
    }

    for (i = 0; i < CY_FX_SIB_PORTS; i++)
    {
        unitsFound = 0;

        /* Query each of the storage ports to identify whether a device has been detected. */
        status = CyU3PSibQueryDevice (i, &glDevInfo[i]);
        if (status == CY_U3P_SUCCESS)
        {
            AppDebugPrint (8, "Found a device on port %d\r\n", i);
            AppDebugPrint (6, "\tType=%d, numBlks=%d, eraseSize=%d, clkRate=%d\r\n",
                    glDevInfo[i].cardType, glDevInfo[i].numBlks, glDevInfo[i].eraseSize, glDevInfo[i].clkRate);
            AppDebugPrint (6, "\tblkLen=%d removable=%d, writeable=%d, locked=%d\r\n",
                    glDevInfo[i].blkLen, glDevInfo[i].removable, glDevInfo[i].writeable, glDevInfo[i].locked);
            AppDebugPrint (6, "\tddrMode=%d, opVoltage=%d, busWidth=%d, numUnits=%d\r\n",
                    glDevInfo[i].ddrMode, glDevInfo[i].opVoltage, glDevInfo[i].busWidth, glDevInfo[i].numUnits);
            AppDebugPrint (6, "\tcardCmdClass=%d\r\n", glDevInfo[i].ccc);

            for (j = 0, unitsFound = 0; ((j < glDevInfo[i].numUnits) && (unitsFound < CY_FX_SIB_PARTITIONS)); j++)
            {
                status = CyU3PSibQueryUnit (i, j, &unitInfo);
                if (status == CY_U3P_SUCCESS)
                {
                    if (unitInfo.location == CY_U3P_SIB_LOCATION_USER)
                    {
                        AppDebugPrint (6, "Dev %d, Unit %d: blkSize=%d numBlocks=%d\r\n", i, j,
                                unitInfo.blockSize, unitInfo.numBlocks);

                        /* Update the state of LUNs that are mapped to the partition that is found. */
                        for (k = 0; k < CY_FX_MSC_LUN_COUNT; k++)
                        {
                            if ((glLunDatabase[k].port == i) && (glLunDatabase[k].unit == j))
                            {
                                glLunDatabase[k].state     = CyTrue;
                                glLunDatabase[k].capacity  = unitInfo.numBlocks - 1;
                                glLunDatabase[k].writeable = (glDevInfo[i].writeable && ((glMscWritable[i] & (1 << j)) != 0)) ?
                                    CyTrue : CyFalse;
                            }
                        }

                        unitsFound++;
                    }
                    else
                    {
                        AppDebugPrint (6, "Port %d: Skipping boot partition %d\r\n", i, j);
                    }
                }
                else
                {
                    AppDebugPrint (2, "Error: Failed to query partition %d on port %d\r\n", j, i);
                    break;
                }
            }

            /* Commit data in 8MB chunks. */
            CyU3PSibSetWriteCommitSize (i, CY_FX_SIB_WR_CMT_SIZE);
        }
        else
        {
            AppDebugPrint (2, "StorageQueryDev (%d) failed with code %d\r\n", i, status);
        }
    }
}



/* AP USB connect/disconnect request handler */
static uint16_t
CyFxAppHandleConnectRqt (
        CyBool_t connect)
{
    CyU3PReturnStatus_t    apiStat;
    uint16_t               status = CY_WB_ERROR_SUCCESS;

    if (connect)
    {
        if (glPIBUsbConnected)
            return CY_WB_ERROR_USB_CONNECTED;

        /* Configure MSC Interface Endpoints */
        apiStat = CyFxAppEndPointControl (CyTrue);
        if (apiStat != CY_U3P_SUCCESS)
        {
            return (glWbErrMap[apiStat]);
        }

        /* Set the USB callbacks as required. */
        CyU3PUsbRegisterSetupCallback (CyFxAppUsbSetupCallback, CyFalse);
        CyU3PUsbRegisterEventCallback (CyFxAppUsbEventCallback);
        CyU3PUsbRegisterLPMRequestCallback (CyFxAppUsbLpmCallback);

        /* Query LUN Information */
        CyFxAppQueryDevStatus ();

        /* Connect to USB Host */
        apiStat = CyU3PConnectState (CyTrue, glPIBSSEnable);
        if (apiStat == CY_U3P_SUCCESS)
            glPIBUsbConnected = CyTrue;
        else
        {
            status = glWbErrMap[apiStat];
        }
    }
    else
    {
        if (!glPIBUsbConnected)
            return CY_WB_ERROR_USB_NOT_CONNECTED;

        /* Disconnect from USB Host */
        apiStat = CyU3PConnectState (CyFalse, glPIBSSEnable);
        if (apiStat == CY_U3P_SUCCESS)
        {
            glPIBUsbConnected = CyFalse;

            apiStat = CyFxAppEndPointControl (CyFalse);
            if (apiStat != CY_U3P_SUCCESS)
                status = glWbErrMap[apiStat];
        }
        else
            status = glWbErrMap[apiStat];
    }
    
    /* Reset Data Path */
    CyFxAppResetMscDataPath ();
    glDevConfigured = CyFalse;
    glInPhaseError  = CyFalse;
    
    return status;
}


/* USB Connection Start Request Handler */
static uint16_t
CyFxAppHandleUsbStart (
        uint8_t sockOut,
        uint8_t sockIn,
        uint8_t usbParam
        )
{
    CyU3PDmaChannelConfig_t dmaConfig;
    CyU3PDmaSocketId_t InsocketId, OutSocketId;    
    uint16_t status = CY_WB_ERROR_SUCCESS;

    /* PIB Socket fetcher for descriptor transfer */
    InsocketId  = (CyU3PDmaSocketId_t)CyU3PDmaGetSckId (CY_U3P_PIB_IP_BLOCK_ID, sockIn);
    OutSocketId = (CyU3PDmaSocketId_t)CyU3PDmaGetSckId (CY_U3P_PIB_IP_BLOCK_ID, sockOut);    

    /* Create a channel for receiving USB descriptors. */
    dmaConfig.size           = 512;
    dmaConfig.count          = 1;
    dmaConfig.dmaMode        = CY_U3P_DMA_MODE_BYTE;
    dmaConfig.prodHeader     = 0;
    dmaConfig.prodFooter     = 0;
    dmaConfig.consHeader     = 0;
    dmaConfig.prodAvailCount = 0;
    dmaConfig.prodSckId      = InsocketId;
    dmaConfig.consSckId      = CY_U3P_CPU_SOCKET_CONS;
    dmaConfig.notification   = 0;
    dmaConfig.cb             = 0;

    status = CyU3PDmaChannelCreate (&glChRdHandleP2EP0, CY_U3P_DMA_TYPE_MANUAL_IN, &dmaConfig);
    if (status != CY_U3P_SUCCESS)
    {
        return CY_WB_ERROR_OUT_OF_MEMORY;
    }   

    dmaConfig.prodSckId      = CY_U3P_CPU_SOCKET_PROD;
    dmaConfig.consSckId      = OutSocketId;
    status = CyU3PDmaChannelCreate (&glChWrHandleEP02P, CY_U3P_DMA_TYPE_MANUAL_OUT, &dmaConfig);
    if (status != CY_U3P_SUCCESS)
    {
        return CY_WB_ERROR_OUT_OF_MEMORY;
    }  
   
    /* USB Param Info */
    CyU3PUsbVBattEnable ((usbParam & 0x02) ? (CyTrue) : (CyFalse));
    glPIBSSEnable = (usbParam & 0x01) ? (CyTrue) : (CyFalse);
    glIsMscEnbl   = (usbParam & 0x04) ? (CyTrue) : (CyFalse);

    return status;
}


/* USB Connection Stop Handler */
static void
CyFxAppHandleUsbStop (
        void)
{
    uint8_t i = 0;

    /* Destroy EP0 to P-Port Channels */
    CyU3PDmaChannelDestroy (&glChRdHandleP2EP0);
    CyU3PDmaChannelDestroy (&glChWrHandleEP02P); 
    
    /* ToDo: Destroy EP to P-Port Channels */
    for (i = 0; i <= glP2UChHandleOffset; i++)
    {
        CyU3PDmaChannelDestroy (&glChHandleP2U[i]);
    }

    for (i = 0; i <= glU2PChHandleOffset; i++)
    {
        CyU3PDmaChannelDestroy (&glChHandleU2P[i]);
    }

    glP2UChHandleOffset = 0;
    glU2PChHandleOffset = 0;

    glPIBSSEnable = CyFalse;
}


/* LUN Configuration Handler */
void
CyFxAppHandleLunConfig (
        uint16_t usb_visible,
        uint16_t usb_writeable,
        uint8_t  mscIf
        )
{
    uint8_t i = 0, j = 0;

    glPibMaxLun = 0;
    CyFxMscSetLunDefaultParams ();

    glMscWritable[CY_FX_MSC_PORT0]   = CY_U3P_GET_LSB (usb_writeable);
    glMscWritable[CY_FX_MSC_PORT1]   = CY_U3P_GET_MSB (usb_writeable);
    glMscPartitions[CY_FX_MSC_PORT0] = CY_U3P_GET_LSB (usb_visible);
    glMscPartitions[CY_FX_MSC_PORT1] = CY_U3P_GET_MSB (usb_visible);  

    /* If any partition is enabled for MASS STORAGE ACCESS, map an LUN to it. */
    for (i = 0; i < CY_FX_SIB_PORTS; i++)
    {
        for (j = 0; j < CY_FX_SIB_PARTITIONS; j++)
        {
            if ((glMscPartitions[i] & (1 << j)) != 0)
            {
                glLunDatabase[glPibMaxLun].port = i;
                glLunDatabase[glPibMaxLun].unit = j;

                glPibMaxLun++;            
            }
        }
    }

    /* Minimum LUN == 1 */
    if (glPibMaxLun == 0)
        glPibMaxLun = 1;

    /* Update the LUN states based on the SD card presence. */
    CyFxAppQueryDevStatus ();
    CyFxMscUpdateLunOwners ();
}


/* Append MSC Interface specific descriptor to Configuration Descriptor */
uint16_t
CyFxAppAddMSCDesc (
        uint8_t *desc_p,
        uint16_t len
        )
{
    uint16_t size = 0;
    uint8_t *tgt_addr, *src_addr;
    uint16_t mscIfaceDescLen = 0; 
   
    /* USB Speed Tracker */
    switch (CyU3PUsbGetSpeed ())
    {
        case CY_U3P_SUPER_SPEED:
        mscIfaceDescLen = CY_FX_MSC_SS_INTFDESC_LENGTH;            
        src_addr = CyFxUSBSSBotIfaceDscr;           
            break;
        case CY_U3P_HIGH_SPEED:
        mscIfaceDescLen = CY_FX_MSC_HS_INTFDESC_LENGTH;
        src_addr = CyFxUSBHSBotIfaceDscr;
            break;
        default:
        mscIfaceDescLen = CY_FX_MSC_FS_INTFDESC_LENGTH;
        src_addr = CyFxUSBFSBotIfaceDscr;
            break;
    }

    /* Update the total size to include the MSC interface. */
    tgt_addr = desc_p;
    size = CY_U3P_MAKEWORD(tgt_addr[CY_FX_MSC_CONFDESC_LENGTH_OFFSET + 1],
            tgt_addr[CY_FX_MSC_CONFDESC_LENGTH_OFFSET]);
    size += mscIfaceDescLen;
    tgt_addr[CY_FX_MSC_CONFDESC_LENGTH_OFFSET] = CY_U3P_GET_LSB(size);
    tgt_addr[CY_FX_MSC_CONFDESC_LENGTH_OFFSET + 1] = CY_U3P_GET_MSB(size);

    if (len >= 5)
    {
        /* Update the MSC interface number to be the last one. */
        src_addr[2] = tgt_addr[4];
        glMscIfaceIndex = tgt_addr[4];
        /* Update the number of interfaces in the configuration. */
        tgt_addr[4] = tgt_addr[4] + 1;
    }

    /* Copy data to the end of the descriptor received from AP. */
    tgt_addr = (desc_p + len);
    CyU3PMemCopy (tgt_addr, src_addr, mscIfaceDescLen);  
    tgt_addr = desc_p;

    return (CY_U3P_MIN(size, glRqtLen));
}


/* USB Host Data Xfer Length Computer */
uint16_t 
CyFxAppHostDescLenComputer (
        uint16_t size
        )
{
    uint16_t count = 0;

    switch (glDescType)
    {
        case CY_U3P_USB_DEVICE_DESCR:
            count = CY_FX_MSC_DSCR_DEVICE_LEN;
            break;

        case CY_U3P_USB_CONFIG_DESCR:
            count = CY_U3P_MIN (glRqtLen, size);
            break;

        case CY_U3P_USB_STRING_DESCR:
            count = CY_U3P_MIN (glRqtLen, size);
            break;

        case CY_U3P_USB_DEVQUAL_DESCR:
            count = CY_FX_MSC_DSCR_DEVQUAL_LEN;
            break;

        case CY_U3P_BOS_DESCR:
            count = CY_U3P_MIN (glRqtLen, size);
            break;  

        default:
            count = size;
            break;
    }

    return count;
}


/* EP0 Data Xfer Request Handler */
uint16_t
CyFxAppHandleSetEP0Xfer (
        uint8_t isRead,
        uint16_t length)
{

    CyBool_t sendZLP = CyFalse;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;
    uint16_t size = length, max = 0;
    uint16_t offset = 0;
    CyU3PDmaBuffer_t dmaBuf;

    max = (CyU3PUsbGetSpeed() == CY_U3P_SUPER_SPEED)  ? (512) : (64);

    if (isRead)
    {
        CyU3PMemSet (glMscDataBuffer, 0, length);

        /* Read all the data into a single buffer, getting upto 512 bytes in each call. */
        while (length != 0)
        {
        /* P to CPU ==> CPU to EP0 */       
        /* EP0 Data Xfer Length Limit = 2KB */
            dmaBuf.buffer = glMscDataBuffer + offset;
            dmaBuf.count  = (length > 512) ? 512 : length;
            dmaBuf.size   = (length > 512) ? 512 : (length + 15) & 0xFFF0;
        dmaBuf.status = 0;
        
            offset += 512;
            length -= dmaBuf.count;

        status = CyU3PDmaChannelSetupRecvBuffer (&glChRdHandleP2EP0, &dmaBuf);
        if (status == CY_U3P_SUCCESS)
        {
                status = CyU3PDmaChannelWaitForRecvBuffer (&glChRdHandleP2EP0, &dmaBuf, 10000);
        }
        
        if (status != CY_U3P_SUCCESS)
        {
            CyU3PDmaChannelReset (&glChRdHandleP2EP0);
            CyU3PUsbStall(0, CyTrue, CyFalse);
            return (glWbErrMap[status]);
        }        
        }

        /* Restore the length variable. */
        length = size;

        /* Append Configuration Descriptor */
        if (glIsDescRqt)
        {
            /* Add MSC Descriptors */
            if ((glIsMscEnbl == CyTrue) && (glDescType == CY_U3P_USB_CONFIG_DESCR))
            {
                size = CyFxAppAddMSCDesc (glMscDataBuffer, length);
            }

            /* Load default values */
            glIsDescRqt = CyFalse;                              
            size = CyFxAppHostDescLenComputer (size);
        }

            size = CY_U3P_MIN (glRqtLen, size);
            /* If the data sent back is a multiple of max packet size but is less
             * than the requested length, then an explicit ZLP needs to be sent
             * to the USB host. */
             if ((size != 0) && (size < glRqtLen) && ((size & (max - 1)) == 0))
                sendZLP = CyTrue;

             /* Forward Data to EP0 */ 
             /* Multi Packet Handling */
        status = CyU3PUsbSendEP0Data (size, glMscDataBuffer);

        /* If we send less data than the host asked for, see if we need to send a ZLP. */
        if ((status == CY_U3P_SUCCESS) && (sendZLP))
        {           
            status = CyU3PUsbSendEP0Data (0, glMscDataBuffer);
        }

        if (status != CY_U3P_SUCCESS)
        {
            /* There was some error. We should try stalling EP0. */
            CyU3PUsbStall(0, CyTrue, CyFalse);
        }
    }
    else
    {
        /* EP0 to CPU ==> CPU to P */       
        /* EP0 Data Xfer Length Limit = 2KB */
        uint16_t readCnt = 0;

        CyU3PMemSet (glMscDataBuffer, 0, length);

        status = CyU3PUsbGetEP0Data (length, glMscDataBuffer, &readCnt);
        if ((status == CY_U3P_SUCCESS) && (length == readCnt))
        {                                  
            dmaBuf.buffer = glMscDataBuffer;
            dmaBuf.count  = length;
            dmaBuf.size   = (length + 15) & 0xFFF0;
            dmaBuf.status = 0;
            
            status = CyU3PDmaChannelSetupSendBuffer (&glChWrHandleEP02P, &dmaBuf);
            if (status == CY_U3P_SUCCESS)
            {
                status = CyU3PDmaChannelWaitForCompletion (&glChWrHandleEP02P, 10000);
            }
            
            if (status != CY_U3P_SUCCESS)
            {
                CyU3PDmaChannelReset (&glChWrHandleEP02P);
                CyU3PUsbStall(0, CyTrue, CyFalse);
                return (glWbErrMap[status]);
            }              
        }                   
    }

    return glWbErrMap[status];
}

/* DMA channels creator for the U-P data transfers */
/* ToDo: Dynamic DMA Handle fetching */
static uint16_t
CyFxAppDmaConfig (
        uint8_t ep,
        uint8_t socknum,
        uint8_t burstlen,
        uint16_t *pktsize_p,
        CyBool_t isHighBw)
{
    CyU3PDmaChannelConfig_t dmaConfig;
    uint8_t epnum = ep & 0x7F;
    CyU3PReturnStatus_t status;
    uint16_t size = *pktsize_p;

    if (isHighBw)
    {
        dmaConfig.size  = size * burstlen;
        dmaConfig.count = 3;    /* Use triple buffering for now. */
    }
    else
    {
        /* The burst length determines the amount of buffering. */
        dmaConfig.size  = size;
        dmaConfig.count = (burstlen > 2) ? burstlen : 2;
    }

    dmaConfig.prodAvailCount = 0;
    dmaConfig.dmaMode = CY_U3P_DMA_MODE_BYTE;
    dmaConfig.prodHeader = 0;
    dmaConfig.prodFooter = 0;
    dmaConfig.consHeader = 0;
    dmaConfig.cb = NULL;

    /* BULK IN P->U Read Channel */
    if (ep & 0x80)
    {
        /* Max P->U Channels == 15 */
        if (glP2UChHandleOffset > 14)
        {
            return CY_WB_ERROR_NOT_SUPPORTED;
        }

        dmaConfig.prodSckId = (CyU3PDmaSocketId_t)CyU3PDmaGetSckId (CY_U3P_PIB_IP_BLOCK_ID, socknum);
        dmaConfig.consSckId = (CyU3PDmaSocketId_t)CyU3PDmaGetSckId (CY_U3P_UIB_IP_BLOCK_ID, epnum);

        status = CyU3PDmaChannelCreate (&glChHandleP2U[glP2UChHandleOffset], CY_U3P_DMA_TYPE_AUTO, &dmaConfig);
        if(status != CY_U3P_SUCCESS)
        {
            return glWbErrMap[status];
        }
        CyU3PDmaChannelSetXfer (&glChHandleP2U[glP2UChHandleOffset], 0);
        glP2UChHandleOffset++;
    }
    else
    {
        /* Max U->P Channels == 15 */
        if (glU2PChHandleOffset > 14)
        {
            return CY_WB_ERROR_NOT_SUPPORTED;
        }
        
        /* BULK OUT U->P Write Channel */
        dmaConfig.prodSckId   = (CyU3PDmaSocketId_t)CyU3PDmaGetSckId (CY_U3P_UIBIN_IP_BLOCK_ID, epnum);
        dmaConfig.consSckId   = (CyU3PDmaSocketId_t)CyU3PDmaGetSckId (CY_U3P_PIB_IP_BLOCK_ID, socknum);

        status = CyU3PDmaChannelCreate (&glChHandleU2P[glU2PChHandleOffset], CY_U3P_DMA_TYPE_AUTO, &dmaConfig);
        if(status != CY_U3P_SUCCESS)
        {
            return glWbErrMap[status];
        }

        CyU3PDmaChannelSetXfer (&glChHandleU2P[glU2PChHandleOffset], 0);
        glU2PChHandleOffset++;
    }

    *pktsize_p = dmaConfig.size;
    return glWbErrMap[status];
}

/* AP initiated USB request Handler */
static void
CyFxAppHandleUsbRqt (
        void)
{
    CyBool_t  stall = CyFalse;
    uint8_t  rqt, tag, ep = 0;
    uint16_t status = CY_WB_ERROR_SUCCESS;
    CyU3PReturnStatus_t apiStat = CY_U3P_SUCCESS;

    /* MBOX Request & Tag ID fetcher */
    rqt = (uint8_t)(glPibRqtData[CY_RQT_USB_RQT_CONTEXT][0] & 0xFF);
    tag = (uint8_t)((glPibRqtData[CY_RQT_USB_RQT_CONTEXT][0] & 0x3800) >> 11);

    switch (rqt)
    {
        case CY_RQT_START_USB:
            {
                uint8_t sockOut, sockIn, usbParam;

                sockOut  = CY_U3P_GET_MSB(glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1]);
                sockIn   = CY_U3P_GET_LSB(glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1]);
      
                usbParam = CY_U3P_GET_LSB(glPibRqtData[CY_RQT_USB_RQT_CONTEXT][2]);

                status = CyFxAppHandleUsbStart (sockOut, sockIn, usbParam);

                CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, status);
            }
            break;

        case CY_RQT_STOP_USB:
            {
                glPIBSSEnable = CyFalse;
                CyFxAppHandleUsbStop ();
                CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, CY_WB_ERROR_SUCCESS);
            }
            break;

        case CY_RQT_GET_CONNECT_STATE:
            {
                apiStat = CyU3PGetConnectState ();
                if (apiStat == CY_U3P_SUCCESS)
                {
                    CyFxAppSendConnectStatus (tag, glWbErrMap[apiStat]);
                }
                else
                {
                    CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, glWbErrMap[apiStat]);            
                }
            }
            break;

        case CY_RQT_SET_CONNECT_STATE:
            {
                status = CyFxAppHandleConnectRqt (glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1]);
                CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, status);
            }
            break;
            
        case CY_RQT_SET_USB_CONFIG:
            {
                CyFxAppHandleLunConfig (glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1], glPibRqtData[CY_RQT_USB_RQT_CONTEXT][2],
                        CY_U3P_GET_LSB(glPibRqtData[CY_RQT_USB_RQT_CONTEXT][3]));
                CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, CY_WB_ERROR_SUCCESS);
            }
            break;

        case CY_RQT_STALL_ENDPOINT:
            {
                CyBool_t toggle = CyFalse;

                ep     = glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1];
                stall  = (glPibRqtData[CY_RQT_USB_RQT_CONTEXT][2] & 0x01) ? (CyTrue) : (CyFalse);
                toggle = (glPibRqtData[CY_RQT_USB_RQT_CONTEXT][2] & 0x02) ? (CyTrue) : (CyFalse);                
                apiStat = CyU3PUsbStall (ep, stall, toggle);
                CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, glWbErrMap[apiStat]);
            }
            break;               
    
        case CY_RQT_GET_STALL:
            {
                ep = glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1];
                apiStat = CyU3PUsbGetEpCfg (ep, NULL, &stall);
                if (apiStat == CY_U3P_SUCCESS)
                {
                    CyFxAppSendEPStatus (tag, CyFalse, stall);
                }
                else
                {
                    CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, glWbErrMap[apiStat]);                
                }
            }
            break;

        case CY_RQT_USB_EP0_XFER:
            {
                uint8_t isRead;
                uint16_t length;

                isRead   =  glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1] & 0x01;
                length   =  glPibRqtData[CY_RQT_USB_RQT_CONTEXT][2];

                CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, status);                                                 
                status = CyFxAppHandleSetEP0Xfer (isRead, length);
            }
            break;           

        case CY_RQT_SET_ENDPOINT_CONFIG:
            {
                CyBool_t epBwType = CyFalse;
                uint8_t sock = 0;
                uint16_t size = 0;
                CyU3PEpConfig_t     epConf = {0};

                ep     = (uint8_t) CY_U3P_GET_MSB (glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1]);
                sock   = (uint8_t) CY_U3P_GET_LSB (glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1] & 0x7F);
                
                epConf.enable   = (CyBool_t)(CY_U3P_GET_LSB (glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1])) >> 0x07;
                epConf.epType   = (CyU3PUsbEpType_t) (glPibRqtData[CY_RQT_USB_RQT_CONTEXT][2] >> 0x0E);
                epConf.pcktSize = (uint16_t)glPibRqtData[CY_RQT_USB_RQT_CONTEXT][2] & 0x3FFF;
                epConf.burstLen = (uint8_t)CY_U3P_GET_LSB (glPibRqtData[CY_RQT_USB_RQT_CONTEXT][3]);
                epConf.streams  = 0;
                epConf.isoPkts  = glPibRqtData[CY_RQT_USB_RQT_CONTEXT][3] >> 0x08;

                epBwType   = glPibRqtData[CY_RQT_USB_RQT_CONTEXT][3] >> 0x0F;

                /* MSC Interface EP not allowed to used for E<->U Xfer */
                if (ep == CY_FX_MSC_EP_BULK_IN || ep == CY_FX_MSC_EP_BULK_OUT)
                {
                    CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, CY_WB_ERROR_INVALID_PARAMETER); 
                }

                /* Auto Channel for P <-> U transfer*/
                /* Set Xfer for Infinite data */
                if (epConf.enable)
                {
                    uint16_t *size_p = &size;
                    size = epConf.pcktSize;

                    status = CyFxAppDmaConfig (ep, sock, epConf.burstLen, size_p, epBwType);
                }
                else
                {
                    /* USB Stop takes care of channel destroy */                    
                    status = CY_WB_ERROR_SUCCESS;
                }
                
                if (status == CY_U3P_SUCCESS)
                {
                    CyU3PSetEpConfig (ep, &epConf);   
                    CyFxAppSendEPConfig (tag, size, 2, (ep & 0x7F));
                }
                else
                {
                    CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, status);
                }
            }
            break;               

        case CY_RQT_ENDPOINT_SET_NAK:
            {
                ep = glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1];

                apiStat = CyU3PUsbSetEpNak (ep, (uint8_t)(glPibRqtData[CY_RQT_USB_RQT_CONTEXT][2]));
                
                CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, glWbErrMap[apiStat]);  
            }
            break;               

        case CY_RQT_GET_ENDPOINT_NAK:
            {
                CyBool_t nak;
                ep = glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1];

                status = CyU3PUsbGetEpCfg (ep, &nak, NULL);
                if (apiStat == CY_U3P_SUCCESS)
                {
                    CyFxAppSendEPStatus (tag, CyTrue, nak);
                }
                else
                {
                    CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, glWbErrMap[apiStat]);
                }
            }           
            break;               

        case CY_RQT_ACK_SETUP_PACKET:
            {
                CyU3PUsbAckSetup ();
                CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, CY_WB_ERROR_SUCCESS);
            }
            break;               

        case CY_RQT_USB_REMOTE_WAKEUP:
            {
                apiStat = CyU3PUsbDoRemoteWakeup ();
                CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, glWbErrMap[apiStat]);
            }
            break;               

        case CY_RQT_CANCEL_ASYNC_TRANSFER:
            {
                CyU3PDmaChannel *ch;
                ep = glPibRqtData[CY_RQT_USB_RQT_CONTEXT][1];
                
                if (ep & 0x80)
                {
                    /* Egress socket. */
                    ch = CyU3PDmaChannelGetHandle ((CyU3PDmaSocketId_t) \
                            (CY_U3P_UIB_SOCKET_CONS_0 + (ep & 0x0F)));
                }
                else
                {
                    /* Ingress socket. */
                    ch = CyU3PDmaChannelGetHandle ((CyU3PDmaSocketId_t) \
                            (CY_U3P_UIB_SOCKET_PROD_0 + (ep & 0x0F)));
                }

                if (ch != NULL)
                {
                    status = CyU3PDmaChannelReset (ch);
                    if ((ep & 0x0F) == 0)
                    {
                        CyU3PUsbStall(0, CyTrue, CyFalse);
                    }
                    else
                    {
                        /* These are U2P channels. Setup the channel for
                         * infinite transfer. */
                        CyU3PDmaChannelSetXfer (ch, 0);
                    }
                }
                else
                {
                    status = CY_U3P_ERROR_FAILURE;
                }
                
                CyFxAppSendStatusResponse (CY_RQT_USB_RQT_CONTEXT, tag, status);
            }
            break;    

        default:
            break;
    }
}

/* CSW Transmit Function */
void
CyFxAppSendScsiCsw (
        void)
{
    CyU3PDmaBuffer_t dmaBuf;
    CyU3PReturnStatus_t status;

    glCurrentCmd = 0xFF;

    /* Compute the CSW information. */
    glMscCswBuffer[0] = 'U';
    glMscCswBuffer[1] = 'S';
    glMscCswBuffer[2] = 'B';
    glMscCswBuffer[3] = 'S';
    CyU3PMemCopy (glMscCswBuffer + 4, glMscCbwBuffer + 4, 4);
    *((uint32_t *)(glMscCswBuffer + 8)) = glMscResidue;
    glMscCswBuffer[12] = glMscCmdStatus;

    dmaBuf.buffer = glMscCswBuffer;
    dmaBuf.status = 0;
    dmaBuf.size   = 32;
    dmaBuf.count  = CY_FX_MSC_CSW_MAX_COUNT;
    status = CyU3PDmaChannelSetupSendBuffer (&glChRdHandleS2U, &dmaBuf);
    if (status == CY_U3P_SUCCESS)
        glMscState = CY_FX_MSC_STATE_CSW;
    else
    {
        CyU3PDmaChannelReset (&glChRdHandleS2U);
        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_DATASENT_EVENT_FLAG, CYU3P_EVENT_OR);
    }
}


/*
 * Entry function for the mscAppThread
 */
void
PibAppThread_Entry (
        uint32_t input)
{
    CyU3PReturnStatus_t status;
    uint16_t wakeReason;
    uint32_t evMask = CY_FX_MSC_SETCONF_EVENT_FLAG | CY_FX_MSC_RESET_EVENT_FLAG | CY_FX_MSC_CBW_EVENT_FLAG |
        CY_FX_MSC_DATASENT_EVENT_FLAG | CY_FX_MSC_SIBCB_EVENT_FLAG | CY_FX_MSC_BMC_INIT_FLAG |
        CY_FX_MSC_GEN_CTX_FLAG | CY_FX_MSC_USB_CTX_FLAG | CY_FX_MSC_STR_CTX_FLAG | CY_FX_MSC_USB_SUSP_EVENT_FLAG |
        CY_FX_MSC_DATA_RECV_EVENT_FLAG;
    uint32_t         evStat;

    CyU3PDmaBuffer_t dmaBuf;

    /* Initialize the Debug Module */
    CyFxMscApplnDebugInit ();

    /* Initialize the MSC Application */
    CyFxPibApplnInit ();

    for (;;)
    {
        status = CyU3PEventGet (&glPibAppEvent, evMask, CYU3P_EVENT_OR_CLEAR, &evStat, CYU3P_WAIT_FOREVER);
        if (status == CY_U3P_SUCCESS)
        {
            /* PMMC Init Cmplt Evt */
            if (evStat & CY_FX_MSC_BMC_INIT_FLAG)
            {               
                CyFxAppSendFwInfo (CyTrue, 0);
            }

            /* General AP MBOX Request Handler */
            if (evStat & CY_FX_MSC_GEN_CTX_FLAG)
            {
                CyFxAppHandleGeneralRqt ();
            }

            /* Do not handle any commands from BMC while there is a pending SCSI command. */
            if ((glMscState != CY_FX_MSC_STATE_DATA)  && (glMscState != CY_FX_MSC_STATE_STATUS))
            {
                if (evStat & CY_FX_MSC_STR_CTX_FLAG)
                {
                    CyFxAppHandleStorageRqt ();
                }

                if (evStat & CY_FX_MSC_USB_CTX_FLAG)
                {
                    CyFxAppHandleUsbRqt ();
                }
            }
            else
            {
                CyU3PEventSet (&glPibAppEvent, (evStat & (CY_FX_MSC_STR_CTX_FLAG | CY_FX_MSC_USB_CTX_FLAG)),
                        CYU3P_EVENT_OR);
            }

            if (evStat & CY_FX_MSC_RESET_EVENT_FLAG)
            {
                if (glMscState == CY_FX_MSC_STATE_DATA)
                {
                    /* Make sure that any ongoing operations are aborted. */
                    CyU3PSibAbortRequest (0);
                    CyU3PSibAbortRequest (1);
                }

                glMscState = CY_FX_MSC_STATE_INACTIVE;
            }

            if (evStat & CY_FX_MSC_SETCONF_EVENT_FLAG)
            {
                glMscState = CY_FX_MSC_STATE_CBW;
            }

            if (evStat & CY_FX_MSC_USB_SUSP_EVENT_FLAG)
            {
                /* Place FX3S in Low Power Suspend mode, with USB bus activity as the wakeup source. */
                status = CyU3PSysEnterSuspendMode (CY_U3P_SYS_USB_BUS_ACTVTY_WAKEUP_SRC, 0, &wakeReason);
            }

            if (evStat & CY_FX_MSC_CBW_EVENT_FLAG)
            {
                if (glInPhaseError)
                {
                    /* Phase error: Send an error CSW and then stall both endpoints. */
                    glMscCmdStatus = 2;
                    glMscResidue   = 0;
                    
                    CyFxAppSendScsiCsw ();

                    CyU3PUsbStall (CY_FX_MSC_EP_BULK_IN,  CyTrue, CyFalse);
                    CyU3PUsbStall (CY_FX_MSC_EP_BULK_OUT, CyTrue, CyFalse);
                }
                else
                {             
                    glMscDriverBusy = CyTrue;

                    CyU3PUsbLPMDisable ();
                    if (CyU3PUsbGetSpeed() == CY_U3P_SUPER_SPEED)
                    {
                        /* Keep U1/U2 entry disabled whenever we are processing a command. */
                        CyU3PUsbSetLinkPowerState (CyU3PUsbLPM_U0);
                    }
                    else
                    {
                        CyU3PUsb2Resume ();                    
                    }

                    /* Parse CBW here. */
                    glMscState = CY_FX_MSC_STATE_DATA;
                    CyFxAppParseScsiCommand ();
                }
            }

            if (evStat & CY_FX_MSC_DATA_RECV_EVENT_FLAG)
            {
                /* Data received for Mode Select command. */
                CyFxHandleModeSelectData ();
            }           

            if (evStat & CY_FX_MSC_DATASENT_EVENT_FLAG)
            {
                /* If CSW has been sent, go to CBW state. */
                if (glMscState == CY_FX_MSC_STATE_CSW)
                    glMscState = CY_FX_MSC_STATE_CBW;

                /* If data has been sent, go to CSW state. */
                if (glMscState == CY_FX_MSC_STATE_DATA)
                {
                    /* If we sent less data than the host wanted, stall the endpoint afterwards. */
                    if ((glMscResidue != 0) && (glMscCmdStatus == 0))
                        CyU3PUsbStall (CY_FX_MSC_EP_BULK_IN, CyTrue, CyFalse);

                    glMscState = CY_FX_MSC_STATE_STATUS;
                }
            }

            if (evStat & CY_FX_MSC_SIBCB_EVENT_FLAG)
            {
                /* Stall the endpoint if any data transfer is expected. */
                if ((glMscCmdStatus) && (glMscResidue))
                {
                    /* The DMA channel has already been reset. */
                    if (glCmdDirection)
                        CyU3PUsbStall (CY_FX_MSC_EP_BULK_IN, CyTrue, CyFalse);
                    else
                        CyU3PUsbStall (CY_FX_MSC_EP_BULK_OUT, CyTrue, CyFalse);
                }
                else
                {
                    /* Wait for all of the data to be transferred to the host in the case of a read. */
                    if (glCmdDirection)
                        CyU3PDmaChannelWaitForCompletion (&glChRdHandleS2U, CYU3P_WAIT_FOREVER);
                }

                /* Now move to the STATUS state to try and send the CSW. */
                glMscState = CY_FX_MSC_STATE_STATUS;
            }

            if (glMscState == CY_FX_MSC_STATE_CBW)
            {
                /* Re-enable Link power management now that the command has been handled. */
                glMscDriverBusy = CyFalse;
                CyU3PUsbLPMEnable ();

                CyU3PMemSet (glMscCbwBuffer, 0xFF, 32);
                dmaBuf.buffer = glMscCbwBuffer;
                dmaBuf.status = 0;
                dmaBuf.count  = 0;
                dmaBuf.size   = 64;     /* CBW is expected to be smaller than 64 bytes. */

                status = CyU3PDmaChannelSetupRecvBuffer (&glChWrHandleU2S, &dmaBuf);
                if (status == CY_U3P_SUCCESS)
                {
                    glMscState = CY_FX_MSC_STATE_WAITING;

                    /* Make sure that we bring the link back to U0, so that the ERDY can be sent. */
                    if (CyU3PUsbGetSpeed() == CY_U3P_SUPER_SPEED)
                        CyU3PUsbSetLinkPowerState (CyU3PUsbLPM_U0);
                }
                else
                    CyU3PDmaChannelReset (&glChWrHandleU2S);
            }

            if (glMscState == CY_FX_MSC_STATE_STATUS)
            {
                CyFxAppSendScsiCsw ();
            }
        }
    }
}

/*
 * Application define function which creates the threads. This is called from
 * the tx_application _define function.
 */
void
CyFxApplicationDefine (
        void)
{
    void *ptr;
    uint32_t retThrdCreate;

    /* Allocate the memory for the threads */
    ptr = CyU3PMemAlloc (CY_FX_APP_THREAD_STACK);

    /* Create the thread for the application */
    retThrdCreate = CyU3PThreadCreate (&mscAppThread,   /* MSC App Thread structure */
                          "25:MSC Application",         /* Thread ID and Thread name */
                          PibAppThread_Entry,           /* MSC App Thread Entry function */
                          0,                            /* No input parameter to thread */
                          ptr,                          /* Pointer to the allocated thread stack */
                          CY_FX_APP_THREAD_STACK,       /* MSC App Thread stack size */
                          CY_FX_APP_THREAD_PRIORITY,    /* MSC App Thread priority */
                          CY_FX_APP_THREAD_PRIORITY,    /* MSC App Thread priority */
                          CYU3P_NO_TIME_SLICE,          /* No time slice for the application thread */
                          CYU3P_AUTO_START              /* Start the Thread immediately */
                          );

    /* Check the return code */
    if (retThrdCreate != 0)
    {
        /* Thread Creation failed with the error code retThrdCreate */

        /* Add custom recovery or debug actions here */

        /* Application cannot continue */
        /* Loop indefinitely */
        while(1);
    }
}

/*[]*/




