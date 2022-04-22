/*
 ## Cypress USB 3.0 Platform header file (cyfx3i2c.h)
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

#ifndef _INCLUDED_CYFX3I2C_H_
#define _INCLUDED_CYFX3I2C_H_

#include <cyfx3error.h>
#include <cyu3types.h>
#include <cyu3externcstart.h>

/*@@FX3 Boot I2C Interface
   The FX3 booter includes a I2C interface driver and provides a set of
   APIs that allow the configuration of the I2C interface properties and data
   exchange with one or more I2C slave devices.
 */

/**************************************************************************
 ******************************* Data Types *******************************
 **************************************************************************/

/*@@FX3 Boot I2C Data Types
   This section documents the data types that are defined as part of the I2C
   driver.
*/

/* Summary
   Structure defining the configuration of the I2C interface.

   Description
   This structure encapsulates all of the configurable parameters that can be
   selected for the I2C interface. The CyFx3BootI2cSetConfig() function accepts a
   pointer to this structure, and updates all of the interface parameters.

   The I2C block can function in the bit rate range of 100 KHz to 1MHz. In default
   mode of operation, the timeouts need to be kept disabled.

   In the register mode of operation (isDma is false), the data transfer APIs are
   blocking and return only after the requested amount of data has been read or
   written. In such a case, the I2C specific callbacks are meaningless; and the
   CyFx3BootI2cSetConfig API expects that no callback is specified when the register
   mode is selected.

   See Also
   * CyFx3BootI2cSetConfig
 */
typedef struct CyFx3BootI2cConfig_t
{
    uint32_t bitRate;           /* Bit rate for the interface. (Eg: 100000 for 100KHz*/
    CyBool_t isDma;             /* CyFalse: Register transfer mode,
                                   CyTrue: DMA transfer mode */
    uint32_t busTimeout;        /* Number of core clocks SCK can be
                                   held low by the slave byte transmission
                                   before triggering a timeout error.
                                   0xFFFFFFFFU means no timeout. */
    uint16_t dmaTimeout;        /* Number of core clocks DMA can remain
                                   not ready before flagging an error.
                                   0xFFFF means no timeout. */
} CyFx3BootI2cConfig_t;

/* Summary
   Structure defining the preamble to be sent on the I2C interface.

   Description
   All I2C data transfer requires a preamble, which contains the slave address
   and the direction of the transfer. Here we are extending this to include
   the command that will typically be sent to the I2C device which will trigger
   a data transfer.

   The ctrlMask indicate the start / stop bit conditions after each byte of
   preamble.

   For example if you look at an I2C EEPROM, it requires the address to
   read / write from. This is considered as the command and the data which is
   being read / written is considered as the actual data transfer. So the two
   I2C operations are combined into one I2C API call using the parameters of
   the structure.

   Typical I2C EEPROM page Write operation:

   Byte 0:
        Bit 7 - 1: Slave address.
        Bit 0    : 0 - Indicating this is a write from master.

   Byte 1, 2: Address to which the data has to be written.

   The buffer field shall hold the above three bytes, the length field shall be
   three and the ctrlMask field is zero.

   Typical I2C EEPROM page Read operation:

   Byte 0:
        Bit 7 - 1: Slave address.
        Bit 0    : 0 - Indicating this is a write from master.

   Byte 1, 2: Address to which the data has to be written.

   Byte 3:
        Bit 7 - 1: Slave address.
        Bit 0    : 1 - Indicating this is a read operation.

   The buffer field shall hold the above four bytes, the length field shall be
   four and ctrlMask field is 0x0004 as a start bit is required after the third
   byte (third bit is set).

   See Also
   * CyFx3BootI2cSetConfig
 */
typedef struct CyFx3BootI2cPreamble_t
{
    uint8_t  buffer[8];         /* The extended preamble information. */
    uint8_t  length;            /* The length of the preamble to be sent.
                                   Should be between 1 and 8. */
    uint16_t ctrlMask;          /* This field controls the start stop condition
                                   after every byte of preamble data. Bits 0 - 7
                                   is a bit mask for start condition and Bits 8 - 15
                                   is a bit mask for stop condition. If both are
                                   set, then stop takes priority. */
} CyFx3BootI2cPreamble_t;

