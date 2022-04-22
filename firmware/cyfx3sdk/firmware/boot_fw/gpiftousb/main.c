/*
 ## Cypress FX3 Boot Firmware Example Source file (main.c)
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

#include "cyfx3usb.h"
#include "cyfx3device.h"
#include "cyfx3utils.h"
#include "cyfx3gpio.h"
#include "cyfx3uart.h"
#include "cyfx3dma.h"
#include "defines.h"
#include "sock_regs.h"

#define MANUAL_DMA
#define PRODUCE_DESCRIPTOR_BASE         (4)
#define CONSUME_DESCRIPTOR_BASE         (8)

static uint8_t  glUsbState = 0;
static uint8_t  glUsbConfig = 0;
static uint8_t  glAltSetting = 0;
static uint8_t  glUsbDevStatus = 0;
static uint8_t  glCheckForDisconnect = 0;
static uint8_t  glInCompliance = 0;
static CyBool_t glGpifDoInit = CyFalse;
static CyBool_t glGpifStarted = CyFalse;

static CyU3PUsbDescrPtrs    *gpUsbDescPtr; /* Pointer to the USB Descriptors */
static CyFx3BootUsbEp0Pkt_t  gEP0;

/* USB descriptors. */
extern uint8_t gbDevDesc[];
extern uint8_t gbCfgDesc[];
extern uint8_t gbDevQualDesc[];
extern uint8_t gbLangIDDesc[];
extern uint8_t gbManufactureDesc[];
extern uint8_t gbProductDesc[];
extern uint8_t gbSsDevDesc[];
extern uint8_t gbBosDesc[];
extern uint8_t gbSsConfigDesc[];
extern uint8_t gbFsConfigDesc[];

/* 4KB of buffer area used for control endpoint transfers. */
#define gpUSBData                   (uint8_t*)(USB_DMA_BUF_ADDRESS)
#define USB_DATA_BUF_SIZE           (1024*4)

/* Configure the DMA descriptors required for the GPIF to USB data transfer. */
static int
ConfigureDmaDescriptors (
        CyBool_t isManual)
{
    CyFx3BootErrorCode_t     stat;
    CyFx3BootDmaDescriptor_t dscrConf;
    uint16_t                 i, j;

    /* In the case of an AUTO channel, a single descriptor chain with both producer and consumer socket
     * links is created. In the case of a MANUAL channel, two descriptor chains with producer links only
     * and consumer links only are created. */

    if (isManual)
    {
        /* Producer descriptor chain: Base descriptor index is 4. */
        for (i = PRODUCE_DESCRIPTOR_BASE, j = 0; j < DMA_BUF_COUNT; i++, j++)
        {
            dscrConf.buffer = (uint8_t *)(DMA_BUF_BASE + j * DMA_BUF_SIZE);
            dscrConf.sync   = CY_U3P_EN_PROD_EVENT | CY_U3P_EN_PROD_INT |
                (CY_DMA_PIB_SOCKET_0 << CY_U3P_PROD_SCK_POS) |
                CY_U3P_EN_CONS_EVENT | CY_U3P_EN_CONS_INT |
                (CY_DMA_CPU_SOCKET_CONS << CY_U3P_CONS_SCK_POS);
            dscrConf.chain  = (j == (DMA_BUF_COUNT - 1)) ?
                ((PRODUCE_DESCRIPTOR_BASE << CY_U3P_RD_NEXT_DSCR_POS) |
                 (PRODUCE_DESCRIPTOR_BASE << CY_U3P_WR_NEXT_DSCR_POS)) :
                (((i + 1) << CY_U3P_RD_NEXT_DSCR_POS) | ((i + 1) << CY_U3P_WR_NEXT_DSCR_POS));
            dscrConf.size   = (DMA_BUF_SIZE);

            stat = CyFx3BootDmaSetDscrConfig (i, &dscrConf);
            if (stat != CY_FX3_BOOT_SUCCESS)
            {
                CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "SetDscrConfig returned %d\r\n", stat);
                return stat;
            }
        }

        /* Consumer descriptor chain: Base descriptor index is 8. */
        for (i = CONSUME_DESCRIPTOR_BASE, j = 0; j < DMA_BUF_COUNT; i++, j++)
        {
            dscrConf.buffer = (uint8_t *)(DMA_BUF_BASE + j * DMA_BUF_SIZE);
            dscrConf.sync   = CY_U3P_EN_PROD_EVENT | CY_U3P_EN_PROD_INT |
                (CY_DMA_CPU_SOCKET_PROD << CY_U3P_PROD_SCK_POS) |
                CY_U3P_EN_CONS_EVENT | CY_U3P_EN_CONS_INT |
                (CY_DMA_UIB_SOCKET_CONS_1 << CY_U3P_CONS_SCK_POS);
            dscrConf.chain  = (j == (DMA_BUF_COUNT - 1)) ?
                ((CONSUME_DESCRIPTOR_BASE << CY_U3P_RD_NEXT_DSCR_POS) |
                 (CONSUME_DESCRIPTOR_BASE << CY_U3P_WR_NEXT_DSCR_POS)) :
                (((i + 1) << CY_U3P_RD_NEXT_DSCR_POS) | ((i + 1) << CY_U3P_WR_NEXT_DSCR_POS));
            dscrConf.size   = (DMA_BUF_SIZE);

            stat = CyFx3BootDmaSetDscrConfig (i, &dscrConf);
            if (stat != CY_FX3_BOOT_SUCCESS)
            {
                CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "SetDscrConfig returned %d\r\n", stat);
                return stat;
            }
        }
    }
    else
    {
        /* Common descriptor chain: Base descriptor index is 4. */
        for (i = PRODUCE_DESCRIPTOR_BASE, j = 0; j < DMA_BUF_COUNT; i++, j++)
        {
            dscrConf.buffer = (uint8_t *)(DMA_BUF_BASE + j * DMA_BUF_SIZE);
            dscrConf.sync   = CY_U3P_EN_CONS_EVENT | CY_U3P_EN_PROD_EVENT |
                (CY_DMA_PIB_SOCKET_0 << CY_U3P_PROD_SCK_POS) | (CY_DMA_UIB_SOCKET_CONS_1 << CY_U3P_CONS_SCK_POS);
            dscrConf.chain  = (j == (DMA_BUF_COUNT - 1)) ?
                ((PRODUCE_DESCRIPTOR_BASE << CY_U3P_RD_NEXT_DSCR_POS) |
                 (PRODUCE_DESCRIPTOR_BASE << CY_U3P_WR_NEXT_DSCR_POS)) :
                (((i + 1) << CY_U3P_RD_NEXT_DSCR_POS) | ((i + 1) << CY_U3P_WR_NEXT_DSCR_POS));
            dscrConf.size   = (DMA_BUF_SIZE);

            stat = CyFx3BootDmaSetDscrConfig (i, &dscrConf);
            if (stat != CY_FX3_BOOT_SUCCESS)
            {
                CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "SetDscrConfig returned %d\r\n", stat);
                return stat;
            }
        }
    }

    return CY_FX3_BOOT_SUCCESS;
}

