/*
## Cypress USB 3.0 Platform Header file (cyu3lpp.h)
## =====================================
##
##  Copyright Cypress Semiconductor Corporation, 2010-2011,
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
## =====================================
*/

#ifndef _INCLUDED_CYU3_LPP_H_
#define _INCLUDED_CYU3_LPP_H_

#include <cyu3os.h>
#include <cyu3types.h>
#include <cyu3system.h>
#include "cyu3externcstart.h"

/*
 * Summary
 * The LPP interface manager module is responsible for handling the in-bound and out-bound 
 * transfer of data through the Low power peripheral interfaces on the WB device i.e. I2s,
 * I2c, Uart, Spi, Gpio. This file defines the data structures and the interfaces for LPP 
 * interface management.
 */

/**************************************************************************
 ********************************Data Types********************************
 **************************************************************************/

/* Lpp modules */
/*Summary
  Clock divider values for sampling simple GPIOs.

  Description
  The simple GPIOs are always sampled at a frequency
  lower than the fast clock. This divider value can
  be selected for power optimizations.

  See Also
  * CyU3PGpioSimpleClkDiv_t
  * CyU3PSysClkSrc_t
  * CyU3PGpioInit
 */
typedef enum CyU3PGpioSimpleClkDiv_t
{
    CY_U3P_GPIO_SIMPLE_DIV_BY_2 = 0,    /* Fast clock by 2. */
    CY_U3P_GPIO_SIMPLE_DIV_BY_4,        /* Fast clock by 4. */
    CY_U3P_GPIO_SIMPLE_DIV_BY_16,       /* Fast clock by 16. */
    CY_U3P_GPIO_SIMPLE_DIV_BY_64,       /* Fast clock by 64. */
    CY_U3P_GPIO_SIMPLE_NUM_DIV          /* Number of divider enumerations. */
} CyU3PGpioSimpleClkDiv_t;


/*Summary
  Enumerated list for IO modes.

  Description
  Enumerations to control the pull up or pull down feature on IOs.
  The pull-up or pull-down provided is very weak and it takes about 5us
  to be effective at the pads after configurations.

  See Also
  * CyU3PGpioSetIoMode
 */
typedef enum CyU3PGpioIoMode_t
{
    CY_U3P_GPIO_IO_MODE_NONE = 0,       /* No internal pull-up or pull-down.
                                           Default condition. */
    CY_U3P_GPIO_IO_MODE_WPU,            /* A weak pull-up is provided on the IO. */
    CY_U3P_GPIO_IO_MODE_WPD             /* A weak pull-down is provided on the IO. */

} CyU3PGpioIoMode_t;

/*Summary
  Clock configuration information for the GPIO block.

  Description
  The clock for the GPIO block can be configured to required frequency
  especially for the complex GPIO pins. The default values can be

  fastClkDiv = 2
  slowClkDiv = 0
  simpleDiv = CY_U3P_GPIO_SIMPLE_DIV_BY_2
  clkSrc = CY_U3P_SYS_CLK.

  These default values must be used if only simple GPIO is required.

  See Also
  * CyU3PGpioSimpleClkDiv_t
  * CyU3PSysClockSrc_t
  * CyU3PGpioSetClock
 */
typedef struct CyU3PGpioClock_t
{
    uint8_t fastClkDiv;                 /* Divider value for the GPIO fast clock.
                                           This is used for all complex GPIO sampling
                                           unless otherwise specified. The min value
                                           is 2 and max value is 16. */
    uint8_t slowClkDiv;                 /* Divider value for the GPIO slow clock.
                                           The clock is based out of fast clock.
                                           The min value is 2 and max value is 64.
                                           If zero is used, then slow clock is not
                                           used and is disabled. */
    CyBool_t halfDiv;                   /* This allows the fast clock to be divided
                                           by a non integral value of 0.5. This can be
                                           done only if slow clock is disabled. */
    CyU3PGpioSimpleClkDiv_t simpleDiv;  /* Divider value from fast clock for sampling
                                          simple GPIOs. */
    CyU3PSysClockSrc_t clkSrc;          /* The clock source to be used for this peripheral. */
} CyU3PGpioClock_t;