/* Summary
   List of I2C specific error/status codes.

   Description
   This type lists the various I2C specific error/status codes.
 */
typedef enum CyFx3BootI2cError_t
{
    CY_FX3_BOOT_I2C_ERROR_NAK_BYTE_0 = 0,          /* Slave NACK-ed the zeroth byte of the preamble. */
    CY_FX3_BOOT_I2C_ERROR_NAK_BYTE_1,              /* Slave NACK-ed the first byte of the preamble. */
    CY_FX3_BOOT_I2C_ERROR_NAK_BYTE_2,              /* Slave NACK-ed the second byte of the preamble. */
    CY_FX3_BOOT_I2C_ERROR_NAK_BYTE_3,              /* Slave NACK-ed the third byte of the preamble. */
    CY_FX3_BOOT_I2C_ERROR_NAK_BYTE_4,              /* Slave NACK-ed the fourth byte of the preamble. */
    CY_FX3_BOOT_I2C_ERROR_NAK_BYTE_5,              /* Slave NACK-ed the fifth byte of the preamble. */
    CY_FX3_BOOT_I2C_ERROR_NAK_BYTE_6,              /* Slave NACK-ed the sixth byte of the preamble. */
    CY_FX3_BOOT_I2C_ERROR_NAK_BYTE_7,              /* Slave NACK-ed the seventh byte of the preamble. */
    CY_FX3_BOOT_I2C_ERROR_NAK_DATA,                /* Slave sent a NACK during the data phase of a
                                          transfer. */
    CY_FX3_BOOT_I2C_ERROR_PREAMBLE_EXIT_NACK_ACK,  /* Poll operation has exited due to the slave
                                                 returning an ACK or a NACK handshake. */
    CY_FX3_BOOT_I2C_ERROR_PREAMBLE_EXIT,           /* Poll operation with address repetition timed out. */
    CY_FX3_BOOT_I2C_ERROR_NAK_TX_UNDERFLOW,        /* Underflow in buffer during transmit/write operation. */
    CY_FX3_BOOT_I2C_ERROR_NAK_TX_OVERFLOW,         /* Overflow of buffer during transmit operation. */
    CY_FX3_BOOT_I2C_ERROR_NAK_RX_UNDERFLOW,        /* Underflow in buffer during receive/read operation. */
    CY_FX3_BOOT_I2C_ERROR_NAK_RX_OVERFLOW          /* Overflow of buffer during receive operation. */

} CyFx3BootI2cError_t;

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/*@@FX3 Boot I2C Functions
   This section documents the functions defined as part of the I2C driver.
   
 */

/* Summary
   Starts the I2C interface block.

   Description
   This function powers up the I2C interface block on the FX3 device and is
   expected to be the first I2C API function that is called by the application.
   IO configuration function is expected to have been called prior to this
   call.

   This function also sets up the I2C interface at a default rate of 100KHz.

   Return Value
   * CY_FX3_BOOT_SUCCESS             - When the SetConfig is successful
   * CY_FX3_BOOT_ERROR_NOT_SUPPORTED - If the FX3 part in use does not support the I2C feature.
   

   See Also
   * CyFx3BootI2cDeinit
 */
extern CyFx3BootErrorCode_t
CyFx3BootI2cInit (
        void
        );

/* Summary
   Stops the I2C module. 
  
   Description
   This function disables and powers off the I2C interface. This function can
   be used to shut off the interface to save power when it is not in use.

   Return Value
   * None

   See also
   * CyFx3BootI2cInit
 */
extern void
CyFx3BootI2cDeInit(
        void
        );

