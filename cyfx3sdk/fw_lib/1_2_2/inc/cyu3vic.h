/*
 ## Cypress USB 3.0 Platform header file (cyu3vic.h)
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

#ifndef _INCLUDED_CYU3PVIC_H_
#define _INCLUDED_CYU3PVIC_H_

/* Summary
 * Internal functions for managing the VIC on the FX3 device.
 */

#include "cyu3types.h"
#include "cyu3externcstart.h"

/**************************************************************************
 ******************************* Data types *******************************
 **************************************************************************/

/* Summary
 * Vector numbers for different interrupt sources.
 *
 * Description
 * The following enumeration gives the allowed VIC vector numbers.
 */
typedef enum
{
    CY_U3P_VIC_GCTL_CORE_VECTOR = 0,
    CY_U3P_VIC_SWI_VECTOR,
    CY_U3P_VIC_DEBUG_RX_VECTOR,
    CY_U3P_VIC_DEBUG_TX_VECTOR,
    CY_U3P_VIC_WDT_VECTOR,
    CY_U3P_VIC_BIAS_CORRECT_VECTOR,             /*  5 */
    CY_U3P_VIC_PIB_DMA_VECTOR,
    CY_U3P_VIC_PIB_CORE_VECTOR,
    CY_U3P_VIC_UIB_DMA_VECTOR,
    CY_U3P_VIC_UIB_CORE_VECTOR,
    CY_U3P_VIC_UIB_CONTROL_VECTOR,              /* 10 */
    CY_U3P_VIC_SIB_DMA_VECTOR,
    CY_U3P_VIC_SIB0_CORE_VECTOR,
    CY_U3P_VIC_SIB1_CORE_VECTOR,
    CY_U3P_VIC_RESERVED_15_VECTOR,
    CY_U3P_VIC_I2C_CORE_VECTOR,                 /* 15 */
    CY_U3P_VIC_I2S_CORE_VECTOR,
    CY_U3P_VIC_SPI_CORE_VECTOR,
    CY_U3P_VIC_UART_CORE_VECTOR,
    CY_U3P_VIC_GPIO_CORE_VECTOR,
    CY_U3P_VIC_LPP_DMA_VECTOR,                  /* 20 */
    CY_U3P_VIC_GCTL_PWR_VECTOR
} CyU3PVicVector_t;

/**************************************************************************
 ********************** Global variable declarations **********************
 **************************************************************************/

extern uint16_t glOsTimerInterval;  /* Internal variable for OS timer
                                       configuration. Should not be
                                       modified from outside the library. */

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/* Summary
   Enable the interrupt for the specified vector number.
  
   Description
   The interrupts are disabled by default and the required interrupt must
   be enabled as per requirement.
  
   Return value
   None

   See Also
   CyU3PVicDisableInt
   CyU3PVicClearInt
 */
extern void
CyU3PVicEnableInt(
        uint32_t vectorNum /* Vector number to be enabled (0 - 31). */
        );

/* Summary
   Disable the interrupt for the specified vector number.
  
   Description
   The interrupts are disabled by default. This function disables the
   selected interrupt.
  
   Return value
   None
   
   See Also
   CyU3PVicEnableInt
   CyU3PVicClearInt
 */
extern void
CyU3PVicDisableInt (
        uint32_t vectorNum /* interrupt vector number (0 - 31) */
        );

/* Summary
   Clear any interrupt that has occurred.
  
   Description
   The current interrupt has to be cleared before other interrupts
   can be handled. This source of the interrupt must be cleared before this.
  
   Return value
   None
   
   See Also
   CyU3PVicEnableInt
   CyU3PVicDisableInt
 */
extern void
CyU3PVicClearInt (
        void);

/* Summary
   Get the raw interrupt status.
  
   Description
   The status of each interrupt is represented by individual bits.
  
   Return value
   The status of each interrupts. The return is a 32 bit data,
   each bit specifying the status of the (0 - 31) interrupt vectors.
   
   See Also
   CyU3PVicIRQGetStatus
   CyU3PVicFIQGetStatus
 */
extern uint32_t
CyU3PVicIntGetStatus (
        void); 

