/*
## Cypress USB 3.0 Platform header file (cyfx3uart.h)
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

#ifndef _INCLUDED_CYFX3_UART_H_
#define _INCLUDED_CYFX3_UART_H_

#include <cyu3types.h>
#include <cyfx3error.h>
#include <cyu3externcstart.h>

/* Summary
   The UART interface manager module is responsible for handling the transfer
   of data through the UART interface on the device. This file defines the
   data structures and software interfaces for UART interface management.
 */

/**************************************************************************
 ******************************* Macros *******************************
 **************************************************************************/

/**************************************************************************
 ******************************* Data Types *******************************
 **************************************************************************/

/*@@FX3 Boot UART Interface
 The UART interface driver and APIs in the FX3 booter provide a mechanism to
 configure the UART properties and to do data read/write transfers through the
 UART interface.
*/

/*@@FX3 Boot UART Data Types
 This section documents the data types that are defined as part of the Boot UART APIs.
*/

/* Summary
   List of baud rates supported by the UART.

   Description
   This enumeration lists the various baud rate settings that are supported by
   the UART interface and driver implementation. The specific baud rates achieved
   will be close approximations of these standard values based on the clock
   frequencies that can be obtained on the FX3 hardware.
  
   See Also
   * CyFx3BootUartConfig_t
 */
typedef enum CyFx3BootUartBaudrate_t
{
    CY_FX3_BOOT_UART_BAUDRATE_4800 = 4800,           /* 4800 baud. */
    CY_FX3_BOOT_UART_BAUDRATE_9600 = 9600,           /* 9600 baud. */
    CY_FX3_BOOT_UART_BAUDRATE_19200 = 19200,         /* 19200 baud. */
    CY_FX3_BOOT_UART_BAUDRATE_38400 = 38400,         /* 38400 baud. */
    CY_FX3_BOOT_UART_BAUDRATE_57600 = 57600,         /* 57600 baud. */
    CY_FX3_BOOT_UART_BAUDRATE_115200 = 115200        /* 115200 baud. */
} CyFx3BootUartBaudrate_t;

/* Summary
   List of number of stop bits to be used in UART communication.

   Description
   This enumeration lists the various number of stop bit settings that the
   UART interface can be configured to have. Only 1 and 2 are supported on
   the FX3 device.

   See Also
   * CyFx3BootUartConfig_t
 */
typedef enum CyFx3BootUartStopBit_t
{
    CY_FX3_BOOT_UART_ONE_STOP_BIT = 1,       /* 1 stop bit */
    CY_FX3_BOOT_UART_TWO_STOP_BIT = 2        /* 2 stop bit */
} CyFx3BootUartStopBit_t;

/* Summary
   List of parity settings supported by the UART interface.

   Description
   This enumeration lists the various parity settings that the UART interface
   can be configured to support.

   See Also
   * CyFx3BootUartConfig_t
 */
typedef enum CyFx3BootUartParity_t
{
    CY_FX3_BOOT_UART_NO_PARITY      = 0,     /* No parity bits. */
    CY_FX3_BOOT_UART_EVEN_PARITY,            /* Even parity. */
    CY_FX3_BOOT_UART_ODD_PARITY,             /* Odd parity. */
    CY_FX3_BOOT_UART_NUM_PARITY              /* Number of parity enumerations. */
} CyFx3BootUartParity_t;

/* Summary
   Configuration parameters for the UART interface.

   Description
   This structure defines all of the configurable parameters for the UART
   interface such as baud rate, stop and parity bits etc. A pointer to this
   structure is passed in to the CyFx3BootUartSetConfig function to configure
   the UART interface.

   The isDma member specifies whether the UART should be configured to transfer
   data one byte at a time, or in terms of large (user configurable size) blocks.

   All of the parameters can be changed dynamically by calling the CyFx3BootUartSetConfig
   function repeatedly.

   See Also
   * CyFx3BootUartBaudrate_t
   * CyFx3BootUartStopBit_t
   * CyFx3BootUartParity_t
   * CyFx3BootUartSetConfig
 */
typedef struct CyFx3BootUartConfig_t
{
    CyBool_t                    txEnable;   /* Enable the transmitter. */
    CyBool_t                    rxEnable;   /* Enable the receiver. */
    CyBool_t                    flowCtrl;   /* Enable Flow control for Both RX and TX. */
    CyBool_t                    isDma;      /* CyFalse: Byte by byte transfer;
                                               CyTrue:  Block based transfer. */
    CyFx3BootUartBaudrate_t     baudRate;   /* Baud rate for data transfer. */
    CyFx3BootUartStopBit_t      stopBit;    /* The number of stop bits appended. */
    CyFx3BootUartParity_t       parity;     /* Parity configuration */

} CyFx3BootUartConfig_t;

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/*@@FX3 Boot UART Functions
 This section documents the functions that are defined as part of the Boot UART APIs.
 */

/* Summary
   Starts the UART hardware block on the device.

   Description
   This function powers up the UART hardware block on the device and should be the
   first UART related function called by the application.

   Return Value
   * CY_FX3_BOOT_SUCCESS                - When the UART block is successfully initialized
   * CY_FX3_BOOT_ERROR_NOT_SUPPORTED    - If the FX3 part in use does not support the UART interface

   See Also
   * CyFx3BootUartDeInit
 */
