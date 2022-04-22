/*
 ## Cypress USB 3.0 Platform source file (cyfxechodrv.c)
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

/* This file illustrates the USB Echo device driver. */

#include "cyu3system.h"
#include "cyu3os.h"
#include "cyu3dma.h"
#include "cyu3error.h"
#include "cyu3usb.h"
#include "cyu3usbhost.h"
#include "cyu3usbotg.h"
#include "cyu3utils.h"
#include "cyfxusbhost.h"

static uint8_t         gEchoInEp = 0;
static uint8_t         gEchoOutEp = 0;
static uint8_t         gLoopInEp = 0;
static uint8_t         gLoopOutEp = 0;
static CyU3PDmaChannel gEchoOutChannel;
static CyU3PDmaChannel gEchoInChannel;
static CyU3PDmaChannel gLoopOutChannel;
static CyU3PDmaChannel gLoopInChannel;

static CyU3PReturnStatus_t
CyFxEchoSendData (
        CyBool_t isInterrupt,
        uint16_t count)
{
    CyU3PDmaBuffer_t       buf_p;
    CyU3PUsbHostEpStatus_t epStatus;
    CyU3PReturnStatus_t    status = CY_U3P_SUCCESS;

    CyU3PDmaChannel *chHandle;
    uint8_t          endpoint;

    if (isInterrupt)
    {
        chHandle = &gLoopOutChannel;
        endpoint = gLoopOutEp;
    }
    else
    {
        chHandle = &gEchoOutChannel;
        endpoint = gEchoOutEp;
    }

    status = CyU3PDmaChannelGetBuffer (chHandle, &buf_p, CYU3P_WAIT_FOREVER);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (2, "Failed to get empty buffer to commit\r\n");
        return status;
    }

    if (count > buf_p.size)
    {
        CyU3PDebugPrint (2, "Invalid count specified\r\n");
        return status;
    }

    status = CyU3PDmaChannelCommitBuffer (chHandle, count, 0);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (2, "Failed to commit DMA buffer\r\n");
        return status;
    }

    status = CyU3PUsbHostEpSetXfer (endpoint, CY_U3P_USB_HOST_EPXFER_NORMAL, count);
    if (status == CY_U3P_SUCCESS)
    {
        status = CyU3PUsbHostEpWaitForCompletion (endpoint, &epStatus, 5000);
    }

    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (2, "Out transfer failed with status %d\r\n", status);
    }

    return status;
}

static CyU3PReturnStatus_t
CyFxEchoReceiveData (
        CyBool_t isInterrupt,
        uint16_t count)
{
    CyU3PDmaBuffer_t       buf_p;
    CyU3PUsbHostEpStatus_t epStatus;
    CyU3PReturnStatus_t    status = CY_U3P_SUCCESS;

    CyU3PDmaChannel *chHandle;
    uint8_t          endpoint;

    if (isInterrupt)
    {
        chHandle = &gLoopInChannel;
        endpoint = gLoopInEp;
    }
    else
    {
        chHandle = &gEchoInChannel;
        endpoint = gEchoInEp;
    }

    status = CyU3PUsbHostEpSetXfer (endpoint, CY_U3P_USB_HOST_EPXFER_NORMAL, count);
    if (status == CY_U3P_SUCCESS)
    {
        status = CyU3PUsbHostEpWaitForCompletion (endpoint, &epStatus, 5000);
    }

    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (2, "In transfer failed with status %d\r\n", status);
        return status;
    }

    status = CyU3PDmaChannelGetBuffer (chHandle, &buf_p, CYU3P_NO_WAIT);
    if (status != CY_U3P_SUCCESS)
    {
        /* If we received a full packet and the buffer was not filled, GetBuffer() will fail. Ignore this failure. */
        CyU3PDebugPrint (2, "Failed to get full buffer\r\n");
        return CY_U3P_SUCCESS;
    }

    /* If the transfer was finished prematurely (short packet), print the actual count. */
    if (buf_p.count != count)
        CyU3PDebugPrint (4, "Obtained buffer with %d bytes of data\r\n", buf_p.count);

    CyU3PDmaChannelDiscardBuffer (chHandle);

    return status;
}

