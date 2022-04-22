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

#include "cyfx3device.h"
#include "cyfx3utils.h"
#include "cyfx3gpio.h"
#include "defines.h"

/****************************************************************************
 * main:
 ****************************************************************************/
int
main (
        void)
{
    CyFx3BootErrorCode_t        status;
    CyFx3BootIoMatrixConfig_t   ioCfg;
    CyFx3BootGpioSimpleConfig_t gpioConf;
    CyBool_t                    value;

    /* HW and SW initialization code  */
    CyFx3BootDeviceInit (CyTrue);

    /* Enable the GPIOs for the LED and the switch. */
    ioCfg.isDQ32Bit = CyFalse;
    ioCfg.useUart   = CyFalse;
    ioCfg.useI2C    = CyFalse;
    ioCfg.useI2S    = CyFalse;
    ioCfg.useSpi    = CyFalse;
    ioCfg.gpioSimpleEn[0] = 0;
    ioCfg.gpioSimpleEn[1] = (1 << (APP_LED_GPIO - 32)) | (1 << (APP_SWITCH_GPIO - 32));

    status = CyFx3BootDeviceConfigureIOMatrix (&ioCfg);
    if (status != CY_FX3_BOOT_SUCCESS)
    {
        return status;
    }

    /* Initialize the GPIO module. */
    CyFx3BootGpioInit ();

    /* Configure the GPIO for reading the switch input. */
    gpioConf.inputEn     = CyTrue;
    gpioConf.driveLowEn  = CyFalse;
    gpioConf.driveHighEn = CyFalse;
    gpioConf.outValue    = CyFalse;
    gpioConf.intrMode    = CY_FX3_BOOT_GPIO_NO_INTR;

    status = CyFx3BootGpioSetSimpleConfig (APP_SWITCH_GPIO, &gpioConf);
    if (status != CY_FX3_BOOT_SUCCESS)
        return status;

    /* Configure the GPIO for driving the LED. */
    gpioConf.inputEn     = CyFalse;
    gpioConf.driveLowEn  = CyTrue;
    gpioConf.driveHighEn = CyTrue;
    gpioConf.outValue    = CyFalse;
    gpioConf.intrMode    = CY_FX3_BOOT_GPIO_NO_INTR;

    status = CyFx3BootGpioSetSimpleConfig (APP_LED_GPIO, &gpioConf);
    if (status != CY_FX3_BOOT_SUCCESS)
        return status;

    /* Flash the LED to indicate that initialization succeeded. */
    CyFx3BootGpioSetValue (APP_LED_GPIO, CyTrue);
    CyFx3BootBusyWait (50000);
    CyFx3BootGpioSetValue (APP_LED_GPIO, CyFalse);
    CyFx3BootBusyWait (50000);
    CyFx3BootGpioSetValue (APP_LED_GPIO, CyTrue);
    CyFx3BootBusyWait (50000);
    CyFx3BootGpioSetValue (APP_LED_GPIO, CyFalse);

    /* We keep looping around and copying the state of the SWITCH GPIO onto the LED GPIO. */
    while (1)
    {
        CyFx3BootGpioGetValue (APP_SWITCH_GPIO, &value);
        CyFx3BootGpioSetValue (APP_LED_GPIO, value);
        CyFx3BootBusyWait (50000);
    }

    return 0;
}

/*[]*/