/*
   Summary
   Prototype of serial peripheral interrupt handler function.

   Description
   Each serial peripheral (I2C, I2S, SPI, UART and GPIO) on the FX3 device has
   some interrupts associated with it. The drivers for these blocks can register
   an interrupt handler function that the FX3 firmware framework will call when
   an interrupt is received. This block specific interrupt handler is registered
   through the CyU3PLppInit function.

   See Also
   * CyU3PLppInit
*/
typedef void (*CyU3PLppInterruptHandler) (
        void);

/**************************************************************************
 ********************************* MACROS *********************************
 **************************************************************************/

/* GPIO interrupt event mask. */
#define CY_U3P_LPP_EVENT_GPIO_INTR      (1 << 3)
/* I2S interrupt event mask. */
#define CY_U3P_LPP_EVENT_I2S_INTR       (1 << 4)
/* I2C interrupt event mask. */
#define CY_U3P_LPP_EVENT_I2C_INTR       (1 << 5)
/* UART interrupt event mask. */
#define CY_U3P_LPP_EVENT_UART_INTR      (1 << 6)
/* SPI interrupt event mask. */
#define CY_U3P_LPP_EVENT_SPI_INTR       (1 << 7)

#define CY_U3P_I2C_DEFAULT_LOCK_TIMEOUT	    (CYU3P_WAIT_FOREVER)    /* Default wait option for mutex lock. */
#define CY_U3P_SPI_DEFAULT_LOCK_TIMEOUT	    (CYU3P_WAIT_FOREVER)    /* Default wait option for getting mutex lock. */
#define CY_U3P_UART_DEFAULT_LOCK_TIMEOUT    (CYU3P_WAIT_FOREVER)    /* Default wait option for getting mutex lock. */
#define CY_U3P_I2S_DEFAULT_LOCK_TIMEOUT     (CYU3P_WAIT_FOREVER)    /* Default wait option for mutex lock. */

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/* Summary
   Function to check if the boot firmware has left the GPIO block powered ON.

   Description
   In systems which make use of the boot firmware, it is possible that some of the
   GPIOs have been left configured by the boot firmware. In such a case, the GPIO
   block on the FX3 device should not be reset during firmware initialization.
   This function is used to check whether the boot firmware has requested the GPIO
   block to be left powered ON across firmware initialization.

   Note
   Please note that all the pins that have been left configured by the boot firmware
   need to be selected as simple GPIOs during IO Matrix configuration. Otherwise, these
   pins will be tri-state because the pin functionality is overridden.

   See Also
   * CyU3PDeviceConfigureIOMatrix

   Return Values
   * CyTrue if the boot firmware has left GPIO on, CyFalse otherwise.
 */
extern CyBool_t
CyU3PLppGpioBlockIsOn (
        void);

/* Summary
   This function registers the specified peripheral block as active.

   Description
   The serial peripheral blocks on the FX3 device share some resources. While the individual peripheral
   blocks (GPIO, I2C, UART, SPI and I2S) can be turned on/off at runtime; the shared resources need to
   be kept initialized while any of these blocks are on. This function is used to manage the shared
   peripheral resources and also to keep track of which peripheral blocks are on.

   This function need to be called after initializing the clock for the corresponding peripheral interface.

   Return Value
   * CY_U3P_SUCCESS                - If the call is successful.
   * CY_U3P_ERROR_ALREADY_STARTED  - The block is already initialized.
   * CY_U3P_ERROR_NOT_SUPPORTED    - If the serial peripheral block is not supported by the FX3 part.
   * CY_U3P_ERROR_INVALID_SEQUENCE - If the block init is being called without turning on the corresponding clock.

   See Also
   * CyU3PLppDeInit
   * CyU3PUartSetClock
   * CyU3PI2cSetClock
   * CyU3PI2sSetClock
   * CyU3PSpiSetClock
 */
extern CyU3PReturnStatus_t
CyU3PLppInit (
        CyU3PLppModule_t lppModule,             /* The peripheral block being initialized. */
        CyU3PLppInterruptHandler intrHandler    /* Interrupt handler function for the peripheral block. */
        );

