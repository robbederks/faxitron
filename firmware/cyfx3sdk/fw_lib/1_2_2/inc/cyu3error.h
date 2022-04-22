/*
 ## Cypress USB 3.0 Platform header file (cyu3error.h)
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

#ifndef _INCLUDED_CYU3ERROR_H_
#define _INCLUDED_CYU3ERROR_H_

#include "cyu3types.h"
#include "cyu3externcstart.h"

/* This file lists the error codes that are returned by the firmware library functions. */

/*@@Error Codes
   Summary
   Error codes returned by FX3 APIs.

   Description
   The FX3 API error codes can be used to identify the cause of failure.
   Each API has documentation about its return values.
*/

/* Summary
   List of error codes defined and returned by the FX3 firmware library.

   Description
   The Error codes help identify the cause of the failure. These error
   can be RTOS returned error codes or API returned codes. The API error
   codes start from 0x40.
 */
typedef enum  CyU3PErrorCode_t
{
    CY_U3P_SUCCESS = 0,                 /* Success code */

    /* RTOS defined error codes. These need to be updated as part of porting a new RTOS. */
    CY_U3P_ERROR_DELETED,               /* The OS object being accessed has been deleted. */
    CY_U3P_ERROR_BAD_POOL,              /* Bad memory pool passed to a function. */
    CY_U3P_ERROR_BAD_POINTER,           /* Bad (NULL or unaligned) pointer passed to a function. */
    CY_U3P_ERROR_INVALID_WAIT,          /* Non-zero wait requested from interrupt context. */
    CY_U3P_ERROR_BAD_SIZE,              /* Invalid size value passed into a function. */
    CY_U3P_ERROR_BAD_EVENT_GRP,         /* Invalid event group passed into a function. */
    CY_U3P_ERROR_NO_EVENTS,             /* Failed to set/get the event flags specified. */
    CY_U3P_ERROR_BAD_OPTION,            /* Invalid task option value specified for the function. */
    CY_U3P_ERROR_BAD_QUEUE,             /* Invalid message queue passed to a function. */
    CY_U3P_ERROR_QUEUE_EMPTY,           /* The message queue being read is empty. */
    CY_U3P_ERROR_QUEUE_FULL,            /* The message queue being written to is full. */
    CY_U3P_ERROR_BAD_SEMAPHORE,         /* Invalid semaphore pointer passed to a function. */
    CY_U3P_ERROR_SEMGET_FAILED,         /* A semaphore get operation failed. */
    CY_U3P_ERROR_BAD_THREAD,            /* Invalid thread pointer passed to a function. */
    CY_U3P_ERROR_BAD_PRIORITY,          /* Invalid thread priority value passed to a function. */
    CY_U3P_ERROR_MEMORY_ERROR,          /* Failed to allocate memory. */
    CY_U3P_ERROR_DELETE_FAILED,         /* Failed to delete an object because it is not idle. */
    CY_U3P_ERROR_RESUME_FAILED,         /* Failed to resume a thread. */
    CY_U3P_ERROR_INVALID_CALLER,        /* OS function failed because the current caller is not allowed. */
    CY_U3P_ERROR_SUSPEND_FAILED,        /* Failed to suspend a thread. */
    CY_U3P_ERROR_BAD_TIMER,             /* Invalid timer pointer passed to a function. */
    CY_U3P_ERROR_BAD_TICK,              /* Invalid (0) tick value passed to a timer function. */
    CY_U3P_ERROR_ACTIVATE_FAILED,       /* Failed to activate a timer. */
    CY_U3P_ERROR_BAD_THRESHOLD,         /* Invalid thread pre-emption threshold value specified. */
    CY_U3P_ERROR_SUSPEND_LIFTED,        /* Thread suspension was cancelled. */
    CY_U3P_ERROR_WAIT_ABORTED,          /* Wait operation was aborted. */
    CY_U3P_ERROR_WAIT_ABORT_FAILED,     /* Failed to abort wait operation on a thread. */
    CY_U3P_ERROR_BAD_MUTEX,             /* Invalid Mutex pointer passed to a function. */
    CY_U3P_ERROR_MUTEX_FAILURE,         /* Failed to get a mutex. */
    CY_U3P_ERROR_MUTEX_PUT_FAILED,      /* Failed to put a mutex because it is not currently owned. */
    CY_U3P_ERROR_INHERIT_FAILED,        /* Error in priority inheritance. */
    CY_U3P_ERROR_NOT_IDLE,              /* Operation failed because relevant object is not idle or done. */

    /* Error codes defined in the FX3 firmware library. These values are offset from the RTOS defined
       error code values. */
    CY_U3P_ERROR_BAD_ARGUMENT = 0x40,   /* One or more parameters to a function are invalid. */
    CY_U3P_ERROR_NULL_POINTER,          /* A null pointer has been passed in unexpectedly. */
    CY_U3P_ERROR_NOT_STARTED,           /* The object/module being referred to has not been started. */
    CY_U3P_ERROR_ALREADY_STARTED,       /* An object/module that is already active is being started. */
    CY_U3P_ERROR_NOT_CONFIGURED,        /* Object/module referred to has not been configured. */
    CY_U3P_ERROR_TIMEOUT,               /* Timeout on relevant operation. */
    CY_U3P_ERROR_NOT_SUPPORTED,         /* Operation requested is not supported in current mode. */
    CY_U3P_ERROR_INVALID_SEQUENCE,      /* Invalid function call sequence. */
    CY_U3P_ERROR_ABORTED,               /* Function call failed as it was aborted by another thread/isr. */
    CY_U3P_ERROR_DMA_FAILURE,           /* DMA engine failed to completed requested operation. */
    CY_U3P_ERROR_FAILURE,               /* Failure due to a non-specific system error. */
    CY_U3P_ERROR_BAD_INDEX,             /* Bad index value was passed in as parameter. Ex: for string descriptor. */
    CY_U3P_ERROR_BAD_ENUM_METHOD,       /* Bad enumeration method specified. */
    CY_U3P_ERROR_INVALID_CONFIGURATION, /* Invalid configuration specified. */
    CY_U3P_ERROR_CHANNEL_CREATE_FAILED, /* Internal DMA channel creation failed. */
    CY_U3P_ERROR_CHANNEL_DESTROY_FAILED,/* Internal DMA channel destroy failed. */
    CY_U3P_ERROR_BAD_DESCRIPTOR_TYPE,   /* Invalid descriptor type specified. */
    CY_U3P_ERROR_XFER_CANCELLED,        /* USB transfer was cancelled. */
    CY_U3P_ERROR_FEATURE_NOT_ENABLED,   /* When a USB feature like remote wakeup is not enabled. */
    CY_U3P_ERROR_STALLED,               /* When a USB request / data transfer is stalled. */
    CY_U3P_ERROR_BLOCK_FAILURE,         /* The block accessed has a fatal error and needs to be re-initialized. */
    CY_U3P_ERROR_LOST_ARBITRATION,      /* Loss of bus arbitration, invalid bus behaviour or bus busy.  */
    CY_U3P_ERROR_STANDBY_FAILED,        /* Failed to enter standby mode because one or more wakeup events are active. */
    CY_U3P_ERROR_NO_REENUM_REQUIRED = 0xFE, /* FX3 booter supports the NoReEnumeration feature that enables to have 
                                           a single USB enumeration across the FX3 booter and the final application.
                                           This error code is returned by CyU3PUsbStart () to indicate that the 
                                           NoReEnumeration is successful and the sequence followed for the regular
                                           enumeration post CyU3PUsbStart () is to be skipped. */
    CY_U3P_ERROR_OPERN_DISABLED = 0xFF  /* The requested feature / operation is not enabled in current configuration. */

} CyU3PErrorCode_t;

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3ERROR_H_ */

/*[]*/

