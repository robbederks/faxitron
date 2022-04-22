/*
## Cypress USB 3.0 Platform header file (cyu3gpio.h)
## ===========================
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
## ===========================
*/

#ifndef _INCLUDED_CYU3_GPIO_H_
#define _INCLUDED_CYU3_GPIO_H_

#include "cyu3types.h"
#include "cyu3system.h"
#include "cyu3externcstart.h"
#include "cyu3lpp.h"

/*@@GPIO Interface
   Summary
   The GPIO interface manager module is responsible for handling general
   purpose IO pins. This section defines the data structures and software
   interfaces for GPIO interface management.
   
   GPIO(general purpose I/O) pins are a special (simple) case of low
   performance serial peripherals that do not need DMA capability. Two 
   modes of GPIO pins are available with FX3 devices - Simple and Complex
   GPIOs.
   
   Simple GPIO provides software controlled and observable input and output
   capability only. Complex GPIO’s contain a timer and support a variety of
   timed behaviors (pulsing, time measurements, one-shot etc.).

   The GPIOs can only be configured individually and multiple GPIOs cannot
   be updated simultaneously.
*/

/**************************************************************************
******************************* Macros ***********************************
**************************************************************************/

/**************************************************************************
******************************* Data Types *******************************
**************************************************************************/

/*@@GPIO data types
This section documents the enumerations and data types that
are defined as part of the GPIO Interface Manager.
*/

/*Summary
  Enumerated list of all GPIO complex modes.

  Description
  This enumeration list complex modes supported.
  The CY_U3P_GPIO_MODE_SAMPLE_NOW, CY_U3P_GPIO_MODE_PULSE_NOW,
  CY_U3P_GPIO_MODE_PULSE, CY_U3P_GPIO_MODE_MEASURE_LOW_ONCE,
  CY_U3P_GPIO_MODE_MEASURE_HIGH_ONCE, CY_U3P_GPIO_MODE_MEASURE_NEG_ONCE,
  CY_U3P_GPIO_MODE_MEASURE_POS_ONCE and CY_U3P_GPIO_MODE_MEASURE_ANY_ONCE
  modes cannot be set by user. They need special APIs for operations.
  To use any of these modes, the complex GPIO must be configured with 
  CY_U3P_GPIO_MODE_STATIC configuration.

  See Also
  * CyU3PGpioTimerMode_t
  * CyU3PGpioIntrMode_t
  */