/* Summary
   Function that registers that a specified peripheral block has been made inactive.

   Description
   This function registers that a specified peripheral block is no longer in use. The function
   along with CyU3PLppInit() keeps track of the peripheral blocks that are ON; and manages the
   power on/off of the shared resources for these blocks.

   Return Value
   * CY_U3P_SUCCESS             - If the call is successful.   
   * CY_U3P_ERROR_NOT_SUPPORTED - If the serial peripheral selected is not supported by the FX3 device
   * CY_U3P_ERROR_NOT_STARTED   - If the serial peripheral being stopped has not been started.
  
   See Also
   * CyU3PLppInit
 */
extern CyU3PReturnStatus_t
CyU3PLppDeInit (
        CyU3PLppModule_t lppModule              /* The peripheral block being de-initialized. */
        );

/* Summary
   This function configures and turns on the clocks used by the GPIO block on the FX3.

   Description
   The GPIO block on the FX3 device makes use of two different clocks (fast clock and slow clock). This
   function is used to select the frequency for these clocks and to turn them on.

   Return Value
   * CY_U3P_SUCCESS            - If the clocks were successfully configured and enabled.
   * CY_U3P_ERROR_BAD_ARGUMENT - If the clock configuration specified is invalid.
  
   See Also
   * CyU3PGpioClock_t
   * CyU3PGpioStopClock
 */
extern CyU3PReturnStatus_t
CyU3PGpioSetClock(
        CyU3PGpioClock_t *clk_p                 /* The desired clock parameters. */
        );

/* Summary
   Function sets the required frequency for the I2S block.

   Description
   The clock for the I2S block can be configured to required frequency. 

   Bit-clock calculation:
   The minimum sample rate supported is 8KHz and the maximum sample rate is 192KHz.
   The corresponding bit-clock frequency is calculated based on the sample length,
   number of samples and the padding mode used and is defined as per the I2S spec.
   The default calculation is bit clock = 64 * sample rate. It should be noted that
   even though the clock dividers and the API allows frequencies above and below the
   rated range, the device behaviour is not guaranteed. The actual sample rate is
   derived out of SYS_CLK. Since the divider needs to be integral or with half
   divider, the frequency approximation is done using the following algorithm:

   If x is the actual divider and n is the required integral divider to be used,
   then following conditions are used to evaluate:

   x = (SYS_CLK) / (bit clock);

   if (x - floor(x)) < 0.25 ==> n = floor(x);

   if (((x - floor(x)) >= 0.25) &&
   (x - floor(x)) < 0.5) ==> n = floor(x) + half divider;

   if (x - floor(x)) >= 0.75 ==> n = floor(x) + 1;

   Return Value
   * CY_U3P_SUCCESS             - If the I2S interface clock was setup as required.
   * CY_U3P_ERROR_NOT_SUPPORTED - If the FX3 device in use does not support the I2S interface.
   * CY_U3P_ERROR_BAD_ARGUMENT  - When invalid arqument is passed to the function 
   
   See Also
   * CyU3PI2sStopClock
 */
extern CyU3PReturnStatus_t
CyU3PI2sSetClock (
        uint32_t clkRate                        /* Desired interface clock frequency. */
        );

/* Summary
   Function sets the required frequency for the I2C block.

   Description
   The clock for the I2C block can be configured to required frequency.

   Bitrate calculation:
   The maximum bitrate supported is 1MHz and minimum bitrate is 100KHz. It should
   be noted that even though the dividers and the API allows frequencies above and
   below the rated range, the device behaviour is not guaranteed. The actual
   bit rate is derived out of the SYS_CLK / 16. The I2C block requires the clocking
   to be 10X the required bitrate. Since the divider needs to be integral or with
   half-divider, the frequency approximation is done with the following method:

   If x is the actual divider and n is the required integral divider to be used,
   then following conditions are used to evaluate:

   x = (SYS_CLK / 16) / (bitrate * 10);

   if (x - floor(x)) < 0.25 ==> n = floor(x);

   if (((x - floor(x)) >= 0.25) &&
   (x - floor(x)) < 0.5) ==> n = floor(x) + half divider;

   if (x - floor(x)) >= 0.75 ==> n = floor(x) + 1;

   Return Value
   * CY_U3P_SUCCESS            - If the I2C interface clock was setup as required.
   * CY_U3P_ERROR_BAD_ARGUMENT - When invalid arqument is passed to the function 
  
   See Also
   * CyU3PI2cStopClock
 */