/* Summary
   Sets the I2C interface parameters.

   Description
   This function is used to configure the I2C master interface based on the
   desired baud rate and address length settings to talk to the desired slave.
   This function should be called repeatedly to change the settings if
   different settings are to be used to communicate with different slave
   devices. This can be called on the fly repetitively without calling
   CyFx3BootI2cInit. But this will reset the FIFO and hence the data in pipe will
   be lost. In register mode, all APIs are blocking in nature. 

   Return Value
   * CY_FX3_BOOT_SUCCESS            - When the SetConfig is successful
   * CY_FX3_BOOT_ERROR_NOT_STARTED  - If the I2C block has not been initialized
   * CY_FX3_BOOT_ERROR_NULL_POINTER - When the config parameter is NULL
   * CY_FX3_BOOT_ERROR_TIMEOUT      - When there is timeout happening during configuration

   See Also
   * CyFx3BootI2cConfig_t
 */
extern CyFx3BootErrorCode_t 
CyFx3BootI2cSetConfig ( 
        CyFx3BootI2cConfig_t *config   /* I2C configuration settings */
        );

/* Summary
   Perform a read or write operation to the I2C slave.

   Description
   This function is used to send the extended preamble over the I2C bus. This
   is used in conjunction with data transfer phase in DMA/Register mode. 

   The byteCount represents the amount of data to be read or written in the
   data phase and the isRead parameter specifies the direction of transfer.
   Data transfer can be done either in the DMA mode or the register mode.

   The CyFx3BootI2cWaitForCompletion() can be used to detect the end of a DMA
   data transfer that is requested through this function.

   Return Value
   * CY_FX3_BOOT_SUCCESS              - When the SendCommand is successful
   * CY_FX3_BOOT_ERROR_NULL_POINTER   - When the preamble is NULL
   * CY_FX3_BOOT_ERROR_TIMEOUT        - When the transfer times out

   See Also 
   * CyFx3BootI2cReceiveBytes
   * CyFx3BootI2cTransmitBytes 
 */
extern CyFx3BootErrorCode_t
CyFx3BootI2cSendCommand (
        CyFx3BootI2cPreamble_t *preamble, /* Preamble information to be sent out
                                             before the data transfer. */
        uint32_t byteCount,               /* Size of the transfer in bytes. */
        CyBool_t isRead                   /* Direction of transfer;
                                             CyTrue: Read, CyFalse: Write. */
        );

/* Summary
   Writes a small number of bytes to an I2C slave.

   Description
   This function is used to write data one byte at a time to an I2C slave.
   This function requires that the I2C interface be configured in register
   (non-DMA) mode.  The function call can be repeated on ERROR_TIMEOUT
   without any error recovery. The retry is done continuously without any
   delay. If any delay is required, then it should be added in the application
   firmware.

   This function internally calls CyFx3BootI2cSendCommand. So user must not call
   CyFx3BootI2cSendCommand before calling this function.

   Return Value
   * CY_FX3_BOOT_SUCCESS              - When the TransmitBytes is successful or byteCount is 0
   * CY_FX3_BOOT_ERROR_NULL_POINTER   - When the preamble or data are NULL
   * CY_FX3_BOOT_ERROR_XFER_FAILURE   - When a transfer fails
   * CY_FX3_BOOT_ERROR_TIMEOUT        - When the transfer times out

   See Also 
   * CyFx3BootI2cSetConfig
   * CyFx3BootI2cReceiveBytes
 */
extern CyFx3BootErrorCode_t 
CyFx3BootI2cTransmitBytes ( 
        CyFx3BootI2cPreamble_t *preamble, /* Preamble information to be sent out before
                                             the data transfer. */
        uint8_t *data,                    /* Pointer to buffer containing data to be written. */
        uint32_t byteCount,               /* Size of the transfer in bytes. */
        uint32_t retryCount               /* Number of times to retry request if a byte is NAKed
                                             by the slave. */
        );

