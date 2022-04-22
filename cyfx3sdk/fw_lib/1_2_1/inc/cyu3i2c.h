/*
 ## Cypress USB 3.0 Platform header file (cyu3i2c.h)
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

#ifndef _INCLUDED_CYU3I2C_H_
#define _INCLUDED_CYU3I2C_H_

#include <cyu3types.h>
#include <cyu3system.h>
#include <cyu3lpp.h>
#include "cyu3externcstart.h"

/*@@I2C Interface
   The FX3 API library includes a I2C interface driver and provides a set of
   APIs that allow the configuration of the I2C interface properties and data
   exchange with one or more I2C slave devices.
 */

/**************************************************************************
 ******************************* Data Types *******************************
 **************************************************************************/

/*@@I2C Data Types
   This section documents the data types that are defined as part of the I2C
   driver and API library.
*/

/* Summary
   List of I2C related event types.

   Description
   This enumeration lists the various I2C related event codes that are notified
   to the user application through an event callback.

   Note
   In the case of a DMA read of data that does not fill the DMA buffer(s) associated
   with the read DMA channel, the DMA transfer remains pending after the CY_U3P_I2C_EVENT_RX_DONE
   event is delivered. The data can only be retrieved from the DMA buffer after the DMA transfer
   is terminated through the CyU3PDmaChannelSetWrapUp API.

   See Also
   * CyU3PI2cError_t
   * CyU3PI2cCb_t
 */
typedef enum CyU3PI2cEvt_t
{
    CY_U3P_I2C_EVENT_RX_DONE = 0,       /* Reception is completed */
    CY_U3P_I2C_EVENT_TX_DONE,           /* Transmission is done */
    CY_U3P_I2C_EVENT_TIMEOUT,           /* Bus timeout has happened */
    CY_U3P_I2C_EVENT_LOST_ARBITRATION,  /* Lost arbitration */
    CY_U3P_I2C_EVENT_ERROR              /* Error has happened */
} CyU3PI2cEvt_t;

/* Summary
   List of I2C specific error/status codes.

   Description
   This type lists the various I2C specific error/status codes that are sent to
   the event callback as event data, when the event type is CY_U3P_I2C_ERROR_EVT.

   See Also
   * CyU3PI2cEvt_t
   * CyU3PI2cCb_t
 */
typedef enum CyU3PI2cError_t
{
    CY_U3P_I2C_ERROR_NAK_BYTE_0 = 0,          /* Slave NACK-ed the zeroth byte of the preamble. */
    CY_U3P_I2C_ERROR_NAK_BYTE_1,              /* Slave NACK-ed the first byte of the preamble. */
    CY_U3P_I2C_ERROR_NAK_BYTE_2,              /* Slave NACK-ed the second byte of the preamble. */
    CY_U3P_I2C_ERROR_NAK_BYTE_3,              /* Slave NACK-ed the third byte of the preamble. */
    CY_U3P_I2C_ERROR_NAK_BYTE_4,              /* Slave NACK-ed the fourth byte of the preamble. */
    CY_U3P_I2C_ERROR_NAK_BYTE_5,              /* Slave NACK-ed the fifth byte of the preamble. */
    CY_U3P_I2C_ERROR_NAK_BYTE_6,              /* Slave NACK-ed the sixth byte of the preamble. */
    CY_U3P_I2C_ERROR_NAK_BYTE_7,              /* Slave NACK-ed the seventh byte of the preamble. */
    CY_U3P_I2C_ERROR_NAK_DATA,                /* Slave sent a NACK during the data phase of a transfer. */
    CY_U3P_I2C_ERROR_PREAMBLE_EXIT_NACK_ACK,  /* Poll operation has exited due to the slave
                                                 returning an ACK or a NACK handshake. */
    CY_U3P_I2C_ERROR_PREAMBLE_EXIT,           /* Poll operation with address repetition timed out. */
    CY_U3P_I2C_ERROR_NAK_TX_UNDERFLOW,        /* Underflow in buffer during transmit/write operation. */
    CY_U3P_I2C_ERROR_NAK_TX_OVERFLOW,         /* Overflow of buffer during transmit operation. */
    CY_U3P_I2C_ERROR_NAK_RX_UNDERFLOW,        /* Underflow in buffer during receive/read operation. */
    CY_U3P_I2C_ERROR_NAK_RX_OVERFLOW          /* Overflow of buffer during receive operation. */
} CyU3PI2cError_t;