#ifdef MANUAL_DMA

static volatile uint16_t NextProduceDescriptor = PRODUCE_DESCRIPTOR_BASE;
static volatile uint16_t NextConsumeDescriptor = CONSUME_DESCRIPTOR_BASE;
static volatile uint32_t ProduceIntrCount = 0;
static volatile uint32_t ConsumeIntrCount = 0;

/* Handler for FX3 DMA interrupts. */
static void
DmaInterruptHandler (
        uint16_t sckId,
        uint32_t sckStat)
{
    CyFx3BootDmaSocket_t     sockConf;
    CyFx3BootDmaDescriptor_t prodDscr, consDscr;
    uint16_t                 activeDscr, currentDscr;
    uint32_t                 intStat;

    /* Note: Since each socket may have multiple buffers ready at one point in time, it is possible
     * that multiple interrupts of the same type (produce event or consume event) happen before the
     * first interrupt is serviced by firmware. The only way to identify the number of produce or
     * consume events is to look at the active descriptor in the socket and verify that we handle
     * the chain upto that point.
     *
     * The following sequence is the recommended way of ensuring that we do not miss interrupts and
     * can track the descriptor sequence accurately.
     * 1. Keep clearing the interrupt and reading the socket status until there are no more interrupts.
     * 2. Take the dscrChain value at this point as the current descriptor for the socket.
     *
     * The following code implements this scheme for both produce event and consume event processing.
     */

    /* Interrupt from the producer socket. */
    if (sckId == CY_DMA_PIB_SOCKET_0)
    {
        ProduceIntrCount++;

        /* New buffer produced on the PIB side. It is possible that multiple buffers have been produced by
         * the time we get to handle the interrupt. Make sure we handle all buffers upto the current buffer. */

        CyFx3BootDmaGetSocketConfig (sckId, &sockConf);
        intStat = sockConf.intr;

        do {
            /* Clear the active interrupts and read the interrupt status again until all interrupts are cleared. */
            CyFx3BootDmaClearSockInterrupts (sckId, sockConf.intr);
            CyFx3BootDmaGetSocketConfig (sckId, &sockConf);
            intStat |= sockConf.intr;
        } while ((sockConf.intr & sockConf.intrMask) != 0);

        if ((intStat & CY_U3P_PRODUCE_EVENT) != 0)
        {
            /* The current descriptor is the value of dscrChain at the point when interrupt status has reached 0. */
            activeDscr = (uint16_t)(sockConf.dscrChain);

            do {
                currentDscr = CONSUME_DESCRIPTOR_BASE + NextProduceDescriptor - PRODUCE_DESCRIPTOR_BASE;

                CyFx3BootDmaGetDscrConfig (NextProduceDescriptor, &prodDscr);
                CyFx3BootDmaGetDscrConfig (currentDscr, &consDscr);

                /* Copy the information from this into the corresponding consumer descriptor. */
                consDscr.size = prodDscr.size;
                CyFx3BootDmaSetDscrConfig (currentDscr, &consDscr);

                /* Send an event to the consumer socket. */
                CyFx3BootDmaSendSocketEvent (CY_DMA_UIB_SOCKET_CONS_1, currentDscr, CyTrue);

                /* Move to the next producer descriptor. */
                NextProduceDescriptor = (uint16_t)((prodDscr.chain & CY_U3P_WR_NEXT_DSCR_MASK) >>
                        CY_U3P_WR_NEXT_DSCR_POS);
            } while (NextProduceDescriptor != activeDscr);
        }
    }

    /* Interrupt from the consumer socket. */
    if (sckId == CY_DMA_UIB_SOCKET_CONS_1)
    {
        ConsumeIntrCount++;

        /* Buffer consumed on the USB side. As multiple buffers may have been consumed by the time we
         * got here, we need to make sure that we handle all buffer upto the current buffer. */

        CyFx3BootDmaGetSocketConfig (sckId, &sockConf);
        intStat = sockConf.intr;

        do {
            /* Clear the active interrupts and read the interrupt status again until all interrupts are cleared. */
            CyFx3BootDmaClearSockInterrupts (sckId, sockConf.intr);
            CyFx3BootDmaGetSocketConfig (sckId, &sockConf);
            intStat |= sockConf.intr;
        } while ((sockConf.intr & sockConf.intrMask) != 0);

        if ((intStat & CY_U3P_CONSUME_EVENT) != 0)
        {
            /* The current descriptor is the value of dscrChain at the point when interrupt status has reached 0. */
            activeDscr = (uint16_t)(sockConf.dscrChain);

            do {
                currentDscr = PRODUCE_DESCRIPTOR_BASE + NextConsumeDescriptor - CONSUME_DESCRIPTOR_BASE;

                CyFx3BootDmaGetDscrConfig (NextConsumeDescriptor, &consDscr);
                CyFx3BootDmaGetDscrConfig (currentDscr, &prodDscr);

                /* Make the producer descriptor free. */
                prodDscr.size = DMA_BUF_SIZE;
                CyFx3BootDmaSetDscrConfig (currentDscr, &prodDscr);

                /* Send an event to the producer socket. */
                CyFx3BootDmaSendSocketEvent (CY_DMA_PIB_SOCKET_0, currentDscr, CyFalse);

                /* Move to the next consumer descriptor. */
                NextConsumeDescriptor = (uint16_t)((consDscr.chain & CY_U3P_RD_NEXT_DSCR_MASK) >>
                        CY_U3P_RD_NEXT_DSCR_POS);
            } while (NextConsumeDescriptor != activeDscr);
        }
    }
}