typedef enum CyU3PGpioComplexMode_t
{
    CY_U3P_GPIO_MODE_STATIC = 0,        /* Drives simple static values on GPIO. */
    CY_U3P_GPIO_MODE_TOGGLE,            /* Toggles the output when timer = threshold. */
    CY_U3P_GPIO_MODE_SAMPLE_NOW,        /* Read current timer value into threshold.
                                           This is a one time operation and resets mode
                                           to static. This mode should not be set by the
                                           configure function. This will be done by the
                                           CyU3PGpioComplexSampleNow API. */
    CY_U3P_GPIO_MODE_PULSE_NOW,         /* This mode is valid only for output configuration.
                                           Toggle value immediately, set timer = 0, then
                                           toggle output back when timer == threshold. Once
                                           this is done, set mode = static. This mode should
                                           not be set by the configure function. This will be
                                           done by the CyU3PGpioComplexPulseNow API. */
    CY_U3P_GPIO_MODE_PULSE,             /* This mode is valid only for output configuration.
                                           Toggle value when timer = 0, then toggle output
                                           when timer = threshold. Once this is done
                                           it sets mode = static. This mode should not be set
                                           by the configure function. This will be done by the
                                           CyU3PGpioComplexPulse API. */
    CY_U3P_GPIO_MODE_PWM,               /* This mode is valid only for output configurations.
                                           Toggle value when timer = 0, and toggle output
                                           when timer = threshold. This is a continuous operation. */
    CY_U3P_GPIO_MODE_MEASURE_LOW,       /* This is valid only for input configuration.
                                           Measure the time the signal is low. It sets timer = 0
                                           on negative edge of input, then loads threshold with
                                           timer value on positive edge of input. This is a
                                           continuous operation. */
    CY_U3P_GPIO_MODE_MEASURE_HIGH,      /* This is valid only for input configuration.
                                           Measure the time the signal is high. It sets timer = 0
                                           on positive edge of input, then loads threshold with
                                           timer value on negative edge of input. This is a
                                           continuous operation. */
    CY_U3P_GPIO_MODE_MEASURE_LOW_ONCE,  /* This is valid only for input configuration.
                                           Measure the time the signal is low once. It sets
                                           timer = 0 on negative edge of input, then loads threshold
                                           with timer value on positive edge of input. This is a
                                           single operation and at the end of it, sets the mode to
                                           static. This mode should not be set by the configure function.
                                           This will be done by the CyU3PGpioComplexMeasureOnce API. */
    CY_U3P_GPIO_MODE_MEASURE_HIGH_ONCE, /* This is valid only for input configuration.
                                           Measure the time the signal is high once. It sets
                                           timer = 0 on positive edge of input, then loads threshold
                                           with timer value on negative edge of input. This is a
                                           single operation and at the end of it, sets the mode
                                           to static. This mode should not be set by the configure function.
                                           This will be done by the CyU3PGpioComplexMeasureOnce API. */
    CY_U3P_GPIO_MODE_MEASURE_NEG,       /* This is valid only for input configuration.
                                           Measure when the signal goes low. It updates the threshold
                                           with value of timer on negative egde of input.
                                           This is a continuous operation. */
    CY_U3P_GPIO_MODE_MEASURE_POS,       /* This is valid only for input configuration.
                                           Measure when the signal goes high. It updates the threshold
                                           with value of timer on positive egde of input.
                                           This is a continuous operation. */
    CY_U3P_GPIO_MODE_MEASURE_ANY,       /* This is valid only for input configuration.
                                           Measure when the signal changes. It updates the threshold
                                           with value of timer on positive or negative egde of input.
                                           This is a continuous operation. */
    CY_U3P_GPIO_MODE_MEASURE_NEG_ONCE,  /* This is valid only for input configuration.
                                           Measure when the signal goes low once. It updates the
                                           threshold with value of timer on negative egde of input.
                                           This is a single operation and at the end of it, sets the
                                           mode to static. This mode should not be set by the configure function.
                                           This will be done by the CyU3PGpioComplexMeasureOnce API. */
    CY_U3P_GPIO_MODE_MEASURE_POS_ONCE,  /* This is valid only for input configuration.
                                           Measure when the signal goes high once. It updates the 
                                           threshold with value of timer on positive egde of input.
                                           This is a single operation and at the end of it, sets the
                                           mode to static. This mode should not be set by the configure function.
                                           This will be done by the CyU3PGpioComplexMeasureOnce API. */
    CY_U3P_GPIO_MODE_MEASURE_ANY_ONCE   /* This is valid only for input configuration.
                                           Measure when the signal changes once. It updates the
                                           threshold with value of timer on positive or negative
                                           egde of input. This is a single operation and at the
                                           end of it, sets the mode to static. This mode should not be set
                                           by the configure function. This will be done by the
                                           CyU3PGpioComplexMeasureOnce API. */
} CyU3PGpioComplexMode_t;

/*Summary
  Enumerated list for timer mode GPIOs.

  Description
  Enumerations to control the timer mode configuration of
  complex GPIO pins.

  See Also
  * CyU3PGpioComplexMode_t
  * CyU3PGpioIntrMode_t
  */
typedef enum CyU3PGpioTimerMode_t
{
    CY_U3P_GPIO_TIMER_SHUTDOWN = 0,	/* Shut down timer */
    CY_U3P_GPIO_TIMER_HIGH_FREQ,	/* Use High frequency (fast clock). */
    CY_U3P_GPIO_TIMER_LOW_FREQ,		/* Use low frequency (slow clock). */
    CY_U3P_GPIO_TIMER_STANDBY_FREQ,	/* Use standby frequency (32 KHz). */
    CY_U3P_GPIO_TIMER_POS_EDGE,		/* Use positive edge of input. */
    CY_U3P_GPIO_TIMER_NEG_EDGE,		/* Use negative edge of input. */
    CY_U3P_GPIO_TIMER_ANY_EDGE,		/* Use dual edge of input. */
    CY_U3P_GPIO_TIMER_RESERVED		/* Reserved */

} CyU3PGpioTimerMode_t;

