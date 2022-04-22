/*
## Cypress USB 3.0 Platform header file (cyu3uart.h)
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

#ifndef _INCLUDED_CYU3_UART_H_
#define _INCLUDED_CYU3_UART_H_

#include "cyu3types.h"
#include "cyu3externcstart.h"
#include "cyu3lpp.h"

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

/*@@Serial Peripheral Interfaces
 Summary
 The serial peripheral interfaces and the corresponding API in the FX3 library
 support data exchange between the FX3 device and external peripherals or
 controllers that are connected through standard peripheral interfaces.

 Description
 The FX3 device supports a set of serial peripheral interfaces that can be used
 to connect a variety of slave or peer devices to FX3. The peripheral
 interfaces supported by the device are:
   1. UART
   2. I2C
   3. GPIOs
   4. SPI
   5. I2S

 The I2C, SPI and I2S interface implementations on the FX3 device are master
 mode only and can talk to a variety of slave devices. The I2C interface is
 also capable of functioning in multi-master mode where other I2C master
 devices are present on the bus.

 The FX3 device supports configuration of several IO pins as general purpose
 IOs, which can be multiplexed to support other functions and interfaces. FX3
 provides software controlled pull up or pull down resistors internally on all
 digital I/O pins. The pins can be pulled high through a resistor on all I/O
 pins or can be pulled low through a resistor on all I/O pins and can be used
 to prevent the pins from floating.
 */

/*@@UART Interface
 The UART interface driver and APIs in the FX3 library provide a mechanism to
 configure the UART properties and to do data read/write transfers through the
 UART interface.
*/

/*@@UART data types
 This section documents the data types that are defined as part of the UART
 driver and API library.
*/

/* Summary
   List of UART related event types.

   Description
   This enumeration lists the various UART related event codes that are notified
   to the user application through an event callback.

   See Also
   * CyU3PUartError_t
   * CyU3PUartIntrCb_t
 */
typedef enum CyU3PUartEvt_t
{
    CY_U3P_UART_EVENT_RX_DONE = 0,       /* Reception is completed */
    CY_U3P_UART_EVENT_TX_DONE,           /* Transmission is done */
    CY_U3P_UART_EVENT_ERROR              /* Error has happened */
} CyU3PUartEvt_t;

/* Summary
   List of UART specific error/status codes.

   Description
   This type lists the various UART specific error/status codes that are sent to
   the event callback as event data, when the event type is CY_U3P_UART_ERROR_EVT.

   See Also
   * CyU3PUartEvt_t
   * CyU3PUartIntrCb_t
 */
typedef enum CyU3PUartError_t
{
    CY_U3P_UART_ERROR_NAK_BYTE_0 = 0,          /* Missing stop bit. */
    CY_U3P_UART_ERROR_RX_PARITY_ERROR = 1,     /* RX parity error. */
    CY_U3P_UART_ERROR_TX_OVERFLOW = 12,        /* Overflow of FIFO during transmit operation. */
    CY_U3P_UART_ERROR_RX_UNDERFLOW = 13,       /* Underflow in FIFO during receive/read operation. */
    CY_U3P_UART_ERROR_RX_OVERFLOW = 14         /* Overflow of FIFO during receive operation. */

} CyU3PUartError_t;

/* Summary
   List of baud rates supported by the UART.

   Description
   This enumeration lists the various baud rate settings that are supported by
   the UART interface and driver implementation. The specific baud rates achieved
   will be close approximations of these standard values based on the clock
   frequencies that can be obtained on the FX3 hardware. The actual baud rate
   acheived for both 403.2MHz and 416MHz SYS_CLK is listed below.
  
   See Also
   * CyU3PUartConfig_t
 */