/* Summary
   Structure defining the configuration of the I2C interface.

   Description
   This structure encapsulates all of the configurable parameters that can be
   selected for the I2C interface. The CyU3PI2cSetConfig() function accepts a
   pointer to this structure, and updates all of the interface parameters.

   The I2C block can function in the bit rate range of 100 KHz to 1MHz. In default
   mode of operation, the timeouts need to be kept disabled.

   In the register mode of operation (isDma is false), the data transfer APIs are
   blocking and return only after the requested amount of data has been read or
   written. In such a case, the I2C specific callbacks are meaningless; and the
   CyU3PI2cSetConfig API expects that no callback is specified when the register
   mode is selected.

   See Also
   * CyU3PI2cSetConfig
 */
typedef struct CyU3PI2cConfig_t
{
    uint32_t bitRate;           /* Bit rate for the interface. (Eg: 100000 for 100KHz) */
    CyBool_t isDma;             /* CyFalse: Register transfer mode,
                                   CyTrue: DMA transfer mode */
    uint32_t busTimeout;        /* Number of core clocks SCK can be
                                   held low by the slave byte transmission
                                   before triggering a timeout error.
                                   0xFFFFFFFFU means no timeout. */
    uint16_t dmaTimeout;        /* Number of core clocks DMA can remain
                                   not ready before flagging an error.
                                   0xFFFF means no timeout. */
} CyU3PI2cConfig_t;

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
   * CyU3PI2cSetConfig
 */
typedef struct CyU3PI2cPreamble_t
{
    uint8_t  buffer[8];         /* The extended preamble information. */
    uint8_t  length;            /* The length of the preamble to be sent.
                                   Should be between 1 and 8. */
    uint16_t ctrlMask;          /* This field controls the start stop condition
                                   after every byte of preamble data. Bits 0 - 7
                                   is a bit mask for start condition and Bits 8 - 15
                                   is a bit mask for stop condition. If both are
                                   set, then stop takes priority. */
} CyU3PI2cPreamble_t;

/*
   Summary
   Prototype of I2C event callback function.

   Description
   This function type defines a callback to be called after I2C interrupt has
   been received. A function of this type can be registered with the LPP driver
   as a callback function and will be called whenever an event of interest occurs.

   The I2C has to be configured for DMA mode of transfer for callbacks
   to be registered.

   See Also
   * CyU3PI2cEvt_t
   * CyU3PI2cError_t   
   * CyU3PRegisterI2cCallBack
 */
typedef void (*CyU3PI2cIntrCb_t)(
        CyU3PI2cEvt_t evt,      /* Type of event that occured. */
        CyU3PI2cError_t error   /* Specifies the actual error/status code when
                                   the event is of type CY_U3P_I2C_EVENT_ERROR. */
        );
/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/*@@I2C Functions
   This section documents the functions defined as part of the I2C driver
   and API library.
 */

