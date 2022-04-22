/*
 ## Cypress USB 3.0 Platform header file (cyfx3spi.h)
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

#ifndef _INCLUDED_CYFX3SPI_H_
#define _INCLUDED_CYFX3SPI_H_

#include <cyu3types.h>
#include <cyfx3error.h>

#include <cyu3externcstart.h>
/*@@FX3 Boot SPI Interface
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

/*@@FX3 Boot SPI Data Types
   This section documents the enums and data structures that are defined as
   part of the SPI Interface Driver.
 */

/* Summary
   Enumeration defining the lead and lag times of SSN with respect to SCK.

   Description
   The SSN needs to lead the SCK at the beginning of a transaction and SSN needs
   to lag the SCK at the end of a transfer. This enumeration gives customization
   allowed for this.  This enumeration is required only for hardware controlled SSN.

   See Also
   * CyFx3BootSpiConfig_t
   * CyFx3BootSpiSetConfig
 */
typedef enum CyFx3BootSpiSsnLagLead_t
{
    CY_FX3_BOOT_SPI_SSN_LAG_LEAD_ZERO_CLK = 0,     /* SSN is in sync with SCK. */
    CY_FX3_BOOT_SPI_SSN_LAG_LEAD_HALF_CLK,         /* SSN leads / lags SCK by a
                                                 half clock cycle. */
    CY_FX3_BOOT_SPI_SSN_LAG_LEAD_ONE_CLK,          /* SSN leads / lags SCK by one
                                                 clock cycle. */
    CY_FX3_BOOT_SPI_SSN_LAG_LEAD_ONE_HALF_CLK,     /* SSN leads / lags SCK by one
                                                 and half clock cycles. */
    CY_FX3_BOOT_SPI_NUM_SSN_LAG_LEAD               /* Number of enumerations. */

} CyFx3BootSpiSsnLagLead_t;

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
   * CyFx3BootSpiConfig_t
   * CyFx3BootSpiSetConfig
 */
typedef enum CyFx3BootSpiSsnCtrl_t
{
    CY_FX3_BOOT_SPI_SSN_CTRL_FW = 0,         /* SSN is controlled by API and is not
                                           at clock boundaries. It is asserted
                                           at beginning of transfer is de-asserted
                                           at end of transfer. */
    CY_FX3_BOOT_SPI_SSN_CTRL_HW_END_OF_XFER, /* SSN is controlled by hardware and is
                                           done in sync with clock. The SSN is
                                           asserted at the beginning of a transfer
                                           and is de-asserted at the end of a
                                           transfer or when no data is available
                                           to transmit (underrun). */
    CY_FX3_BOOT_SPI_SSN_CTRL_HW_EACH_WORD,   /* SSN is controlled by the hardware and
                                           is done in sync with clock. The SSN is
                                           asserted at the beginning of transfer of
                                           every word and de-asserted at the end of
                                           the transfer of that word. So if a transfer
                                           consists of 64 word, the SSN is asserted
                                           and de-asserted 64 times. */
    CY_FX3_BOOT_SPI_SSN_CTRL_HW_CPHA_BASED,  /* If CPHA is 0, the SSN control is per word,
                                           and if CPHA is 1, then the SSN control is
                                           per transfer. */
    CY_FX3_BOOT_SPI_SSN_CTRL_NONE,           /* SSN control is done externally. The SSN
                                           lines are selected by the application and
                                           is ignored by the hardware / API. */
    CY_FX3_BOOT_SPI_NUM_SSN_CTRL             /* Number of enumerations. */

} CyFx3BootSpiSsnCtrl_t;

/* Summary
   Structure defining the configuration of SPI interface.

   Description
   This structure encapsulates all of the configurable parameters that can be
   selected for the SPI interface. The CyFx3BootSpiSetConfig() function accepts a
   pointer to this structure, and updates all of the interface parameters.

   See Also
   * CyFx3BootSpiSsnCtrl_t
   * CyFx3BootSpiSclkParam_t
   * CyFx3BootSpiSetConfig
 */
typedef struct CyFx3BootSpiConfig_t
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
    CyFx3BootSpiSsnCtrl_t    ssnCtrl;    /* SSN control */
    CyFx3BootSpiSsnLagLead_t leadTime;   /* Time between SSN's assertion and first SCLK's
                                        edge. This is at the beginning of a 
                                        transfer and is valid only for hardware
                                        controlled SSN. Zero lead time is not
                                        supported. */
    CyFx3BootSpiSsnLagLead_t lagTime;    /* Time between the last SCK edge to SSN's
                                        de-assertion. This is at the end of a 
                                        transfer and is valid only for hardware
                                        controlled SSN. For CPHA = 1, lag time
                                        cannot be zero. */
    uint32_t            clock;       /* SPI clock frequency in Hz. */
    uint8_t             wordLen;     /* Word length in bits. Valid values are 4 - 32. */
} CyFx3BootSpiConfig_t;

/**************************************************************************
 *************************** FUNCTION PROTOTYPE ***************************
 **************************************************************************/

/*@@FX3 Boot SPI Functions
   This section documents the API functions that are defined as part of the
   SPI Interface Driver.
 */

/* Summary
   Starts the SPI interface block on the device.

   Description
   This function powers up the SPI interface block on the device and is expected
   to be the first SPI API function that is called by the application.

   Return value
   * CY_FX3_BOOT_SUCCESS - if the SPI block was successfully initialized.
   * CY_FX3_BOOT_ERROR_NOT_SUPPORTED - if the FX3 part in use does not support the SPI interface.

   See Also
   * CyFx3BootSpiDeinit
 */
