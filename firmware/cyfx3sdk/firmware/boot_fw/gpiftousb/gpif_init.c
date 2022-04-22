
/*
 ## Cypress FX3 Boot Firmware Example Source file (gpif_init.c)
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

#include "cyfx3pib.h"
#include "cyfx3uart.h"
#include "defines.h"
#include "cyfxgpif2config.h"

int
GpifInit (
        void)
{
    CyFx3BootPibClock_t  pibClock;
    CyFx3BootErrorCode_t stat;

    pibClock.clkSrc      = CY_FX3_BOOT_SYS_CLK;
    pibClock.clkDiv      = 4;
    pibClock.isHalfDiv   = CyFalse;
    pibClock.isDllEnable = CyFalse;
    stat = CyFx3BootPibInit (&pibClock, CyFalse);
    if (stat != CY_FX3_BOOT_SUCCESS)
    {
        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Pib Init failed with code %d\r\n", stat);
        CyFx3BootDeviceReset ();
        return -1;
    }

    CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Pib Init Done\r\n");

    stat = CyFx3BootGpifLoad (&CyFxGpifConfig);
    if (stat != CY_FX3_BOOT_SUCCESS)
    {
        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Gpif Load failed with code %d\r\n", stat);
        CyFx3BootDeviceReset ();
        return -2;
    }

    stat = CyFx3BootGpifSMStart (START, ALPHA_START);
    if (stat != CY_FX3_BOOT_SUCCESS)
    {
        CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "Gpif Start failed with code %d\r\n", stat);
        CyFx3BootDeviceReset ();
        return -3;
    }

    CyFx3BootUartPrintMessage ((uint8_t *)UART_DMA_BUF_ADDRESS, 256, "GPIF state machine started\r\n");
    return 0;
}

void
GpifDeinit (
        void)
{
    CyFx3BootGpifDisable (CyTrue);
    CyFx3BootPibDeinit ();
}

/*[]*/