/*Summary
  Enumerated list for interrupt mode GPIOs.

  Description
  Enumerations to control the triggering of interrupts
  for GPIOs configured as interrupts.

See Also
  * CyU3PGpioComplexMode_t
  * CyU3PGpioTimerMode_t
  */
typedef enum CyU3PGpioIntrMode_t
{
    CY_U3P_GPIO_NO_INTR = 0,        /* No Interrupt is triggered*/
    CY_U3P_GPIO_INTR_POS_EDGE,      /* Interrupt is triggered for positive edge of input. */
    CY_U3P_GPIO_INTR_NEG_EDGE,      /* Interrupt is triggered for negative edge of input. */
    CY_U3P_GPIO_INTR_BOTH_EDGE,     /* Interrupt is triggered for both edge of input. */
    CY_U3P_GPIO_INTR_LOW_LEVEL,     /* Interrupt is triggered for Low level of input. */
    CY_U3P_GPIO_INTR_HIGH_LEVEL,    /* Interrupt is triggered for High level of input. */
    CY_U3P_GPIO_INTR_TIMER_THRES,   /* Interrupt is triggered when the timer crosses the
                                       threshold. Valid only for complex GPIO pin. */
    CY_U3P_GPIO_INTR_TIMER_ZERO     /* Interrupt is triggered when the timer reaches zero.
                                       Valid only for complex GPIO pin. */
} CyU3PGpioIntrMode_t;


/*Summary
  Structure contains configuration information for
  simple GPIOs.

  Description
  The GPIO can be configured to behave in different ways.

  The input and output stages can be configured seperately. Since both the
  input and the output stages can be enabled simultaneously, care should be
  taken while configuration. For example, if an input pin has the output
  stage also enabled, the external device attached might be damaged.

  For normal mode of output operation, when a GPIO needs to be configured as
  output, both driveLowEn and driveHighEn needs to be CyTrue and inputEn needs
  to be CyFalse. Similarly for normal input operation, inputEn must be CyTrue
  and both driveLowEn and driveHighEn should be CyFalse.
  
  When output stage is enabled, the outValue field contains the initial state
  of the pin. CyTrue means high and CyFalse means low.

  See Also
  * CyU3PGpioIntrMode_t
  */
typedef struct CyU3PGpioSimpleConfig_t
{
    CyBool_t outValue;              /* Initial output on the GPIO if configured as
                                       output: CyFalse = 0, CyTrue = 1. */
    CyBool_t driveLowEn;            /* When set true, the output driver is enabled
                                       for outValue = CyFalse (0), otherwise tristated. */
    CyBool_t driveHighEn;           /* When set true, the output driver is enabled
                                       for outValue = CyTrue (1), otherwise tristated. */
    CyBool_t inputEn;               /* When set true, the input state is enabled. */
    CyU3PGpioIntrMode_t intrMode;   /* Interrupt mode for the GPIO. */
} CyU3PGpioSimpleConfig_t;

/*Summary
  Structure contains configuration information for
  complex GPIO pins.

  Description
  The complex GPIO can be configured to behave in different ways.
  The input and output stages can be configured seperately. Since both the
  input and the output stages can be enabled simultaneously, care should be
  taken while configuration. For example, if an input pin has the output
  stage also enabled, the external device attached might be damaged.

  For normal mode of output operation, when a GPIO needs to be configured as
  output, both driveLowEn and driveHighEn needs to be CyTrue and inputEn needs
  to be CyFalse. Similarly for normal input operation, inputEn must be CyTrue
  and both driveLowEn and driveHighEn should be CyFalse.
  
  When output stage is enabled, the outValue field contains the initial state
  of the pin. CyTrue means high and CyFalse means low.

  The complex GPIO pins can be used to generate various signals like PWM. It
  can also be used to measure various input parameters like low time period.
  The behaviour of the pin is decided by the pinMode configuration. Refer to
  CyU3PGpioComplexMode_t for more details.

  See Also
  * CyU3PGpioComplexMode_t
  * CyU3PGpioTimerMode_t
  * CyU3PGpioIntrMode_t
  */

