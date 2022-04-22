/*
 ## Cypress USB 3.0 Platform header file (cyfx3utils.h)
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

#ifndef __CYFX3UTILS_H__
#define __CYFX3UTILS_H__

#include <cyu3types.h>

#include <cyu3externcstart.h>
/* Summary
   Delay subroutine.

   Description
   A simple delay subroutine. The period is in terms of microseconds.
   The sys clock frequency is assumed to be 416 MHz for this calculation.
*/
extern void
CyFx3BootBusyWait (
        uint16_t usWait /* Number of microseconds to wait */
        );

#include <cyu3externcend.h>
#endif /* __CYFX3UTILS_H__ */