static CyU3PReturnStatus_t
DoEchoDeviceTesting (
        uint8_t outEp,
        uint8_t inEp,
        uint8_t outIntrEp,
        uint8_t inIntrEp)
{
    CyU3PReturnStatus_t     status;
    CyU3PUsbHostEpConfig_t  epCfg;
    CyU3PDmaChannelConfig_t dmaCfg;
    uint32_t                iter;

    /* Store the endpoint numbers. */
    gEchoOutEp = outEp;
    gEchoInEp  = inEp;
    gLoopOutEp = outIntrEp;
    gLoopInEp  = inIntrEp;

    /* Add the IN endpoint. */
    CyU3PMemSet ((uint8_t *)&epCfg, 0, sizeof(epCfg));
    epCfg.type         = CY_U3P_USB_EP_BULK;
    epCfg.mult         = 1;
    epCfg.maxPktSize   = 512;
    epCfg.pollingRate  = 0;
    epCfg.fullPktSize  = 512;
    epCfg.isStreamMode = CyFalse;

    status = CyU3PUsbHostEpAdd (inEp, &epCfg);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (2, "Failed to add endpoint %x\r\n", inEp);
        return status;
    }

    status = CyU3PUsbHostEpAdd (outEp, &epCfg);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PUsbHostEpRemove (inEp);
        CyU3PDebugPrint (2, "Failed to add endpoint %x\r\n", outEp);
        return status;
    }

    epCfg.type         = CY_U3P_USB_EP_INTR;
    epCfg.mult         = 1;
    epCfg.maxPktSize   = 512;
    epCfg.pollingRate  = 1;
    epCfg.fullPktSize  = 512;
    epCfg.isStreamMode = CyFalse;

    status = CyU3PUsbHostEpAdd (inIntrEp, &epCfg);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (2, "Failed to add endpoint %x\r\n", inIntrEp);
        CyU3PUsbHostEpRemove (inEp);
        CyU3PUsbHostEpRemove (outEp);
        return status;
    }

    status = CyU3PUsbHostEpAdd (outIntrEp, &epCfg);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (2, "Failed to add endpoint %x\r\n", outIntrEp);
        CyU3PUsbHostEpRemove (inIntrEp);
        CyU3PUsbHostEpRemove (inEp);
        CyU3PUsbHostEpRemove (outEp);
        return status;
    }

    /* Create a DMA channel for IN EP. */
    CyU3PMemSet ((uint8_t *)&dmaCfg, 0, sizeof(dmaCfg));
    dmaCfg.size           = 16384;
    dmaCfg.count          = 4;
    dmaCfg.prodSckId      = (CyU3PDmaSocketId_t)(CY_U3P_UIB_SOCKET_PROD_0 + (0x0F & inEp));
    dmaCfg.consSckId      = CY_U3P_CPU_SOCKET_CONS;
    dmaCfg.dmaMode        = CY_U3P_DMA_MODE_BYTE;
    dmaCfg.notification   = 0;
    dmaCfg.cb             = NULL;
    dmaCfg.prodHeader     = 0;
    dmaCfg.prodFooter     = 0;
    dmaCfg.consHeader     = 0;
    dmaCfg.prodAvailCount = 0;
    status = CyU3PDmaChannelCreate (&gEchoInChannel, CY_U3P_DMA_TYPE_MANUAL_IN, &dmaCfg);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PUsbHostEpRemove (outIntrEp);
        CyU3PUsbHostEpRemove (inIntrEp);
        CyU3PUsbHostEpRemove (inEp);
        CyU3PUsbHostEpRemove (outEp);
        CyU3PDebugPrint (2, "Failed to create IN DMA channel\r\n");
        return status;
    }

    /* Create a DMA channel for OUT EP. */
    dmaCfg.prodSckId = CY_U3P_CPU_SOCKET_PROD;
    dmaCfg.consSckId = (CyU3PDmaSocketId_t)(CY_U3P_UIB_SOCKET_CONS_0 + outEp);
    status = CyU3PDmaChannelCreate (&gEchoOutChannel, CY_U3P_DMA_TYPE_MANUAL_OUT, &dmaCfg);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PUsbHostEpRemove (outIntrEp);
        CyU3PUsbHostEpRemove (inIntrEp);
        CyU3PUsbHostEpRemove (inEp);
        CyU3PUsbHostEpRemove (outEp);
        CyU3PDmaChannelDestroy (&gEchoInChannel);
        CyU3PDebugPrint (2, "Failed to create OUT DMA channel\r\n");
        return status;
    }

    /* Create a DMA channel to write to the interrupt OUT endpoint. */
    dmaCfg.size           = 512;
    dmaCfg.count          = 4;
    dmaCfg.prodSckId      = CY_U3P_CPU_SOCKET_PROD;
    dmaCfg.consSckId      = (CyU3PDmaSocketId_t)(CY_U3P_UIB_SOCKET_CONS_0 + outIntrEp);
    dmaCfg.dmaMode        = CY_U3P_DMA_MODE_BYTE;
    dmaCfg.notification   = 0;
    dmaCfg.cb             = NULL;
    dmaCfg.prodHeader     = 0;
    dmaCfg.prodFooter     = 0;
    dmaCfg.consHeader     = 0;
    dmaCfg.prodAvailCount = 0;
    status = CyU3PDmaChannelCreate (&gLoopOutChannel, CY_U3P_DMA_TYPE_MANUAL_OUT, &dmaCfg);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (2, "Failed to create interrupt OUT DMA channel\r\n");
        CyU3PUsbHostEpRemove (outIntrEp);
        CyU3PUsbHostEpRemove (inIntrEp);
        CyU3PUsbHostEpRemove (inEp);
        CyU3PUsbHostEpRemove (outEp);
        CyU3PDmaChannelDestroy (&gEchoInChannel);
        CyU3PDmaChannelDestroy (&gEchoOutChannel);
        return status;
    }

    dmaCfg.prodSckId      = (CyU3PDmaSocketId_t)(CY_U3P_UIB_SOCKET_PROD_0 + (0x0F & inIntrEp));
    dmaCfg.consSckId      = CY_U3P_CPU_SOCKET_CONS;
    status = CyU3PDmaChannelCreate (&gLoopInChannel, CY_U3P_DMA_TYPE_MANUAL_IN, &dmaCfg);
    if (status != CY_U3P_SUCCESS)
    {
        CyU3PDebugPrint (2, "Failed to create interrupt IN DMA channel\r\n");
        CyU3PUsbHostEpRemove (outIntrEp);
        CyU3PUsbHostEpRemove (inIntrEp);
        CyU3PUsbHostEpRemove (inEp);
        CyU3PUsbHostEpRemove (outEp);
        CyU3PDmaChannelDestroy (&gEchoInChannel);
        CyU3PDmaChannelDestroy (&gEchoOutChannel);
        CyU3PDmaChannelDestroy (&gLoopOutChannel);
        return status;
    }

    /* Prepare the channels for data transfer. */
    CyU3PDmaChannelSetXfer (&gEchoOutChannel, 0);
    CyU3PDmaChannelSetXfer (&gEchoInChannel, 0);
    CyU3PDmaChannelSetXfer (&gLoopOutChannel, 0);
    CyU3PDmaChannelSetXfer (&gLoopInChannel, 0);

    /* Delay to allow the echo device to get ready. */
    CyU3PThreadSleep (100);

    CyU3PDebugPrint (4, "Timestamp before OUT transfers is %d\r\n", CyU3PGetTime ());
    for (iter = 0; iter < 1000; iter++)
    {
        status = CyFxEchoSendData (CyFalse, 31);
        if (status == CY_U3P_SUCCESS)
            status = CyFxEchoSendData (CyFalse, 16384);

        if (status != CY_U3P_SUCCESS)
        {
            CyU3PDebugPrint (2, "CyFxEchoSendData failed on iteration %d\r\n", iter);
            break;
        }
    }
    CyU3PDebugPrint (4, "Timestamp after OUT transfers is %d\r\n", CyU3PGetTime ());
    CyU3PDebugPrint (4, "SendData test done\r\n");

    CyU3PDebugPrint (4, "Timestamp before IN transfers is %d\r\n", CyU3PGetTime ());
    for (iter = 0; iter < 1000; iter++)
    {
        status = CyFxEchoReceiveData (CyFalse, 16384);
        if (status != CY_U3P_SUCCESS)
        {
            CyU3PDebugPrint (2, "CyFxEchoReceiveData failed on iteration %d\r\n", iter);
            break;
        }
    }
    CyU3PDebugPrint (4, "Timestamp after IN transfers is %d\r\n", CyU3PGetTime ());
    CyU3PDebugPrint (4, "ReceiveData test done\r\n");

    CyU3PDebugPrint (4, "\r\nTesting interrupt loop-back functionality\r\n");
    for (iter = 0; iter < 512; iter++)
    {
        status = CyFxEchoSendData (CyTrue, iter + 1);
        if (status != CY_U3P_SUCCESS)
        {
            CyU3PDebugPrint (2, "Interrupt OUT send failed on iteration %d\r\n", iter);
            break;
        }
        else
        {
            status = CyFxEchoReceiveData (CyTrue, 512);
            if (status != CY_U3P_SUCCESS)
            {
                CyU3PDebugPrint (2, "Interrupt IN received failed on iteration %d\r\n", iter);
                break;
            }
        }
    }

    if (status != CY_U3P_SUCCESS)
    {
        CyU3PUsbHostEpRemove (outIntrEp);
        CyU3PUsbHostEpRemove (inIntrEp);
        CyU3PUsbHostEpRemove (inEp);
        CyU3PUsbHostEpRemove (outEp);
        CyU3PDmaChannelDestroy (&gEchoInChannel);
        CyU3PDmaChannelDestroy (&gEchoOutChannel);
        CyU3PDmaChannelDestroy (&gLoopOutChannel);
        CyU3PDmaChannelDestroy (&gLoopInChannel);
    }

    return status;
}