typedef struct CyU3PGpioComplexConfig_t
{
    CyBool_t outValue;              /* Initial output on the GPIO if configured as
                                       output: CyFalse = 0, CyTrue = 1. */
    CyBool_t driveLowEn;            /* When set true, the output driver is enabled
                                       for outValue = CyFalse(0), otherwise tristated. */
    CyBool_t driveHighEn;           /* When set true, the output driver is enabled
                                       for outValue = CyTrue(1), otherwise tristated. */
    CyBool_t inputEn;               /* When set true, the input state is enabled. */
    CyU3PGpioComplexMode_t pinMode; /* The GPIO complex mode */
    CyU3PGpioIntrMode_t intrMode;   /* Interrupt mode for the GPIO. */
    CyU3PGpioTimerMode_t timerMode; /* Timer mode. */
    uint32_t timer;                 /* Timer initial value. */
    uint32_t period;                /* Timer period. */
    uint32_t threshold;             /* Timer threshold. */

} CyU3PGpioComplexConfig_t;

/*
   Summary
   Prototype of GPIO event callback function.
   
   Description
   This function type defines a callback to be called after GPIO interrupt has
   been received. A function of this type can be registered with the LPP driver
   as a callback function and will be called whenever an event of interest occurs.

   See Also
   * CyU3PRegisterGpioCallBack
   
*/
typedef void (*CyU3PGpioIntrCb_t)(
        uint8_t gpioId             /* Indicates the pin that triggered the interrupt.
                                      For complex pin interrupts, if no GPIO is
                                      configured for complex GPIO (timer use case),
                                      then this will hold 0 - 7 depending on the
                                      timer used. */
        );

/**************************************************************************
********************** Global variable declarations **********************
**************************************************************************/

/**************************************************************************
*************************** Function prototypes **************************
**************************************************************************/

/*@@GPIO Functions
This section documents the API functions that are defined as
part of the GPIO Interface Manager.
*/


