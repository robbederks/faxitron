/*
## Cypress PIB firmware source (cyfx3s_pibctxt.c)
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

#include <cyu3spi.h>
#include <cyu3mbox.h>
#include <cyu3gpio.h>

#include "cywbprotocol.h"
#include "cyfx3_pib.h"
#include "cyfx3_pibctxt.h"

/* Summary:
   This file contains the functions to handle the Storage, USB and General requests sent by the AP as well
   as functions for transmitting responses and events to the Application Processor.
*/

/************************************************************************************/
/********************************GLOBAL VARIABLES************************************/
/************************************************************************************/

CyBool_t glPibSendEvents = CyTrue;
uint16_t glPibRqtData[PIB_MAX_VALID_CONTEXT][PIB_MAX_REQUEST_SIZE];
uint8_t  glPibRqtOffset[PIB_MAX_VALID_CONTEXT] = {0};

/* Callback for receiving notification of mailbox messages from the external processor. */
void
CyFxAppMboxCallback (
        CyBool_t isNewMsg)
{
    CyU3PReturnStatus_t status;
    CyU3PMbox fxAppMbox;
    uint16_t mId = 0;
    uint16_t d0, d1, d2;
    uint16_t ctx;
    uint8_t  offset;

    /* Only incoming messages are handled here. Outgoing messages are queued directly from the thread. */
    if (isNewMsg)
    {
        /* If we received a new mailbox message, the BMC is active. We can start sending data again. */
        glPibSendEvents = CyTrue;

        /* Read the request into the global buffer and send an event to the thread. */
        status = CyU3PMboxRead (&fxAppMbox);
        if (status == CY_U3P_SUCCESS)
        {
            /* Start parsing the request. */
            mId = CYPIB_GET_MESSAGEID (fxAppMbox);
            d0  = CYPIB_GET_DATA0 (fxAppMbox);
            d1  = CYPIB_GET_DATA1 (fxAppMbox);
            d2  = CYPIB_GET_DATA2 (fxAppMbox);

            ctx = (mId & CYPIB_MBOX_MID_CTX_MASK) >> CYPIB_MBOX_MID_CTX_POS;
            if (ctx >= PIB_MAX_VALID_CONTEXT)
            {
                CyU3PDebugPrint (2, "Message received on invalid context %d\r\n", ctx);
                return;
            }

            /* If the previous request is not closed, drop it and move to the new request. */
            if ((glPibRqtOffset[ctx] == 0) ||
                    ((glPibRqtData[ctx][0] & CYPIB_MBOX_MID_INFO_MASK) != (mId & CYPIB_MBOX_MID_INFO_MASK)))
            {
                glPibRqtOffset[ctx] = 0;
                if (mId & CYPIB_MBOX_MID_LAST_MASK)
                {
                    glPibRqtData[ctx][0] = mId;
                    glPibRqtData[ctx][1] = d0;
                    glPibRqtData[ctx][2] = d1;
                    glPibRqtData[ctx][3] = d2;
                    glPibRqtOffset[ctx]  = 4;
                }
                else
                {
                    glPibRqtData[ctx][0] = mId;
                    glPibRqtData[ctx][1] = d1;
                    glPibRqtData[ctx][2] = d2;
                    glPibRqtOffset[ctx]  = 3;
                }
            }
            else
            {
                offset = glPibRqtOffset[ctx];
                glPibRqtData[ctx][offset++] = d0;
                glPibRqtData[ctx][offset++] = d1;
                glPibRqtData[ctx][offset++] = d2;
                glPibRqtOffset[ctx] = offset;
            }

            /* Full request has been received, signal the thread. */
            if (mId & CYPIB_MBOX_MID_LAST_MASK)
            {
                glPibRqtOffset[ctx] = 0;

                switch (ctx)
                {
                    case CY_RQT_GENERAL_RQT_CONTEXT:
                        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_GEN_CTX_FLAG, CYU3P_EVENT_OR);
                        break;
                    case CY_RQT_USB_RQT_CONTEXT:
                        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_USB_CTX_FLAG, CYU3P_EVENT_OR);
                        break;
                    case CY_RQT_STORAGE_RQT_CONTEXT:
                        CyU3PEventSet (&glPibAppEvent, CY_FX_MSC_STR_CTX_FLAG, CYU3P_EVENT_OR);
                        break;
                }
            }
        }
    }
}

