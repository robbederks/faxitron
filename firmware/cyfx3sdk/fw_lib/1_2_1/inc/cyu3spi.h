/*
 ## Cypress USB 3.0 Platform header file (cyu3spi.h)
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

#ifndef _INCLUDED_CYU3SPI_H_
#define _INCLUDED_CYU3SPI_H_

#include "cyu3types.h"
#include "cyu3externcstart.h"
#include "cyu3lpp.h"

/*@@SPI Interface
   SPI (Serial Peripheral Interface) is a serial interface defined for
   inter-device communication. The FX3 device includes a SPI master
   that can connect to a variety of SPI slave devices and function at
   various speeds and modes. The SPI driver module provides functions to
   configure the SPI interface parameters and to perform data read/writes
   from/to the slave devices.
 */

/**************************************************************************
 ******************************* Data Types *******************************
 **************************************************************************/

/*@@SPI data types
   This section documents the enums and data structures that are defined as
   part of the SPI Interface Driver.
 */

/* Summary
   List of SPI related event types.

   Description
   This enumeration lists the various SPI related event codes that are notified
   to the user application through an event callback.

   See Also
   * CyU3PSpiError_t
   * CyU3PSpiCb_t
 */
typedef enum CyU3PSpiEvt_t
{
    CY_U3P_SPI_EVENT_RX_DONE = 0,             /* Reception is completed */
    CY_U3P_SPI_EVENT_TX_DONE,                 /* Transmission is done */
    CY_U3P_SPI_EVENT_ERROR                    /* Error has occurred */
} CyU3PSpiEvt_t;

/* Summary
   List of SPI specific error / status codes.

   Description
   This type lists the various SPI specific error / status codes that are sent
   to the event callback as event data, when the event type is CY_U3P_SPI_EVENT_ERROR.

   See Also
   * CyU3PSpiEvt_t
 */
typedef enum CyU3PSpiError_t
{
    CY_U3P_SPI_ERROR_TX_OVERFLOW = 12,       /* Write to TX_FIFO when FIFO is full. */
    CY_U3P_SPI_ERROR_RX_UNDERFLOW = 13,      /* Read from RX_FIFO when FIFO is empty. */
    CY_U3P_SPI_ERROR_NONE = 15               /* No error has happened. */
} CyU3PSpiError_t;

/* Summary
   Enumeration defining the lead and lag times of SSN with respect to SCK

   Description
   The SSN needs to lead the SCK at the beginning of a transaction and SSN needs
   to lag the SCK at the end of a transfer. This enumeration gives customization
   allowed for this.  This enumeration is required only for hardware controlled SSN.

   See Also
   * CyU3PSpiConfig_t
   * CyU3PSpiSetConfig
 */
typedef enum CyU3PSpiSsnLagLead_t
{
    CY_U3P_SPI_SSN_LAG_LEAD_ZERO_CLK = 0,     /* SSN is in sync with SCK. */
    CY_U3P_SPI_SSN_LAG_LEAD_HALF_CLK,         /* SSN leads / lags SCK by a
                                                 half clock cycle. */
    CY_U3P_SPI_SSN_LAG_LEAD_ONE_CLK,          /* SSN leads / lags SCK by one
                                                 clock cycle. */
    CY_U3P_SPI_SSN_LAG_LEAD_ONE_HALF_CLK,     /* SSN leads / lags SCK by one
                                                 and half clock cycles. */
    CY_U3P_SPI_NUM_SSN_LAG_LEAD               /* Number of enumerations. */

} CyU3PSpiSsnLagLead_t;

/* Summary
   Enumeration defining the various ways in which the SSN for a SPI
   device can be controlled.
 
   Description
   The SSN can be controlled by the firmware (API) which is not synchronized
   with SPI clock. It is asserted at the beginning of a transfer and is
   de-asserted at the end of the transfer.

   The SSN can be controlled by the hardware. In this case the SSN assert and
   de-asserts are done in sync with SPI clock.

   The SSN can be controlled by the application, external to the API. This can
   be done using GPIOs. In this case the hardware / API is not aware of the SSN.

   The APIs allow only control of one SSN line. If there are more than one SPI
   slave device, then at-most one device can be controlled by hardware / API.

   For example, if there are two devices, and SSN for one is controlled by
   hardware / API, SetConfig request needs to be called whenever the device to
   be addressed changes. On the other hand if the SSN for both devices are
   controlled externally (via GPIO), then SetConfig needs to be called only
   once, provided rest of the configuration stays the same.

   See Also
   * CyU3PSpiConfig_t
   * CyU3PSpiSetConfig
 */