/* Summary
   Initializes the GPIO Interface Manager.

   Description
   This function is used to initialize the GPIO Interface Manager module.
   This function resets the GPIO hardware block and initializes all the
   registers. This function should be called only once during system
   initialization.

   The application GPIO interrupt handler is registered using this function.
   The interrupt handler is invoked in the interrupt context based on the
   interrupt modes configured. Since the handler is invoked in interrupt context
   any blocking call should be avoided inside this handler. DMA APIs should not be
   invoked in this handler. Debug prints will not function in this handler. If any
   processing needs to be done in the handler then they will have to be handled
   in application thread using OS Events.

   The fast clock can run at maximum frequency of SYS_CLK / 2 and a minimum of
   SYS_CLK / 256. The complex GPIOs can be clocked at these. The simple GPIOs
   will be clocked at a fast_clock / simpleDiv. The SYS_CLK can either be 403.2MHz
   or 416MHz depending on the input clock / crystal to the device. Refer to the
   datasheet for more information.

   The slow clock, if enabled can run at a maximum frequency of fast_clock / 2
   and a minimum of fast_clock / 64.5. The slow clock is only useful for setting
   a  different clock for the timers.

   There is only one instance of fast_clock and slow_clock and all the GPIOs
   are driven with this.

   Return Value
   * CY_U3P_SUCCESS               - If the GPIO initialization is successful
   * CY_U3P_ERROR_ALREADY_STARTED - If the GPIO has already been initialized
   * CY_U3P_ERROR_NULL_POINTER    - If clk_p is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT    - If an incorrect/invalid clock value is passed

   See Also
   * CyU3PGpioClock_t
   * CyU3PGpioIntrCb_t
   * CyU3PGpioDeInit
   * CyU3PGpioIntrMode_t
   * CyU3PGpioDisable
   * CyU3PGpioDeInit
   * CyU3PGpioSetSimpleConfig
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioSimpleSetValue
   * CyU3PGpioSimpleGetValue
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
extern CyU3PReturnStatus_t
CyU3PGpioInit (
        CyU3PGpioClock_t *clk_p,
        CyU3PGpioIntrCb_t irq);

/* Summary
   De-initializes the GPIO Interface Manager.

   Description
   This function resets the GPIO hardware block
   and de-initializes it.

   Return Value
   * CY_U3P_SUCCESS           - If the GPIO de-init is successful
   * CY_U3P_ERROR_NOT_STARTED - If the GPIO block was not previously initialized

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDisable
   * CyU3PGpioSetSimpleConfig
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioSimpleSetValue
   * CyU3PGpioSimpleGetValue
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
extern CyU3PReturnStatus_t
CyU3PGpioDeInit (
        void);

/* Summary
   Configures a simple GPIO.

   Description
   This function is used to configure and enable a simple GPIO pin.
   This function needs to be called before using the simple GPIO.
   Refer to CyU3PGpioSimpleConfig_t for more information on
   configuration parameters.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If any of the parameters are incorrect/invalid
   * CY_U3P_ERROR_NULL_POINTER   - If cfg_p is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - If the GPIO pin has not been previously enabled in the IO matrix
                                   configuration

   See Also
   * CyU3PGpioSimpleConfig_t
   * CyU3PGpioInit
   * CyU3PGpioDisable
   * CyU3PGpioDeInit
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioSimpleSetValue
   * CyU3PGpioSimpleGetValue
 */
CyU3PReturnStatus_t
CyU3PGpioSetSimpleConfig (
       uint8_t gpioId,                  /* GPIO id to be modified. */
       CyU3PGpioSimpleConfig_t *cfg_p   /* Pointer to the configure
                                           information. */
       );

/* Summary
   Configures a complex GPIO pin.

   Description
   This function is used to configure and enable a complex GPIO pin. This
   function needs to be called before using the complex GPIO pin.

   There are only 8 complex GPIOs available. The 60 GPIO lines can be
   configured to modulo 8 complex pins. So one of GPIOs 0, 8, 16, ... can be 
   configured to complex pin 0 and so on. It should be noted that only one of
   them can exist.

   To use only the timer without any associated GPIO pin, the pin should be
   configured with inputEn = CyFalse, driveLowEn = CyFalse,
   driveHighEn = CyFalse and the timer number can be specified from 0 to 7.
   This override can be used only if none of the GPIOs that can be mapped to
   the complex pin are configured to be complex GPIO pin.

   Refer to CyU3PGpioComplexConfig_t for more information on
   configuration parameters.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If any of the parameters are incorrect/invalid
   * CY_U3P_ERROR_NULL_POINTER   - If cfg_p is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - If the GPIO pin has not been previously enabled
                                   in the IO matrix configuration

   See Also
   * CyU3PGpioComplexMode_t
   * CyU3PGpioComplexConfig_t
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
CyU3PReturnStatus_t
CyU3PGpioSetComplexConfig (
       uint8_t gpioId,              	/* GPIO id to be modified */
       CyU3PGpioComplexConfig_t *cfg_p  /* Config information. */
       );

/* Summary
   Disables a GPIO pin.

   Description
   This function is used to disable a gpio pin.
   A GPIO is enabled when a Config call is made.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the GPIO id is invalid

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioSetSimpleConfig
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioSimpleSetValue
   * CyU3PGpioSimpleGetValue
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
CyU3PReturnStatus_t
CyU3PGpioDisable (
        uint8_t gpioId		/* GPIO ID to be disabled */
	);

