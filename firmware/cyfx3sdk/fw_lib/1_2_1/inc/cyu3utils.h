/*
 ## Cypress USB 3.0 Platform header file (cyu3utils.h)
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

#ifndef _INCLUDED_CYU3P_UTILS_H_
#define _INCLUDED_CYU3P_UTILS_H_

#include "cyu3types.h"
#include "cyu3externcstart.h"
#include "assert.h"

/*@@Utility Functions
   Summary
   Utility functions provided for ease of use.

   Description
   The utility APIs functions are generic functions that provides
   easy usage function. They are also used by the FX3 library for
   its internal use.
*/

/**************************************************************************
 ********************************** Macros ********************************
 **************************************************************************/

/*
   Summary
   Minimum of two numbers.

   Description
   The macro determines the smaller of the two
   numbers passed to the function.
*/
#define CY_U3P_MIN(a, b)         (((a) > (b)) ? (b) : (a))

/*
   Summary
   Maximum of two numbers.

   Description
   The macro determines the larger of the two
   numbers passed to the function.
*/
#define CY_U3P_MAX(a, b)         (((a) > (b)) ? (a) : (b))

/*
   Summary
   Creates a word from two eight bit numbers.

   Description
   The macro combines two eight bit unsigned numbers
   to form a single 16 bit unsigned number.
*/
#define CY_U3P_MAKEWORD(u, l)    ((uint16_t)(((u) << 8) | (l)))

/*
   Summary
   Retrieves the LSB from a 16 bit number.

   Description
   The macro retrieves the least significant byte from
   a 16 bit unsigned number. The numbers are expected to
   be stored in little endian format.
*/
#define CY_U3P_GET_LSB(w)        ((uint8_t)((w) & UINT8_MAX))

/*
   Summary
   Retrieves the MSB from a 16 bit number.

   Description
   The macro retrieves the most significant byte from
   a 16 bit unsigned number. The numbers are expected to
   be stored in little endian format.
*/
#define CY_U3P_GET_MSB(w)        ((uint8_t)((w) >> 8))

/*
   Summary
   Creates a word from four eight bit numbers.

   Description
   The macro combines four eight bit unsigned numbers
   to form a single 32 bit unsigned number.
*/
#define CY_U3P_MAKEDWORD(b3, b2, b1, b0) ((uint32_t)((((uint32_t)(b3)) << 24) | (((uint32_t)(b2)) << 16) | \
                                         (((uint32_t)(b1)) << 8) | ((uint32_t)(b0))))

/*
   Summary
   Retrieves byte 0 from a 32 bit number.

   Description
   The macro retrieves the first byte (LSB) from a 32 bit number.
   The numbers are expected to be stored in little endian format.
*/
#define CY_U3P_DWORD_GET_BYTE0(d)        ((uint8_t)((d) & 0xFF))

/*
   Summary
   Retrieves byte 1 from a 32 bit number.

   Description
   The macro retrieves the second byte from a 32 bit number.
   The numbers are expected to be stored in little endian format.
*/
#define CY_U3P_DWORD_GET_BYTE1(d)        ((uint8_t)(((d) >>  8) & 0xFF))

/*
   Summary
   Retrieves byte 2 from a 32 bit number.

   Description
   The macro retrieves the third byte from a 32 bit number.
   The numbers are expected to be stored in little endian format.
*/
#define CY_U3P_DWORD_GET_BYTE2(d)        ((uint8_t)(((d) >> 16) & 0xFF))

/*
   Summary
   Retrieves byte 3 from a 32 bit number.

   Description
   The macro retrieves the fourth byte from a 32 bit number.
   The numbers are expected to be stored in little endian format.
*/
#define CY_U3P_DWORD_GET_BYTE3(d)        ((uint8_t)(((d) >> 24) & 0xFF))

/*
   Summary
   Assert function.

   Description
   If the assert expression evaluates to FALSE, the assert
   macro calls the __aeabi_assert() function.
*/
#define CyU3PAssert(cond)         assert(cond)

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/* Summary
   Copy data word-wise from one memory location to another.

   Description
   This is a memcpy equivalent function. This requires that the addresses
   provided are four byte aligned. Since the ARM core is 32-bit, this API
   does a faster copy of data than the 1 byte equivalent (CyU3PMemCopy).
   This API is also used by the firmware library. The implementation does
   not handle the case of overlapping buffers.

   Return value
   None

   See Also
   CyU3PMemCopy
 */