/* Summary
   Reads a small number of bytes one by one from an I2C slave.

   Description
   This function reads a few bytes one at a time from the I2C slave selected
   through the preamble parameter. The I2C interface should be configured in
   register (non-DMA) mode to make use of this function. The function call
   can be repeated on ERROR_TIMEOUT without any error recovery. The retry is
   done continuously without any delay. If any delay is required, then it
   should be added in the application firmware.

   This function internally calls CyFx3BootI2cSendCommand. So user must not call
   CyFx3BootI2cSendCommand before calling this function.

   Return Value
   * CY_FX3_BOOT_SUCCESS              - When the ReceiveBytes is successful or byteCount is 0
   * CY_FX3_BOOT_ERROR_NULL_POINTER   - When the preamble or data are NULL
   * CY_FX3_BOOT_ERROR_XFER_FAILURE   - When a transfer fails
   * CY_FX3_BOOT_ERROR_TIMEOUT        - When the transfer times out

   See Also
   * CyFx3BootI2cSetConfig
   * CyFx3BootI2cTransmitBytes
 */
extern CyFx3BootErrorCode_t 
CyFx3BootI2cReceiveBytes (
        CyFx3BootI2cPreamble_t *preamble, /* Preamble information to be sent out before
                                             the data transfer. */
        uint8_t *data,                    /* Pointer to buffer where the data is to be placed. */
        uint32_t byteCount,               /* Size of the transfer in bytes. */
        uint32_t retryCount               /* Number of times to retry request if preamble is NAKed
                                             or an error is encountered. */
        );

/* Summary
   Poll an I2C slave until all of the preamble is ACKed.

   Description
   This function waits for a ACK handshake from the slave, and can be used
   to ensure that the slave device has reached a desired state before issuing
   the next transaction or shutting the interface down. This function call
   returns when the specified handshake has been received or when the wait has
   timed out. The retry is done continuously without any delay. If any delay is
   required, then it should be added in the application firmware. The function
   call can be repeated on ERROR_TIMEOUT without any error recovery.

   Return Value
   * CY_FX3_BOOT_SUCCESS              - When the device returns the desired handshake
   * CY_FX3_BOOT_ERROR_NULL_POINTER   - When the preamble is NULL.
   * CY_FX3_BOOT_ERROR_TIMEOUT        - When a timeout occurs.
 */
extern CyFx3BootErrorCode_t
CyFx3BootI2cWaitForAck (
        CyFx3BootI2cPreamble_t *preamble, /* Preamble information to be sent out before
                                             the data transfer. */
        uint32_t retryCount               /* Number of times to retry request if preamble is NAKed
                                             or an error is encountered. */
        );

/* Summary
   This function is used to setup a dma from CPU to I2C or vice versa.

   Description
   This function is a blocking call. This function is used to read/write length number 
   of bytes from/to I2C. This function can be used only if the I2C has been configured
   for DMA transfer mode.
   This function is used to create a one shot DMA channel to transfer to the
   data. Infinite transfers are not supported.

   Return Value
   * CY_FX3_BOOT_SUCCESS              - if the data transfer is successful
   * CY_FX3_BOOT_ERROR_XFER_FAILURE   - if the data transfer encountered any error
   * CY_FX3_BOOT_ERROR_TIMEOUT        - if the data transfer times out

   See Also
   * CyFx3BootSpiDisableBlockXfer
   * CyFx3BootSpiSetBlockXfer
 */
extern
CyFx3BootErrorCode_t
CyFx3BootI2cDmaXferData (
        CyBool_t isRead,    /* isRead = CyTrue for read operations 
                               isRead = CyFalse for write operations */
        uint32_t address,   /* address of the buffer from/to which data is to
                               be transferred */
        uint32_t length,    /* length of the data to be transferred 
                               Maximum length of the data that can be transferred is as
                               defined by the Page size of the I2C device. */
        uint32_t timeout    /* Timeout in 10s of us. Also refer the macros CY_FX3_BOOT_NO_WAIT 
                               and CY_FX3_BOOT_WAIT_FOREVER */
        );

#include <cyu3externcend.h>

#endif /* _INCLUDED_CYFX3I2C_H_ */

/* [] */