typedef enum CyU3PUartBaudrate_t
{
    CY_U3P_UART_BAUDRATE_100    = 100,     /* Baud: 100,     Actual @403MHz: 100.00,     @416MHz: 100.00,    */
    CY_U3P_UART_BAUDRATE_300    = 300,     /* Baud: 300,     Actual @403MHz: 300.00,     @416MHz: 300.01,    */
    CY_U3P_UART_BAUDRATE_600    = 600,     /* Baud: 600,     Actual @403MHz: 600.00,     @416MHz: 599.99     */
    CY_U3P_UART_BAUDRATE_1200   = 1200,    /* Baud: 1200,    Actual @403MHz: 1200.00,    @416MHz: 1200.01    */
    CY_U3P_UART_BAUDRATE_2400   = 2400,    /* Baud: 2400,    Actual @403MHz: 2400.00,    @416MHz: 2399.96    */
    CY_U3P_UART_BAUDRATE_4800   = 4800,    /* Baud: 4800,    Actual @403MHz: 4800.00,    @416MHz: 4800.15    */
    CY_U3P_UART_BAUDRATE_9600   = 9600,    /* Baud: 9600,    Actual @403MHz: 9600.00,    @416MHz: 9599.41    */
    CY_U3P_UART_BAUDRATE_10000  = 10000,   /* Baud: 10000,   Actual @403MHz: 10000.00,   @416MHz: 10000.00   */
    CY_U3P_UART_BAUDRATE_14400  = 14400,   /* Baud: 14400,   Actual @403MHz: 14400.00,   @416MHz: 14400.44   */
    CY_U3P_UART_BAUDRATE_19200  = 19200,   /* Baud: 19200,   Actual @403MHz: 19200.00,   @416MHz: 19202.36   */
    CY_U3P_UART_BAUDRATE_38400  = 38400,   /* Baud: 38400,   Actual @403MHz: 38385.38,   @416MHz: 38404.73   */
    CY_U3P_UART_BAUDRATE_50000  = 50000,   /* Baud: 50000,   Actual @403MHz: 50000.00,   @416MHz: 50000.00   */
    CY_U3P_UART_BAUDRATE_57600  = 57600,   /* Baud: 57600,   Actual @403MHz: 57600.00,   @416MHz: 57585.83   */
    CY_U3P_UART_BAUDRATE_75000  = 75000,   /* Baud: 75000,   Actual @403MHz: 75000.00,   @416MHz: 75036.08   */
    CY_U3P_UART_BAUDRATE_100000 = 100000,  /* Baud: 100000,  Actual @403MHz: 100000.00,  @416MHz: 100000.00  */
    CY_U3P_UART_BAUDRATE_115200 = 115200,  /* Baud: 115200,  Actual @403MHz: 115068.49,  @416MHz: 115299.33  */
    CY_U3P_UART_BAUDRATE_153600 = 153600,  /* Baud: 153600,  Actual @403MHz: 153658.54,  @416MHz: 153392.33  */
    CY_U3P_UART_BAUDRATE_200000 = 200000,  /* Baud: 200000,  Actual @403MHz: 200000.00,  @416MHz: 200000.00  */
    CY_U3P_UART_BAUDRATE_225000 = 225000,  /* Baud: 225000,  Actual @403MHz: 225000.00,  @416MHz: 225108.23  */
    CY_U3P_UART_BAUDRATE_230400 = 230400,  /* Baud: 230400,  Actual @403MHz: 230136.99,  @416MHz: 230088.50  */
    CY_U3P_UART_BAUDRATE_300000 = 300000,  /* Baud: 300000,  Actual @403MHz: 300000.00,  @416MHz: 300578.03  */
    CY_U3P_UART_BAUDRATE_400000 = 400000,  /* Baud: 400000,  Actual @403MHz: 400000.00,  @416MHz: 400000.00  */
    CY_U3P_UART_BAUDRATE_460800 = 460800,  /* Baud: 460800,  Actual @403MHz: 462385.32,  @416MHz: 460176.99  */
    CY_U3P_UART_BAUDRATE_500000 = 500000,  /* Baud: 500000,  Actual @403MHz: 499009.90,  @416MHz: 500000.00  */
    CY_U3P_UART_BAUDRATE_750000 = 750000,  /* Baud: 750000,  Actual @403MHz: 752238.81,  @416MHz: 753623.19  */
    CY_U3P_UART_BAUDRATE_921600 = 921600,  /* Baud: 921600,  Actual @403MHz: 916363.64,  @416MHz: 928571.43  */
    CY_U3P_UART_BAUDRATE_1M     = 1000000, /* Baud: 1000000, Actual @403MHz: 1008000.00, @416MHz: 1000000.00 */
    CY_U3P_UART_BAUDRATE_2M     = 2000000, /* Baud: 2000000, Actual @403MHz: 2016000.00, @416MHz: 2000000.00 */
    CY_U3P_UART_BAUDRATE_3M     = 3000000, /* Baud: 3000000, Actual @403MHz: 2964705.88, @416MHz: 3058823.52 */
    CY_U3P_UART_BAUDRATE_4M     = 4000000, /* Baud: 4000000, Actual @403MHz: 3876923.08, @416MHz: 4000000.00 */
    CY_U3P_UART_BAUDRATE_4M608K = 4608000  /* Baud: 4608000, Actual @403MHz: 4581818.18, @416MHz: 4727272.72 */

} CyU3PUartBaudrate_t;

