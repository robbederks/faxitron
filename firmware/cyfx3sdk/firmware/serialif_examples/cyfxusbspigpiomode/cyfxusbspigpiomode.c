/*
## Cypress USB 3.0 Platform source file (cyfxusbspigpiomode.c)
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

/* This application is used read / write to SPI flash devices from USB.
   The FX3 accesses by using a set of GPIO lines configured as Clock, Slave
   select, MISO and MOSI lines.
   The firmware enumerates as a custom device communicating with the
   cyUsb3.sys driver and provides a set of vendor commands that can be
   used to access the attached SPI flash device.
 */

#include "cyu3system.h"
#include "cyu3os.h"
#include "cyu3dma.h"
#include "cyu3error.h"
#include "cyu3usb.h"
#include "cyu3gpio.h"
#include "cyu3uart.h"
#include "cyu3utils.h"
#include "cyfxusbspigpiomode.h"
#include "gpio_regs.h"

CyU3PThread appThread;                  /* Application thread object. */
CyBool_t glIsApplnActive = CyFalse;     /* Application status. */

/* Firmware ID variable that may be used to verify SPI firmware. */
const uint8_t glFirmwareID[32] __attribute__ ((aligned (32))) = { 'F', 'X', '3', ' ', 'S', 'P', 'I', '\0' };

uint8_t glEp0Buffer[4096] __attribute__ ((aligned (32)));

uint16_t glSpiPageSize = 0x100;  /* SPI Page size to be used for transfers. */

/* Application error handler. */
void
CyFxAppErrorHandler (
        CyU3PReturnStatus_t apiRetStatus    /* API return status */
        )
{
    /* Application failed with the error code apiRetStatus */

    /* Add custom debug or recovery actions here */

    /* Loop indefinitely */
    for (;;)
    {
        /* Thread sleep : 100 ms */
        CyU3PThreadSleep (100);
    }
}

/* Initialize the debug module with UART. */
CyU3PReturnStatus_t
CyFxDebugInit (
        void)
{
    CyU3PUartConfig_t uartConfig;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    /* Initialize and configure the UART for logging. */
    status = CyU3PUartInit ();
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    CyU3PMemSet ((uint8_t *)&uartConfig, 0, sizeof (uartConfig));
    uartConfig.baudRate = CY_U3P_UART_BAUDRATE_115200;
    uartConfig.stopBit  = CY_U3P_UART_ONE_STOP_BIT;
    uartConfig.parity   = CY_U3P_UART_NO_PARITY;
    uartConfig.txEnable = CyTrue;
    uartConfig.rxEnable = CyFalse;
    uartConfig.flowCtrl = CyFalse;
    uartConfig.isDma    = CyTrue;
    status = CyU3PUartSetConfig (&uartConfig, NULL);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* Set the dma for an inifinity transfer */
    status = CyU3PUartTxSetBlockXfer (0xFFFFFFFF);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* Start the debug module for printing log messages. */
    status = CyU3PDebugInit (CY_U3P_LPP_SOCKET_UART_CONS, 8);

    return status;
}

/* This function intializes the GPIO module. GPIO Ids 53-56 are used for communicating
   with the SPI slave device. */