/* Set up the MANUAL DMA from PIB socket 0 to USB CONS Socket 1. This includes using DMA interrupt handlers
 * to update the descriptors and send socket events. */
static int
ManualDmaSetup (
        void)
{
    CyFx3BootErrorCode_t     stat;
    CyFx3BootDmaSocket_t     sockConf;

    CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Manual DMA Setup called\r\n");

    /* Make sure the sockets are disabled at start. */
    CyFx3BootDmaDisableSocket (CY_DMA_PIB_SOCKET_0);
    CyFx3BootDmaDisableSocket (CY_DMA_UIB_SOCKET_CONS_1);

    /* Register interrupt callback for DMA interrupts. We are enabling DMA callbacks for USB and PIB DMA interrupts. */
    CyFx3BootDmaRegisterCallback (DmaInterruptHandler, CyTrue, CyTrue, CyFalse);

    /* Configure the DMA descriptors for both producer and consumer. */
    stat = ConfigureDmaDescriptors (CyTrue);
    if (stat != CY_FX3_BOOT_SUCCESS)
        return stat;

    /* Configure the producer socket on the GPIF side. */
    sockConf.dscrChain = PRODUCE_DESCRIPTOR_BASE;
    sockConf.xferSize  = 0;
    sockConf.xferCount = 0;
    sockConf.status    = CY_U3P_TRUNCATE | CY_U3P_EN_PROD_EVENTS | CY_U3P_SUSP_TRANS;
    sockConf.intr      = 0;
    sockConf.intrMask  = CY_U3P_PRODUCE_EVENT;          /* Enable interrupt on produce event. */
    stat = CyFx3BootDmaSetSocketConfig (CY_DMA_PIB_SOCKET_0, &sockConf);
    if (stat != CY_FX3_BOOT_SUCCESS)
    {
        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "SetSockConfig returned %d\r\n", stat);
        return stat;
    }

    /* Configure the consumer socket on the USB side. */
    sockConf.dscrChain = CONSUME_DESCRIPTOR_BASE;
    sockConf.xferSize  = 0;
    sockConf.xferCount = 0;
    sockConf.status    = CY_U3P_TRUNCATE | CY_U3P_EN_CONS_EVENTS | CY_U3P_SUSP_TRANS;
    sockConf.intr      = 0;
    sockConf.intrMask  = CY_U3P_CONSUME_EVENT;          /* Enable interrupt on consume event. */
    stat = CyFx3BootDmaSetSocketConfig (CY_DMA_UIB_SOCKET_CONS_1, &sockConf);
    if (stat != CY_FX3_BOOT_SUCCESS)
    {
        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "SetSockConfig returned %d\r\n", stat);
        return stat;
    }

    /* Enable the sockets for transfer. */
    CyFx3BootDmaEnableSocket (CY_DMA_PIB_SOCKET_0);
    CyFx3BootDmaEnableSocket (CY_DMA_UIB_SOCKET_CONS_1);
    CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Sockets enabled\r\n");
    return 0;
}