/* Summary
   List of number of stop bits to be used in UART communication.

   Description
   This enumeration lists the various number of stop bit settings that the
   UART interface can be configured to have. Only 1 and 2 are supported on
   the FX3 device.

   See Also
   * CyU3PUartConfig_t
 */
typedef enum CyU3PUartStopBit_t
{
    CY_U3P_UART_ONE_STOP_BIT = 1,       /* 1 stop bit */
    CY_U3P_UART_TWO_STOP_BIT = 2        /* 2 stop bit */
} CyU3PUartStopBit_t;

/* Summary
   List of parity settings supported by the UART interface.

   Description
   This enumeration lists the various parity settings that the UART interface
   can be configured to support.

   See Also
   * CyU3PUartConfig_t
 */
typedef enum CyU3PUartParity_t
{
    CY_U3P_UART_NO_PARITY      = 0,     /* No parity bits. */
    CY_U3P_UART_EVEN_PARITY,            /* Even parity. */
    CY_U3P_UART_ODD_PARITY,             /* Odd parity. */
    CY_U3P_UART_NUM_PARITY              /* Number of parity enumerations. */
} CyU3PUartParity_t;

/* Summary
   Configuration parameters for the UART interface.

   Description
   This structure defines all of the configurable parameters for the UART
   interface such as baud rate, stop and parity bits etc. A pointer to this
   structure is passed in to the CyU3PUartSetConfig function to configure
   the UART interface.

   The isDma member specifies whether the UART should be configured to transfer
   data one byte at a time, or in terms of large (user configurable size) blocks.

   All of the parameters can be changed dynamically by calling the CyU3PUartSetConfig
   function repeatedly.

   See Also
   * CyU3PUartBaudrate_t
   * CyU3PUartStopBit_t
   * CyU3PUartParity_t
   * CyU3PUartSetConfig
 */
typedef struct CyU3PUartConfig_t
{
    CyBool_t                txEnable;   /* Enable the transmitter. */
    CyBool_t                rxEnable;   /* Enable the receiver. */
    CyBool_t                flowCtrl;   /* Enable hardware flow control for Both RX and TX. */
    CyBool_t                isDma;      /* CyFalse: Byte by byte transfer;
                                           CyTrue: Block based transfer. */
    CyU3PUartBaudrate_t     baudRate;   /* Baud rate for data transfer. */
    CyU3PUartStopBit_t      stopBit;    /* The number of stop bits appended. */
    CyU3PUartParity_t       parity;     /* Parity configuration */

} CyU3PUartConfig_t;

/*
   Summary
   Prototype of UART event callback function.

   Description
   This function type defines a callback to be called after UART interrupt has
   been received. A function of this type can be registered with the UART driver
   as a callback function and will be called whenever an event of interest occurs.

   The UART has to be configured for DMA mode of transfer for callbacks
   to be registered.

   See Also
   * CyU3PUartEvt_t
   * CyU3PUartError_t
   * CyU3PRegisterUartCallBack
 */
typedef void (*CyU3PUartIntrCb_t)(
        CyU3PUartEvt_t evt,      /* Type of event that occured. */
        CyU3PUartError_t error   /* Specifies the actual error/status code when
                                   the event is of type CY_U3P_UART_EVENT_ERROR. */
        );

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/*@@UART Functions
 This section documents the functions that are defined as part of the UART
 driver and API library.
 */