CyU3PReturnStatus_t
CyFxGpioInit (void)
{
    CyU3PGpioClock_t gpioClock;
    CyU3PGpioSimpleConfig_t gpioConfig;
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

    /* Initialize the GPIO module. */
    gpioClock.fastClkDiv = 2;
    gpioClock.slowClkDiv = 0;
    gpioClock.simpleDiv  = CY_U3P_GPIO_SIMPLE_DIV_BY_2;
    gpioClock.clkSrc     = CY_U3P_SYS_CLK;
    gpioClock.halfDiv    = 0;

    apiRetStatus = CyU3PGpioInit(&gpioClock, NULL);
    if (apiRetStatus != 0)
    {
        /* Error Handling */
        CyU3PDebugPrint (4, "CyU3PGpioInit failed, error code = %d\n", apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Configure GPIO 53 as output(SPI_CLOCK). */
    gpioConfig.outValue    = CyFalse;
    gpioConfig.inputEn     = CyFalse;
    gpioConfig.driveLowEn  = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.intrMode    = CY_U3P_GPIO_NO_INTR;

    apiRetStatus = CyU3PGpioSetSimpleConfig(FX3_SPI_CLK, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error handling */
        CyU3PDebugPrint (4, "CyU3PGpioSetSimpleConfig for GPIO Id %d failed, error code = %d\n",
                FX3_SPI_CLK, apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Configure GPIO 54 as output(SPI_SSN) */
    gpioConfig.outValue    = CyTrue;
    gpioConfig.inputEn     = CyFalse;
    gpioConfig.driveLowEn  = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.intrMode    = CY_U3P_GPIO_NO_INTR;

    apiRetStatus = CyU3PGpioSetSimpleConfig(FX3_SPI_SS, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error handling */
        CyU3PDebugPrint (4, "CyU3PGpioSetSimpleConfig for GPIO Id %d failed, error code = %d\n",
                FX3_SPI_SS, apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Configure GPIO 55 as input(MISO) */
    gpioConfig.outValue    = CyFalse;
    gpioConfig.inputEn     = CyTrue;
    gpioConfig.driveLowEn  = CyFalse;
    gpioConfig.driveHighEn = CyFalse;
    gpioConfig.intrMode    = CY_U3P_GPIO_NO_INTR;

    apiRetStatus = CyU3PGpioSetSimpleConfig(FX3_SPI_MISO, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error handling */
        CyU3PDebugPrint (4, "CyU3PGpioSetSimpleConfig for GPIO Id %d failed, error code = %d\n",
                FX3_SPI_MISO, apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Configure GPIO 56 as output(MOSI) */
    gpioConfig.outValue    = CyFalse;
    gpioConfig.inputEn     = CyFalse;
    gpioConfig.driveLowEn  = CyTrue;
    gpioConfig.driveHighEn = CyTrue;
    gpioConfig.intrMode    = CY_U3P_GPIO_NO_INTR;

    apiRetStatus = CyU3PGpioSetSimpleConfig(FX3_SPI_MOSI, &gpioConfig);
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error handling */
        CyU3PDebugPrint (4, "CyU3PGpioSetSimpleConfig for GPIO Id %d failed, error code = %d\n",
                FX3_SPI_MOSI, apiRetStatus);
        CyFxAppErrorHandler(apiRetStatus);
    }

    return apiRetStatus;
}

/* This function pulls up/down the SPI Clock line. */
CyU3PReturnStatus_t
CyFxSpiSetClockValue (
        CyBool_t isHigh        /* Cyfalse: Pull down the Clock line,
                                  CyTrue: Pull up the Clock line */
        )
{
    CyU3PReturnStatus_t status;

    status = CyU3PGpioSetValue (FX3_SPI_CLK, isHigh);

    return status;
}

/* This function pulls up/down the slave select line. */
CyU3PReturnStatus_t
CyFxSpiSetSsnLine (
        CyBool_t isHigh        /* Cyfalse: Pull down the SSN line,
                                  CyTrue: Pull up the SSN line */
        )
{
#ifndef FX3_USE_GPIO_REGS
    CyU3PReturnStatus_t status;

    status = CyU3PGpioSetValue (FX3_SPI_SS, isHigh);

    return status;
#else
    uvint32_t *regPtrSS;
    regPtrSS = &GPIO->lpp_gpio_simple[FX3_SPI_SS];
    if(isHigh)
    {
        *regPtrSS |=CYFX_GPIO_HIGH;
    }
    else
    {
        *regPtrSS&=~CYFX_GPIO_HIGH;
    }    

    return CY_U3P_SUCCESS;
#endif
}

/* This function transmits the byte to the SPI slave device one bit a time.
   Most Significant Bit is transmitted first.
 */
CyU3PReturnStatus_t
CyFxSpiWriteByte (
        uint8_t data)
{
    uint8_t i = 0;
#ifdef FX3_USE_GPIO_REGS
    CyBool_t value;
    uvint32_t *regPtrMOSI, *regPtrClock;
	regPtrMOSI = &GPIO->lpp_gpio_simple[FX3_SPI_MOSI];
	regPtrClock = &GPIO->lpp_gpio_simple[FX3_SPI_CLK];
#endif
    for (i = 0; i < 8; i++)
    {
#ifndef FX3_USE_GPIO_REGS
        /* Most significant bit is transferred first. */
        CyU3PGpioSetValue (FX3_SPI_MOSI, ((data >> (7 - i)) & 0x01));

        CyFxSpiSetClockValue (CyTrue);
        CyU3PBusyWait (1);
        CyFxSpiSetClockValue (CyFalse);
        CyU3PBusyWait (1);
#else
        /* Most significant bit is transferred first. */        
		value =((data >> (7 - i)) & 0x01);
		if(value)
		{
			*regPtrMOSI |=	CYFX_GPIO_HIGH;
		}
		else
		{
			*regPtrMOSI &=~CYFX_GPIO_HIGH;
		}
		*regPtrClock|=CYFX_GPIO_HIGH;		
        CyU3PBusyWait (1);
        *regPtrClock&=~CYFX_GPIO_HIGH;
        CyU3PBusyWait (1);
#endif
    }

    return CY_U3P_SUCCESS;
}

/* This function receives the byte from the SPI slave device one bit at a time.
   Most Significant Bit is received first.
 */
CyU3PReturnStatus_t
CyFxSpiReadByte (
        uint8_t *data)
{
    uint8_t i = 0;
    CyBool_t temp = CyFalse;

#ifdef FX3_USE_GPIO_REGS
    uvint32_t *regPtrClock;
	regPtrClock = &GPIO->lpp_gpio_simple[FX3_SPI_CLK];
#endif
    *data = 0;

    for (i = 0; i < 8; i++)
    {
#ifndef FX3_USE_GPIO_REGS
        CyFxSpiSetClockValue (CyTrue);

        CyU3PGpioGetValue (FX3_SPI_MISO, &temp);
        *data |= (temp << (7 - i));

        CyU3PBusyWait (1);

        CyFxSpiSetClockValue (CyFalse);
        CyU3PBusyWait (1);
#else
        *regPtrClock|=CYFX_GPIO_HIGH;        
		temp = (GPIO->lpp_gpio_simple[FX3_SPI_MISO] & CY_U3P_LPP_GPIO_IN_VALUE)>>1;
        *data |= (temp << (7 - i));
        CyU3PBusyWait (1);
        *regPtrClock&=~CYFX_GPIO_HIGH;	
        CyU3PBusyWait (1);
#endif
    }

    return CY_U3P_SUCCESS;
}

/* This function is used to transmit data to the SPI slave device. The function internally
   calls the CyFxSpiWriteByte function to write to the slave device.
 */
CyU3PReturnStatus_t
CyFxSpiTransmitWords (
        uint8_t *data,
        uint32_t byteCount)
{
    uint32_t i = 0;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    if ((!byteCount) || (!data))
    {
        return CY_U3P_ERROR_BAD_ARGUMENT;
    }

    for (i = 0; i < byteCount; i++)
    {
        status = CyFxSpiWriteByte (data[i]);

        if (status != CY_U3P_SUCCESS)
        {
            break;
        }
    }

    return status;
}

/* This function is used receive data from the SPI slave device. The function internally
   calls the CyFxSpiReadByte function to read data from the slave device.
 */
CyU3PReturnStatus_t
CyFxSpiReceiveWords (
        uint8_t *data,
        uint32_t byteCount)
{
    uint32_t i = 0;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    if ((!byteCount) || (!data))
    {
        return CY_U3P_ERROR_BAD_ARGUMENT;
    }

    for (i = 0; i < byteCount; i++)
    {
        status = CyFxSpiReadByte (&data[i]);

        if (status != CY_U3P_SUCCESS)
        {
            break;
        }
    }

    return status;
}

/* Wait for the status response from the SPI flash. */
CyU3PReturnStatus_t
CyFxSpiWaitForStatus (
        void)
{
    uint8_t buf[2], rd_buf[2];
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    /* Wait for status response from SPI flash device. */
    do
    {
        buf[0] = 0x06;  /* Write enable command. */

        CyFxSpiSetSsnLine (CyFalse);
        status = CyFxSpiTransmitWords (buf, 1);
        CyFxSpiSetSsnLine (CyTrue);
        if (status != CY_U3P_SUCCESS)
        {
            CyU3PDebugPrint (2, "SPI WR_ENABLE command failed\n\r");
            return status;
        }

        buf[0] = 0x05;  /* Read status command */

        CyFxSpiSetSsnLine (CyFalse);
        status = CyFxSpiTransmitWords (buf, 1);
        if (status != CY_U3P_SUCCESS)
        {
            CyU3PDebugPrint (2, "SPI READ_STATUS command failed\n\r");
            CyFxSpiSetSsnLine (CyTrue);
            return status;
        }

        status = CyFxSpiReceiveWords (rd_buf, 2);
        CyFxSpiSetSsnLine (CyTrue);
        if(status != CY_U3P_SUCCESS)
        {
            CyU3PDebugPrint (2, "SPI status read failed\n\r");
            return status;
        }

    } while ((rd_buf[0] & 1)|| (!(rd_buf[0] & 0x2)));

    return CY_U3P_SUCCESS;
}

/* SPI read / write for programmer application. */
CyU3PReturnStatus_t
CyFxSpiTransfer (
        uint16_t  pageAddress,
        uint16_t  byteCount,
        uint8_t  *buffer,
        CyBool_t  isRead)
{
    uint8_t location[4];
    uint32_t byteAddress = 0;
    uint16_t pageCount = (byteCount / glSpiPageSize);
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    if (byteCount == 0)
    {
        return CY_U3P_SUCCESS;
    }
    if ((byteCount % glSpiPageSize) != 0)
    {
        pageCount ++;
    }

    byteAddress  = pageAddress * glSpiPageSize;
    CyU3PDebugPrint (2, "SPI access - addr: 0x%x, size: 0x%x, pages: 0x%x.\r\n",
            byteAddress, byteCount, pageCount);

    while (pageCount != 0)
    {
        location[1] = (byteAddress >> 16) & 0xFF;       /* MS byte */
        location[2] = (byteAddress >> 8) & 0xFF;
        location[3] = byteAddress & 0xFF;               /* LS byte */

        if (isRead)
        {
            location[0] = 0x03; /* Read command. */

            status = CyFxSpiWaitForStatus ();
            if (status != CY_U3P_SUCCESS)
                return status;

            CyFxSpiSetSsnLine (CyFalse);
            status = CyFxSpiTransmitWords (location, 4);
            if (status != CY_U3P_SUCCESS)
            {
                CyU3PDebugPrint (2, "SPI READ command failed\r\n");
                CyFxSpiSetSsnLine (CyTrue);
                return status;
            }

            status = CyFxSpiReceiveWords (buffer, glSpiPageSize);
            if (status != CY_U3P_SUCCESS)
            {
                CyFxSpiSetSsnLine (CyTrue);
                return status;
            }

            CyFxSpiSetSsnLine (CyTrue);
        }
        else /* Write */
        {
            location[0] = 0x02; /* Write command */

            status = CyFxSpiWaitForStatus ();
            if (status != CY_U3P_SUCCESS)
                return status;

            CyFxSpiSetSsnLine (CyFalse);
            status = CyFxSpiTransmitWords (location, 4);
            if (status != CY_U3P_SUCCESS)
            {
                CyU3PDebugPrint (2, "SPI WRITE command failed\r\n");
                CyFxSpiSetSsnLine (CyTrue);
                return status;
            }

            status = CyFxSpiTransmitWords (buffer, glSpiPageSize);
            if (status != CY_U3P_SUCCESS)
            {
                CyFxSpiSetSsnLine (CyTrue);
                return status;
            }

            CyFxSpiSetSsnLine (CyTrue);
        }

        /* Update the parameters */
        byteAddress  += glSpiPageSize;
        buffer += glSpiPageSize;
        pageCount --;

        CyU3PThreadSleep (10);
    }
    return CY_U3P_SUCCESS;
}

/* Function to erase SPI flash sectors. */
static CyU3PReturnStatus_t
CyFxSpiEraseSector (
     CyBool_t  isErase,
     uint8_t   sector,
     uint8_t  *wip)
{
    uint32_t temp = 0;
    uint8_t  location[4], rdBuf[2];
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    if ((!isErase) && (wip == NULL))
    {
        return CY_U3P_ERROR_BAD_ARGUMENT;
    }

    location[0] = 0x06;  /* Write enable. */

    CyFxSpiSetSsnLine (CyFalse);
    status = CyFxSpiTransmitWords (location, 1);
    CyFxSpiSetSsnLine (CyTrue);
    if (status != CY_U3P_SUCCESS)
        return status;

    if (isErase)
    {
        location[0] = 0xD8; /* Sector erase. */
        temp        = sector * 0x10000;
        location[1] = (temp >> 16) & 0xFF;
        location[2] = (temp >> 8) & 0xFF;
        location[3] = temp & 0xFF;

        CyFxSpiSetSsnLine (CyFalse);
        status = CyFxSpiTransmitWords (location, 4);
        CyFxSpiSetSsnLine (CyTrue);
    }
    else
    {
        location[0] = 0x05; /* Read status */

        CyFxSpiSetSsnLine (CyFalse);
        status = CyFxSpiTransmitWords (location, 1);
        if (status != CY_U3P_SUCCESS)
        {
            CyFxSpiSetSsnLine (CyTrue);
            return status;
        }

        status = CyFxSpiReceiveWords (rdBuf, 2);
        CyFxSpiSetSsnLine (CyTrue);
        *wip = rdBuf[0] & 0x1;
    }

    return status;
}

CyBool_t
CyFxUSBSetupCB (
        uint32_t setupdat0,
        uint32_t setupdat1)
{
    /* Fast enumeration is used. Only requests addressed to the interface, class,
     * vendor and unknown control requests are received by this function. */

    uint8_t  bRequest, bReqType;
    uint8_t  bType, bTarget;
    uint16_t wValue, wIndex, wLength;
    CyBool_t isHandled = CyFalse;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    /* Decode the fields from the setup request. */
    bReqType = (setupdat0 & CY_U3P_USB_REQUEST_TYPE_MASK);
    bType    = (bReqType & CY_U3P_USB_TYPE_MASK);
    bTarget  = (bReqType & CY_U3P_USB_TARGET_MASK);
    bRequest = ((setupdat0 & CY_U3P_USB_REQUEST_MASK) >> CY_U3P_USB_REQUEST_POS);
    wValue   = ((setupdat0 & CY_U3P_USB_VALUE_MASK)   >> CY_U3P_USB_VALUE_POS);
    wIndex   = ((setupdat1 & CY_U3P_USB_INDEX_MASK)   >> CY_U3P_USB_INDEX_POS);
    wLength   = ((setupdat1 & CY_U3P_USB_LENGTH_MASK)   >> CY_U3P_USB_LENGTH_POS);

    if (bType == CY_U3P_USB_STANDARD_RQT)
    {
        /* Handle SET_FEATURE(FUNCTION_SUSPEND) and CLEAR_FEATURE(FUNCTION_SUSPEND)
         * requests here. It should be allowed to pass if the device is in configured
         * state and failed otherwise. */
        if ((bTarget == CY_U3P_USB_TARGET_INTF) && ((bRequest == CY_U3P_USB_SC_SET_FEATURE)
                    || (bRequest == CY_U3P_USB_SC_CLEAR_FEATURE)) && (wValue == 0))
        {
            if (glIsApplnActive)
                CyU3PUsbAckSetup ();
            else
                CyU3PUsbStall (0, CyTrue, CyFalse);

            isHandled = CyTrue;
        }
    }

    /* Handle supported vendor requests. */
    if (bType == CY_U3P_USB_VENDOR_RQT)
    {
        isHandled = CyTrue;

        switch (bRequest)
        {
            case CY_FX_RQT_ID_CHECK:
                CyU3PUsbSendEP0Data (8, (uint8_t *)glFirmwareID);
                break;

            case CY_FX_RQT_SPI_FLASH_WRITE:
                status = CyU3PUsbGetEP0Data (wLength, glEp0Buffer, NULL);
                if (status == CY_U3P_SUCCESS)
                {
                    status = CyFxSpiTransfer (wIndex, wLength,
                            glEp0Buffer, CyFalse);
                }
                break;

            case CY_FX_RQT_SPI_FLASH_READ:
                CyU3PMemSet (glEp0Buffer, 0, sizeof (glEp0Buffer));
                status = CyFxSpiTransfer (wIndex, wLength,
                        glEp0Buffer, CyTrue);
                if (status == CY_U3P_SUCCESS)
                {
                    status = CyU3PUsbSendEP0Data (wLength, glEp0Buffer);
                }
                break;

            case CY_FX_RQT_SPI_FLASH_ERASE_POLL:
                status = CyFxSpiEraseSector ((wValue) ? CyTrue : CyFalse,
                        (wIndex & 0xFF), glEp0Buffer);
                if (status == CY_U3P_SUCCESS)
                {
                    CyFxSpiWaitForStatus ();
                    if (wValue == 0)
                    {
                        status = CyU3PUsbSendEP0Data (wLength, glEp0Buffer);
                    }
                    else
                    {
                        CyU3PUsbAckSetup ();
                    }
                }
                break;

            default:
                /* This is unknown request. */
                isHandled = CyFalse;
                break;
        }

        /* If there was any error, return not handled so that the library will
         * stall the request. Alternatively EP0 can be stalled here and return
         * CyTrue. */
        if (status != CY_U3P_SUCCESS)
        {
            isHandled = CyFalse;
        }
    }

    return isHandled;
}

/* This is the callback function to handle the USB events. */
void
CyFxUSBEventCB (
    CyU3PUsbEventType_t evtype, /* Event type */
    uint16_t            evdata  /* Event data */
    )
{
    switch (evtype)
    {
        case CY_U3P_USB_EVENT_SETCONF:
            glIsApplnActive = CyTrue;
            /* Disable the low power entry to optimize USB throughput */
            CyU3PUsbLPMDisable();
            break;

        case CY_U3P_USB_EVENT_RESET:
        case CY_U3P_USB_EVENT_DISCONNECT:
            glIsApplnActive = CyFalse;
            break;

        default:
            break;
    }
}

/* Callback function to handle LPM requests from the USB 3.0 host. This function is invoked by the API
   whenever a state change from U0 -> U1 or U0 -> U2 happens. If we return CyTrue from this function, the
   FX3 device is retained in the low power state. If we return CyFalse, the FX3 device immediately tries
   to trigger an exit back to U0.

   This application does not have any state in which we should not allow U1/U2 transitions; and therefore
   the function always return CyTrue.
 */
CyBool_t
CyFxApplnLPMRqtCB (
        CyU3PUsbLinkPowerMode link_mode)
{
    return CyTrue;
}

/* Initialize all interfaces for the application. */
CyU3PReturnStatus_t
CyFxUsbSpiGpioInit (
        uint16_t pageSize)
{
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    glSpiPageSize   = pageSize;

    /* Initialize the GPIO Pins for the SPI interface. */
    status = CyFxGpioInit ();
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* Start the USB functionality. */
    status = CyU3PUsbStart();
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* The fast enumeration is the easiest way to setup a USB connection,
     * where all enumeration phase is handled by the library. Only the
     * class / vendor requests need to be handled by the application. */
    CyU3PUsbRegisterSetupCallback(CyFxUSBSetupCB, CyTrue);

    /* Setup the callback to handle the USB events. */
    CyU3PUsbRegisterEventCallback(CyFxUSBEventCB);

    /* Register a callback to handle LPM requests from the USB 3.0 host. */
    CyU3PUsbRegisterLPMRequestCallback(CyFxApplnLPMRqtCB);    
    
    /* Set the USB Enumeration descriptors */

    /* Super speed device descriptor. */
    status = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB30DeviceDscr);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* High speed device descriptor. */
    status = CyU3PUsbSetDesc(CY_U3P_USB_SET_HS_DEVICE_DESCR, 0, (uint8_t *)CyFxUSB20DeviceDscr);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* BOS descriptor */
    status = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_BOS_DESCR, 0, (uint8_t *)CyFxUSBBOSDscr);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* Device qualifier descriptor */
    status = CyU3PUsbSetDesc(CY_U3P_USB_SET_DEVQUAL_DESCR, 0, (uint8_t *)CyFxUSBDeviceQualDscr);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* Super speed configuration descriptor */
    status = CyU3PUsbSetDesc(CY_U3P_USB_SET_SS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBSSConfigDscr);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* High speed configuration descriptor */
    status = CyU3PUsbSetDesc(CY_U3P_USB_SET_HS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBHSConfigDscr);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* Full speed configuration descriptor */
    status = CyU3PUsbSetDesc(CY_U3P_USB_SET_FS_CONFIG_DESCR, 0, (uint8_t *)CyFxUSBFSConfigDscr);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* String descriptor 0 */
    status = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 0, (uint8_t *)CyFxUSBStringLangIDDscr);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* String descriptor 1 */
    status = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 1, (uint8_t *)CyFxUSBManufactureDscr);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* String descriptor 2 */
    status = CyU3PUsbSetDesc(CY_U3P_USB_SET_STRING_DESCR, 2, (uint8_t *)CyFxUSBProductDscr);
    if (status != CY_U3P_SUCCESS)
    {
        return status;
    }

    /* Connect the USB Pins with super speed operation enabled. */
    status = CyU3PConnectState(CyTrue, CyTrue);

    return status;
}

/*
 * Entry function for the application thread. This function performs
 * the initialization of the Debug, GPIO and USB modules and then
 * executes in a loop printing out heartbeat messages through the UART.
 */
void
AppThread_Entry (
        uint32_t input)
{
    uint8_t count = 0;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    /* Initialize the debug interface. */
    status = CyFxDebugInit ();
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_error;
    }

    /* Initialize the application. */
    status = CyFxUsbSpiGpioInit (0x100);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_error;
    }

    for (;;)
    {
        CyU3PDebugPrint (4, "%x: Device initialized. Firmware ID: %x %x %x %x %x %x %x %x\r\n",
                count++, glFirmwareID[3], glFirmwareID[2], glFirmwareID[1], glFirmwareID[0],
                glFirmwareID[7], glFirmwareID[6], glFirmwareID[5], glFirmwareID[4]);
        CyU3PThreadSleep (1000);
    }

handle_error:
    CyU3PDebugPrint (4, "%x: Application failed to initialize. Error code: %d.\n", status);
    while (1);
}

/* Application define function which creates the application threads. */
void
CyFxApplicationDefine (
        void)
{
    void *ptr = NULL;
    uint32_t retThrdCreate = CY_U3P_SUCCESS;

    /* Allocate the memory for the threads and create threads */
    ptr = CyU3PMemAlloc (APPTHREAD_STACK);
    retThrdCreate = CyU3PThreadCreate (&appThread, /* Thread structure. */
            "21:AppThread",                        /* Thread ID and name. */        
            AppThread_Entry,                       /* Thread entry function. */
            0,                                     /* Thread input parameter. */
            ptr,                                   /* Pointer to the allocated thread stack. */
            APPTHREAD_STACK,                       /* Allocated thread stack size. */
            APPTHREAD_PRIORITY,                    /* Thread priority. */
            APPTHREAD_PRIORITY,                    /* Thread pre-emption threshold: No preemption. */
            CYU3P_NO_TIME_SLICE,                   /* No time slice. Thread will run until task is
                                                      completed or until the higher priority 
                                                      thread gets active. */
            CYU3P_AUTO_START                       /* Start the thread immediately. */
            );

    /* Check the return code */
    if (retThrdCreate != 0)
    {
        /* Thread creation failed with the error code retThrdCreate */

        /* Add custom recovery or debug actions here */

        /* Application cannot continue. Loop indefinitely */
        while(1);
    }
}

/*
 * Main function
 */
int
main (void)
{
    CyU3PIoMatrixConfig_t io_cfg;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    /* Initialize the device */
    status = CyU3PDeviceInit (NULL);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* Initialize the caches. Enable both Instruction and Data Caches. */
    status = CyU3PDeviceCacheControl (CyTrue, CyTrue, CyTrue);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* Configure the IO matrix for the device. On the FX3 DVK board, the COM port 
     * is connected to the IO(46:49) in 16-bit GPIF mode. GPIO Ids 53-56 are used
     * to control the SPI slave device.
     * */
    CyU3PMemSet ((uint8_t *)&io_cfg, 0, sizeof(io_cfg));
    io_cfg.isDQ32Bit = CyFalse;
    io_cfg.s0Mode = CY_U3P_SPORT_INACTIVE;
    io_cfg.s1Mode = CY_U3P_SPORT_INACTIVE;
    io_cfg.useUart   = CyTrue;
    io_cfg.useI2C    = CyFalse;
    io_cfg.useI2S    = CyFalse;
    io_cfg.useSpi    = CyFalse;
    io_cfg.lppMode   = CY_U3P_IO_MATRIX_LPP_DEFAULT;
    /* 53-56 GPIOs are enabled. */
    io_cfg.gpioSimpleEn[0]  = 0;
    io_cfg.gpioSimpleEn[1]  = 0x01E00000;
    io_cfg.gpioComplexEn[0] = 0;
    io_cfg.gpioComplexEn[1] = 0;
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

/* [ ] */