/* Summary
   Starts the I2C interface block on the FX3.

   Description
   This function powers up the I2C interface block on the FX3 device and is
   expected to be the first I2C API function that is called by the application.

   This function also sets up the I2C interface at a default rate of 100KHz.

   Return Value
   * CY_U3P_SUCCESS               - When the Init is successful
   * CY_U3P_ERROR_NOT_CONFIGURED  - When I2C has not been enabled in IO configuration 
   * CY_U3P_ERROR_ALREADY_STARTED - When the I2C has been already initialized

   See Also
   * CyU3PI2cDeinit
   * CyU3PI2cSetConfig
   * CyU3PI2cSendCommand
   * CyU3PI2cTransmitBytes
   * CyU3PI2cReceiveBytes
   * CyU3PI2cWaitForAck
   * CyU3PI2cWaitForBlockXfer
   * CyU3PSetI2cDriveStrength
 */
extern CyU3PReturnStatus_t
CyU3PI2cInit (
        void);

/* Summary
   Stops the I2C module.
  
   Description
   This function disables and powers off the I2C interface. This function can
   be used to shut off the interface to save power when it is not in use.

   Return Value
   * CY_U3P_SUCCESS           - When the DeInit is successful
   * CY_U3P_ERROR_NOT_STARTED - When the I2C module has not been previously initialized

   See also
   * CyU3PI2cInit
   * CyU3PI2cSetConfig
   * CyU3PI2cSendCommand
   * CyU3PI2cTransmitBytes
   * CyU3PI2cReceiveBytes
   * CyU3PI2cWaitForAck
   * CyU3PI2cWaitForBlockXfer
   * CyU3PSetI2cDriveStrength
 */
extern CyU3PReturnStatus_t
CyU3PI2cDeInit(
        void);

/* Summary
   Sets the I2C interface parameters.

   Description
   This function is used to configure the I2C master interface based on the
   desired baud rate and address length settings to talk to the desired slave.
   This function should be called repeatedly to change the settings if
   different settings are to be used to communicate with different slave
   devices. This can be called on the fly repetitively without calling
   CyU3PI2cInit. But this will reset the FIFO and hence the data in pipe will
   be lost. If a DMA channel is present, a Reset has to be issued.

   In DMA mode, the callback parameter "cb" passed to this function is used to
   notify the user of data transfer completion or error conditions. In register
   mode, all APIs are blocking in nature. So in these cases, the callback
   argument is not required. User must pass NULL as cb when using the register
   mode.

   Bitrate calculation:
   The maximum bitrate supported is 1MHz and minimum bitrate is 100KHz. It should
   be noted that even though the dividers and the API allows frequencies above and
   below the rated range, the device behaviour is not guaranteed. The actual
   bit rate is derived out of the SYS_CLK / 16. The I2C block requires the clocking
   to be 10X the required bitrate. Since the divider needs to be integral or with
   half-divider, the frequency approximation is done with the following method:

   If x is the actual divider and n is the required integral divider to be used,
   then following conditions are used to evaluate:

      x = (SYS_CLK / 16) / (bitrate * 10);

      if (x - floor(x)) < 0.25 ==> n = floor(x);

      if (((x - floor(x)) >= 0.25) &&
          (x - floor(x)) < 0.5) ==> n = floor(x) + half divider;

      if (x - floor(x)) >= 0.75 ==> n = floor(x) + 1;

   Return Value
   * CY_U3P_SUCCESS            - When the SetConfig is successful
   * CY_U3P_ERROR_NOT_STARTED  - When the I2C has not been initialized
   * CY_U3P_ERROR_NULL_POINTER - When the config parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - When the arguments are incorrect
   * CY_U3P_ERROR_TIMEOUT      - When there is timeout happening during configuration
   * CY_U3P_ERROR_MUTEX_FAILURE- When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PI2cCb_t
   * CyU3PI2cEvt_t
   * CyU3PI2cError_t
   * CyU3PI2cIntrCb_t
   * CyU3PI2cConfig_t
   * CyU3PI2cInit
   * CyU3PI2cDeinit
   * CyU3PI2cSendCommand
   * CyU3PI2cTransmitBytes
   * CyU3PI2cReceiveBytes
   * CyU3PI2cWaitForAck
   * CyU3PI2cWaitForBlockXfer
   * CyU3PSetI2cDriveStrength
 */