/* Summary
   Starts the UART hardware block on the device.

   Description
   This function powers up the UART hardware block on the device and should be the
   first UART related function called by the application.

   Return Value
   * CY_U3P_SUCCESS               - if the init was successful
   * CY_U3P_ERROR_ALREADY_STARTED - if the UART block had been previously initialized
   * CY_U3P_ERROR_NOT_CONFIGURED  - if UART was not enabled during IO configuration

   See Also
   * CyU3PUartDeInit
   * CyU3PUartSetConfig
   * CyU3PUartTransmitBytes
   * CyU3PUartReceiveBytes
   * CyU3PUartTxSetBlockXfer
   * CyU3PUartRxSetBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PUartInit (
        void);

/* Summary
   Stops the UART hardware block.

   Description
   This function disables and powers off the UART hardware block on the device.

   Return Value
   * CY_U3P_SUCCESS           - if the de-init was successful
   * CY_U3P_ERROR_NOT_STARTED - if UART was not initialized

   See Also
   * CyU3PUartInit
   * CyU3PUartSetConfig
   * CyU3PUartTransmitBytes
   * CyU3PUartReceiveBytes
   * CyU3PUartTxSetBlockXfer
   * CyU3PUartRxSetBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PUartDeInit(
        void);

/* Summary
   Sets the UART interface parameters.

   Description
   This function configures the UART block with the desired user parameters such
   as transfer mode, baud rate etc. This function should be called repeatedly to
   make any change to the set of configuration parameters. This can be called on
   the fly repetitively without calling CyU3PUartInit. But this will reset the
   FIFO and hence the data in pipe will be lost.  If a DMA channel is present,
   a Reset has to be issued.

   Baudrate calculation:
   The maximum baud rate supported is 4MHz and the minimum is 100Hz. The UART
   block requires an internal clocking of 16X. It should be noted that even though
   the clock dividers and the API allows frequencies above and below the rated range,
   the device behaviour is not guaranteed. The actual clock rate is derived out of
   SYS_CLK for all baud rates >= 600. For rates below this SYS_CLK / 16 is used.
   Since the divider needs to be integral or with half divider, the frequency
   approximation is done using the following algorithm:

   If x is the actual divider and n is the required integral divider to be used,
   then following conditions are used to evaluate:

      x = (source clock) / (baudrate * 16);
      // source clock = SYS_CLK for baud >= 600 and source clock = (SYS_CLK / 16) for baud below 600.

      if (x - floor(x)) < 0.25 ==> n = floor(x);

      if (((x - floor(x)) >= 0.25) &&
          (x - floor(x)) < 0.5) ==> n = floor(x) + half divider;

      if (x - floor(x)) >= 0.75 ==> n = floor(x) + 1;

   Return Value
   * CY_U3P_SUCCESS            - if the configuration was set successfully
   * CY_U3P_ERROR_NOT_STARTED  - if UART was not initialized
   * CY_U3P_ERROR_NULL_POINTER - if a NULL pointer is passed
   * CY_U3P_ERROR_BAD_ARGUMENT - if any of the parameters are invalid
   * CY_U3P_ERROR_MUTEX_FAILURE - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PUartEvt_t
   * CyU3PUartError_t
   * CyU3PUartParity_t
   * CyU3PUartConfig_t
   * CyU3PUartIntrCb_t
   * CyU3PUartStopBit_t
   * CyU3PUartBaudrate_t
   * CyU3PUartInit
   * CyU3PUartDeInit
   * CyU3PUartTransmitBytes
   * CyU3PUartReceiveBytes
   * CyU3PUartTxSetBlockXfer
   * CyU3PUartRxSetBlockXfer

 */
extern CyU3PReturnStatus_t
CyU3PUartSetConfig (
        CyU3PUartConfig_t *config,  /* Pointer to structure containing config information */
        CyU3PUartIntrCb_t cb        /* Callback for getting the events */
        );

/* Summary
   Sets the number of bytes to be transmitted by the UART.

   Description
   This function sets the size of the desired data transmission through the
   UART. The value 0xFFFFFFFFU can be used to specify infinite or indefinite
   data transmission.

   This function is to be used when the UART is configured for DMA mode of
   transfer. If this function is called when the UART is configured in register
   mode, it will return with an error.

   Return Value
   * CY_U3P_SUCCESS              - if the transfer size was set successfully
   * CY_U3P_ERROR_NOT_CONFIGURED - if UART was not configured or initialized
                                   OR if the UART was configured for register mode
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PUartEvt_t
   * CyU3PUartError_t
   * CyU3PUartParity_t
   * CyU3PUartConfig_t
   * CyU3PUartIntrCb_t
   * CyU3PUartStopBit_t
   * CyU3PUartBaudrate_t
   * CyU3PUartInit
   * CyU3PUartDeInit
   * CyU3PUartSetConfig
   * CyU3PUartTransmitBytes
   * CyU3PUartReceiveBytes
   * CyU3PUartRxSetBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PUartTxSetBlockXfer (
        uint32_t txSize                         /* Desired transfer size. */
        );