/* Summary
   Query the state of GPIO input.

   Description
   For input pins this reads the latest status on the GPIO, while
   for output pins this returns the last updated outValue.

   The function can only return the status of the selected
   GPIO. Input state of multiple GPIOs cannot be sampled at
   the same time.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the GPIO id is invalid
   * CY_U3P_ERROR_NULL_POINTER   - If value_p is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - If the GPIO pin has not been previously enabled in the IO matrix
                                   configuration

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetSimpleConfig
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioSimpleSetValue
   * CyU3PGpioSimpleGetValue
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
CyU3PReturnStatus_t
CyU3PGpioGetValue (
        uint8_t  gpioId,                /* GPIO id to be queried. */
        CyBool_t *value_p               /* Output parameter that will be filled
                                           with the GPIO value. */
        );

/* Summary
   Query the state of simple GPIO input.

   Description
   This is a lightweight API which does not check for errors.
   Use this only if a fast API access is required. For input pins
   this reads the latest status on the GPIO, while for output pins
   this returns the last updated outValue.

   The function can only return the status of the selected
   GPIO. Input state of multiple GPIOs cannot be sampled at
   the same time.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the GPIO id is invalid
   * CY_U3P_ERROR_NULL_POINTER   - If value_p is NULL

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetSimpleConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioSimpleSetValue
 */
CyU3PReturnStatus_t
CyU3PGpioSimpleGetValue (
        uint8_t  gpioId,                /* GPIO id to be queried. */
        CyBool_t *value_p               /* Output parameter that will be filled
                                           with the GPIO value. */
        );

/* Summary
   Set the state of GPIO pin output.

   Description
   The function is valid only for output GPIOs. The function states what value
   needs to be pushed to the pin. A CyFalse means 0 and a CyTrue means 1. This
   is not failed for a pin configured as an input, but will not change anything
   on the hardware and the pin remains configured as input.

   The function can only update the status of the selected GPIO. Output state of
   multiple GPIOs cannot be updated at the same time.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the GPIO id is invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - If the GPIO pin has not been previously enabled in the IO matrix
                                   configuration

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetSimpleConfig
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioSimpleSetValue
   * CyU3PGpioSimpleGetValue
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
CyU3PReturnStatus_t
CyU3PGpioSetValue (
        uint8_t  gpioId,   /* GPIO id to be modified. */
        CyBool_t  value    /* Value to set on the GPIO pin. */
        );

/* Summary
   Set the state of simple GPIO pin output.

   Description
   This is a lightweight API which does not check for errors. Use this only if 
   a fast API access is required. The function is valid only for output GPIOs.
   The function states what value needs to be pushed to the pin.
   A CyFalse means 0 and a CyTrue means 1. This is not failed for a pin configured
   as an input, but will not change anything on the hardware and the pin remains
   configured as input.

   The function can only update the status of the selected GPIO. Output state of
   multiple GPIOs cannot be updated at the same time.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the GPIO id is invalid

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetSimpleConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioSimpleGetValue
 */
CyU3PReturnStatus_t
CyU3PGpioSimpleSetValue (
        uint8_t  gpioId,   /* GPIO id to be modified. */
        CyBool_t  value    /* Value to set on the GPIO pin. */
        );

/* Summary
   States of all GPIOs in the system.

   Description
   The API returns the status of every IO in the system. These values are
   prior to the input buffer and will return the IO states regardless of
   the configuration. Each bit corresponds to the IO for the system and
   the valid range is from 0-60. The upper 3 bits of gpioVal1 is invalid.
   The value is returned if the pointer passed to the function is non-zero.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetSimpleConfig
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioSimpleSetValue
   * CyU3PGpioSimpleGetValue
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
CyU3PReturnStatus_t
CyU3PGpioGetIOValues (
        uint32_t *gpioVal0_p, /* 32-bit bit mask for the GPIOs 0-31. */
        uint32_t *gpioVal1_p  /* 32-bit bit mask for the GPIOs 32-60. */
        );