typedef enum CyU3PSpiSsnCtrl_t
{
    CY_U3P_SPI_SSN_CTRL_FW = 0,         /* SSN is controlled by API and is not
                                           at clock boundaries. It is asserted
                                           at beginning of transfer is de-asserted
                                           at end of transfer. */
    CY_U3P_SPI_SSN_CTRL_HW_END_OF_XFER, /* SSN is controlled by hardware and is
                                           done in sync with clock. The SSN is
                                           asserted at the beginning of a transfer
                                           and is de-asserted at the end of a
                                           transfer or when no data is available
                                           to transmit (underrun). */
    CY_U3P_SPI_SSN_CTRL_HW_EACH_WORD,   /* SSN is controlled by the hardware and
                                           is done in sync with clock. The SSN is
                                           asserted at the beginning of transfer of
                                           every word and de-asserted at the end of
                                           the transfer of that word. So if a transfer
                                           consists of 64 word, the SSN is asserted
                                           and de-asserted 64 times. */
    CY_U3P_SPI_SSN_CTRL_HW_CPHA_BASED,  /* If CPHA is 0, the SSN control is per word,
                                           and if CPHA is 1, then the SSN control is
                                           per transfer. */
    CY_U3P_SPI_SSN_CTRL_NONE,           /* SSN control is done externally. The SSN
                                           lines are selected by the application and
                                           is ignored by the hardware / API. */
    CY_U3P_SPI_NUM_SSN_CTRL             /* Number of enumerations. */

} CyU3PSpiSsnCtrl_t;

/* Summary
   Structure defining the configuration of SPI interface.

   Description
   This structure encapsulates all of the configurable parameters that can be
   selected for the SPI interface. The CyU3PSpiSetConfig() function accepts a
   pointer to this structure, and updates all of the interface parameters.

   See Also
   * CyU3PSpiSsnCtrl_t
   * CyU3PSpiSclkParam_t
   * CyU3PSpiSetConfig
 */
typedef struct CyU3PSpiConfig_t
{
    CyBool_t             isLsbFirst; /* Data shift mode - CyFalse: MSB first,
                                        CyTrue: LSB first */
    CyBool_t             cpol;       /* Clock polarity - CyFalse(0): SCK idles low,
                                        CyTrue(1): SCK idles high */
    CyBool_t             cpha;       /* Clock phase - CyFalse(0): Slave samples at
                                        idle-active edge, CyTrue(1): Slave samples
                                        at active-idle edge */
    CyBool_t             ssnPol;     /* Polarity of SSN line. CyFalse (0): SSN is
                                        active low, CyTrue (1): SSN is active high. */
    CyU3PSpiSsnCtrl_t    ssnCtrl;    /* SSN control */
    CyU3PSpiSsnLagLead_t leadTime;   /* Time between SSN's assertion and first SCLK's
                                        edge. This is at the beginning of a 
                                        transfer and is valid only for hardware
                                        controlled SSN. Zero lead time is not
                                        supported. */
    CyU3PSpiSsnLagLead_t lagTime;    /* Time between the last SCK edge to SSN's
                                        de-assertion. This is at the end of a 
                                        transfer and is valid only for hardware
                                        controlled SSN. For CPHA = 1, lag time
                                        cannot be zero. */
    uint32_t            clock;       /* SPI clock frequency in Hz. */
    uint8_t             wordLen;     /* Word length in bits. Valid values are 4 - 32. */
} CyU3PSpiConfig_t;

/*
   Summary
   Prototype of SPI event callback function.

   Description
   This function type defines a callback to be called after SPI interrupt has
   been received. A function of this type can be registered with the LPP driver
   as a callback function and will be called whenever an event of interest occurs.

   See Also
   * CyU3PSpiEvt_t
   * CyU3PSpiError_t
   * CyU3PRegisterSpiCallBack
 */
typedef void (*CyU3PSpiIntrCb_t) (
        CyU3PSpiEvt_t evt,          /* Type of event that occured */
        CyU3PSpiError_t error       /* Specifies the actual error/status code when
                                       the event is of type CY_U3P_SPI_EVENT_ERROR */
        );

/**************************************************************************
 *************************** FUNCTION PROTOTYPE ***************************
 **************************************************************************/

/*@@SPI Functions
   This section documents the API functions that are defined as part of the
   SPI Interface Driver.
 */