static CyU3PReturnStatus_t
CyFxAppMboxWrite (
        CyU3PMbox *mbox)
{
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    if (glPibSendEvents)
    {
        status = CyU3PMboxWrite (mbox);

        /* Sending a message timed out. Assume that BMC is not present and stop
           sending messages. */
        if (status != CY_U3P_SUCCESS)
            glPibSendEvents = CyFalse;
    }

    return status;
}

void
CyFxAppSendFwInfo (
        CyBool_t isEvt,
        uint16_t tag
        )
{
    CyU3PMbox rspMbox;
    uint32_t  hdr;

    if (isEvt)
    {
        hdr = (CYPIB_MBOX_GEN_EVT_MASK | CY_EVT_INITIALIZATION_COMPLETE);
    }
    else
    {   
        hdr = CY_RESP_FIRMWARE_VERSION | tag;
    }

    rspMbox.w1 = (hdr << CYPIB_MBOX_MID_POS) | 0x0004;  /* 4 word response. */
    rspMbox.w0 = CY_U3P_MAKEDWORD (PIB_FW_VERS_MAJOR, PIB_FW_VERS_MINOR, PIB_FW_VERS_PATCH, 0);
    CyFxAppMboxWrite (&rspMbox);

    rspMbox.w1 = ((CYPIB_MBOX_MID_LAST_MASK | hdr) << CYPIB_MBOX_MID_POS) | PIB_FW_VERS_BUILD;
    rspMbox.w0 = 0;
    CyFxAppMboxWrite (&rspMbox);
}

/*
 * Send success failure response
 */
void
CyFxAppSendStatusResponse (
        uint8_t  context,
        uint8_t  tag, 
        uint16_t errCode)
{
    CyU3PMbox rspMbox;

    /* Format the message as required. */
    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 0, tag, context, CY_RESP_SUCCESS_FAILURE) | errCode;
    rspMbox.w0 = 0;
    CyFxAppMboxWrite (&rspMbox);
}

void
CyFxAppSendSuspendEvent (
        uint8_t  wakeUpSource)
{
    CyU3PMbox rspMbox;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 1, 0, CY_RQT_GENERAL_RQT_CONTEXT, CY_EVT_OUT_OF_SUSPEND) | (wakeUpSource);
    rspMbox.w0 = 0;
    CyFxAppMboxWrite (&rspMbox);
}

CyU3PReturnStatus_t
CyFxAppPrepareFX3ForStandby (
        void)
{
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    if (CyU3PIsSibActive ())
    {
        return CY_U3P_ERROR_INVALID_SEQUENCE;
    }

    status = CyU3PPibDeInit ();
    if (status == CY_U3P_SUCCESS)
    {
        CyU3PMboxDeInit ();
        CyU3PDebugDeInit ();
        CyU3PUartDeInit ();
        CyU3PGpioDeInit ();
    }

    return status;
}

