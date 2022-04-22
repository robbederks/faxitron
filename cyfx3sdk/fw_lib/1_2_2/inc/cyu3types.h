/*
 ## Cypress USB 3.0 Platform header file (cyu3types.h)
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

#ifndef _INCLUDED_CYU3TYPES_H_
#define _INCLUDED_CYU3TYPES_H_

/*
 * Definitions for basic types used in the West Bridge firmware sources.
 * If a compiler provided version is available, use it for most of these
 * definitions by not defining CYU3_DEFINE_BASIC_TYPES.
 */
#ifdef CYU3_DEFINE_BASIC_TYPES

/* Signed integer types. */
typedef signed   char      int8_t;
typedef signed   short     int16_t;
typedef signed   int       int32_t;
typedef signed   long long int64_t;     /* Use with caution. */

/* Unsigned integer types. */
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;    /* Use with caution. */

/* MACROS for min and max values for various types. */

#define INT8_MIN        (-128)
#define INT8_MAX        (127)
#define UINT8_MAX       (255)

#define INT16_MIN       (-32768)
#define INT16_MAX       (32767)
#define UINT16_MAX      (65535)

#define INT32_MIN       (~0x7FFFFFFF)
#define INT32_MAX       (2147483647)
#define UINT32_MAX      (4294967295u)

#else

/* Use the standard (compiler provided) versions. */
#include <stdint.h>

#endif

/*
 * Unsigned volatile integer types for register access. These are not
 * expected to be provided by the compiler.
 */

typedef volatile unsigned char  uvint8_t;
typedef volatile unsigned short uvint16_t;
typedef volatile unsigned long  uvint32_t;

typedef int CyBool_t;
#define CyTrue                  (1)
#define CyFalse                 (0)

/* Data type for return codes. 0 indicates success. The specific error codes
 * supported are defined separately. */
typedef unsigned int CyU3PReturnStatus_t;

#endif /* _INCLUDED_CYU3TYPES_H_ */

/*[]*/

