/*
 ## Cypress USB 3.0 Platform header file (cyu3i2s.h)
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

#ifndef _INCLUDED_CYU3I2S_H_
#define _INCLUDED_CYU3I2S_H_

#include "cyu3types.h"
#include "cyu3externcstart.h"
#include "cyu3lpp.h"

/*@@I2S Interface
   The I2S (Inter-IC Sound) interface is a serial interface defined for
   communication of stereophonic audio data between devices. The FX3 device
   includes a I2S master interface which can be connected to an I2S peripheral
   and send out audio data. The I2S driver module provides functions to
   configure the I2S interface and to send mono or stereo audio output on the
   I2S link.
 */

/**************************************************************************
 ******************************* Data Types *******************************
 **************************************************************************/

/*@@I2S data types
   This section documents the data types that are defined as
   part of the I2S driver and API library.
 */

/* Summary
   List of I2S related event types.

   Description
   This enumeration lists the various I2S related event codes that are notified
   to the user application through an event callback.

   See Also
   * CyU3PI2sCb_t
   * CyU3PI2sError_t
 */
typedef enum CyU3PI2sEvt_t
{
    CY_U3P_I2S_EVENT_TXL_DONE = 0,         /* Left Transmission is done */
    CY_U3P_I2S_EVENT_TXR_DONE,             /* Right Transmission is done */
    CY_U3P_I2S_EVENT_PAUSED,               /* Pause has taken effect */
    CY_U3P_I2S_EVENT_ERROR                 /* Error has happened */

} CyU3PI2sEvt_t;

/* Summary
   List of I2S specific error/status codes.

   Description
   This type lists the various I2S specific error/status codes that are sent to the
   event callback as event data, when the event type is CY_U3P_I2S_ERROR_EVT.

   See Also
   * CyU3PI2sEvt_t
  */
typedef enum CyU3PI2sError_t
{
    CY_U3P_I2S_ERROR_LTX_UNDERFLOW = 11,   /* A left channel underflow occurred. */
    CY_U3P_I2S_ERROR_RTX_UNDERFLOW,        /* A right channel underflow occurred. */
    CY_U3P_I2S_ERROR_LTX_OVERFLOW,         /* A left channel overflow occurred. */
    CY_U3P_I2S_ERROR_RTX_OVERFLOW          /* A right channel overflow occurred. */

} CyU3PI2sError_t;

/* Summary
   List of supported bit widths.

   Description
   This type lists the supported bitwidths on the I2S interface.

   See Also
   * CyU3PI2sConfig_t
 */
typedef enum CyU3PI2sSampleWidth_t
{
    CY_U3P_I2S_WIDTH_8_BIT = 0,         /* 8  bit */
    CY_U3P_I2S_WIDTH_16_BIT,            /* 16 bit */
    CY_U3P_I2S_WIDTH_18_BIT,            /* 18 bit */
    CY_U3P_I2S_WIDTH_24_BIT,            /* 24 bit */
    CY_U3P_I2S_WIDTH_32_BIT,            /* 32 bit */
    CY_U3P_I2S_NUM_BIT_WIDTH            /* Number of options. */

} CyU3PI2sSampleWidth_t;

/* Summary
   List of supported sample rates.

   Description
   This type lists the supported sample rates for audio playback through the
   I2S interface.

   See Also
   * CyU3PI2sConfig_t
 */
typedef enum CyU3PI2sSampleRate_t
{
    CY_U3P_I2S_SAMPLE_RATE_8KHz = 8000,           /* 8    KHz */
    CY_U3P_I2S_SAMPLE_RATE_16KHz = 16000,         /* 16   KHz */
    CY_U3P_I2S_SAMPLE_RATE_32KHz = 32000,         /* 32   KHz */
    CY_U3P_I2S_SAMPLE_RATE_44_1KHz = 44100,       /* 44.1 KHz */
    CY_U3P_I2S_SAMPLE_RATE_48KHz = 48000,         /* 48   KHz */
    CY_U3P_I2S_SAMPLE_RATE_96KHz = 96000,         /* 96   KHz */
    CY_U3P_I2S_SAMPLE_RATE_192KHz = 192000        /* 192  KHz */

} CyU3PI2sSampleRate_t;

/* Summary
   List of the supported padding modes.

   Description
   This types lists the padding modes supprted on the I2S interface.

   See Also
   * CyU3PI2sConfig_t
 */