/* Summary
   Starts the SPI interface block on the device.

   Description
   This function powers up the SPI interface block on the device and is expected
   to be the first SPI API function that is called by the application.

   Return value
   * CY_U3P_SUCCESS               - When the init is successful
   * CY_U3P_ERROR_NOT_CONFIGURED  - When SPI has not been enabled in the IO configuration
   * CY_U3P_ERROR_ALREADY_STARTED - When the SPI block has already been initialized

   See Also
   * CyU3PSpiDeInit
   * CyU3PSpiSetConfig
   * CyU3PSpiSetSsnLine
   * CyU3PSpiTransmitWords
   * CyU3PSpiReceiveWords
   * CyU3PSpiSetBlockXfer
   * CyU3PSpiWaitForBlockXfer
   * CyU3PSpiDisableBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PSpiInit (
        void
        );

/* Summary
   Stops the SPI module.

   Description
   This function disables and powers off the SPI interface. This function can
   be used to shut off the interface to save power when it is not in use.

   Return value
   * CY_U3P_SUCCESS           - When the de-init is successful
   * CY_U3P_ERROR_NOT_STARTED - When the SPI block has not been initialized 

   See also
   * CyU3PSpiInit
   * CyU3PSpiSetConfig
   * CyU3PSpiSetSsnLine
   * CyU3PSpiTransmitWords
   * CyU3PSpiReceiveWords
   * CyU3PSpiSetBlockXfer
   * CyU3PSpiWaitForBlockXfer
   * CyU3PSpiDisableBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PSpiDeInit (
        void);

/* Summary
   Sets and configures SPI interface parameters.

   Description
   This function is used to configure the SPI master interface based on the
   desired settings to talk to the desired slave. This function can be called
   repeatedly to change the settings if different settings are to be used to
   communicate with different slave devices.

   This will reset the FIFO and hence the data in pipe will be lost. If a DMA
   channel is present, a DMA reset has to be issued.

   The callback parameter is used to specify an event callback function that
   will be called by the driver when an SPI interrupt occurs.

   SPI-clock calculation:
   The maximum SPI clock supported is 33MHz and the minimum is 10KHz. The SPI block
   requires an internal clocking of 2X. It should be noted that even though the
   clock dividers and the API allows frequencies above and below   the rated range,
   the device behaviour is not guaranteed. The actual clock rate is derived out of
   SYS_CLK. Since the divider needs to be integral or with half divider, the
   frequency approximation is done using the following algorithm:

   If x is the actual divider and n is the required integral divider to be used,
   then following conditions are used to evaluate:

      x = (SYS_CLK) / (SPI_clock * 2);

      if (x - floor(x)) < 0.25 ==> n = floor(x);

      if (((x - floor(x)) >= 0.25) &&
          (x - floor(x)) < 0.5) ==> n = floor(x) + half divider;

      if (x - floor(x)) >= 0.75 ==> n = floor(x) + 1;

   Return value
   * CY_U3P_SUCCESS                - When the SetConfig is successful
   * CY_U3P_ERROR_NOT_STARTED      - When the SPI block has not been initialized
   * CY_U3P_ERROR_NULL_POINTER     - When the config pointer is NULL 
   * CY_U3P_ERROR_BAD_ARGUMENT     - When a configuration value is invalid
   * CY_U3P_ERROR_INVALID_SEQUENCE - When API call sequence is invalid, eg: a previously
                                     set up DMA transfer has not been disabled
   * CY_U3P_ERROR_TIMEOUT          - When the operation times out
   * CY_U3P_ERROR_MUTEX_FAILURE    - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PSpiCb_t
   * CyU3PSpiEvt_t
   * CyU3PSpiError_t
   * CyU3PSpiConfig_t
   * CyU3PSpiIntrCb_t
   * CyU3PSpiSsnCtrl_t
   * CyU3PSpiSclkParam_t
   * CyU3PSpiSsnLagLead_t
   * CyU3PSpiInit
   * CyU3PSpiDeInit
   * CyU3PSpiSetSsnLine
   * CyU3PSpiTransmitWords
   * CyU3PSpiReceiveWords
   * CyU3PSpiSetBlockXfer
   * CyU3PSpiWaitForBlockXfer
   * CyU3PSpiDisableBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PSpiSetConfig (
        CyU3PSpiConfig_t *config,   /* pointer to the SPI config structure */
        CyU3PSpiIntrCb_t cb         /* Callback for receiving SPI events */
        );

