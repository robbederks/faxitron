/*
 ## Cypress FX3 Boot Firmware Header (cyfx3uart.h)
 ## ===========================
 ##
 ##  Copyright Cypress Semiconductor Corporation, 2010-2018,
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

/** \file cyfx3uart.h
    \brief The UART interface manager module is responsible for handling the
    transfer of data through the UART interface on the device. This file
    defines the data structures and APIs for UART interface management.
 */

/**************************************************************************
 ******************************* Data Types *******************************
 **************************************************************************/

/** \brief List of baud rates supported by the UART.

    <B>Description</B>\n
    This enumeration lists the various baud rate settings that are supported by
    the UART interface and driver implementation. The specific baud rates achieved
    will be close approximations of these standard values based on the clock
    frequencies that can be obtained on the FX3 hardware.

    **\see
    *\see CyFx3BootUartConfig_t
 */
typedef enum CyFx3BootUartBaudrate_t
{
    CY_FX3_BOOT_UART_BAUDRATE_4800 = 4800,              /**< 4800 baud. */
    CY_FX3_BOOT_UART_BAUDRATE_9600 = 9600,              /**< 9600 baud. */
    CY_FX3_BOOT_UART_BAUDRATE_19200 = 19200,            /**< 19200 baud. */
    CY_FX3_BOOT_UART_BAUDRATE_38400 = 38400,            /**< 38400 baud. */
    CY_FX3_BOOT_UART_BAUDRATE_57600 = 57600,            /**< 57600 baud. */
    CY_FX3_BOOT_UART_BAUDRATE_115200 = 115200           /**< 115200 baud. */
} CyFx3BootUartBaudrate_t;

/** \brief List of number of stop bits to be used in UART communication.

    <B>Description</B>\n
    This enumeration lists the various number of stop bit settings that the
    UART interface can be configured to have. Only 1 and 2 are supported on
    the FX3 device.

    **\see
    *\see CyFx3BootUartConfig_t
 */
typedef enum CyFx3BootUartStopBit_t
{
    CY_FX3_BOOT_UART_ONE_STOP_BIT = 1,                  /**< 1 stop bit */
    CY_FX3_BOOT_UART_TWO_STOP_BIT = 2                   /**< 2 stop bit */
} CyFx3BootUartStopBit_t;

/** \brief List of parity settings supported by the UART interface.

    <B>Description</B>\n
    This enumeration lists the various parity settings that the UART interface
    can be configured to support.

    **\see
    *\see CyFx3BootUartConfig_t
 */
typedef enum CyFx3BootUartParity_t
{
    CY_FX3_BOOT_UART_NO_PARITY      = 0,                /**< No parity bits. */
    CY_FX3_BOOT_UART_EVEN_PARITY,                       /**< Even parity. */
    CY_FX3_BOOT_UART_ODD_PARITY,                        /**< Odd parity. */
    CY_FX3_BOOT_UART_NUM_PARITY                         /**< Number of parity enumerations. */
} CyFx3BootUartParity_t;

/** \brief Configuration parameters for the UART interface.

    <B>Description</B>\n
    This structure defines all of the configurable parameters for the UART
    interface such as baud rate, stop and parity bits etc. A pointer to this
    structure is passed in to the CyFx3BootUartSetConfig function to configure
    the UART interface.

    **\see
    *\see CyFx3BootUartBaudrate_t
    *\see CyFx3BootUartStopBit_t
    *\see CyFx3BootUartParity_t
    *\see CyFx3BootUartSetConfig
 */