extern CyU3PReturnStatus_t
CyU3PI2cSetClock (
        uint32_t bitRate                        /* Desired interface clock frequency. */
        );

/* Summary
   Function sets the required frequency for the UART block.

   Description
   The clock for the UART block can be configured to required frequency.

   Baudrate calculation:
   The maximum baud rate supported is 4MHz and the minimum is 100Hz. The UART
   block requires an internal clocking of 16X. It should be noted that even though
   the clock dividers and the API allows frequencies above and below the rated range,
   the device behaviour is not guaranteed. The actual clock rate is derived out of
   SYS_CLK for all baud rates >= 600. For rates below this SYS_CLK / 16 is used.
   Since the divider needs to be integral or with half divider, the frequency
   approximation is done using the following algorithm:

   If x is the actual divider and n is the required integral divider to be used,
   then following conditions are used to evaluate:

   x = (source clock) / (baudrate * 16);
   source clock = SYS_CLK for baud >= 600 and source clock = (SYS_CLK / 16) for baud below 600.

   if (x - floor(x)) < 0.25 ==> n = floor(x);
   
   if (((x - floor(x)) >= 0.25) &&
   (x - floor(x)) < 0.5) ==> n = floor(x) + half divider;
   
   if (x - floor(x)) >= 0.75 ==> n = floor(x) + 1;
   
   Return Value
   * CY_U3P_SUCCESS             - If the UART clock and baud rate was setup as required.
   * CY_U3P_ERROR_BAD_ARGUMENT  - When invalid arqument is passed to the function 
   * CY_U3P_ERROR_NOT_SUPPORTED - If the FX3 part in use does not support the UART interface
   
   See Also
   * NONE
 */
extern CyU3PReturnStatus_t
CyU3PUartSetClock (
        uint32_t baudRate                       /* Desired baud rate for the UART interface. */
        );

/* Summary
   This function set the required frequency the SPI block.

   Description
   The clock for the SPI block can be configured to required frequency.

   SPI-clock calculation:
   The maximum SPI clock supported is 33MHz and the minimum is 10KHz. The SPI block
   requires an internal clocking of 2X. It should be noted that even though the
   clock dividers and the API allows frequencies above and below   the rated range,
   the device behaviour is not guaranteed. The actual clock rate is derived out of
   SYS_CLK. Since the divider needs to be integral or with half divider, the
   frequency approximation is done using the following algorithm:

   If x is the actual divider and n is the required integral divider to be used,
   then following conditions are used to evaluate:

   x = (SYS_CLK) / (SPI_clock * 2);

   if (x - floor(x)) < 0.25 ==> n = floor(x);

   if (((x - floor(x)) >= 0.25) &&
   (x - floor(x)) < 0.5) ==> n = floor(x) + half divider;

   if (x - floor(x)) >= 0.75 ==> n = floor(x) + 1;

   Return Value
   * CY_U3P_SUCCESS             - If the SPI clock has been successfully turned on.
   * CY_U3P_ERROR_NOT_SUPPORTED - If the SPI interface is not supported by the current FX3 device.
   * CY_U3P_ERROR_BAD_ARGUMENT  - When invalid arqument is passed to the function 
  
   See Also
   * CyU3PSpiStopClock
 */
extern CyU3PReturnStatus_t
CyU3PSpiSetClock (
        uint32_t clock                          /* Clock frequency desired. */
        );

/* Summary
   This function disables the clock of the SPI block.

   Description
   The clock for the SPI block is disabled. This should be called after the corresponding driver has been
   deinitialized.

   Return Value
   * CY_U3P_SUCCESS               - if the SPI clock is turned off successfully.
   * CY_U3P_ERROR_NOT_SUPPORTED   - if the SPI interface is not supported by the current FX3 device.
  
   See Also
   * CyU3PSpiSetClock
 */