/* Disable the DMA data path. */
static void
ManualDmaTearDown (
        void)
{
    /* We only need to disable the sockets to stop the DMA transfer. */
    CyFx3BootDmaDisableSocket (CY_DMA_PIB_SOCKET_0);
    CyFx3BootDmaDisableSocket (CY_DMA_UIB_SOCKET_CONS_1);
    CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Sockets disabled\r\n");

    /* Unregister the DMA callback. */
    CyFx3BootDmaRegisterCallback (0, CyFalse, CyFalse, CyFalse);
}

#else

/* Set up the AUTO DMA from PIB socket 0 to USB CONS Socket 1. */
static int
AutoDmaSetup (
        void)
{
    CyFx3BootErrorCode_t     stat;
    CyFx3BootDmaSocket_t     sockConf;

    CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Auto DMA Setup called\r\n");

    /* Make sure the sockets are disabled at start. */
    CyFx3BootDmaDisableSocket (CY_DMA_PIB_SOCKET_0);
    CyFx3BootDmaDisableSocket (CY_DMA_UIB_SOCKET_CONS_1);

    stat = ConfigureDmaDescriptors (CyFalse);
    if (stat != CY_FX3_BOOT_SUCCESS)
        return stat;

    /* Configure the producer socket on the GPIF side. */
    sockConf.dscrChain = PRODUCE_DESCRIPTOR_BASE;
    sockConf.xferSize  = 0;
    sockConf.xferCount = 0;
    sockConf.status    = CY_U3P_TRUNCATE | CY_U3P_EN_PROD_EVENTS | CY_U3P_SUSP_TRANS;
    sockConf.intr      = 0;
    sockConf.intrMask  = 0;             /* Not enabling any interrupts. */
    stat = CyFx3BootDmaSetSocketConfig (CY_DMA_PIB_SOCKET_0, &sockConf);
    if (stat != CY_FX3_BOOT_SUCCESS)
    {
        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "SetSockConfig returned %d\r\n", stat);
        return stat;
    }

    /* Configure the consumer socket on the USB side. */
    sockConf.dscrChain = PRODUCE_DESCRIPTOR_BASE;
    sockConf.xferSize  = 0;
    sockConf.xferCount = 0;
    sockConf.status    = CY_U3P_TRUNCATE | CY_U3P_EN_CONS_EVENTS | CY_U3P_SUSP_TRANS;
    sockConf.intr      = 0;
    sockConf.intrMask  = 0;             /* Not enabling any interrupts. */
    stat = CyFx3BootDmaSetSocketConfig (CY_DMA_UIB_SOCKET_CONS_1, &sockConf);
    if (stat != CY_FX3_BOOT_SUCCESS)
    {
        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "SetSockConfig returned %d\r\n", stat);
        return stat;
    }

    /* Enable the sockets for transfer. */
    CyFx3BootDmaEnableSocket (CY_DMA_PIB_SOCKET_0);
    CyFx3BootDmaEnableSocket (CY_DMA_UIB_SOCKET_CONS_1);
    CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Sockets enabled\r\n");
    return 0;
}

/* Disable the DMA data path. */
static void
AutoDmaTearDown (
        void)
{
    /* We only need to disable the sockets to stop the DMA transfer. */
    CyFx3BootDmaDisableSocket (CY_DMA_PIB_SOCKET_0);
    CyFx3BootDmaDisableSocket (CY_DMA_UIB_SOCKET_CONS_1);
    CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Sockets disabled\r\n");
}

#endif

/* Function to handle the GET_STATUS Standard request. */
static int
HandleUsbGetStatus (
        void)
{
    uint16_t status_val = 0;
    CyBool_t epStatus;

    switch (gEP0.bmReqType & USB_RQT_TARGET_MASK)
    {
        case USB_TARGET_DEVICE:
            if (CyFx3BootUsbGetSpeed () == CY_FX3_BOOT_SUPER_SPEED)
            {
                /* Pick up the self-power/bus-power setting from the configuration descriptor. */
                status_val = glUsbDevStatus | ((gpUsbDescPtr->usbSSConfigDesc_p[7] & 0x40) ? 1 : 0);
            }
            else
            {
                /* Pick up the self-power/bus-power setting from the configuration descriptor. */
                status_val = glUsbDevStatus | ((gpUsbDescPtr->usbHSConfigDesc_p[7] & 0x40) ? 1 : 0);
            }
            break;

        case USB_TARGET_INTERFACE:
            if (!glUsbConfig)
            {
                /* GET_STATUS on INTERFACE should be failed if received before SET_CONFIG. */
                return eStall;
            }
            break;

        case USB_TARGET_ENDPOINT:
            /* Check status of endpoint. */
            if (CyFx3BootUsbGetEpCfg (gEP0.bIdx0, 0, &epStatus) != 0)
            {
                return eStall;
            }
            if (epStatus)
                status_val = 1;
            break;

        default:
            return eStall;
    }

    gEP0.pData[0] = GET_LSB(status_val);
    gEP0.pData[1] = GET_MSB(status_val);

    return eDataIn;
}