extern CyU3PReturnStatus_t 
CyU3PI2cSetConfig ( 
        CyU3PI2cConfig_t *config,   /* I2C configuration settings */
        CyU3PI2cIntrCb_t cb         /* Callback for getting the events */
        );

/* Summary
   Perform a read or write operation to the I2C slave.

   Description
   This function is used to send the extended preamble over the I2C bus. This
   is used in conjunction with data transfer phase in DMA mode. The function is
   also called from the API library for register mode operation.

   The byteCount represents the amount of data to be read or written in the
   data phase and the isRead parameter specifies the direction of transfer.
   The transfer will happen through the I2C Consumer / Producer DMA channel if
   the I2C interface is configured in DMA mode, or through the I2C Ingress/Egress
   registers if the interface is configured in register mode.

   The CyU3PI2cWaitForBlockXfer API or the CY_U3P_I2C_EVENT_RX_DONE/CY_U3P_I2C_EVENT_TX_DONE
   event callbacks can be used to detect the end of a DMA data transfer that is
   requested through this function.

   Return Value
   * CY_U3P_SUCCESS              - When the SendCommand is successful
   * CY_U3P_ERROR_NULL_POINTER   - When the preamble is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT   - When the preamble length is 0 or greater than 8
                                   Also when byteCount is 0xFFFFFFFF (only finite length
                                   transfers are allowed)
   * CY_U3P_ERROR_TIMEOUT        - When the I2c bus is busy.
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also 
   * CyU3PI2cPreamble_t
   * CyU3PI2cInit
   * CyU3PI2cDeinit
   * CyU3PI2cSetConfig
   * CyU3PI2cTransmitBytes
   * CyU3PI2cReceiveBytes
   * CyU3PI2cWaitForAck
   * CyU3PI2cWaitForBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PI2cSendCommand (
        CyU3PI2cPreamble_t *preamble,  /* Preamble information to be sent out
                                          before the data transfer. */
        uint32_t byteCount,            /* Size of the transfer in bytes. */
        CyBool_t isRead                /* Direction of transfer;
                                          CyTrue: Read, CyFalse: Write. */
        );

/* Summary
   Writes a small number of bytes to an I2C slave.

   Description
   This function is used to write data one byte at a time to an I2C slave.
   This function requires that the I2C interface be configured in register
   (non-DMA) mode. The function call can be repeated on ERROR_TIMEOUT
   without any error recovery. The retry is done continuously without any
   delay. If any delay is required, then it should be added in the application
   firmware.

   The API will return when FX3 has transmitted the data. If the slave device
   requires additional time for completing the write operation, then either
   sufficient delay must be provided or if the slave device supports WaitForAck
   functionality, then CyU3PI2cWaitForAck API can be used. Refer to the I2C
   slave datasheet for more details.

   This function inherently calls CyU3PI2cSendCommand. So user must not call
   CyU3PI2cSendCommand before calling this function.

   Return Value
   * CY_U3P_SUCCESS              - When the TransmitBytes is successful or byteCount is 0
   * CY_U3P_ERROR_NULL_POINTER   - When the preamble or data are NULL
   * CY_U3P_ERROR_FAILURE        - When a transfer fails with an error defined in CyU3PI2cError_t.
   * CY_U3P_ERROR_BLOCK_FAILURE  - When the I2c block encounters a fatal error and requires reinit.
   * CY_U3P_ERROR_NOT_CONFIGURED - When the I2C is configured for DMA mode of operation
                                   OR I2C is not configured
				   OR I2C is not initiialized
   * CY_U3P_ERROR_TIMEOUT        - I2C bus timeout occurred.
   * CY_U3P_ERROR_LOST_ARBITRATION-Lost the bus arbitration or there was an invalid bus activity.
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also 
   * CyU3PI2cPreamble_t
   * CyU3PI2cInit
   * CyU3PI2cDeinit
   * CyU3PI2cSetConfig
   * CyU3PI2cSendCommand
   * CyU3PI2cReceiveBytes
   * CyU3PI2cWaitForAck
   * CyU3PI2cWaitForBlockXfer
 */