/* 
   Summary
   Assert / Deassert the SSN Line.

   Description
   Asserts / deaaserts SSN Line for the default slave device. This is possible
   only if the SSN line control is configured for FW controlled mode.

   Return value
   * CY_U3P_SUCCESS               - When the call is successful
   * CY_U3P_ERROR_NOT_CONFIGURED  - When SPI has not been configured or initialized
                                    OR SSN is not firmware controlled.
   * CY_U3P_ERROR_MUTEX_FAILURE   - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PSpiInit
   * CyU3PSpiDeInit
   * CyU3PSpiSetConfig
   * CyU3PSpiTransmitWords
   * CyU3PSpiReceiveWords
   * CyU3PSpiSetBlockXfer
   * CyU3PSpiWaitForBlockXfer
   * CyU3PSpiDisableBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PSpiSetSsnLine (
        CyBool_t isHigh        /* Cyfalse: Pull down the SSN line,
                                  CyTrue: Pull up the SSN line */
        );

/*
   Summary
   Transmits data word by word over the SPI interface.

   Description
   This function is used to transmit data word by word. The function can be called
   only if there is no active DMA transfer on the bus. If a SetBlockXfer was called,
   then a DisableBlockXfer needs to be called.

   Return value
   * CY_U3P_SUCCESS               - When the TransmitWords is successful
   * CY_U3P_ERROR_NOT_CONFIGURED  - When SPI has not been configured or initialized
   * CY_U3P_ERROR_NULL_POINTER    - When the data pointer is NULL 
   * CY_U3P_ERROR_ALREADY_STARTED - When a DMA transfer is active
   * CY_U3P_ERROR_BAD_ARGUMENT    - When a configured word length is invalid
   * CY_U3P_ERROR_TIMEOUT         - When the operation times out
   * CY_U3P_ERROR_MUTEX_FAILURE   - When there is a failure in acquiring a mutex lock
   
   See Also
   * CyU3PSpiInit
   * CyU3PSpiDeInit
   * CyU3PSpiSetConfig
   * CyU3PSpiSetSsnLine
   * CyU3PSpiReceiveWords
   * CyU3PSpiSetBlockXfer
   * CyU3PSpiWaitForBlockXfer
   * CyU3PSpiDisableBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PSpiTransmitWords (
        uint8_t *data,       /* Source data pointer. This needs to be padded to nearest
                                byte if the word length is not byte aligned. */
        uint32_t byteCount   /* This needs to be a multiple of word length aligned where
                                each word is aligned to a byte.*/
        );

/*
   Summary
   Receives data word by word over the Spi interface.

   Description
   This function can be called only when there is no active DMA transfer. If
   there was any previous DMA operation with SPI, then a DisableBlockXfer
   needs to be called.

   Return value
   * CY_U3P_SUCCESS               - When the ReceiveWords is successful
   * CY_U3P_ERROR_NOT_CONFIGURED  - When SPI has not been configured or initialized
   * CY_U3P_ERROR_NULL_POINTER    - When the data pointer is NULL 
   * CY_U3P_ERROR_ALREADY_STARTED - When a DMA transfer is active
   * CY_U3P_ERROR_BAD_ARGUMENT    - When a configured word length is invalid
   * CY_U3P_ERROR_TIMEOUT         - When the operation times out
   * CY_U3P_ERROR_MUTEX_FAILURE   - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PSpiInit
   * CyU3PSpiDeInit
   * CyU3PSpiSetConfig
   * CyU3PSpiSetSsnLine
   * CyU3PSpiTransmitWords
   * CyU3PSpiSetBlockXfer
   * CyU3PSpiWaitForBlockXfer
   * CyU3PSpiDisableBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PSpiReceiveWords (
        uint8_t *data,       /* Destination buffer pointer */
        uint32_t byteCount   /* The byte size of the buffer. This needs to be a multiple
                                of word length aligned to nearest byte size. For example,
                                if the word length is 18 bits then each word would be
                                placed in 3 bytes (valid data on LSBs). If the byte count
                                is not aligned, then the call returns with an error.*/
        );