void
CyFxAppHandleGeneralRqt (
        void)
{
    uint8_t  rqt, tag;    
    uint16_t mId;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    mId = glPibRqtData[CY_RQT_GENERAL_RQT_CONTEXT][0];
    rqt = (uint8_t)(mId & CYPIB_MBOX_MID_RQT_MASK);
    tag = (uint8_t)((mId & CYPIB_MBOX_MID_TAG_MASK) >> CYPIB_MBOX_MID_TAG_POS);

    switch (rqt)
    {
        case CY_RQT_GET_FIRMWARE_VERSION:
            CyFxAppSendFwInfo (CyFalse, tag);
            break;

        case CY_RQT_ENTER_STANDBY_MODE:
            {
                status = CyU3PSysCheckStandbyParam (glPibRqtData[CY_RQT_GENERAL_RQT_CONTEXT][1],
                        glPibRqtData[CY_RQT_GENERAL_RQT_CONTEXT][2], (uint8_t *)0x40060000);
                if (status == CY_U3P_SUCCESS)
                {
                    /* De-Init LPP, PIB Blocks */
                    status = CyFxAppPrepareFX3ForStandby ();
                    if (status == CY_U3P_SUCCESS)
                    {
                        CyFxAppSendStatusResponse (CY_RQT_GENERAL_RQT_CONTEXT, tag, CY_WB_ERROR_SUCCESS);
                        CyU3PMboxWait ();                    
                        
                        /* Go into standby mode and wait for wakeup signal. The I-TCM content and GPIO register
                         * * state will be backed up in the memory area starting at address 0x40060000. */                
                        status = CyU3PSysEnterStandbyMode (glPibRqtData[CY_RQT_GENERAL_RQT_CONTEXT][1],
                                glPibRqtData[CY_RQT_GENERAL_RQT_CONTEXT][2], (uint8_t *)0x40060000);
                    }
                }
                
                if (status != CY_U3P_SUCCESS)
                {
                    /* Re-Init Debug Block for logs */
                    CyFxMscApplnDebugInit();
                    CyFxAppErrorHandler (status);
                }

                CyFxAppErrorHandler (1);
            }
            break;

        case CY_RQT_ENTER_SUSPEND_MODE:
            {
                uint16_t temp = 0;

                status = CyU3PSysCheckSuspendParams (glPibRqtData[CY_RQT_GENERAL_RQT_CONTEXT][1],
                        glPibRqtData[CY_RQT_GENERAL_RQT_CONTEXT][2]);
                if (status == CY_U3P_SUCCESS)
                {
                    CyFxAppSendStatusResponse (CY_RQT_GENERAL_RQT_CONTEXT, tag, CY_WB_ERROR_SUCCESS);
                    CyU3PSysSendEnterSuspendStatus ();

                    status = CyU3PSysEnterSuspendMode (glPibRqtData[CY_RQT_GENERAL_RQT_CONTEXT][1],
                            glPibRqtData[CY_RQT_GENERAL_RQT_CONTEXT][2], &temp);
                }

                if (status == CY_U3P_SUCCESS)
                {
                    CyFxAppSendSuspendEvent (temp);
                }
                else
                {
                    if (status == CY_U3P_ERROR_BAD_ARGUMENT)
                    {
                        status = CY_WB_ERROR_INVALID_PARAMETER;
                    }
                    else
                    {
                        status = CY_WB_ERROR_GENERAL_FW_FAILURE;
                    }
                    
                    CyFxAppSendStatusResponse (CY_RQT_GENERAL_RQT_CONTEXT, tag, status);
                }
            }
            break;

        default:
            CyFxAppSendStatusResponse (CY_RQT_GENERAL_RQT_CONTEXT, tag, CY_WB_ERROR_NOT_SUPPORTED);
            break;
    }
}

void
CyFxAppSendHotPlugEvent (
        uint8_t  port,
        CyBool_t isRemove)
{
    CyU3PMbox rspMbox;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 1, 0, CY_RQT_STORAGE_RQT_CONTEXT, CY_EVT_MEDIA_CHANGED) |
        (port << 12) | isRemove;
    rspMbox.w0 = 0;
    CyFxAppMboxWrite (&rspMbox);
}