/* Summary
   Get the IRQ interrupt status.
  
   Description
   The status of each interrupt is represented by individual bits.
  
   Return value
   The status of each interrupts. The return is a 32 bit data,
   each bit specifying the status of the (0 - 31) interrupt vectors.
   
   See Also
   CyU3PVicIntGetStatus
   CyU3PVicFIQGetStatus
 */
extern uint32_t
CyU3PVicIRQGetStatus (
        void);

/* Summary
   Get the FIQ interrupt status.
  
   Description
   The status of each interrupt is represented by individual bits.
  
   Return value
   The status of each interrupts. The return is a 32 bit data,
   each bit specifying the status of the (0 - 31) interrupt vectors.
   
   See Also
   CyU3PVicIntGetStatus
   CyU3PVicIRQGetStatus
 */
extern uint32_t
CyU3PVicFIQGetStatus (
        void);

/* Summary
   Set the priority level of the interrupt vector.
  
   Description
   There are 16 priority levels. 0 being the highest and 15 being the
   lowest.
  
   Return value
   None
   
   See Also
   CyU3PVicIntGetPriority
 */
extern void
CyU3PVicIntSetPriority (
        uint32_t vectorNum, /* interrupt vector number (0 - 31) */
        uint32_t priority   /* priority level to be set (0 - 15) */
        );

/* Summary
   Get the priority level of the interrupt vector.
  
   Description
   There are 16 priority levels. 0 being the highest and 16 being the
   lowest.
  
   Parameters
   vectorNum: The interrupt vector number (0 - 31).
  
   Return value
   The priority level for the particular interrupt vector.
   
   See Also
   CyU3PVicIntSetPriority
 */
extern uint32_t
CyU3PVicIntGetPriority (
        uint32_t vectorNum);

/* Summary
   Set the priority level mask.
  
   Description
   Each bit of the priority mask corresponds to the interrupts
   with that particular priority and a 1 corresponding to the priority 
   masks interrupts with that priority level.
  
   Return value
   None
   
   See Also
   CyU3PVicIntGetPrioriryMask
 */
extern void
CyU3PVicIntSetPrioriryMask (
        uint32_t priorityMask /* priority mask value to be set. (Bits 0 - 15) */
        );

/* Summary
   Get the priority level mask.
  
   Description
   Each bit of the priority mask corresponds to the interrupts
   with that particular priority and a 1 corresponding to the priority 
   masks interrupts with that priority level.
  
   Return value
   The existing priority mask (Bits 0 - 15).
   
   See Also
   CyU3PVicIntSetPrioriryMask
 */
extern uint32_t
CyU3PVicIntGetPrioriryMask (
        void);

/* Summary
   The function initializes the VIC.
  
   Description
   The function is invoked internally during device initialization.
  
   Return value
   None
  
   See Also
   CyU3PVicSetupIntVectors
 */
extern void
CyU3PVicInit (
        void);

/* Summary
   The function initializes the interrupt vector table.
  
   Description
   The function is invoked internally during VIC initialization.
  
   Return value
   None
  
   See Also
   CyU3PVicInit
 */
extern void
CyU3PVicSetupIntVectors (
        void);

/* Summary
   This function disables all FX3 interrupts at the VIC level.
  
   Description
   This function can be used to disable all FX3 interrupts at the VIC level.
   The function returns a mask that represent the interrupts that were enabled
   before this function was called. It is expected that this mask would be used
   to re-enable the interrupts using the CyU3PVicEnableInterrupts function.
  
   Return value
   A uint32_t mask that represents the interrupts that were enabled.
  
   See Also
   * CyU3PVicEnableInterrupts
 */
extern uint32_t
CyU3PVicDisableAllInterrupts (
        void);

/* Summary
   This function enables the specified interrupts at the VIC level.
  
   Description
   This function can be used to re-enable interrupts that were previously
   disabled through the CyU3PVicDisableAllInterrupts function. These two
   functions can be used together to save and restore interrupt states
   across critical sections of code.
  
   Return value
   None
  
   See Also
   * CyU3PVicDisableAllInterrupts
 */
extern void
CyU3PVicEnableInterrupts (
        uint32_t mask  /* Bit-mask representing interrupts to be enabled. */
        );

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3PVIC_H_ */

/*[]*/