extern CyU3PReturnStatus_t
CyU3PSpiStopClock (
        void);

/* Summary
   This function disables the clock of the I2C.

   Description
   The clock for the I2C block is disabled. It is expected while deinitialization the I2C.

   Return Value
   * CY_U3P_SUCCESS            - If the I2C clock was successfully turned off.
  
   See Also
   * CyU3PI2cSetClock
 */
extern CyU3PReturnStatus_t
CyU3PI2cStopClock (
        void);

/* Summary
   This function disables the clock of the GPIO.

   Description
   The clock for the GPIO block is disabled. It is expected while deinitialization the GPIO.

   Return Value
   * CY_U3P_SUCCESS            - If the GPIO clocks were successfully turned off.

   See Also
   * CyU3PGpioSetClock
 */
extern CyU3PReturnStatus_t
CyU3PGpioStopClock (
        void);

/* Summary
   This function disables the clock of the I2S block.

   Description
   The clock for the I2s block is disabled. This function should be called after the corresponding driver has been
   de-initialized.

   Return Value
   * CY_U3P_SUCCESS             - If the I2S clock is successfully turned off.
   * CY_U3P_ERROR_NOT_SUPPORTED - If the I2S interface is not supported by the current FX3 device.
  
   See Also
   * CyU3PI2sSetClock
 */
extern CyU3PReturnStatus_t
CyU3PI2sStopClock (
        void);

/* Summary
   This function disables the clock of the UART.

   Description
   Disable the clock to the UART block. This needs to be done after the UART driver deinit is complete.

   Return Value
   * CY_U3P_SUCCESS             - if the UART clock was successfully turned off.
   * CY_U3P_ERROR_NOT_SUPPORTED - if the UART interface is not supported on the FX3 part.
  
   See Also
   * CyU3PUartSetClock
 */
extern CyU3PReturnStatus_t
CyU3PUartStopClock (
        void);

/* Summary
   Set the IO drive strength for the I2C interface.

   Description
   The function sets the IO Drive strength for the I2C interface.
   The default IO drive strength for I2C is set to CY_U3P_DS_THREE_QUARTER_STRENGTH.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the I2C block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the Drive strength requested is invalid
  
   See Also
   * CyU3PDriveStrengthState_t
 */
extern CyU3PReturnStatus_t
CyU3PSetI2cDriveStrength (
        CyU3PDriveStrengthState_t i2cDriveStrength      /* Drive strength desired for I2C signals. */
        );

/* Summary
   Set IO mode for the selected GPIO.

   Description
   By default there is no pull-up or pull-down provided. The
   API can be used to provide a weak pull-up / pull-down on the
   IO. It takes about 5us to be active after configuration.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the gpioId or ioMode requested is invalid
  
   See Also
   * CyU3PGpioIoMode_t
 */
extern CyU3PReturnStatus_t
CyU3PGpioSetIoMode (
        uint8_t gpioId,                                 /* GPIO Pin to be updated. */
        CyU3PGpioIoMode_t ioMode                        /* Desired pull-up/pull-down mode. */
        );

/* Summary
   Set the IO drive strength for all the GPIOs.

   Description
   The function sets the IO Drive strength for all the GPIOs.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the Drive strength requested is invalid
  
   See Also
   * CyU3PDriveStrengthState_t
 */
extern CyU3PReturnStatus_t
CyU3PSetGpioDriveStrength (
        CyU3PDriveStrengthState_t gpioDriveStrength     /* GPIO Drive strength */
        );

/* Summary
   This function sends an event to the LPP library thread.

   Description
   The event mask is internal to the library and this function
   should be invoked only from the library.

   Return Value
   * CY_U3P_SUCCESS              - If the event has been sent successfully.
 */
extern CyU3PReturnStatus_t
CyU3PLppEventSend (
        uint32_t eventMask                              /* The event mask to be set. */
        );

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3_LPP_H_ */

/*[]*/