/* Function to handle the GET_DESCRIPTOR Standard request */
static int
HandleUsbGetDescriptor (
        void)
{
    uint32_t  len = 0;
    uint8_t  *p = 0;
    uint8_t  *cfg_p      = 0;
    uint8_t  *dev_p      = 0;
    uint8_t  *othercfg_p = 0;
    uint8_t   usbSpeed;

    usbSpeed = CyFx3BootUsbGetSpeed ();
    switch (usbSpeed)
    {
        case CY_FX3_BOOT_HIGH_SPEED:
            dev_p      = (uint8_t*)gpUsbDescPtr->usbDevDesc_p;
            cfg_p      = (uint8_t*)gpUsbDescPtr->usbHSConfigDesc_p;
            othercfg_p = (uint8_t *)gpUsbDescPtr->usbFSConfigDesc_p;

            gpUsbDescPtr->usbHSConfigDesc_p[1] = CY_U3P_USB_CONFIG_DESCR;
            gpUsbDescPtr->usbFSConfigDesc_p[1] = CY_U3P_USB_OTHERSPEED_DESCR;
            break;

        case CY_FX3_BOOT_FULL_SPEED:
            dev_p      = (uint8_t*)gpUsbDescPtr->usbDevDesc_p;
            cfg_p      = (uint8_t*)gpUsbDescPtr->usbFSConfigDesc_p;
            othercfg_p = (uint8_t *)gpUsbDescPtr->usbHSConfigDesc_p;

            gpUsbDescPtr->usbHSConfigDesc_p[1] = CY_U3P_USB_OTHERSPEED_DESCR;
            gpUsbDescPtr->usbFSConfigDesc_p[1] = CY_U3P_USB_CONFIG_DESCR;
            break;

        case CY_FX3_BOOT_SUPER_SPEED:
            dev_p = (uint8_t*)gpUsbDescPtr->usbSSDevDesc_p;
            cfg_p = (uint8_t*)gpUsbDescPtr->usbSSConfigDesc_p;
            break;

        default:
            return eStall;
    }

    switch (gEP0.bVal1)
    {
        case CY_U3P_USB_DEVICE_DESCR:
            {
                p   = dev_p;
                len = p[0];
            }
            break;

        case CY_U3P_BOS_DESCR:
            {
                p   = (uint8_t *)gpUsbDescPtr->usbSSBOSDesc_p;
                len = MAKE_WORD(p[3], p[2]);
            }
            break;

        case CY_U3P_USB_CONFIG_DESCR:
            {
                p   = cfg_p;
                len = MAKE_WORD(p[3], p[2]);
            }
            break;

        case CY_U3P_USB_DEVQUAL_DESCR:
            {
                /* Not supported in Super-Speed. */
                if (usbSpeed == CY_FX3_BOOT_SUPER_SPEED)
                        return eStall;

                p   = (uint8_t*)gpUsbDescPtr->usbDevQualDesc_p;
                len = p[0];
            }
            break;

        case CY_U3P_USB_STRING_DESCR:
            {
                /* Ensure that we do not index past the limit of the array. */
                if (gEP0.bVal0 < CY_FX3_USB_MAX_STRING_DESC_INDEX)
                {
                    p = (uint8_t*)gpUsbDescPtr->usbStringDesc_p[gEP0.bVal0];
                    if (p == 0)
                        return eStall;

                    len = p[0];
                }
                else
                    return eStall;
            }
            break;

        case CY_U3P_USB_OTHERSPEED_DESCR:
            {
                if (othercfg_p == 0)
                    return eStall;
                p   = othercfg_p;
                len = MAKE_WORD(p[3], p[2]);
            }
            break;

        default:
            return eStall;
    }

    if (p != 0)
    {
        CyFx3BootMemCopy (gpUSBData, p, len);
        if (gEP0.wLen > len)
        {
            gEP0.wLen = len;
        }

        return eDataIn;
    }

    /* Stall EP0 if the descriptor sought is not available. */
    return eStall;
}

static CyFx3BootErrorCode_t
ApplnStartStop (
        CyBool_t isStart)
{
    CyFx3BootUsbEpConfig_t epCfg;
    uint8_t  usbSpeed = 0;
    uint32_t retVal  = 0;

    /* Get the Bus speed */
    usbSpeed = CyFx3BootUsbGetSpeed ();
    switch (usbSpeed)
    {
        case CY_FX3_BOOT_SUPER_SPEED:
            epCfg.pcktSize = 1024;
            break;
        case CY_FX3_BOOT_FULL_SPEED:
            epCfg.pcktSize = 64;
            break;
        default:
            epCfg.pcktSize = 512;
            break;
    }

    /* Configure the endpoint. */
    epCfg.enable   = isStart;
    epCfg.epType   = CY_FX3_BOOT_USB_EP_BULK;
    epCfg.burstLen = 1;
    epCfg.streams  = 0;
    epCfg.isoPkts  = 0;
    retVal = CyFx3BootUsbSetEpConfig (DATA_ENDPOINT, &epCfg);
    if (retVal != 0)
    {
        return retVal;
    }

    if (isStart)
    {
        /* Flag the main loop to start the GPIF interface. */
        glGpifDoInit = CyTrue;
    }
    else
    {
        /* Stop the GPIF state machine if it is already running. */
        if (glGpifStarted)
        {
#ifdef MANUAL_DMA
            ManualDmaTearDown ();
#else
            AutoDmaTearDown ();
#endif
            GpifDeinit ();
            glGpifStarted = CyFalse;
        }
    }

    return CY_FX3_BOOT_SUCCESS;
}

