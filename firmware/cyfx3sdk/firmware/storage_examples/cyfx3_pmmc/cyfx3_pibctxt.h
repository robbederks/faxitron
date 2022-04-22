/*
 ## Cypress PIB Firmware Header File (cyfx3_pibctxt.h)
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

/* This file contains MBOX related macros and function declarations utilized for the PIB application */

#ifndef _INCLUDED_CYFX3_PIBCTXT_H_
#define _INCLUDED_CYFX3_PIBCTXT_H_

#include <cyu3types.h>
#include <cyu3mbox.h>
#include <cyu3externcstart.h>

/**< Mask to get message id from dword1 of mailbox message. */
#define CYPIB_MBOX_MID_MASK            (0xFFFF0000)

/**< Mask to get data0 from dword1 of mailbox message. */
#define CYPIB_MBOX_D0_MASK             (0x0000FFFF)

/**< Mask to get data1 from dword0 of mailbox message. */
#define CYPIB_MBOX_D1_MASK             (0xFFFF0000)

/**< Mask to get data2 from dword0 of mailbox message. */
#define CYPIB_MBOX_D2_MASK             (0x0000FFFF)

/**< Position of message id in dword1 of mailbox message. */
#define CYPIB_MBOX_MID_POS             (16)

/**< Position of data0 in dword1 of mailbox message. */
#define CYPIB_MBOX_D0_POS              (0)

/**< Position of data1 in dword0 of mailbox message. */
#define CYPIB_MBOX_D1_POS              (16)

/**< Position of data2 in dword0 of mailbox message. */
#define CYPIB_MBOX_D2_POS              (0)

/**< Macro to get message id field from mailbox message structure. */
#define CYPIB_GET_MESSAGEID(mbx)       (uint16_t)(((mbx).w1 & CYPIB_MBOX_MID_MASK) >> CYPIB_MBOX_MID_POS)

/**< Macro to get data0 word from mailbox message structure. */
#define CYPIB_GET_DATA0(mbx)           (uint16_t)(((mbx).w1 & CYPIB_MBOX_D0_MASK) >> CYPIB_MBOX_D0_POS)

/**< Macro to get data1 word from mailbox message structure. */
#define CYPIB_GET_DATA1(mbx)           (uint16_t)(((mbx).w0 & CYPIB_MBOX_D1_MASK) >> CYPIB_MBOX_D1_POS)

/**< Macro to get data2 word from mailbox message structure. */
#define CYPIB_GET_DATA2(mbx)           (uint16_t)(((mbx).w0 & CYPIB_MBOX_D2_MASK) >> CYPIB_MBOX_D2_POS)

#define CYPIB_MBOX_MID_CTX_MASK        (0x0700)        /**< Mask to get context from message id. */
#define CYPIB_MBOX_MID_CTX_POS         (8)             /**< Position of context field in message id. */
#define CYPIB_MBOX_MID_LAST_MASK       (0x8000)        /**< Mask for last bit in message id. */
#define CYPIB_MBOX_MID_LAST_POS        (15)            /**< Position of last bit in message id. */
#define CYPIB_MBOX_MID_EVT_POS         (14)            /**< Position of event bit in message id. */
#define CYPIB_MBOX_MID_INFO_MASK       (0x7FFF)        /**< Mask for all info fields in message id. */
#define CYPIB_MBOX_MID_TAG_MASK        (0x3800)        /**< Mask to get tag field from message id. */
#define CYPIB_MBOX_MID_TAG_POS         (11)            /**< Position of tag field in message id. */
#define CYPIB_MBOX_MID_RQT_MASK        (0x00FF)        /**< Mask to get request code from message id. */

#define CYPIB_MBOX_MAKE_MID(last,ev,tag,ctx,rqtid)                                                \
    (((uint32_t)(((last) << CYPIB_MBOX_MID_LAST_POS) | ((ev) << CYPIB_MBOX_MID_EVT_POS) |        \
                 ((tag) << CYPIB_MBOX_MID_TAG_POS) | ((ctx) << CYPIB_MBOX_MID_CTX_POS) |         \
                 (rqtid))) << CYPIB_MBOX_MID_POS)

#define CYPIB_MBOX_GEN_EVT_MASK        (0x4000)

/******************************************************************************/

/** \brief Mailbox message handler function.

    **Description**\n
    Callback function that handles mailbox messages from the PIB API.

    **Returns**\n
    * None
 */
extern void
CyFxAppMboxCallback (
        CyBool_t isNewMsg               /**< Whether the callback indicates a new message received. */
        );

/** \brief Send firmware version and feature information to the PIB API.

    **Description**\n
    Function to send firmware version and feature support information to the PIB API running on the
    BMC side. This is called in response to a get firmware information request, or asynchronously when
    FX3S senses that the BMC has connected to it on the PMMC interface.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendFwInfo (
        CyBool_t isEvt,                 /**< Whether this is a response or an asynchronous event. */
        uint16_t tag                    /**< Tag to be associated in the case of a response. */
        );

/** \brief Send the return status for a mailbox request from the PIB API.

    **Description**\n
    This function sends a response indicating the return status of an request received
    from the PIB API.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendStatusResponse (
        uint8_t  context,               /**< Context associated with the request. */
        uint8_t  tag,                   /**< Tag associated with the request. */
        uint16_t errCode                /**< Actual return status (error code) for the request. */
        );