/* Summary
   Update the complex GPIO threshold and period.

   Description
   The function configures the complex GPIO to update the threshold and period
   for the specified GPIO. This should be modified only when the GPIO mode is
   static or PWM.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the IO pin is invalid.

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
CyU3PReturnStatus_t
CyU3PGpioComplexUpdate (
        uint8_t gpioId,         /* The complex GPIO to sample. */
        uint32_t threshold,     /* New timer threshold value to be updated. */
        uint32_t period         /* New timer period value to be updated. */
        );

/* Summary
   Read the latest value for complex GPIO threshold.

   Description
   The function just reads the current threshold value. This is useful when
   the GPIO is used for measurement.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the IO specified is not valid
   * CY_U3P_ERROR_NULL_POINTER   - If the threshold_p is NULL.
   * CY_U3P_ERROR_NOT_CONFIGURED - If the GPIO is not enabled.

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
CyU3PReturnStatus_t
CyU3PGpioComplexGetThreshold (
        uint8_t gpioId,         /* The complex GPIO to sample. */
        uint32_t *threshold_p   /* Returns the current threshold value for the GPIO. */
        );

/* Summary
   Sample the GPIO timer value at an instant.

   Description
   The function configures the complex GPIO to sample the current value
   of the timer. To use this feature, the GPIO should be configured as
   CY_U3P_GPIO_MODE_STATIC and the timer should be operational.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the IO is invalid.
   * CY_U3P_ERROR_NULL_POINTER   - If the value_p is NULL.
   * CY_U3P_ERROR_NOT_CONFIGURED - If the GPIO is not enabled.
   * CY_U3P_ERROR_NOT_SUPPORTED  - Invalid configuration.

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
CyU3PReturnStatus_t
CyU3PGpioComplexSampleNow (
        uint8_t gpioId,         /* The complex GPIO to sample. */
        uint32_t *value_p       /* Returns the current value of the timer when the API was
                                   invoked. */
        );

/* Summary
   Applies a pulse on the corresponding GPIO immediately.

   Description
   The function configures the complex GPIO to to apply a
   pulse on the associated GPIO. The behaviour is as follows:
   Toggles the GPIO value immediately, sets timer = 0, then
   toggles output back when timer == threshold. Once this is done,
   the GPIO mode is reverted back to static. This API does not
   wait until the pulse operation is completed, but just initiates
   the operation. To use this mode, the GPIO should be in output mode,
   and in CY_U3P_GPIO_MODE_STATIC and the timer should be operational.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the IO or threshold is invalid.
   * CY_U3P_ERROR_NOT_CONFIGURED - If the IO is not enabled.
   * CY_U3P_ERROR_NOT_SUPPORTED  - If the GPIO is not configured correctly for
                                   this operation.

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
CyU3PReturnStatus_t
CyU3PGpioComplexPulseNow (
        uint8_t gpioId,         /* The complex GPIO to pulse now. */
        uint32_t threshold      /* Updates the threshold value for the GPIO. */
        );

/* Summary
   Applies a pulse on the corresponding GPIO.

   Description
   The function configures the complex GPIO to to apply a
   pulse on the associated GPIO. The behaviour is as follows:
   Toggles the GPIO value when the timer value becomes 0, then
   toggles output back when timer == threshold. Once this is done,
   the GPIO mode is reverted back to static. This API does not
   wait until the pulse operation is completed, but just initiates
   the operation. To use this mode, the GPIO should be in output mode,
   and in CY_U3P_GPIO_MODE_STATIC and the timer should be operational.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the IO or threshold is invalid.
   * CY_U3P_ERROR_NOT_CONFIGURED - If the IO is not enabled.
   * CY_U3P_ERROR_NOT_SUPPORTED  - If the GPIO is not configured correctly for
                                   this operation.

   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexMeasureOnce
 */
CyU3PReturnStatus_t
CyU3PGpioComplexPulse (
        uint8_t gpioId,         /* The complex GPIO to pulse now. */
        uint32_t threshold      /* Updates the threshold value for the GPIO. */
        );