extern CyFx3BootErrorCode_t
CyFx3BootUartInit (
        void
        );

/* Summary
   Stops the UART hardware block.

   Description
   This function disables and powers off the UART hardware block on the device.

   Return Value
   * None

   See Also
   * CyFx3BootUartInit
 */
extern void
CyFx3BootUartDeInit(
        void
        );

/* Summary
   Sets the UART interface parameters.

   Description
   This function configures the UART block with the desired user parameters such
   as transfer mode, baud rate etc. This function should be called repeatedly to
   make any change to the set of configuration parameters. This can be called on
   the fly repetitively without calling CyFx3BootUartInit. But this will reset the
   FIFO and hence the data in pipe will be lost.

   Return Value
   * CY_FX3_BOOT_SUCCESS            - if the configuration was set successfully
   * CY_FX3_BOOT_ERROR_NOT_STARTED  - if the UART block has not been initialized
   * CY_FX3_BOOT_ERROR_NULL_POINTER - if a NULL pointer is passed

   See Also
   * CyFx3BootUartConfig_t
 */
extern CyFx3BootErrorCode_t
CyFx3BootUartSetConfig (
        CyFx3BootUartConfig_t *config  /* Pointer to structure containing config information */
        );

/* Summary
   Sets the number of bytes to be transmitted by the UART.

   Description
   This function sets the size of the desired data transmission through the UART. 
   Infinite transfers are not supported.

   This function is to be used when the UART is configured for DMA mode of
   transfer.

   Return Value
   * None

   See Also
   * CyFx3BootUartRxSetBlockXfer
 */
extern void
CyFx3BootUartTxSetBlockXfer (
        uint32_t txSize                         /* Desired transfer size. */
        );

/* Summary
   Sets the number of bytes to be received by the UART.

   Description
   This function sets the size of the desired data reception through the UART.
   Infinite transfers are not supported.

   This function is to be used when the UART is configured for DMA mode of
   transfer.

   Return Value
   * None

   See Also
   * CyFx3BootUartTxSetBlockXfer
 */
extern void
CyFx3BootUartRxSetBlockXfer (
        uint32_t rxSize                         /* Desired transfer size. */
        );


/* Summary
   Transmits data through the UART interface on a byte by byte basis.

   Description
   This function is used to transfer "count" number of bytes out through the
   UART register interface. This function can only be used if the UART has been
   configured for register (non-DMA) transfer mode.

   Return Value
   0 -  if the transmit bytes function failed. The failure could be due to the invalid
        parameters having been passed to the function or the block not being setup correctly.
   Non-zero value - Number of bytes that were successfully transmitted. This may not be the 
        same as that of the expected bytes to be transmitted. The caller is expected to handle
        these checks.
   See Also 
   * CyFx3BootUartReceiveBytes
 */
extern uint32_t
CyFx3BootUartTransmitBytes (
        uint8_t *data_p,    /* Pointer to the data to be transferred. */
        uint32_t count      /* Number of bytes to be transferred. */
        );

/* Summary
   Receives data from the UART interface on a byte by byte basis.

   Description
   This function is used to read "count" numbe of bytes from the UART register
   interface. This function can only be used if the UART has been configured for
   register (non-DMA) transfer mode.

   Return Value
   0 -  if the receive bytes function failed. The failure could be due to the invalid
        parameters having been passed to the function or the block not being setup correctly.
   Non-zero value - Number of bytes that were successfully received. This may not be the 
        same as that of the expected bytes to be received. The caller is expected to handle
        these checks.
   Number of bytes that are successfully received. 
   
   See Also
   * CyFx3BootUartTransmitBytes
 */
extern uint32_t
CyFx3BootUartReceiveBytes (
        uint8_t *data_p,              /* Pointer to location where the data read is to be placed. */
        uint32_t count                /* Number of bytes to be received. */
        );

/* Summary
   This function is used to setup a dma from CPU to UART or vice versa.

   Description
   This function is a blocking call. This function is used to read/write length number 
   of bytes from/to UART. This function can be used only if the UART has been configured
   for DMA transfer mode.
   Infinite transfers are not supported.

   Return Value
   * CY_FX3_BOOT_SUCCESS              - if data transfer is successful
   * CY_FX3_BOOT_ERROR_XFER_FAILURE   - if the data transfer encountered any error
   * CY_FX3_BOOT_ERROR_TIMEOUT        - if the data transfer times out

   See Also
   * CyFx3BootUartTransmitBytes
   * CyFx3BootUartReceiveBytes
   * CyFx3BootUartTxSetBlockXfer
   * CyFx3BootUartRxSetBlockXfer
 */
extern CyFx3BootErrorCode_t
CyFx3BootUartDmaXferData (
        CyBool_t isRead,    /* isRead = CyTrue for read operations 
                               isRead = CyFalse for write operations */
        uint32_t address,   /* address of the buffer from/to which data is to
                               be transferred */
        uint32_t length,    /* length of the data to be transferred */
        uint32_t timeout    /* Timeout in 100s of us. Also refer the macros CY_FX3_BOOT_NO_WAIT 
                               and CY_FX3_BOOT_WAIT_FOREVER */
        );

#include <cyu3externcend.h>

#endif /* _INCLUDED_CYFX3_UART_H_ */

/*[]*/