/** \brief Handler for all general requests from the PIB API.

    **Description**\n
    This function handles the general (device) category of requests from the PIB API. This includes
    requests like get firmware information.

    **Returns**\n
    * None
 */
extern void
CyFxAppHandleGeneralRqt (
        void);

/** \brief Send a SD card hotplug event notification.

    **Description**\n
    This function sends an asynchronous notification to the PIB API that an SD card has been
    removed or inserted at runtime.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendHotPlugEvent (
        uint8_t  port,                  /**< Storage port on which the change occured. */
        CyBool_t isRemove               /**< Whether card was inserted (false) or removed (true). */
        );

/** \brief Send the response to a set partition owner request.

    **Description**\n
    This function sends the response to a set partition owner request from the PIB API.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendSetOwnerResp (
        uint8_t tag,                            /**< Tag associated with the request. */
        uint8_t owner0,                         /**< Bit map representing partition owners on port 0. */
        uint8_t owner1                          /**< Bit map representing partition owners on port 1. */
        );

/** \brief Send the response to a query port request from the PIB API.

    **Description**\n
    This function sends the response to a query port (get number of SD cards connected) request from
    the PIB API.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendQueryPortResp (
        uint8_t tag,                            /**< Tag associated with the request. */
        uint8_t port,                           /**< Port being queried. */
        uint8_t val                             /**< Value to be returned. */
        );

/** \brief Send the response to a query media properties request.

    **Description**\n
    This function sends the response to a query media properties request from the PIB API.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendQueryMediaResp (
        uint8_t  tag,                   /**< Tag associated with the request. */
        uint8_t  port,                  /**< Port on which media is being queried. */
        uint8_t  devType,               /**< Type of device identified. */
        uint16_t blkSize,               /**< Sector size for data access. */
        CyBool_t isLocked,              /**< Whether device is password locked. */
        CyBool_t isWriteable,           /**< Whether device is write enabled. */
        uint8_t  partCount,             /**< Number of partitions found. */
        uint32_t eraseSize              /**< Size of device erase unit. */
        );

/** \brief Send the response to a query partition request.

    **Description**\n
    This functions sends the response to a query partition properties request from the PIB API.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendQueryUnitResp (
        uint8_t  tag,                   /**< Tag associated with the request. */
        uint8_t  port,                  /**< Port on which the partition is being queried. */
        uint8_t  unit,                  /**< Id of partition being queried. */
        uint16_t blkSize,               /**< Sector size for data access. Will be the same for all partitions. */
        uint32_t startAddr,             /**< Start address for the partition. */
        uint32_t unitSize               /**< Size of the partition in sector units. */
        );

/** \brief Send the response to a read SD card register request.

    **Description**\n
    This function sends the response to a request to read one of the OCR, CID or CSD registers
    of an SD card.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendReadRegResponse (
        uint8_t  tag,                   /**< Tag associated with the request. */
        uint8_t *data_p,                /**< Buffer containing register data to be sent. */
        uint8_t  dataLen                /**< Length of the register data in bytes. */
        );

/** \brief Send a debug log message to the PIB API.

    **Description**\n
    The PIB firmware sends out log messages for debug purposes through the mailbox interface.
    Each debug message contains three 16-bit words of data, that can be used in a free form manner. This
    function formats the three words of data as a message and sends it to the PIB API.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendDebugMessage (
        uint16_t d0,                    /**< First 16-bit word of data. */
        uint16_t d1,                    /**< Second 16-bit word of data. */
        uint16_t d2                     /**< Third 16-bit word of data. */
        );

/** \brief Send USB Connection Status to the PIB API.

    **Description**\n
    The PIB firmware sends out USB Connection status to the Application Processor stating USB is connected
    or disconnected.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendConnectStatus (
        uint8_t tag,
        uint8_t status
        );

/** \brief Send Endpoint NAK/STALL status to the PIB API.

    **Description**\n
    The PIB firmware sends out Endpoint NAK or STALL status for respective endpoint back to
    the Application Processor.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendEPStatus (
        uint8_t tag,
        CyBool_t isNak,
        uint8_t status
        );


/** \brief Send Setup Request Events to the PIB API.

    **Description**\n
    The PIB firmware sends out setup requests received from the USB Host to the Application Processor. Only AP
    Interface specific and generic Device Descriptor requests are forwarded.

    **Returns**\n
    * None
 */
extern CyU3PReturnStatus_t
CyFxAppSendSetupRqtEvent (
        uint32_t setupdat0,
        uint32_t setupdat1
        );

/** \brief Send Endpoint Configuration to the PIB API.

    **Description**\n
    The PIB firmware sends out respective endpoints configuration to the Application Processor as requested.

    **Returns**\n
    * None
 */
extern void
CyFxAppSendEPConfig (
        uint8_t tag,
        uint16_t size,
        uint16_t count,
        uint8_t  usbSock
        );

/** \brief Send USB related Events to the PIB API.

    **Description**\n
    The PIB firmware sends out USB Events received from the USB Host to the Application Processor. 

    **Returns**\n
    * None
 */
extern void
CyFxAppSendUSBRqtEvent (
        uint16_t evt,
        uint16_t arg
        );

#include <cyu3externcend.h>
#endif  /* _INCLUDED_CYFX3_PIBCTXT_H_ */       

/*[]*/