void
CyU3PMemCopy32 (
        uint32_t *dest,                  /* Pointer to destination buffer. */
        uint32_t *src,                   /* Pointer to source buffer. */
        uint32_t count                   /* Size of the buffer in words
                                            to be copied. */
        );

/* Summary
   Delay function based on busy spinning in a loop.
  
   Description
   This function is used to insert small delays (of the order of micro-seconds) into
   the firmware application. The delay is implemented as a busy spin loop and can be
   used anywhere. The delay loop when operating at 208MHz provides about 1us. This
   API should not be used for large delays as other lower or same priority threads
   will not be able to run during this.
  
   Return value
   None

   See Also
   CyU3PThreadSleep
 */
extern void
CyU3PBusyWait (
        uint16_t usWait                 /* Delay duration in micro-seconds. */
        );

/* Summary
   Compute a checksum over a user specified data buffer.
  
   Description
   This function computes the binary sum of all values in a user specified data buffer
   and can be used as a simple checksum to verify data consistency. This checksum API
   is used by the boot-loader to determine the checksum as well.
  
   Return value:
   * CY_U3P_SUCCESS if the checksum is successfully computed.
   * CY_U3P_ERROR_BAD_ARGUMENT if the parameters provided are erroneous.
 */
extern CyU3PReturnStatus_t
CyU3PComputeChecksum (
        uint32_t *buffer,               /* Pointer to data buffer on which to calculate the checksum. */
        uint8_t   length,               /* Length of data buffer on which to calculate the checksum. */
        uint32_t *chkSum                /* Pointer to buffer that will be filled with the checksum. */
        );

/* Summary
   Function to read one or more FX3 device registers.

   Description
   The FX3 device hardware implements a number of Control and Status registers that
   govern the behavior of and report the current status of each of the blocks. This
   function is used to read one or more contiguous registers from the register space
   of the FX3 device.

   Return value:
   * CY_U3P_SUCCESS if the register read(s) is/are successful.
   * CY_U3P_ERROR_BAD_ARGUMENT if the arguments passed in are erroneous.

   See Also
   * CyU3PWriteDeviceRegisters
 */
extern CyU3PReturnStatus_t
CyU3PReadDeviceRegisters (
        uvint32_t *regAddr,             /* Address of first register to be read. */
        uint8_t    numRefs,             /* Number of registers to be read. */
        uint32_t  *dataBuf              /* Pointer to data buffer into which the registers are to be read. */
        );

/* Summary
   Function to write one or more FX3 device registers.

   Description
   This function is used to write one or more contiguous registers from the register
   space of the FX3 device.

   Note
   Use this function with caution and preferably under guidance from Cypress support
   personnel. The function does not implement any validity/side effect checks on the
   values being written into the registers.

   Return Value
   * CY_U3P_SUCCESS if the register write(s) is/are successful.
   * CY_U3P_ERROR_BAD_ARGUMENT if the arguments passed in are erroneous.

   See Also
   * CyU3PReadDeviceRegisters
 */
extern CyU3PReturnStatus_t
CyU3PWriteDeviceRegisters (
        uvint32_t *regAddr,             /* Address of first register to be written. */
        uint8_t    numRefs,             /* Number of registers to be written. */
        uint32_t  *dataBuf              /* Pointer to data buffer containing data to be written to the registers. */
        );

#if 0
extern void
__aeabi_memset (
        void   *dest,
        size_t  n,
        int     c);
extern void
__aeabi_memset4 (
        void   *dest,
        size_t  n,
        int     c);
extern void
__aeabi_memclr (
        void   *dest,
        size_t  n);
extern void
__aeabi_memclr4 (
        void   *dest,
        size_t  n);
extern void
__aeabi_memcpy (
        void       *dest,
        const void *src,
        size_t      n);
extern void
__aeabi_memcpy4 (
        void       *dest,
        const void *src,
        size_t      n);
#endif

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3P_UTILS_H_ */

/*[]*/