/* Summary
   Sets the number of bytes to be received by the UART.

   Description
   This function sets the size of the desired data reception through the UART. The
   value 0xFFFFFFFFU can be used to specify infinite or indefinite data reception.

   This function is to be used when the UART is configured for DMA mode of
   transfer. If this function is called when the UART is configured in register
   mode, it will return with an error.

   Return Value
   * CY_U3P_SUCCESS              - if the transfer size was set successfully
   * CY_U3P_ERROR_NOT_CONFIGURED - if UART was not configured or initialized
                                   OR if the UART was configured for register mode
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PUartInit
   * CyU3PUartDeInit
   * CyU3PUartSetConfig
   * CyU3PUartTransmitBytes
   * CyU3PUartReceiveBytes
   * CyU3PUartTxSetBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PUartRxSetBlockXfer (
        uint32_t rxSize                         /* Desired transfer size. */
        );

/* Summary
   Transmits data through the UART interface on a byte by byte basis.

   Description
   This function is used to transfer "count" number of bytes out through the
   UART register interface. This function can only be used if the UART has been
   configured for register (non-DMA) transfer mode.

   Return Value
   Number of bytes that are successfully transferred. 

   status argument can take following values:
   * CY_U3P_SUCCESS              - if "count" bytes are transmitted successfully
   * CY_U3P_ERROR_NULL_POINTER   - if the data pointer is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT   - if the count is zero
   * CY_U3P_ERROR_TIMEOUT        - if the data transfer times out
   * CY_U3P_ERROR_NOT_CONFIGURED - if UART was configured for DMA mode of transfers OR
                                   if the UART was not configured or initialized
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also 
   * CyU3PUartInit
   * CyU3PUartDeInit
   * CyU3PUartSetConfig
   * CyU3PUartReceiveBytes
   * CyU3PUartTxSetBlockXfer
   * CyU3PUartRxSetBlockXfer
 */
extern uint32_t
CyU3PUartTransmitBytes (
        uint8_t *data_p,    /* Pointer to the data to be transferred. */
        uint32_t count,     /* Number of bytes to be transferred. */
        CyU3PReturnStatus_t *status /* Status returned from the operation */
        );

/* Summary
   Receives data from the UART interface on a byte by byte basis.

   Description
   This function is used to read "count" numbe of bytes from the UART register
   interface. This function can only be used if the UART has been configured for
   register (non-DMA) transfer mode.

   Return Value
   Number of bytes that are successfully received. 
   
   status argument can take following values:
   * CY_U3P_SUCCESS              - if "count" bytes are received successfully
   * CY_U3P_ERROR_NULL_POINTER   - if the data pointer is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT   - if the count is zero
   * CY_U3P_ERROR_TIMEOUT        - if the data transfer times out
   * CY_U3P_ERROR_NOT_CONFIGURED - if UART was configured for DMA mode of transfers OR
                                   if the UART was not configured or initialized
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PUartInit
   * CyU3PUartDeInit
   * CyU3PUartSetConfig
   * CyU3PUartTransmitBytes
   * CyU3PUartTxSetBlockXfer
   * CyU3PUartRxSetBlockXfer
 */
extern uint32_t
CyU3PUartReceiveBytes (
        uint8_t *data_p,      /* Pointer to location where the data read is to be placed. */
        uint32_t count,       /* Number of bytes to be received. */
        CyU3PReturnStatus_t *status     /* Status returned from the operation */
        );

/* Summary
   This function register the call back function for notification of UART interrupt.

   Description
   This function registers a callback function that will be called for notification of UART interrupts
   and also selects the UART interrupt sources of interest.

   Return Value
   None

   See Also
   * CyU3PUartEvt_t
   * CyU3PUartError_t
 */
extern void 
CyU3PRegisterUartCallBack (
                           CyU3PUartIntrCb_t uartIntrCb);

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3_UART_H_ */

/*[]*/