/* Function to handle the SET_CONFIG Standard request */
static int
HandleUsbSetConfig (
        void)
{
    CyFx3BootErrorCode_t retVal = CY_FX3_BOOT_SUCCESS;

    /* Disable the low power mode to optimize throughput */
    CyFx3BootUsbLPMDisable();

    /* Make sure the configuration is valid. */
    if (gEP0.bVal0 > USB_MAX_CONFIG)
        return eStall;

    glUsbState  = gEP0.bVal0;
    glUsbConfig = gEP0.bVal0;

    retVal = ApplnStartStop ((CyBool_t)glUsbConfig);
    if (retVal != 0)
    {
        return eStall;
    }

    return eStatus;
}

/* Function to handle the GET_INTERFACE Standard request */
static int
HandleUsbGetInterface (
        void)
{
    if (glUsbConfig == 0)
    {
        return eStall;
    }

    gEP0.pData = (uint8_t *)&glAltSetting;
    return eDataIn;
}

/* Function to handle the SET_INTERFACE Standard request */
static int
HandleUsbSetInterface (
        void)
{
    glAltSetting = gEP0.bVal0;
    return eStatus;
}

/* Function to handle the CLEAR_FEATURE Standard request. */
static int
HandleUsbClearFeature (
        void
        )
{
    /* All of the actual handling for the CLEAR_FEATURE request is done in the API.
       We only need to update the device status flags here.
     */
    if (CyFx3BootUsbSetClrFeature (0, (CyBool_t)glUsbState, &gEP0) != 0)
    {
        return eStall;
    }

    if ((gEP0.bmReqType & USB_RQT_TARGET_MASK) == USB_TARGET_DEVICE)
    {
        /* Update the device status flags as required. */
        if (CyFx3BootUsbGetSpeed () == CY_FX3_BOOT_SUPER_SPEED)
        {
            switch (gEP0.bVal0)
            {
            case USB_FS_U1_ENABLE:
                glUsbDevStatus &= ~STAT_U1_ENABLE;
                break;
            case USB_FS_U2_ENABLE:
                glUsbDevStatus &= ~STAT_U2_ENABLE;
                break;
            case USB_FS_LTM_ENABLE:
                glUsbDevStatus &= ~STAT_LTM_ENABLE;
                break;
            default:
                return eStall;
            }
        }
        else
        {
            if (gEP0.bVal0 == USB_FS_REMOTE_WAKE)
            {
                glUsbDevStatus &= ~STAT_REMOTE_WAKEUP;
            }
            else
                return eStall;
        }
    }

    return eStatus;
}

/* Function to handle the SET_FEATURE Standard request. */
static int
HandleUsbSetFeature (
        void
        )
{
    /* All of the actual handling for the SET_FEATURE command is done in the API.
       We only need to update the device status flags here.
     */
    if (CyFx3BootUsbSetClrFeature (1, (CyBool_t)glUsbState, &gEP0) != 0)
    {
        return eStall;
    }

    if ((gEP0.bmReqType & USB_RQT_TARGET_MASK) == USB_TARGET_DEVICE)
    {
        /* Update the device status flags as required. */
        if (CyFx3BootUsbGetSpeed () == CY_FX3_BOOT_SUPER_SPEED)
        {
            switch (gEP0.bVal0)
            {
            case USB_FS_U1_ENABLE:
                glUsbDevStatus |= STAT_U1_ENABLE;
                break;
            case USB_FS_U2_ENABLE:
                glUsbDevStatus |= STAT_U2_ENABLE;
                break;
            case USB_FS_LTM_ENABLE:
                glUsbDevStatus |= STAT_LTM_ENABLE;
                break;
            default:
                return eStall;
            }
        }
        else
        {
            if (gEP0.bVal0 == USB_FS_REMOTE_WAKE)
            {
                glUsbDevStatus |= STAT_REMOTE_WAKEUP;
            }
            else
                return eStall;
        }
    }

    return eStatus;
}

/* Function to handle the GET_CONFIG Standard request. */
static int
HandleUsbGetConfig (
        void)
{
    gEP0.pData = (uint8_t *)&glUsbConfig;
    return eDataIn;
}

static int
HandleUsbSetSel (
        void)
{
    if ((CyFx3BootUsbGetSpeed () == CY_FX3_BOOT_SUPER_SPEED) && (gEP0.bIdx0 == 0) &&
            (gEP0.bIdx1 == 0) && (gEP0.bVal0 == 0) && (gEP0.bVal1 == 0) && (gEP0.wLen == 6))
    {
        /* For now, we are just receiving and discarding the data. If the application wants to use
           the SEL data; the code can be updated to do that. */
        gEP0.wLen = 32;
        return eDataOut;
    }

    return eStall;
}

static int
HandleUsbSetIsocDelay (
        void)
{
    if ((CyFx3BootUsbGetSpeed () != CY_FX3_BOOT_SUPER_SPEED) || (gEP0.bIdx0 != 0) ||
            (gEP0.bIdx1 != 0) || (gEP0.wLen != 0))
    {
        return eStall;
    }

    return eStatus;
}