CyU3PReturnStatus_t
CyFxAppSendSetupRqtEvent (
        uint32_t setupdat0,
        uint32_t setupdat1
        )
{
    CyU3PMbox rspMbox;
    CyU3PReturnStatus_t status;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (0, 1, 0, CY_RQT_USB_RQT_CONTEXT, CY_EVT_USB_SETUP_REQUEST) |
        0x0004;
    rspMbox.w0 = (uint32_t)((CY_U3P_GET_LSW(setupdat0) << 16) | CY_U3P_GET_MSW(setupdat0));
    status = CyFxAppMboxWrite (&rspMbox);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 1, 0, CY_RQT_USB_RQT_CONTEXT, CY_EVT_USB_SETUP_REQUEST) |
        CY_U3P_GET_LSW (setupdat1);
    rspMbox.w0 = (uint32_t)(CY_U3P_GET_MSW (setupdat1) << 16);
    status = CyFxAppMboxWrite (&rspMbox);

    return status;
}

void
CyFxAppSendUSBRqtEvent (
        uint16_t evt,
        uint16_t arg
        )
{
    CyU3PMbox rspMbox;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 1, 0, CY_RQT_USB_RQT_CONTEXT, CY_EVT_USB_EVENT) | evt;
    rspMbox.w0 = (uint32_t) arg;
    CyFxAppMboxWrite (&rspMbox);
}


void
CyFxAppSendQueryPortResp (
        uint8_t tag,
        uint8_t port,
        uint8_t val)
{
    CyU3PMbox rspMbox;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 0, tag, CY_RQT_STORAGE_RQT_CONTEXT, CY_RESP_PORT_DESCRIPTOR) | (port << 12);
    rspMbox.w0 = (val << 16);
    CyFxAppMboxWrite (&rspMbox);
}

void
CyFxAppSendQueryMediaResp (
        uint8_t  tag,
        uint8_t  port,
        uint8_t  devType,
        uint16_t blkSize,
        CyBool_t isLocked,
        CyBool_t isWriteable,
        uint8_t  partCount,
        uint32_t eraseSize)
{
    CyU3PMbox rspMbox;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (0, 0, tag, CY_RQT_STORAGE_RQT_CONTEXT, CY_RESP_DEVICE_DESCRIPTOR) | 0x05;
    rspMbox.w0 = (((port << 12) | devType) << 16) | blkSize;
    CyFxAppMboxWrite (&rspMbox);

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 0, tag, CY_RQT_STORAGE_RQT_CONTEXT, CY_RESP_DEVICE_DESCRIPTOR) |
        (0x8000 | (isLocked << 9) | (isWriteable << 8) | partCount);
    rspMbox.w0 = eraseSize;
    CyFxAppMboxWrite (&rspMbox);
}

void
CyFxAppSendQueryUnitResp (
        uint8_t  tag,
        uint8_t  port,
        uint8_t  unit,
        uint16_t blkSize,
        uint32_t startAddr,
        uint32_t unitSize)
{
    CyU3PMbox rspMbox;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (0, 0, tag, CY_RQT_STORAGE_RQT_CONTEXT, CY_RESP_UNIT_DESCRIPTOR) | 0x06;
    rspMbox.w0 = (((port << 12) | unit) << 16) | blkSize;
    CyFxAppMboxWrite (&rspMbox);

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (0, 0, tag, CY_RQT_STORAGE_RQT_CONTEXT, CY_RESP_UNIT_DESCRIPTOR) |
        CY_U3P_GET_LSW (startAddr);
    rspMbox.w0 = (CY_U3P_GET_MSW (startAddr) << 16) | CY_U3P_GET_LSW (unitSize);
    CyFxAppMboxWrite (&rspMbox);

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 0, tag, CY_RQT_STORAGE_RQT_CONTEXT, CY_RESP_UNIT_DESCRIPTOR) |
        CY_U3P_GET_MSW (unitSize);
    rspMbox.w0 = 0;
    CyFxAppMboxWrite (&rspMbox);
}


