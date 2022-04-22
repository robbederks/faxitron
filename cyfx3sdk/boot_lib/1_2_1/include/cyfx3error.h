/*
 ## Cypress USB 3.0 Platform header file (cyfx3error.h)
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

#ifndef __CYFX3ERROR_H__
#define __CYFX3ERROR_H__

#include <cyu3externcstart.h>

/* Summary:
   Enumeration of error codes returned by the Bootloader
*/
typedef enum CyFx3BootErrorCode_t
{
   CY_FX3_BOOT_SUCCESS = 0x0,               /* Success. */
   CY_FX3_BOOT_ERROR_BAD_ARGUMENT,          /* One or more parameters to a function are invalid. */
   CY_FX3_BOOT_ERROR_NULL_POINTER,          /* A null pointer has been passed in unexpectedly. */
   CY_FX3_BOOT_ERROR_TIMEOUT,               /* Timeout on relevant operation. */
   CY_FX3_BOOT_ERROR_NOT_SUPPORTED,         /* Operation requested is not supported in current mode. */
   CY_FX3_BOOT_ERROR_NOT_CONFIGURED,        /* Peripheral block is not configured. */
   CY_FX3_BOOT_ERROR_BAD_DESCRIPTOR_TYPE,   /* Invalid USB descriptor type. */
   CY_FX3_BOOT_ERROR_XFER_FAILURE,          /* Data Transfer failed. */
   CY_FX3_BOOT_ERROR_NO_REENUM_REQUIRED,    /* Indicates that the booter has successfully configured the FX3 device
                                               after control was transferred from the application. The user need not
                                               go through the cycle of setting the descriptors and issuing connect
                                               call if this error code is returned from the CyFx3BootUsbStart (). */
   CY_FX3_BOOT_ERROR_NOT_STARTED,           /* Indicates that the block being configured has not been initialized. */
   CY_FX3_BOOT_ERROR_MEMORY_ERROR,          /* Indicates that the API was unable to find enough memory to copy the
                                               descriptor set through the CyFx3BootUsbSetDesc() API. */
   CY_FX3_BOOT_ERROR_FAILURE                /* Generic error code. */
} CyFx3BootErrorCode_t;

#include <cyu3externcend.h>

#endif /* __CYFX3ERROR_H__ */