/* Initialize the echo device and do data transfers. */
CyU3PReturnStatus_t
CyFxEchoDriverInit (
        void)
{
    uint16_t length, offset;
    uint8_t  outEp = 0xFF, inEp = 0xFF;
    uint8_t  outIntrEp = 0x0FF, inIntrEp = 0xFF;
    CyU3PReturnStatus_t status;

    /* Read first four bytes of configuration descriptor to determine the total length. */
    status = CyFxSendSetupRqt (0x80, CY_U3P_USB_SC_GET_DESCRIPTOR,
            (CY_U3P_USB_CONFIG_DESCR << 8), 0, 4, glEp0Buffer);
    if (status != CY_U3P_SUCCESS)
    {
        goto enum_error;
    }

    /* Identify the length of the data received. */
    length = CY_U3P_MAKEWORD(glEp0Buffer[3], glEp0Buffer[2]);
    if (length > CY_FX_HOST_EP0_BUFFER_SIZE)
    {
        goto enum_error;
    }

    /* Read the full configuration descriptor. */
    CyU3PMemSet (glEp0Buffer, 0, CY_FX_HOST_EP0_BUFFER_SIZE);
    status = CyFxSendSetupRqt (0x80, CY_U3P_USB_SC_GET_DESCRIPTOR,
            (CY_U3P_USB_CONFIG_DESCR << 8), 0, length, glEp0Buffer);
    if (status != CY_U3P_SUCCESS)
    {
        goto enum_error;
    }

    /* Set the new configuration. */
    status = CyFxSendSetupRqt (0x00, CY_U3P_USB_SC_SET_CONFIGURATION, 1, 0, 0, glEp0Buffer);
    if (status != CY_U3P_SUCCESS)
    {
        goto enum_error;
    }

    /* Read the full configuration descriptor again. */
    CyU3PMemSet (glEp0Buffer, 0, CY_FX_HOST_EP0_BUFFER_SIZE);
    status = CyFxSendSetupRqt (0x80, CY_U3P_USB_SC_GET_DESCRIPTOR,
            (CY_U3P_USB_CONFIG_DESCR << 8), 0, length, glEp0Buffer);
    if (status != CY_U3P_SUCCESS)
    {
        goto enum_error;
    }

    /* Parse the descriptor and find the endpoints. */
    offset = 0;
    while (offset < length)
    {
        if (glEp0Buffer[offset + 1] == CY_U3P_USB_ENDPNT_DESCR)
        {
            if (glEp0Buffer[offset + 3] == CY_U3P_USB_EP_BULK)
            {
                if (glEp0Buffer[offset + 2] & 0x80)
                {
                    inEp = glEp0Buffer[offset + 2];
                }
                else
                {
                    outEp = glEp0Buffer[offset + 2];
                }
            }

            if (glEp0Buffer[offset + 3] ==  CY_U3P_USB_EP_INTR)
            {
                if (glEp0Buffer[offset + 2] & 0x80)
                {
                    inIntrEp = glEp0Buffer[offset + 2];
                }
                else
                {
                    outIntrEp = glEp0Buffer[offset + 2];
                }
            }
        }

        /* Advance to next descriptor. */
        offset += glEp0Buffer[offset];
    }

    /* Verify that we got hold of two valid endpoints. */
    if ((outEp == 0x00) || (outEp > 0x0F) || (inEp == 0x80) || (inEp > 0x8F))
    {
        CyU3PDebugPrint (2, "Failed to get valid endpoints\r\n");
        goto enum_error;
    }

    if ((outIntrEp == 0x00) || (outIntrEp > 0x0F) || (inIntrEp == 0x80) || (inIntrEp > 0x8F))
    {
        CyU3PDebugPrint (2, "Failed to get valid endpoints\r\n");
        goto enum_error;
    }

    CyU3PDebugPrint (4, "Selected endpoints %x, %x, %x and %x for testing\r\n", outEp, inEp, outIntrEp, inIntrEp);
    status = DoEchoDeviceTesting (outEp, inEp, outIntrEp, inIntrEp);
    return status;

enum_error:
    return CY_U3P_ERROR_FAILURE;
}

/* Disables the echo peripheral driver. */
void
CyFxEchoDriverDeInit (
        void)
{
    CyU3PDmaChannelDestroy (&gEchoOutChannel);
    CyU3PDmaChannelDestroy (&gEchoInChannel);
    CyU3PDmaChannelDestroy (&gLoopOutChannel);
    CyU3PDmaChannelDestroy (&gLoopInChannel);

    CyU3PUsbHostEpRemove (gEchoOutEp);
    CyU3PUsbHostEpRemove (gEchoInEp);
    CyU3PUsbHostEpRemove (gLoopOutEp);
    CyU3PUsbHostEpRemove (gLoopInEp);

    CyU3PDebugPrint (4, "Finished echo driver de-init\r\n");
}

/* [ ] */