void
CyFxAppSendReadRegResponse (
        uint8_t  tag,
        uint8_t *data_p,
        uint8_t  dataLen)
{
    CyU3PMbox rspMbox;

    if (dataLen <= 6)
    {
        rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 0, tag, CY_RQT_STORAGE_RQT_CONTEXT, CY_RESP_SD_REGISTER_DATA) |
            CY_U3P_MAKEWORD (data_p[1], data_p[0]);
        rspMbox.w0 = CY_U3P_MAKEDWORD (data_p[5], data_p[4], data_p[3], data_p[2]);
        CyFxAppMboxWrite (&rspMbox);
        return;
    }
    else
    {
        rspMbox.w1 = CYPIB_MBOX_MAKE_MID (0, 0, tag, CY_RQT_STORAGE_RQT_CONTEXT, CY_RESP_SD_REGISTER_DATA) |
            (dataLen / 2);
        rspMbox.w0 = CY_U3P_MAKEDWORD (data_p[3], data_p[2], data_p[1], data_p[0]);
        CyFxAppMboxWrite (&rspMbox);

        data_p  += 4;
        dataLen -= 4;
        while (dataLen)
        {
            rspMbox.w1 = CYPIB_MBOX_MAKE_MID (0, 0, tag, CY_RQT_STORAGE_RQT_CONTEXT, CY_RESP_SD_REGISTER_DATA) |
                CY_U3P_MAKEWORD (data_p[1], data_p[0]);
            rspMbox.w0 = CY_U3P_MAKEDWORD (data_p[5], data_p[4], data_p[3], data_p[2]);
            if (dataLen <= 6)
            {
                rspMbox.w1 |= (CYPIB_MBOX_MID_LAST_MASK << CYPIB_MBOX_MID_POS);
                dataLen = 0;
            }
            else
            {
                data_p  += 6;
                dataLen -= 6;
            }

            CyFxAppMboxWrite (&rspMbox);
        }
    }
}

void
CyFxAppSendDebugMessage (
        uint16_t d0,
        uint16_t d1,
        uint16_t d2)
{
    CyU3PMbox rspMbox;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 1, 0, CY_RQT_GENERAL_RQT_CONTEXT, CY_EVT_DEBUG_MESSAGE) | d0;
    rspMbox.w0 = (d1 << 16) | d2;
    CyFxAppMboxWrite (&rspMbox);
}

void
CyFxAppSendSetOwnerResp (
        uint8_t tag,
        uint8_t owner0,
        uint8_t owner1)
{
    CyU3PMbox rspMbox;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 0, tag, CY_RQT_STORAGE_RQT_CONTEXT, CY_RESP_SET_OWNER) |
        (owner1 << 8) | owner0;
    rspMbox.w0 = 0;
    CyFxAppMboxWrite (&rspMbox);
}

void
CyFxAppSendEPConfig (
        uint8_t tag,
        uint16_t size,
        uint16_t count,
        uint8_t  usbSock
        )
{
    CyU3PMbox rspMbox;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 0, tag, CY_RQT_USB_RQT_CONTEXT, CY_RESP_ENDPOINT_CONFIG) | (size);
    rspMbox.w0 = (uint32_t) ((count << 16) | usbSock);
    CyFxAppMboxWrite (&rspMbox);
}

void
CyFxAppSendConnectStatus (
        uint8_t tag,
        uint8_t status
        )
{
    CyU3PMbox rspMbox;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 0, tag, CY_RQT_USB_RQT_CONTEXT, CY_RESP_CONNECT_STATE) | status;
    rspMbox.w0 = 0;
    CyFxAppMboxWrite (&rspMbox);
}

void
CyFxAppSendEPStatus (
        uint8_t tag,
        CyBool_t isNak,
        uint8_t status
        )
{
    CyU3PMbox rspMbox;

    rspMbox.w1 = CYPIB_MBOX_MAKE_MID (1, 0, tag, CY_RQT_USB_RQT_CONTEXT, ((isNak) ? (CY_RESP_ENDPOINT_NAK)
                : (CY_RESP_ENDPOINT_STALL))) | status;
    rspMbox.w0 = 0;
    CyFxAppMboxWrite (&rspMbox);
}




/*[]*/