typedef enum CyU3PI2sPadMode_t
{
    CY_U3P_I2S_PAD_MODE_NORMAL = 0,             /* I2S normal operation. */
    CY_U3P_I2S_PAD_MODE_LEFT_JUSTIFIED,         /* Left justified.       */
    CY_U3P_I2S_PAD_MODE_RIGHT_JUSTIFIED,        /* Right justified.      */
    CY_U3P_I2S_PAD_MODE_RESERVED,               /* Reserved mode.        */
    CY_U3P_I2S_PAD_MODE_CONTINUOUS,             /* Without padding.      */
    CY_U2P_I2S_NUM_PAD_MODES                    /* Number of pad modes.  */

} CyU3PI2sPadMode_t;

/* Summary
   Structure defining the configuration of the I2S interface.

   Description
   This structure encapsulates all of the configurable parameters that can be
   selected for the I2S interface. The CyU3PI2sSetConfig() function accepts a
   pointer to this structure, and updates all of the interface parameters.

   See Also
   * CyU3PI2sSetConfig
 */typedef struct CyU3PI2sConfig_t
{
    CyBool_t                isMono;          /* CyTrue:Mono, CyFalse:Stereo  */
    CyBool_t                isLsbFirst;      /* CyTrue:LSB First, CyFalse:MSB First */
    CyBool_t                isDma;           /* CyTrue:DMA mode, CyFalse:Register mode */
    CyU3PI2sPadMode_t       padMode;         /* Type of padding to be used */
    CyU3PI2sSampleRate_t    sampleRate;      /* Sample rate for this audio stream. */
    CyU3PI2sSampleWidth_t   sampleWidth;     /* Bit width for samples in this audio stream. */

} CyU3PI2sConfig_t;

/*
   Summary
   Prototype of I2S event callback function.

   Description
   This function type defines a callback to be called after I2S interrupt has
   been received. A function of this type can be registered with the LPP driver
   as a callback function and will be called whenever an event of interest occurs.

   See Also
   * CyU3PI2sEvt_t
   * CyU3PI2sError_t 
   * CyU3PRegisterI2sCallBack
 */
typedef void (*CyU3PI2sIntrCb_t)(
        CyU3PI2sEvt_t evt,        /* Type of the event that occured */
        CyU3PI2sError_t error     /* Specifies the actual error/status code when the
                                     when the event is of type CY_U3P_I2S_ERROR_EVT */
        );

/**************************************************************************
 *************************** Function Prototypes **************************
 **************************************************************************/

/*@@I2S Functions
   This section documents the functions defined as part of the I2S driver and
   API library.
*/

/* Summary
   Starts the I2S interface block on the FX3.

   Description
   This function powers up the I2S interface block on the FX3 device and is
   expected to be the first I2S API function that is called by the application.

   This function sets up the clock to default value (CY_U3P_I2S_SAMPLE_RATE_8K).

   Bit-clock calculation:
   The minimum sample rate supported is 8KHz and the maximum sample rate is 192KHz.
   The corresponding bit-clock frequency is calculated based on the sample length,
   number of samples and the padding mode used and is defined as per the I2S spec.
   The default calculation is bit clock = 64 * sample rate. It should be noted that
   even though the clock dividers and the API allows frequencies above and below the
   rated range, the device behaviour is not guaranteed. The actual sample rate is
   derived out of SYS_CLK. Since the divider needs to be integral or with half
   divider, the frequency approximation is done using the following algorithm:

   If x is the actual divider and n is the required integral divider to be used,
   then following conditions are used to evaluate:

      x = (SYS_CLK) / (bit clock);

      if (x - floor(x)) < 0.25 ==> n = floor(x);

      if (((x - floor(x)) >= 0.25) &&
          (x - floor(x)) < 0.5) ==> n = floor(x) + half divider;

      if (x - floor(x)) >= 0.75 ==> n = floor(x) + 1;

   Return Value
   * CY_U3P_SUCCESS               - When the init is successful
   * CY_U3P_ERROR_ALREADY_STARTED - When the I2S block has already been initialized
   * CY_U3P_ERROR_NOT_CONFIGURED  - When the I2S block has not been selected in the
                                    initial IO configuration

   See Also
   * CyU3PI2sDeinit
   * CyU3PI2sSetConfig
   * CyU3PI2sTransmitBytes
   * CyU3PI2sSetMute
   * CyU3PI2sSetPause
 */
extern CyU3PReturnStatus_t
CyU3PI2sInit (
        void);

/* Summary
   Stops the I2S interface block on the FX3.

   Description
   This function disables and powers off the I2S interface. This function can
   be used to shut off the interface to save power when it is not in use.

   Return Value
   * CY_U3P_SUCCESS           - When the DeInit is successful
   * CY_U3P_ERROR_NOT_STARTED - When the module has not been previously initialized

   See Also
   * CyU3PI2sInit
   * CyU3PI2sSetConfig
   * CyU3PI2sTransmitBytes
   * CyU3PI2sSetMute
   * CyU3PI2sSetPause
 */
extern CyU3PReturnStatus_t
CyU3PI2sDeInit (
        void);