extern CyU3PReturnStatus_t 
CyU3PI2cTransmitBytes ( 
        CyU3PI2cPreamble_t *preamble, /* Preamble information to be sent out before
                                         the data transfer. */
        uint8_t *data,                /* Pointer to buffer containing data to be written. */
        uint32_t byteCount,           /* Size of the transfer in bytes. */
        uint32_t retryCount           /* Number of times to retry request if a byte is NAKed
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

   The API will return when FX3 has received the data. If the slave device
   requires additional time before servicing the next request, then either
   sufficient delay must be provided or if the slave device supports WaitForAck
   functionality, then CyU3PI2cWaitForAck API can be used. Refer to the I2C
   slave datasheet for more details.

   This function inherently calls CyU3PI2cSendCommand. So user must not call
   CyU3PI2cSendCommand before calling this function.

   Return Value
   * CY_U3P_SUCCESS              - When the ReceiveBytes is successful or byteCount is 0
   * CY_U3P_ERROR_NULL_POINTER   - When the preamble or data are NULL
   * CY_U3P_ERROR_FAILURE        - When a transfer fails with an error defined in CyU3PI2cError_t.
   * CY_U3P_ERROR_NOT_CONFIGURED - When the I2C is configured for DMA mode of operation
                                   OR I2C is not configured
				   OR I2C is not initiialized
   * CY_U3P_ERROR_BLOCK_FAILURE  - When the I2c block encounters a fatal error and requires reinit.
   * CY_U3P_ERROR_TIMEOUT        - I2C bus timeout occurred.
   * CY_U3P_ERROR_LOST_ARBITRATION-Lost the bus arbitration or there was an invalid bus activity.
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PI2cPreamble_t
   * CyU3PI2cInit
   * CyU3PI2cDeinit
   * CyU3PI2cSetConfig
   * CyU3PI2cSendCommand
   * CyU3PI2cTransmitBytes
   * CyU3PI2cWaitForAck
   * CyU3PI2cWaitForBlockXfergg
 */
extern CyU3PReturnStatus_t 
CyU3PI2cReceiveBytes (
        CyU3PI2cPreamble_t *preamble, /* Preamble information to be sent out before
                                         the data transfer. */
        uint8_t *data,                /* Pointer to buffer where the data is to be placed. */
        uint32_t byteCount,           /* Size of the transfer in bytes. */
        uint32_t retryCount           /* Number of times to retry request if preamble is NAKed
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

   The API plays the provided preamble bytes continuously until all bytes of the
   preamble are ACKed.

   Return Value
   * CY_U3P_SUCCESS              - When the device returns the desired handshake
   * CY_U3P_ERROR_NULL_POINTER   - When the preamble is NULL.
   * CY_U3P_ERROR_FAILURE        - When a preamble transfer fails with error defined by CyU3PI2cError_t
   * CY_U3P_ERROR_NOT_CONFIGURED - When the I2C is not initialized or not configured
   * CY_U3P_ERROR_BLOCK_FAILURE  - When the I2c block encounters a fatal error and requires reinit.
   * CY_U3P_ERROR_TIMEOUT        - I2C bus timeout occurred.
   * CY_U3P_ERROR_LOST_ARBITRATION-Lost the bus arbitration or there was an invalid bus activity.

   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   * See Also
   * CyU3PI2cPreamble_t
   * CyU3PI2cInit
   * CyU3PI2cDeinit
   * CyU3PI2cSetConfig
   * CyU3PI2cSendCommand
   * CyU3PI2cTransmitBytes
   * CyU3PI2cReceiveBytes
   * CyU3PI2cWaitForBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PI2cWaitForAck (
        CyU3PI2cPreamble_t *preamble, /* Preamble information to be sent out before
                                         the data transfer. */
        uint32_t retryCount           /* Number of times to retry request if preamble is NAKed
                                         or an error is encountered. */
        );

/* Summary
   Wait until the ongoing I2C data transfer is finished.

   Description
   This function can be used to ensure that a previous I2C transaction has
   completed, in the case where the callback is not being used. This function
   is only meaningful in DMA mode of operation because the register based
   read/write APIs are blocking in nature.

   The API will return when FX3 has received/transmitted the data. If the slave device
   requires additional time before servicing the next request, then sufficient delay
   must be provided. If a DMA operation fails, then the I2C block needs to be de-inited
   and re-initialized before sending any further commands.

   Note
   In the case of a DMA read of data that does not fill the DMA buffer(s) associated
   with the read DMA channel, the DMA transfer remains pending after this API call
   returns. The data can only be retrieved from the DMA buffer after the DMA transfer
   is terminated through the CyU3PDmaChannelSetWrapUp API.

   Return Value
   * CY_U3P_SUCCESS              - When the data transfer has been completed successfully
   * CY_U3P_ERROR_NOT_CONFIGURED - When the I2C is not initialized or not configured
   * CY_U3P_ERROR_NOT_SUPPORTED  - When callbacks are enabled
   * CY_U3P_ERROR_FAILURE        - When there is a failure defined by CyU3PI2cError_t
   * CY_U3P_ERROR_BLOCK_FAILURE  - When the I2c block encounters a fatal error and requires reinit.
   * CY_U3P_ERROR_TIMEOUT        - I2C bus timeout occurred.
   * CY_U3P_ERROR_LOST_ARBITRATION-Lost the bus arbitration or there was an invalid bus activity.
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PI2cInit
   * CyU3PI2cDeinit
   * CyU3PI2cSetConfig
   * CyU3PI2cSendCommand
   * CyU3PI2cTransmitBytes
   * CyU3PI2cReceiveBytes
   * CyU3PI2cWaitForAck
 */
extern CyU3PReturnStatus_t
CyU3PI2cWaitForBlockXfer (
        CyBool_t isRead       /* Type of operation to wait on;
                                 CyTrue: Read, CyFalse: Write */
        );

/* Summary
   Retrieves the error code as defined by CyU3PI2cError_t.

   Description
   This function can be used to retrieve the error code when CyU3PI2cTransmitBytes / 
   CyU3PI2cReceiveBytes / CyU3PI2cWaitForAck functions fail with error code
   CY_U3P_ERROR_FAILURE.

   Return Value
   * CY_U3P_SUCCESS              - When the data transfer has been completed successfully
   * CY_U3P_ERROR_NOT_CONFIGURED - When the I2C is not initialized or not configured.
   * CY_U3P_ERROR_NULL_POINTER   - When the pointer passed is NULL.
   * CY_U3P_ERROR_NOT_STARTED    - When there is no error flagged.
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PI2cInit
   * CyU3PI2cDeinit
   * CyU3PI2cSetConfig
   * CyU3PI2cSendCommand
   * CyU3PI2cTransmitBytes
   * CyU3PI2cReceiveBytes
   * CyU3PI2cWaitForAck
 */
extern CyU3PReturnStatus_t
CyU3PI2cGetErrorCode (
        CyU3PI2cError_t *error_p /* Error code. */
        );

/* Summary
   This function register the call back function for notification of I2C interrupt.

   Description
   This function registers a callback function that will be called for notification of I2C interrupts
   and also selects the I2C interrupt sources of interest.

   Return Value
   None

   See Also
   * CyU3PI2cEvt_t
   * CyU3PI2cError_t
 */
extern void 
CyU3PRegisterI2cCallBack (
                          CyU3PI2cIntrCb_t i2cIntrCb);

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3I2C_H_ */