static int
HandleUsbVendorCmd (
        void)
{
    /* We use vendor command 0xE0 to reset the FX3 device. */
    if (gEP0.bReq == 0xE0)
    {
        CyFx3BootUsbAckSetup ();
        CyFx3BootUsbConnect (CyFalse, CyTrue);
        CyFx3BootDeviceReset ();
    }
    else
        return eStall;

    return eStatus;
}

/* Setup Data handler */
static void
HandleUsbSetupCommand (
        uint32_t setupDat0,
        uint32_t setupDat1)
{
    uint32_t *p;
    int       stage = eStall;
    CyFx3BootErrorCode_t status;

    /* Copy the request into the gEP0 structure. */
    p = (uint32_t*)&gEP0;
    p[0] = setupDat0;
    p[1] = setupDat1;
    gEP0.pData = gpUSBData;

    switch (gEP0.bmReqType & USB_RQT_TYPE_MASK)
    {
        case USB_STANDARD_REQUEST:
            switch (gEP0.bReq)
            {
                case USB_SC_GET_STATUS:
                    stage = HandleUsbGetStatus ();
                    break;

                case USB_SC_CLEAR_FEATURE:
                    stage = HandleUsbClearFeature ();
                    break;

                case USB_SC_SET_FEATURE:
                    stage = HandleUsbSetFeature ();
                    break;

                case USB_SC_GET_DESCRIPTOR:
                    stage = HandleUsbGetDescriptor ();
                    break;

                case USB_SC_GET_CONFIGURATION:
                    stage = HandleUsbGetConfig ();
                    break;

                case USB_SC_SET_CONFIGURATION:
                    stage = HandleUsbSetConfig ();
                    break;

                case USB_SC_GET_INTERFACE:
                    stage = HandleUsbGetInterface ();
                    break;

                case USB_SC_SET_INTERFACE:
                    stage = HandleUsbSetInterface ();
                    break;

                case USB_SC_SET_SEL:
                    stage = HandleUsbSetSel ();
                    break;

                case USB_SC_SET_ISOC_DELAY:
                    stage = HandleUsbSetIsocDelay ();
                    break;

                default:
                    stage = eStall;
                    break;
            }
            break;

        case USB_VENDOR_REQUEST:
            stage = HandleUsbVendorCmd ();
            break;

        default:
            stage = eStall;
            break;
    }

    switch (stage)
    {
        case eDataIn:
            CyFx3BootUsbAckSetup ();
            status = CyFx3BootUsbDmaXferData (USB_CTRL_IN_EP, (uint32_t)gEP0.pData, gEP0.wLen,
                    CY_FX3_BOOT_WAIT_FOREVER);
            if (status != CY_FX3_BOOT_SUCCESS)
            {
                CyFx3BootUsbStall (USB_CTRL_IN_EP, CyTrue, CyFalse);
            }
            break;

        case eDataOut:
            /* When receiving data, don't stall the endpoint on error. */
            CyFx3BootUsbAckSetup ();
            CyFx3BootUsbDmaXferData (USB_CTRL_OUT_EP, (uint32_t)gEP0.pData, gEP0.wLen,
                    CY_FX3_BOOT_WAIT_FOREVER);
            break;

        case eStatus:
            CyFx3BootUsbAckSetup ();
            break;

        default:
            CyFx3BootUsbStall (USB_CTRL_OUT_EP, CyTrue, CyFalse);
            break;
    }
}


/* USB Event Handler. This function is invoked from the USB ISR and as such MUST not be
   blocked.
*/
static void
UsbEventCallback (
        CyFx3BootUsbEventType_t event)
{
    switch (event)
    {
        case CY_FX3_BOOT_USB_RESET:
        case CY_FX3_BOOT_USB_CONNECT:
        case CY_FX3_BOOT_USB_DISCONNECT:
            glUsbConfig    = 0;
            glAltSetting   = 0;
            glUsbDevStatus = 0;
            glUsbState     = 0;
            glInCompliance = 0;

            /* Stop the application. */
            ApplnStartStop (CyFalse);
            break;

        case CY_FX3_BOOT_USB_IN_SS_DISCONNECT:
            glCheckForDisconnect = CyTrue;
            break;

        case CY_FX3_BOOT_USB_COMPLIANCE:
            glInCompliance = CyTrue;
            break;

        default:
            break;
    }
}

