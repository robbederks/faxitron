/* Cypress West Bridge API header file (cywbtypes.h)
 ## ===========================
 ##
 ##  Copyright Cypress Semiconductor Corporation, 2009-2011,
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

#ifndef _INCLUDED_CYWBTYPES_H_
#define _INCLUDED_CYWBTYPES_H_

/* Summary
   If set to TRUE, the basic numeric types are defined by the West Bridge API code

   Description
   The West Bridge API relies on some basic integral types to be defined.  These types include
   uint8_t, int8_t, uint16_t, int16_t, uint32_t, and int32_t.  If this macro is defined, the
   West Bridge API will define these types based on some basic assumptions.  If this value is
   set and the West Bridge API is used to set these types, the definition of these types must
   be examined to insure that they are appropriate for the given target architecture and
   compiler.

   Notes
   It is preferred that if the basic platform development environment defines these types
   that the CY_DEFINE_BASIC_TYPES macro be undefined and the appropriate target system header 
   file be added here.
*/
#ifdef CY_DEFINE_BASIC_TYPES

/*
 * Define the basic types.  If these are not defined by your system, define these
 * here by defining the symbol CY_DEFINE_BASIC_TYPES
 */
typedef signed char    int8_t;
typedef signed short   int16_t;
typedef signed int     int32_t;
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

#else

/*
 * Include your target system header file that defines the basic types here if at all
 * possible.
 */
#include <stdint.h>

#endif

#if !defined(__doxygen__)
typedef unsigned char cy_bool;
#define cy_true                          (1)
#define cy_false                         (0)
#endif

/* Types that are not available on specific platforms.
 * These are used only in the reference HAL implementations and
 * are not required for using the API.
 */
#ifdef __unix__
typedef unsigned long DWORD;
typedef void *        LPVOID;
#define WINAPI
#define INFINITE        (0xFFFFFFFF)
#define ptr_to_uint(ptr)  ((unsigned int)(ptr))
#endif

/* Basic types used by the entire API */

/* Summary
   This type represents a socket number.

   Description
   Block data transfers between the Processor and the West Bridge device are done through
   a set of sockets, where each socket is a uni-directional pipe into the Benicia buffers.
   The socket number identifies a specific pipe through which a data flow can be managed.
 */
typedef uint8_t cy_wb_socket_number_t;

/* Summary
   Invalid socket number id. Used to denote errors in socket selection.
 */
#define CYWB_SOCKET_INVALID                     ((cy_wb_socket_number_t)(0xFF))

/* Summary
   This type is used to return status information from an API call.
 */
typedef uint16_t cy_wb_return_status_t;

/* Summary
   This type represents a port number that can be used for storage API functions.
 */
typedef enum cy_wb_stor_port_t
{
    cy_wb_stor_port_s0 = 0,                     /* S0 port. */
    cy_wb_stor_port_s1,                         /* S1 port. */
    cy_wb_stor_port_usb                         /* USB peripheral port. */
} cy_wb_stor_port_t;

/* Summary
   List of LPP modules.

   Description
   This type lists the various LPP Module types supported for read/write
   functions.
 */
typedef enum cy_wb_lpp_intf_t {
    cy_wb_intf_uart = 0,                        /* UART interface. */
    cy_wb_intf_i2c,                             /* I2C interface. */
    cy_wb_intf_i2s,                             /* I2S interface. */
    cy_wb_intf_spi,                             /* SPI interface. */
    cy_wb_intf_max                              /* Sentinel value. */
} cy_wb_lpp_intf_t;

/* Summary
   All APIs provided with this release are marked extern through this definition.
   This definition can be changed to meet the scope changes required in the user
   build environment.

   For example, this can be changed to __declspec(exportdll) to enable exporting
   the API from a DLL.
 */
#define EXTERN extern

/* Summary
   Type definition for structure that stores all West Bridge device related state.

   Description
   Type definition for structure that stores all West Bridge device related state.
   The actual structure is defined elsewhere.
 */
typedef struct cy_wb_device cy_wb_device;

#endif /* _INCLUDED_CYWBTYPES_H_ */