extern CyFx3BootErrorCode_t
CyFx3BootSpiInit (
        void
        );

/* Summary
   Stops the SPI module.

   Description
   This function disables and powers off the SPI interface. This function can
   be used to shut off the interface to save power when it is not in use.

   Return value
   * None

   See also
   * CyFx3BootSpiInit
 */
extern void
CyFx3BootSpiDeInit (
        void);

/* Summary
   Sets and configures Spi interface parameters.

   Description
   This function is used to configure the SPI master interface based on the
   desired settings to talk to the desired slave. This function can be called
   repeatedly to change the settings if different settings are to be used to
   communicate with different slave devices.

   This will reset the FIFO and hence the data in pipe will be lost.

   The callback parameter is used to specify an event callback function that
   will be called by the driver when an SPI interrupt occurs.

   Return value
   * CY_FX3_BOOT_SUCCESS                - When the SetConfig is successful
   * CY_FX3_BOOT_ERROR_NOT_STARTED      - If the SPI block has not been initialized
   * CY_FX3_BOOT_ERROR_NULL_POINTER     - When the config pointer is NULL 
   * CY_FX3_BOOT_ERROR_TIMEOUT          - When the operation times out

   See Also
   * CyFx3BootSpiConfig_t
   * CyFx3BootSpiCb_t

 */
extern CyFx3BootErrorCode_t
CyFx3BootSpiSetConfig (
        CyFx3BootSpiConfig_t *config   /* pointer to the SPI config structure */
        );

/* 
   Summary
   Assert / Deassert the SSN Line.

   Description
   Asserts / deaaserts SSN Line for the default slave device. This is possible
   only if the SSN line control is configured for FW controlled mode.

   Return value
   * None

   See Also
   None
 */
extern void
CyFx3BootSpiSetSsnLine (
        CyBool_t isHigh        /* Cyfalse: Pull down the SSN line,
                                  CyTrue: Pull up the SSN line */
        );

/*
   Summary
   Transmits data word by word over the SPI interface

   Description
   This function is used to transmit data word by word. The function can be called
   only if there is no active DMA transfer on the bus. If a SetBlockXfer was called,
   then a DisableBlockXfer needs to be called.

   Return value
   * CY_FX3_BOOT_SUCCESS                - When the TransmitWords is successful
   * CY_FX3_BOOT_ERROR_NULL_POINTER     - When the data pointer is NULL 
   * CY_FX3_BOOT_ERROR_XFER_FAILURE     - When the TransmitWords fails to transfer the data.
   * CY_FX3_BOOT_ERROR_TIMEOUT          - When the TransmitWords timesout.
   See Also
   * CyFx3BootSpiReceiveWords
 */
extern CyFx3BootErrorCode_t
CyFx3BootSpiTransmitWords (
        uint8_t *data,       /* Source data pointer. This needs to be padded to nearest
                                byte if the word length is not byte aligned. */
        uint32_t byteCount   /* This needs to be a multiple of word length aligned where
                                each word is aligned to a byte.*/
        );

/*
   Summary
   Receives data word by word over the SPI interface.

   Description
   This function can be called only when there is no active DMA transfer. If
   there was any previous DMA operation with SPI, then a DisableBlockXfer
   needs to be called.

   Return value
   * CY_FX3_BOOT_SUCCESS               - When the ReceiveWords is successful
   * CY_FX3_BOOT_ERROR_NULL_POINTER    - When the data pointer is NULL 
   * CY_FX3_BOOT_ERROR_XFER_FAILURE    - When the ReceiveWords fails.
   * CY_FX3_BOOT_ERROR_TIMEOUT         - When the ReceiveWords timesout.

   See Also
   * CyFx3BootSpiTransmitWords
 */
extern CyFx3BootErrorCode_t
CyFx3BootSpiReceiveWords (
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
   * None

   See Also
   * CyFx3BootSpiDisableBlockXfer
 */
extern void
CyFx3BootSpiSetBlockXfer (
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
   * None
   
   See Also
   * CyFx3BootSpiSetBlockXfer
 */
extern void
CyFx3BootSpiDisableBlockXfer ( 
        void );

/* Summary
   This function is used to setup a dma from CPU to SPI or vice versa.

   Description
   This function is a blocking call. This function is used to read/write length number 
   of bytes from/to SPI. This function can be used only if the SPI has been configured
   for DMA transfer mode.
   This function is used to create a one shot DMA channel to transfer to the
   data. Infinite transfers are not supported.

   Return Value
   * CY_FX3_BOOT_SUCCESS            - if the data transfer is successful
   * CY_FX3_BOOT_ERROR_XFER_FAILURE - if the data transfer encountered any error
   * CY_FX3_BOOT_ERROR_TIMEOUT      - if the data transfer times out

   See Also
   * CyFx3BootSpiDisableBlockXfer
   * CyFx3BootSpiSetBlockXfer
 */
extern CyFx3BootErrorCode_t
CyFx3BootSpiDmaXferData (
        CyBool_t isRead,    /* isRead = CyTrue for read operations 
                               isRead = CyFalse for write operations */
        uint32_t address,   /* address of the buffer from/to which data is to
                               be transferred */
        uint32_t length,    /* length of the data to be transferred.
                               Maximum length of the data that can be transferred is as
                               defined by the Page size of the SPI device. */
        uint32_t timeout    /* Timeout in 10s of us. Also refer the macros CY_FX3_BOOT_NO_WAIT 
                               and CY_FX3_BOOT_WAIT_FOREVER */
        );


#include <cyu3externcend.h>
#endif /* _INCLUDED_CYFX3SPI_H_ */
/*[]*/