/* Summary
   Sets the I2S interface parameters.

   Description
   This function is used to configure the I2S master interface based on the
   desired settings. This function should be called repeatedly to change the
   settings if different settings are to be used.

   This function can be called on the fly repetitively without calling
   CyU3PI2sInit. But this will reset the FIFO and hence the data in pipe will
   be lost. If a DMA channel is present, a Reset has to be issued.

   The callback parameter is used to specify an event callback function that
   will be called by the driver when an I2S interrupt occurs.

   Return Value
   * CY_U3P_SUCCESS            - When the SetConfig is successful
   * CY_U3P_ERROR_NOT_STARTED  - When the I2S has not been initialized
   * CY_U3P_ERROR_NULL_POINTER - When the config pointer is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - When the configuration parameters are incorrect
   * CY_U3P_ERROR_TIMEOUT      - When a timeout occurs
   * CY_U3P_ERROR_MUTEX_FAILURE- When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PI2sCb_t
   * CyU3PI2sEvt_t
   * CyU3PI2sError_t
   * CyU3PI2sIntrCb_t
   * CyU3PI2sPadMode_t
   * CyU3PI2sSampleRate_t
   * CyU3PI2sSampleWidth_t
   * CyU3PI2sConfig_t
   * CyU3PI2sInit
   * CyU3PI2sDeinit
   * CyU3PI2sTransmitBytes
   * CyU3PI2sSetMute
   * CyU3PI2sSetPause
 */
extern CyU3PReturnStatus_t
CyU3PI2sSetConfig (
        CyU3PI2sConfig_t *config,   /* I2S configuration settings */
        CyU3PI2sIntrCb_t cb         /* Callback for getting the events */
        );

/* Summary
   Transmits data byte by byte over the I2S interface.

   Description.
   This function sends the data byte by byte over the I2S interface. This is
   allowed only when the I2S interface block is configured for register mode
   transfer. This transfer is always left justified.

   Return Value
   * CY_U3P_SUCCESS              - When the TransmitByte is successful
   * CY_U3P_ERROR_NULL_POINTER   - When the data pointers are NULL
   * CY_U3P_ERROR_TIMEOUT        - When a timeout occurs
   * CY_U3P_ERROR_NOT_CONFIGURED - When the I2S has not been configured for register mode
                                   OR the I2S interface has not been configured
				   OR the I2S interface has not been started
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PI2sInit
   * CyU3PI2sDeinit
   * CyU3PI2sSetConfig
   * CyU3PI2sSetMute
   * CyU3PI2sSetPause
 */
CyU3PReturnStatus_t 
CyU3PI2sTransmitBytes (
        uint8_t *lData,
        uint8_t *rData,
        uint8_t lByteCount,
        uint8_t rByteCount
        );

/* Summary
   Mute / Un-mute I2S master.

   Description
   This function will start discarding data and start driving zeros on the bus.

   Return Value
   * CY_U3P_SUCCESS              - When the Mute is successful
   * CY_U3P_ERROR_NOT_CONFIGURED - When the I2S interface has not been configured or not been started
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PI2sInit
   * CyU3PI2sDeinit
   * CyU3PI2sSetConfig
   * CyU3PI2sTransmitBytes
   * CyU3PI2sSetPause
 */
extern CyU3PReturnStatus_t 
CyU3PI2sSetMute (
        CyBool_t isMute         /* CyTrue: Mute the I2S, CyFalse: Un-Mute the I2S */
        );

/* Summary
   Pause / Resume I2S master.

   Description
   This function will stop sending data on the bus.

   Return Value
   * CY_U3P_SUCCESS              - When the Pause is successful
   * CY_U3P_ERROR_NOT_CONFIGURED - When the I2S interface has not been configured or not been started
   * CY_U3P_ERROR_MUTEX_FAILURE  - When there is a failure in acquiring a mutex lock

   See Also
   * CyU3PI2sInit
   * CyU3PI2sDeinit
   * CyU3PI2sSetConfig
   * CyU3PI2sTransmitBytes
   * CyU3PI2sSetMute
 */
extern CyU3PReturnStatus_t 
CyU3PI2sSetPause (
        CyBool_t isPause       /* CyTrue: Pause the I2S, CyFalse: Resume the I2S */
        );

/* Summary
   This function register the call back function for notification of I2S interrupt.

   Description
   This function registers a callback function that will be called for notification of I2S interrupts
   and also selects the I2S interrupt sources of interest.

   Return Value
   None

   See Also
   * CyU3PI2sEvt_t
   * CyU3PI2sError_t
 */
extern void 
CyU3PRegisterI2sCallBack (
                          CyU3PI2sIntrCb_t i2sIntrCb);

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3I2S_H_ */