/* 
   Summary
   This function enables the DMA transfer.

   Description
   This function switches SPI to DMA mode.

   If the txSize parameter is non-zero then TX is enabled and if rxSize
   parameter is non-zero, then RX is enabled. If both are enabled, then
   SPI block will stall if any of the DMA pipe is stalled.

   txSize == rxSize:
   In this case both TX and RX will function simultaneously. If the DMA pipe
   gets stalled due to buffer non-availability, both TX and RX will be stalled
   and will resume when both DMA pipes are active. So there is no loss of synchronization.

   txSize > rxSize:
   In this case RX will receive only as many words as requested and will stop
   when the count reaches that point. TX will continue to receive data until
   its count runs down.

   txSize < rxSize:
   In this case the TX and RX will function till all TX words are received and
   then will stall. The DisableBlockXfer API needs to be called explicitly for
   TX for the RX to resume.

   A call to SetBlockXfer has to be followed by a call to DisableBlockXfer before the 
   SPI can be used in Register mode.

   Return Value
   * CY_U3P_SUCCESS               - When the SetBlockXfer is successful
   * CY_U3P_ERROR_NOT_CONFIGURED  - When SPI has not been configured or initialized
   * CY_U3P_ERROR_ALREADY_STARTED - When a DMA transfer is active
   * CY_U3P_ERROR_MUTEX_FAILURE   - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PSpiInit
   * CyU3PSpiDeInit
   * CyU3PSpiSetConfig
   * CyU3PSpiSetSsnLine
   * CyU3PSpiTransmitWords
   * CyU3PSpiReceiveWords
   * CyU3PSpiWaitForBlockXfer
   * CyU3PSpiDisableBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PSpiSetBlockXfer (
        uint32_t txSize,  /* Number of words to be transmitted (not bytes) */
        uint32_t rxSize   /* Number of words to be received  (not bytes) */
        );

/*
   Summary
   This function disabled the TX / RX DMA functionality.

   Description
   The function can be called only when DMA has already been activated. The
   function will disable only the block requested for. If both TX and RX gets
   disabled, then it disables the DMA mode as well. This needs to be invoked
   to do register mode operations after a SetBlockXfer call.

   Return Value
   * CY_U3P_SUCCESS               - When the DisableBlockXfer is successful
   * CY_U3P_ERROR_NOT_CONFIGURED  - When SPI has not been configured or initialized 
                                    no DMA transfer has been configured
   * CY_U3P_ERROR_MUTEX_FAILURE   - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PSpiInit
   * CyU3PSpiDeInit
   * CyU3PSpiSetConfig
   * CyU3PSpiSetSsnLine
   * CyU3PSpiTransmitWords
   * CyU3PSpiReceiveWords
   * CyU3PSpiSetBlockXfer
   * CyU3PSpiWaitForBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PSpiDisableBlockXfer (
        CyBool_t rxDisable, /* CyTrue: Disable TX block if active
                               CyFalse: Ignore TX block.*/
        CyBool_t txDisable  /* CyTrue: Disable RX block if active.
                               CyFalse: Ignore RX block.*/
        );

/*
   Summary
   Wait until the ongoing SPI data transfer is finished.

   Description
   This function can be used to ensure that a previous SPI transaction has
   completed, in the case where the callback is not being used.

   Note
   * This function will keep waiting for a transfer complete event until the pre-defined
   timeout period, if it is invoked before any SPI transfers are requested.

   * In the case of a DMA read of data that does not fill the DMA buffer(s) associated
   with the read DMA channel, the DMA transfer remains pending after this API call
   returns. The data can only be retrieved from the DMA buffer after the DMA transfer
   is terminated through the CyU3PDmaChannelSetWrapUp API.

   Return value
   * CY_U3P_SUCCESS               - When the WaitForBlockXfer is successful
   * CY_U3P_ERROR_NOT_CONFIGURED  - When SPI has not been configured or initialized
   * CY_U3P_ERROR_TIMEOUT         - When the operation times out
   * CY_U3P_ERROR_FAILURE         - When the operation encounters an error 
   * CY_U3P_ERROR_MUTEX_FAILURE   - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PSpiInit
   * CyU3PSpiDeInit
   * CyU3PSpiSetConfig
   * CyU3PSpiSetSsnLine
   * CyU3PSpiTransmitWords
   * CyU3PSpiReceiveWords
   * CyU3PSpiSetBlockXfer
   * CyU3PSpiDisableBlockXfer
 */
extern CyU3PReturnStatus_t
CyU3PSpiWaitForBlockXfer (
        CyBool_t isRead       /* 0: Write operation, 1: Read operation */
        );

/* Summary
   This function register the call back function for notification of SPI interrupt.

   Description
   This function registers a callback function that will be called for notification of SPI interrupts
   and also selects the SPI interrupt sources of interest.

   Return Value
   None

   See Also
   * CyU3PSpiEvt_t
   * CyU3PSpiError_t
 */
extern void 
CyU3PRegisterSpiCallBack (
                          CyU3PSpiIntrCb_t spiIntrCb);

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3SPI_H_ */

/*[]*/