typedef struct CyFx3BootUartConfig_t
{
    CyBool_t                txEnable;           /**< Enable the transmitter. */
    CyBool_t                rxEnable;           /**< Enable the receiver. */
    CyBool_t                flowCtrl;           /**< Enable Flow control for both RX and TX. */
    CyBool_t                isDma;              /**< CyFalse: Byte-by-byte transfer, CyTrue: Block based transfer. */
    CyFx3BootUartBaudrate_t baudRate;           /**< Baud rate for data transfer. */
    CyFx3BootUartStopBit_t  stopBit;            /**< The number of stop bits used. */
    CyFx3BootUartParity_t   parity;             /**< Parity configuration. */
} CyFx3BootUartConfig_t;

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/** \brief Starts the UART hardware block on the device.

    <B>Description</B>\n
    This function powers up the UART hardware block on the device and should be the
    first UART related function called by the application.

    <B>Return value</B>\n
    * CY_FX3_BOOT_SUCCESS - when the UART block is successfully initialized.

    **\see
    *\see CyFx3BootUartDeInit
    *\see CyFx3BootUartSetConfig
 */
extern CyFx3BootErrorCode_t
CyFx3BootUartInit (
        void);

/** \brief Stops the UART hardware block.

    <B>Description</B>\n
    This function disables and powers off the UART hardware block on the device.

    <B>Return value</B>\n
    * None

    **\see
    *\see CyFx3BootUartInit
 */
extern void
CyFx3BootUartDeInit(
        void);

/** \brief Sets the UART interface parameters.

    <B>Description</B>\n
    This function configures the UART block with the desired user parameters such
    as transfer mode, baud rate etc. This function should be called repeatedly to
    make any change to the set of configuration parameters. This can be called on
    the fly repetitively without calling CyFx3BootUartInit. But this will reset the
    FIFO and hence the data in pipe will be lost.

    <B>Return value</B>\n
    * CY_FX3_BOOT_SUCCESS            - if the configuration was set successfully.\n
    * CY_FX3_BOOT_ERROR_NOT_STARTED  - if the UART block has not been initialized.\n
    * CY_FX3_BOOT_ERROR_NULL_POINTER - if a NULL pointer is passed.

    **\see
    *\see CyFx3BootUartConfig_t
 */
extern CyFx3BootErrorCode_t
CyFx3BootUartSetConfig (
        CyFx3BootUartConfig_t *config   /**< Pointer to structure containing config information. */
        );

/** \brief Sets the UART transmit data count in the case of DMA mode operation.

    <B>Description</B>\n
    This function sets the size of the desired data transmission through the UART.
    This function is to be used when the UART is configured for DMA mode of
    transfer.

    <B>Return value</B>\n
    * None

    **\see
    *\see CyFx3BootUartRxSetBlockXfer
 */
extern void
CyFx3BootUartTxSetBlockXfer (
        uint32_t txSize                 /**< Desired transfer size. */
        );

/** \brief Sets the number of bytes to be received by the UART in DMA mode.

    <B>Description</B>\n
    This function sets the size of the desired data reception through the UART.
    This function is to be used when the UART is configured for DMA mode of
    transfer.

    <B>Return value</B>\n
    * None

    **\see
    *\see CyFx3BootUartTxSetBlockXfer
 */
extern void
CyFx3BootUartRxSetBlockXfer (
        uint32_t rxSize                 /**< Desired transfer size. */
        );

/** \brief Transmits data through the UART interface on a byte by byte basis.

    <B>Description</B>\n
    This function is used to transfer "count" number of bytes out through the
    UART register interface. This function can only be used if the UART has been
    configured for register (non-DMA) transfer mode.

    <B>Return value</B>\n
    * Amount of data actually transferred. A return of zero may indicate a transfer error.

    **\see
    *\see CyFx3BootUartReceiveBytes
 */
extern uint32_t
CyFx3BootUartTransmitBytes (
        uint8_t *data_p,        /**< Pointer to the data to be transferred. */
        uint32_t count          /**< Number of bytes to be transferred. */
        );

/** \brief Receives data from the UART interface on a byte by byte basis.

    <B>Description</B>\n
    This function is used to read "count" number of bytes from the UART register
    interface. This function can only be used if the UART has been configured for
    register (non-DMA) transfer mode.

    <B>Return value</B>\n
    * Amount of data actually transferred. A return of zero may indicate a transfer error.

    **\see
    *\see CyFx3BootUartTransmitBytes
 */