/* Summary
   Measures various values when the complex GPIO is configured.
   as input

   Description
   The function configures the complex GPIO to to measure the following
   features:
   CY_U3P_GPIO_MODE_MEASURE_LOW_ONCE    - Low period for input.
   CY_U3P_GPIO_MODE_MEASURE_HIGH_ONCE   - High period for input.
   CY_U3P_GPIO_MODE_MEASURE_NEG_ONCE    - Time to negative edge.
   CY_U3P_GPIO_MODE_MEASURE_POS_ONCE    - Time to positive edge.
   CY_U3P_GPIO_MODE_MEASURE_ANY_ONCE    - Time to negative or positve edge (transition).
   To use this mode, the GPIO should be in input mode,
   and in CY_U3P_GPIO_MODE_STATIC and the timer should be operational.
   This API does not wait for the measurement to complete. Invoke the
   CyU3PGpioComplexWaitForCompletion API to wait for completion or wait
   until the threshold field is non-zero. This is a busy loop until the
   operation is completed.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If IO or the mode specified is invalid.
   * CY_U3P_ERROR_NOT_CONFIGURED - If the IO is not enabled.
   * CY_U3P_ERROR_NOT_SUPPORTED  - If the GPIO is not configured correctly for
                                   this operation.
   See Also
   * CyU3PGpioComplexMode_t
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexWaitForCompletion
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
 */
CyU3PReturnStatus_t
CyU3PGpioComplexMeasureOnce (
        uint8_t gpioId,                 /* The complex GPIO to measure. */
        CyU3PGpioComplexMode_t pinMode  /* Mode to be used for measurement. */
        );

/* Summary
   Waits for completion of MeasureOnce, Pulse and PulseNow operations.

   Description
   The function waits for the following modes to complete:
   CY_U3P_GPIO_MODE_PULSE_NOW, CY_U3P_GPIO_MODE_PULSE,
   CY_U3P_GPIO_MODE_MEASURE_LOW_ONCE, CY_U3P_GPIO_MODE_MEASURE_HIGH_ONCE,
   CY_U3P_GPIO_MODE_MEASURE_NEG_ONCE, CY_U3P_GPIO_MODE_MEASURE_POS_ONCE
   and CY_U3P_GPIO_MODE_MEASURE_ANY_ONCE.
   If the isWait option is CyTrue, this is a busy loop until the operation
   is completed. If the isWait option is CyFalse, it just check if the
   operation completed. If the operation is completed, the API returns
   CY_U3P_SUCCESS and if the operation is still on-going, then returns
   CY_U3P_ERROR_TIMEOUT. A timeout error is not an abort. It is just an
   indication that the operation is still on-going. The API can be called
   again to check if the operation is completed.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_NOT_STARTED    - If the GPIO block has not been initialized
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the Drive strength requested is invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - If the IO is not enabled.
   * CY_U3P_ERROR_NOT_SUPPORTED  - If the GPIO is not configured correctly for
                                   this operation.
   See Also
   * CyU3PGpioInit
   * CyU3PGpioDeInit
   * CyU3PGpioDisable
   * CyU3PGpioSetComplexConfig
   * CyU3PGpioSetValue
   * CyU3PGpioGetValue
   * CyU3PGpioGetIOValues
   * CyU3PGpioComplexUpdate
   * CyU3PGpioComplexGetThreshold
   * CyU3PGpioComplexSampleNow
   * CyU3PGpioComplexPulseNow
   * CyU3PGpioComplexPulse
   * CyU3PGpioComplexMeasureOnce
 */
CyU3PReturnStatus_t
CyU3PGpioComplexWaitForCompletion (
        uint8_t gpioId,         /* The complex GPIO to wait for completion. */
        uint32_t *threshold_p,  /* The current threshold value after completion. */
        CyBool_t isWait         /* Whether to wait for completion or just to check if
                                   the operation is completed. */
        );

/* Summary
   This function register the call back function for notification of GPIO interrupt.

   Description
   This function registers a callback function that will be called for notification of GPIO interrupts
   and also selects the GPIO interrupt sources of interest.

   Return Value
   None

   See Also
   * None
 */
void
CyU3PRegisterGpioCallBack (
                           CyU3PGpioIntrCb_t gpioIntrCb);

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3_UART_H_ */

/*[]*/
