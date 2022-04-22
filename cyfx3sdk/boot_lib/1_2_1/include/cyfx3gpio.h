/*
## Cypress USB 3.0 Platform header file (cyfx3gpio.h)
## ===========================
##
##  Copyright Cypress Semiconductor Corporation, 2011-2012,
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

#ifndef _INCLUDED_CYFX3GPIO_H_
#define _INCLUDED_CYFX3GPIO_H_

#include <cyu3types.h>
#include <cyfx3error.h>
#include <cyfx3device.h>

#include <cyu3externcstart.h>
/*@@FX3 Boot GPIO Interface
   Summary
   The GPIO interface module is responsible for handling
   general purpose IO pins. This section defines the data
   structures and software interfaces for GPIO interface
   management.
   
   GPIO(general purpose I/O) pins are a special (simple) case of
   low performance serial peripherals that do not need DMA
   capability. 
   
   Simple GPIO provides software controlled and observable input
   and output capability only.
*/

/**************************************************************************
******************************* Macros ***********************************
**************************************************************************/

/**************************************************************************
******************************* Data Types *******************************
**************************************************************************/

/*@@FX3 Boot GPIO Data Types
This section documents the enumerations and data types that
are defined as part of the GPIO Interface Manager.
*/

/*Summary
  Enumerated list for interrupt mode GPIOs.

  Description
  Enumerations to control the triggering of interrupts
  for GPIOs configured as interrupts.

Note: Interrupts are not supported in this release of booter.

  */
typedef enum CyFx3BootGpioIntrMode_t
{
    CY_FX3_BOOT_GPIO_NO_INTR = 0,        /* No Interrupt is triggered*/
    CY_FX3_BOOT_GPIO_INTR_POS_EDGE,      /* Interrupt is triggered for positive edge of input. */
    CY_FX3_BOOT_GPIO_INTR_NEG_EDGE,      /* Interrupt is triggered for negative edge of input. */
    CY_FX3_BOOT_GPIO_INTR_BOTH_EDGE,     /* Interrupt is triggered for both edge of input. */
    CY_FX3_BOOT_GPIO_INTR_LOW_LEVEL,     /* Interrupt is triggered for Low level of input. */
    CY_FX3_BOOT_GPIO_INTR_HIGH_LEVEL     /* Interrupt is triggered for High level of input. */
} CyFx3BootGpioIntrMode_t;

/*Summary
  Structure contains configuration information for
  simple GPIOs.

  Description
  The GPIO can be configured to behave in different ways.
  This means that not all configurations are active all the time.

  If the pin is configured as input, then the fields driveLowEn and driveHighEn
  should be CyFalse. Also the field outValue is not considered.

  For normal mode of output operation, both driveLowEn and driveHighEn needs
  to be CyTrue.  Also the inputEn should be CyFalse.

  See Also
  * CyFx3BootGpioIntrMode_t
  */
typedef struct CyFx3BootGpioSimpleConfig_t
{
    CyBool_t outValue;              /* Initial output on the GPIO if configured as
                                       output: CyFalse = 0, CyTrue = 1. */
    CyBool_t driveLowEn;            /* When set true, the output driver is enabled
                                       for outValue = CyFalse (0), otherwise tristated. */
    CyBool_t driveHighEn;           /* When set true, the output driver is enabled
                                       for outValue = CyTrue (1), otherwise tristated. */
    CyBool_t inputEn;               /* When set true, the input state is enabled. */
    CyFx3BootGpioIntrMode_t intrMode;   /* Interrupt mode for the GPIO. */
} CyFx3BootGpioSimpleConfig_t;

/**************************************************************************
********************** Global variable declarations **********************
**************************************************************************/

/**************************************************************************
*************************** Function prototypes **************************
**************************************************************************/

/*@@FX3 Boot GPIO Functions
This section documents the API functions that are defined as
part of the GPIO Interface Manager.
*/


/* Summary
   Initializes the GPIO Interface Manager.

   Description
   This function is used to initialize the GPIO Interface Manager module. 
   This function resets the GPIO hardware block and initializes all the
   registers.

   Return Value
   * None

   See Also
   * CyFx3BootGpioIntrCb_t
   * CyFx3BootGpioDeInit
 */
extern void
CyFx3BootGpioInit (
        void
        );

/* Summary
   De-initializes the GPIO Interface Manager.

   Description
   This function resets the GPIO hardware block
   and de-initializes it.

   Return Value
   * None

   See Also
   * CyFx3BootGpioInit
 */
extern void
CyFx3BootGpioDeInit (
        void
        );

/* Summary
   Configures a simple GPIO.

   Description
   This function is used to configure and enable a simple GPIO pin. This
   function needs to be called before using the simple GPIO.

   Return Value
   * CY_FX3_BOOT_SUCCESS              - If the operation is successful
   * CY_FX3_BOOT_ERROR_NULL_POINTER   - If cfg_p is NULL
   * CY_FX3_BOOT_ERROR_NOT_CONFIGURED - If the GPIO pin has not been previously enabled in the IO matrix
                                        configuration

   See Also
   * CyFx3BootGpioSimpleConfig_t
 */
extern CyFx3BootErrorCode_t
CyFx3BootGpioSetSimpleConfig (
       uint8_t gpioId,                    /* GPIO id to be modified. */
       CyFx3BootGpioSimpleConfig_t *cfg_p /* Pointer to the configure
                                             information. */
       );

/* Summary
   Disables a GPIO pin.

   Description
   This function is used to disable a gpio pin.
   A GPIO is enabled when a Config call is made.

   Return Value
   * None

   See Also
   * CyFx3BootGpioGetValue
   * CyFx3BootGpioSetValue
 */
extern void
CyFx3BootGpioDisable (
        uint8_t gpioId		/* GPIO ID to be disabled */
	);

/* Summary
   Query the state of GPIO input.

   Description
   For input pins this reads the latest status on the GPIO, while
   for output pins this returns the last updated outValue.

   Return Value
   * CY_FX3_BOOT_SUCCESS              - If the operation is successful
   * CY_FX3_BOOT_ERROR_NULL_POINTER   - If value_p is NULL
   * CY_FX3_BOOT_ERROR_NOT_CONFIGURED - If the GPIO pin has not been previously enabled in the IO matrix
                                        configuration

   See Also
   * CyFx3BootGpioSetValue
 */
extern CyFx3BootErrorCode_t
CyFx3BootGpioGetValue (
        uint8_t  gpioId,      /* GPIO id to be queried. */
        CyBool_t *value_p     /* Output parameter that will be filled
                                 with the GPIO value. */
        );

/* Summary
   Set the state of GPIO pin output.

   Description
   The function is valid only for output GPIOs. The function states what value
   needs to be pushed to the pin. A CyFalse means 0 and a CyTrue means 1. This
   is not failed for a pin configured as an input, but will not change anything
   on the hardware and the pin remains configured as input.

   Return Value
   * CY_FX3_BOOT_SUCCESS              - If the operation is successful
   * CY_FX3_BOOT_ERROR_NOT_CONFIGURED - If the GPIO pin has not been previously enabled in the IO matrix
                                        configuration

   See Also
   * CyFx3BootGpioGetValue
 */
extern CyFx3BootErrorCode_t
CyFx3BootGpioSetValue (
        uint8_t  gpioId,   /* GPIO id to be modified. */
        CyBool_t  value    /* Value to set on the GPIO pin. */
        );

#include <cyu3externcend.h>
#endif /* _INCLUDED_CYFX3GPIO_H_ */

/*[]*/