extern uint32_t
CyFx3BootUartReceiveBytes (
        uint8_t *data_p,        /**< Pointer to location where the data read is to be placed. */
        uint32_t count          /**< Number of bytes to be received. */
        );

/** \brief This function is used to setup a DMA from CPU to UART or vice versa.

    <B>Description</B>\n
    This function is used to read/write a block of data through the UART interface
    in DMA mode. This is a blocking call which returns only when the transfer is
    complete, or the specified timeout duration has elapsed.

    This API can only be called if the UART is setup for DMA mode of operation.

    <B>Return value</B>\n
    * CY_FX3_BOOT_SUCCESS                - if data transfer is successful.\n
    * CY_FX3_BOOT_ERROR_INVALID_DMA_ADDR - if the address specified is not in the SYSMEM area.\n
    * CY_FX3_BOOT_ERROR_XFER_FAILURE     - if the data transfer encountered any error.\n
    * CY_FX3_BOOT_ERROR_TIMEOUT          - if the data transfer times out.

    **\see
    *\see CyFx3BootUartTxSetBlockXfer
    *\see CyFx3BootUartRxSetBlockXfer
 */
extern CyFx3BootErrorCode_t
CyFx3BootUartDmaXferData (
        CyBool_t isRead,        /**< Transfer direction - isRead=CyTrue for read operations;
                                     and isRead=CyFalse for write operations. */
        uint32_t address,       /**< Address of the buffer from/to which data is to be transferred. */
        uint32_t length,        /**< Length of the data to be transferred. */
        uint32_t timeout        /**< Timeout duration in multiples of 10 us. Can be set to CY_FX3_BOOT_WAIT_FOREVER
                                     to compulsorily wait for end of transfer. */
        );

/** \brief Print a formatted string out through the UART.

    <B>Description</B>\n
    This function supports logging messages through the UART port of the FX3 device.
    A printf-style variable length argument list is supported with several restrictions.
    The function only supports a subset of the output conversion specifications
    supported by the snprintf function. The 'c', 'd', 'u', 'x' and 's' conversion
    specifications are supported. There is no support for float or double formats,
    or for flags, precision or type modifiers.

    This function will use either the register or DMA mode of the UART to transmit the
    log data. The buffer to be used for storing the formatted string has to be passed
    in as an argument, along with maximum length of the string.

    This is a blocking call and will only return after all of the data has been sent
    out from the FX3 device.

    <B>Return Value</B>\n
    * CY_FX3_BOOT_SUCCESS if the message has been sent out through UART.
    * CY_FX3_BOOT_ERROR_NOT_STARTED if the UART is not started.
    * CY_FX3_BOOT_ERROR_NOT_CONFIGURED if the UART is not enabled for transmission.
    * CY_FX3_BOOT_ERROR_INVALID_DMA_ADDR if the buffer provided cannot be used for DMA transfer.
    * CY_FX3_BOOT_ERROR_FAILURE if the message tranmission fails on the UART side.
 */
CyFx3BootErrorCode_t
CyFx3BootUartPrintMessage (
        uint8_t  *outBuf_p,             /**< Pointer to buffer to be used for the formatted string. */
        uint16_t  outBufLen,            /**< Maximum length of the temporary buffer for formatted string. */
        char     *fmt,                  /**< Format string. */
        ...                             /**< Variable list of parameters to be printed. */
        );

/** \brief Set the timeout duration for UART byte mode data transfers.

    <B>Description</B>\n
    The UART byte mode transfer functions (CyFx3BootUartTransmitBytes and CyFx3BootUartReceiveBytes)
    are implemented using a poll loop which continually checks for the completion of the transfer.
    The delay for each of these loops will be approximately 2 us, and the default value of the maximum
    loop count is 1048575. This API can be used to change the timeout loop count to a different value.

    <B>Return value</B>\n
    * None
 */
extern void
CyFx3BootUartSetTimeout (
        uint32_t timeout        /* Number of poll loops to be executed before a timeout is returned. */
        );

#include <cyu3externcend.h>

#endif /* _INCLUDED_CYFX3_UART_H_ */

/*[]*/