static void
UsbInit (
        void)
{
    CyFx3BootErrorCode_t apiRetStatus;

    glUsbConfig          = 0;
    glAltSetting         = 0;
    glUsbDevStatus       = 0;
    glUsbState           = 0;
    glCheckForDisconnect = 0;
    glInCompliance       = 0;

    /* Enable this code for using the USB Bootloader */
    apiRetStatus = CyFx3BootUsbStart (CyFalse, UsbEventCallback);
    if (apiRetStatus != CY_FX3_BOOT_SUCCESS)
    {
        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "UsbStart failed with code %d\r\n",
                apiRetStatus);
        CyFx3BootDeviceReset ();
    }

    CyFx3BootRegisterSetupCallback (HandleUsbSetupCommand);

    /* Register the USB descriptors with the API. */
    CyFx3BootUsbSetDesc (CY_U3P_USB_SET_SS_DEVICE_DESCR, 0, (uint8_t *)gbSsDevDesc);
    CyFx3BootUsbSetDesc (CY_U3P_USB_SET_FS_CONFIG_DESCR, 0, (uint8_t *)gbFsConfigDesc);
    CyFx3BootUsbSetDesc (CY_U3P_USB_SET_SS_CONFIG_DESCR, 0, (uint8_t *)gbSsConfigDesc);
    CyFx3BootUsbSetDesc (CY_U3P_USB_SET_SS_BOS_DESCR, 0, (uint8_t *)gbBosDesc);

    CyFx3BootUsbSetDesc (CY_U3P_USB_SET_HS_DEVICE_DESCR, 0, (uint8_t *)gbDevDesc);
    CyFx3BootUsbSetDesc (CY_U3P_USB_SET_DEVQUAL_DESCR, 0, (uint8_t *)gbDevQualDesc);
    CyFx3BootUsbSetDesc (CY_U3P_USB_SET_HS_CONFIG_DESCR, 0, (uint8_t *)gbCfgDesc);

    CyFx3BootUsbSetDesc (CY_U3P_USB_SET_STRING_DESCR, 0, (uint8_t *)gbLangIDDesc);
    CyFx3BootUsbSetDesc (CY_U3P_USB_SET_STRING_DESCR, 1, (uint8_t *)gbManufactureDesc);
    CyFx3BootUsbSetDesc (CY_U3P_USB_SET_STRING_DESCR, 2, (uint8_t *)gbProductDesc);

    gpUsbDescPtr = CyFx3BootUsbGetDesc ();

    /* Enable USB connection. */
    CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Enabling USB connection\r\n");
    CyFx3BootUsbConnect (CyTrue, CyTrue);
    CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "USB connect done\r\n");
}

/****************************************************************************
 * main:
 ****************************************************************************/
int
main (
        void)
{
    CyFx3BootIoMatrixConfig_t ioCfg;
    CyFx3BootUartConfig_t     uartCfg;
    CyFx3BootErrorCode_t      status;

#ifdef MANUAL_DMA
    uint16_t                  loopCnt = 0;
#endif

    /* HW and SW initialization code  */
    CyFx3BootDeviceInit (CyTrue);

    ioCfg.isDQ32Bit = CyTrue;
    ioCfg.useUart   = CyTrue;
    ioCfg.useI2C    = CyFalse;
    ioCfg.useI2S    = CyFalse;
    ioCfg.useSpi    = CyFalse;
    ioCfg.gpioSimpleEn[0] = 0;
    ioCfg.gpioSimpleEn[1] = 0;

    status = CyFx3BootDeviceConfigureIOMatrix (&ioCfg);
    if (status != CY_FX3_BOOT_SUCCESS)
    {
        return status;
    }

    /* Configure the UART for logging. */
    uartCfg.txEnable = CyTrue;
    uartCfg.rxEnable = CyFalse;
    uartCfg.flowCtrl = CyFalse;
    uartCfg.isDma    = CyFalse;
    uartCfg.baudRate = CY_FX3_BOOT_UART_BAUDRATE_115200;
    uartCfg.stopBit  = CY_FX3_BOOT_UART_ONE_STOP_BIT;
    uartCfg.parity   = CY_FX3_BOOT_UART_NO_PARITY;

    /* Test the debug print capability. */
    {
        int neg = -75, pos = 28;
        uint32_t val = 0xFEDCBA98;
        uint8_t  letter = 'h';
        uint8_t  tstr[] = "Test String";

        if (CyFx3BootUartInit () != CY_FX3_BOOT_SUCCESS)
            CyFx3BootDeviceReset ();

        if (CyFx3BootUartSetConfig (&uartCfg) != CY_FX3_BOOT_SUCCESS)
            CyFx3BootDeviceReset ();

        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Hello world!\r\n");
        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Testing string: %s\r\n", tstr);
        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Testing unsigned: %c %d %d %u %x\r\n",
                letter, letter, val, val, val);
        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Testing signed: %d %d\r\n", neg, pos);
    }

    /* Initialize the USB module. */
    UsbInit ();

    /* Keep handling any events that come out of the USB or GPIF blocks. */
    while (1)
    {
        if (glGpifDoInit)
        {
            /* If GPIF is already running, stop and then re-start it. */
            glGpifDoInit = CyFalse;
            if (glGpifStarted)
            {
                /* Disable the DMA and GPIF before starting it again. */
#ifdef MANUAL_DMA
                ManualDmaTearDown ();
#else
                AutoDmaTearDown ();
#endif
                GpifDeinit ();
            }
            if (GpifInit () == 0)
            {
#ifdef MANUAL_DMA
                ManualDmaSetup ();
#else
                AutoDmaSetup ();
#endif
                glGpifStarted = CyTrue;
            }
        }

        CyFx3BootUsbHandleEvents ();

#ifdef MANUAL_DMA
        /* Print a progress message once in 65536 loop iterations. */
        if (loopCnt++ == 0)
            CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Int count: %d %d\r\n", ProduceIntrCount,
                    ConsumeIntrCount);
#endif
    }

    return 0;
}

/*[]*/

