/*
 ## Cypress USB 3.0 Platform header file (cyu3system.h)
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

#ifndef _INCLUDED_CYU3P_SYSTEM_H_
#define _INCLUDED_CYU3P_SYSTEM_H_

#include <cyu3types.h>
#include <cyu3dma.h>
#include "cyu3externcstart.h"


/**************************************************************************
 ******************************* Macros ***********************************
 **************************************************************************/

/*************************** Device configuration *************************/

/* Summary:
   Wake up sources to be used to wakeup the system from suspend/standby modes. 
 */
#define CY_U3P_SYS_PPORT_WAKEUP_SRC            (0x01) /* Use P-Port as wakeup source. */
#define CY_U3P_SYS_UART_WAKEUP_SRC             (0x02) /* Use UART as wakeup source. */
#define CY_U3P_SYS_USB_VBUS_WAKEUP_SRC         (0x04) /* Use USB VBUS assertion as wakeup source. */
#define CY_U3P_SYS_USB_BUS_ACTVTY_WAKEUP_SRC   (0x08) /* Use USB 2.0 D+, D-, SSRX lines as wakeup source. */
#define CY_U3P_SYS_USB_OTGID_WAKEUP_SRC        (0x10) /* Use USB 2.0 Attach event as wakeup source. */

/********************************** Debug *********************************/

/* DMA buffer size in byte allocated when invoking the CyU3PDebugInit call. This 
 * is fixed and cannot be modified. It is provided only for information. */
#define CY_U3P_DEBUG_DMA_BUFFER_SIZE        (0x100)

/* DMA buffer count allocated when invoking the CyU3PDebugInit call. This is 
 * fixed and cannot be modified. It is provided only for information. */
#define CY_U3P_DEBUG_DMA_BUFFER_COUNT       (8)

/**************************************************************************
 ******************************* Data Types *******************************
 **************************************************************************/

/*@@FX3 Device Configuration
  Summary
  Cypress EZ-USB® FX3™ is the next generation USB 3.0 peripheral controller
  providing highly integrated and flexible features that enable developers to
  add USB 3.0 functionality to any system.

  It has a fully configurable, parallel, General Programmable Interface called
  GPIF II, which can connect to any processor, ASIC, DSP, or FPGA. It has an
  integrated phy and controller along with a 32-bit micro-controller (ARM926EJ-S)
  for powerful data processing and for building custom applications.

  It has an ingenious inter-port DMA architecture which enables data transfers
  greater than 400 MBps. An integrated USB 2.0 OTG controller enables applications
  that need dual role usage scenarios. There is 512 KB of on-chip SRAM for code
  and data. 

  There are also serial peripherals such as UART, SPI, I2C, GPIO and I2S for
  communicating to on board peripherals.

  The FX3 device has a number of IO pins that can be configured to function in
  a variety of modes. The functional mode for each of these pins (or groups of
  pins) should be set based on the desired system level functionality.

  Please refer to the FX3 device datasheet for the complete list of supported
  modes on each of the device interfaces.

  The system initialization sequence has to perform the following:
    - FX3 Device initialization
    - IO configuration
    - RTOS startup
 */

/*************************** Device configuration *************************/

/*@@Device Configuration Data Types
   This section documents the data types used for configuring the FX3 device.
   The device configuration mainly consists of setting the required clock
   frequencies and initializing the IO configuration. The following data types
   are used to specify this information.
 */

/* Summary
   Enumeration of EZ-USB FX3 part numbers.

   Description
   There are multiple EZ-USB FX3 parts which support varying feature sets. Please refer
   to the device data sheets or the Cypress device catalogue for information on the
   features supported by each FX3 part.

   This enumerated type lists the various valid part numbers in the EZ-USB FX3 family.

   See Also
   * CyU3PDeviceGetPartNumber
 */
typedef enum CyU3PPartNumber_t
{
    CYPART_USB3014 = 0,                 /* CYUSB3014: 512 KB RAM; GPIF can be 32 bit; OTG and USB host supported. */
    CYPART_USB3012,                     /* CYUSB3012: 256 KB RAM; GPIF can be 32 bit. */
    CYPART_USB3013,                     /* CYUSB3013: 512 KB RAM; GPIF supports 16 bit bus only. */
    CYPART_USB3011                      /* CYUSB3011: 256 KB RAM; GPIF supports 16 bit bus only. */
} CyU3PPartNumber_t;

/* Summary
   Defines the enumerations for LPP IO line configurations.

   Description
   The default mode should be selected when all peripherals need to be accessed.
   The low performance peripheral can be relocated if single LPP peripheral is
   chosen. Refer to the datasheet for more details on the IOs used in various
   configuration. When GPIF 32-bit is used, this should always be
   CY_U3P_IO_MATRIX_LPP_DEFAULT. I2C can be enabled in all modes and is not
   considered in this enumeration.

   See Also
   * CyU3PDeviceConfigureIOMatrix
 */
typedef enum CyU3PIoMatrixLppMode_t
{
    CY_U3P_IO_MATRIX_LPP_DEFAULT = 0,   /* Default LPP mode where all peripherals are enabled. */
    CY_U3P_IO_MATRIX_LPP_UART_ONLY,     /* LPP layout with GPIF 16-bit and UART only. */
    CY_U3P_IO_MATRIX_LPP_SPI_ONLY,      /* LPP layout with GPIF 16-bit and SPI only. */
    CY_U3P_IO_MATRIX_LPP_I2S_ONLY       /* LPP layout with GPIF 16-bit and I2S only. */
} CyU3PIoMatrixLppMode_t;

/* Summary
   List of Low Performance (Serial) Peripherals supported by the device.

   Description
   This enumeration lists the various serial (low performance) peripheral supported by the FX3 device.
   This type is used to notify the lpp driver about the initialization of individual peripheral modules.

   See Also
   * CyU3PIsLppIOConfigured
 */
typedef enum CyU3PLppModule_t
{
    CY_U3P_LPP_I2C= 0,
    CY_U3P_LPP_I2S,
    CY_U3P_LPP_SPI,
    CY_U3P_LPP_UART,    
    CY_U3P_LPP_GPIO
} CyU3PLppModule_t;

/* Summary
   IO Drive Strength configuration for P port, I2C, GPIO, SPI, UART and I2S.

   Description
   The drive strength for P port and all the other serial interfaces
   (I2C, GPIO, SPI, UART and I2S) can be configured to various values.
   This enumeration defines the possible configurations that can be set.

   See Also
   * CyU3PSetPportDriveStrength
   * CyU3PSetI2cDriveStrength
   * CyU3PSetGpioDriveStrength
   * CyU3PSetSerialIoDriveStrength
 */
typedef enum CyU3PDriveStrengthState_t
{
    CY_U3P_DS_QUARTER_STRENGTH = 0,     /* The drive strength is one-fourth the maximum */
    CY_U3P_DS_HALF_STRENGTH,            /* The drive strength is half the maximum */
    CY_U3P_DS_THREE_QUARTER_STRENGTH,   /* The drive strength is three-fourth the maximum */
    CY_U3P_DS_FULL_STRENGTH             /* The drive strength is the maximum */

} CyU3PDriveStrengthState_t;

/* Summary
   Defines the IO matrix configuration parameters.

   Description
   This structure defines all the IO configuration parameters
   that are required to be set at startup.

   See Also
   * CyU3PIoMatrixLppMode_t
   * CyU3PDeviceConfigureIOMatrix
 */
typedef struct CyU3PIoMatrixConfig_t
{
    CyBool_t isDQ32Bit;   /* CyTrue: The GPIF bus width is 32 bit, CyFalse: The GPIF bus width is 24 bits or lesser. */
    CyBool_t useUart;     /* CyTrue: The UART interface is to be used, CyFalse: The UART interface is not to be used */
    CyBool_t useI2C;      /* CyTrue: The I2C interface is to be used, CyFalse: The I2C interface is not to be used */
    CyBool_t useI2S;      /* CyTrue: The I2S interface is to be used, CyFalse: The I2S interface is not to be used */
    CyBool_t useSpi;      /* CyTrue: The SPI interface is to be used, CyFalse: The SPI interface is not to be used */
    CyU3PIoMatrixLppMode_t lppMode; /* LPP IO configuration to be used. */
    uint32_t gpioSimpleEn[2];   /* Bitmap variable that identifies pins that should be configured as simple GPIOs.    */
    uint32_t gpioComplexEn[2];  /* Bitmap variable that identifies pins that should be configured as complex GPIOs.   */
} CyU3PIoMatrixConfig_t;

/* Summary
   Clock source for a peripheral block.

   Description
   The peripheral blocks can take various clock values based on the system clock
   supplied. This is all derived from the SYS_CLK_PLL supplied to the device.
   The SYS_CLK frequency depends on the input clock fed to the device. Also the
   FSLC pins should be configured correctly. If the input clock to the device is
   19.2 MHz or 38.4MHz then this value is 403.2MHz and if the input clock is 26MHz
   or 52MHz this value is 416MHz.

   See Also
   * CyU3PSysClockConfig_t
   * CyU3PDeviceGetSysClkFreq
 */
typedef enum CyU3PSysClockSrc_t
{
    CY_U3P_SYS_CLK_BY_16 = 0,   /* SYS_CLK divided by 16. */
    CY_U3P_SYS_CLK_BY_4,        /* SYS_CLK divided by 4. */
    CY_U3P_SYS_CLK_BY_2,        /* SYS_CLK divided by 2. */
    CY_U3P_SYS_CLK,             /* SYS_CLK. */
    CY_U3P_NUM_CLK_SRC          /* Number of clock source enumerations. */
} CyU3PSysClockSrc_t;

/*Summary
  The clock divider information for CPU, DMA and MMIO.

  Description
  This structure holds information to set the clock divider for CPU, DMA and
  MMIO. The DMA and MMIO clocks are derived from CPU clock. There is an
  additional condition: DMA clock = N * MMIO clock, where N is a non-zero integer.

  The useStandbyClk parameter specifies whether a 32KHz clock has been supplied
  on the CLKIN_32 pin of the device. This clock is the standby clock for the device.
  If this pin is not connected then the device generates a clock from the main
  oscillator clock.

  The setSysClk400 parameter specifies whether the FX3 device's master clock is to
  be set to a frequency greater than 400 MHz. By default, the FX3 master clock is set
  to 384 MHz when using a 19.2 MHz crystal or clock source. This frequency setting
  may lead to DMA overflow errors on the GPIF, if the GPIF is configured as 32-bit
  wide and is running at 100 MHz. Setting this parameter will switch the master
  clock frequency to 403.2 MHz during the CyU3PDeviceInit call.

  This structure is passed as parameter to CyU3PDeviceInit call.

  See Also
  * CyU3PSysClockSrc_t
  * CyU3PDeviceInit
  * CyU3PDeviceGetSysClkFreq
 */
typedef struct CyU3PSysClockConfig_t
{
    CyBool_t           setSysClk400;    /* Whether the FX3 master (System) clock is to be set to a frequency
                                           greater than 400 MHz. This is required to be set to True if the
                                           GPIF is running in 32-bit mode at 100 MHz. */
    uint8_t            cpuClkDiv;       /* CPU clock divider from clkSrc.
                                           Valid value ranges from 2 - 16. */
    uint8_t            dmaClkDiv;       /* DMA clock divider from CPU clock.
                                           Valid value ranges from 2 - 16. */
    uint8_t            mmioClkDiv;      /* MMIO clock divider from CPU clock.
                                           Valid value ranges from 2 - 16. */
    CyBool_t           useStandbyClk;   /* Whether the standby clock is supplied */
    CyU3PSysClockSrc_t clkSrc;          /* Clock source for CPU clocking. */

} CyU3PSysClockConfig_t;

/********************************** Debug *********************************/

/*Summary
  FX3 API library thread information.

  Description
  This enumeration holds the thread IDs used by various FX3 API library threads.
  The thread ID is defined by the first two characters of the thread name provided
  during thread creation. For example the DMA thread name is "01_DMA_THREAD".
  Thread IDs 0-15 are reserved by the library. Thread IDs from 16 onwards can be
  used by FX3 application. All threads created in FX3 application are expected
  to have the first two characters as the thread ID.

  See Also
  * CyU3PDebugEnable
  * CyU3PDebugDisable
 */
typedef enum CyU3PSysThreadId_t
{
    CY_U3P_THREAD_ID_INT        = 0,    /* Interrupt context. */
    CY_U3P_THREAD_ID_DMA        = 1,    /* Library DMA module thread. */
    CY_U3P_THREAD_ID_SYSTEM     = 2,    /* Library system module thread. */
    CY_U3P_THREAD_ID_PIB        = 3,    /* Library p-port module thread. */
    CY_U3P_THREAD_ID_UIB        = 4,    /* Library USB module thread. */
    CY_U3P_THREAD_ID_LPP        = 5,    /* Library low performance peripheral module thread. */
    CY_U3P_THREAD_ID_DEBUG      = 8,    /* Library debug module thread. */
    CY_U3P_THREAD_ID_LIB_MAX    = 15    /* Max library reserved thread ID. */
} CyU3PSysThreadId_t;

/*Summary
  FX3 debug logger data type.

  Description
  The structure defines the data type sent out by the logger
  function. For CyU3PDebugPrint function, the preamble
  will be the same with msg = 0xFFFF and params giving the
  size of the message in bytes.

  See Also
  * CyU3PDebugLog
  * CyU3PDebugPrint
 */
typedef struct CyU3PDebugLog_t
{
    uint8_t priority;       /* Priority od the message */
    uint8_t threadId;       /* THread Id */
    uint16_t msg;           /* Message Index */
    uint32_t param;         /* 32 bit Parameter */

} CyU3PDebugLog_t;

/**************************************************************************
 ********************** Global variable declarations **********************
 **************************************************************************/

/*************************** Device configuration *************************/

extern uint32_t glSysClkFreq;       /* This holds the SYS_CLK frequency. This
                                       variable is updated and used by API and
                                       should not be used by the application. */
extern CyBool_t glIsICacheEnabled;  /* Internal variable holding information
                                       about I-cache state. */
extern CyBool_t glIsDCacheEnabled;  /* Internal variable holding information
                                       about D-cache state. */

/********************************** Debug *********************************/

extern uint8_t glDebugTraceLevel;   /* Trace level for the debug messages.
                                       This variable must be changed only using
                                       the CyU3PDebugSetTraceLevel API. */

extern uint16_t glDebugLogMask;     /* Debug log enable mask per library thread.
                                       Each bit corresponds to threadId. This
                                       variable must be changed only using the
                                       CyU3PDebugEnable call. */

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/*************************** Device configuration *************************/

/*@@Device Configuration Functions
   This section documents the functions used to configure and initialize the
   FX3 device.
 */

/*Summary
  This is the entry routine for the firmware.
 
  Description
  The tool-chain is expected to make this routine as the entry routine. The
  function shall be defined inside the library. This routine shall invoke
  the CyU3PToolChainInit routine after completing the device setup. This 
  function sets up the stack and does device specific initializations. This 
  function should not be explicitly invoked.
 
  Return value
  * None

  See Also
  * CyU3PToolChainInit
 */
extern void
CyU3PFirmwareEntry (
        void);

/*Summary
  This is the normal entry routine provided by the tool-chain.

  Description
  This routine should provide a jump to the normal entry routine provided
  by the tool-chain (This would be main for most tool-chains).
  This function (main) is expected to be implemented by the user firmware.

  If the user application requires a heap (uses malloc / new), it should be
  initialized here.

  Parameters
  * None
 
  Return value
  * None
 
  See Also
  * CyU3PFirmwareEntry
 */
extern void
CyU3PToolChainInit (
        void);

/*Summary
  This function initializes the device.

  Description
  The function is expected to be invoked as the first call from the main ()
  function. This function configures the various system clocks. This also 
  initializes the VIC and exception vectors. It should be invoked only once.

  Return value
  * CY_U3P_SUCCESS - If the call is successful
  * CY_U3P_ERROR_BAD_ARGUMENT - if the parameters are invalid

  See Also
  * CyU3PSysClockConfig_t
 */
extern CyU3PReturnStatus_t
CyU3PDeviceInit (
        CyU3PSysClockConfig_t *clkCfg     /* The clock configuration for CPU, DMA and MMIO.
                                             For default configuration, pass in NULL as parameter.
                                             This will set CPU divider to 2 (~200MHz), DMA and MMIO
                                             dividers to 2 (~100MHz); and assume 32KHz standby clock
                                             is supplied. These are the maximum frequencies
                                             supported by the FX3 device. */
	);

/*Summary
  This function initializes the CPU caches.

  Description
  The function is expected to be invoked immediately after the CyU3PDeviceInit
  call. This function should be called only once. The function controls the cache
  handling in the system. By default all caches are disabled.
  
  It should be noted that once D-cache is enabled, all buffers used for DMA in the
  system has to be cache line aligned. This means that all DMA buffers have to be
  32 byte aligned and 32 byte multiple. This is taken care of by the CyU3PDmaBufferAlloc
  function. Any buffer allocated outside of this function has to follow the 32 byte
  alignment / multiple rule. This rule is also applicable for all DMA buffer pointers
  passed to library APIs including the USB descriptor buffers assigned using
  CyU3PUsbSetDesc, data pointers provided to CyU3PUsbSendEP0Data, CyU3PUsbGetEP0Data,
  CyU3PUsbHostSendSetupRqt etc.

  The isDmaHandleDCache determines whether the DMA APIs perform cache cleans
  and cache flushes internally. If this is CyFalse, then all cache cleans and
  flushes for buffers used with DMA APIs have to be done explicitly by the user.
  If this is CyTrue, then the DMA APIs will clean the cache lines for buffers used
  to send out data from the device and will flush the cache lines for buffers used
  to receive data.

  Return value
  * CY_U3P_SUCCESS - If the call is successful
  * CY_U3P_ERROR_BAD_ARGUMENT - If the wrong clock values are used

  See Also
  * CyU3PDeviceInit
 */
extern CyU3PReturnStatus_t
CyU3PDeviceCacheControl (
        CyBool_t isICacheEnable,        /* Whether to enable the CPU I-cache. */
        CyBool_t isDCacheEnable,        /* Whether to enable the CPU D-cache. */
        CyBool_t isDmaHandleDCache      /* Whether the DMA APIs should internally take care of cache
                                           handling. The APIs can take care of simple use cases where
                                           the DMA buffers are accessed only through the DMA APIs.
                                           Set CyTrue if this feature needs to be enabled.
                                           If this is not the case, or if the user has better cache
                                           handling mechanism, then set this parameter to CyFalse. */
        );

/*Summary
  This is the FX3 application definition function.
  
  Description
  The FX3 application RTOS primitives are created in this function. This 
  function needs to be defined by the FX3 application firmware. This is
  invoked from the system module after the device and all the modules are
  initialized. No APIs are expected to be invoked from this function. The
  application threads and other required OS primitives can be created here.
  At-least one thread must be created. This function should not be explicitly
  invoked.
  
  Return value
  * None
   
  See Also
  * CyU3PKernelEntry 
 */
extern void
CyFxApplicationDefine (
        void);

/*Summary
  This function returns the API version.
 
  Description
  The function returns the version number placed in cyfxversion.h at the time
  the API library was built.  This can be cross referenced by the application.
  Any parameter can take a NULL pointer if that field is not required.

  Return values
  * CY_U3P_SUCCESS - If the call is successful
  * CY_U3P_ERROR_INVALID_CONFIGURATION - If the library configuration is invalid

  See Also
  * None
 */
extern CyU3PReturnStatus_t
CyU3PSysGetApiVersion (
        uint16_t *majorVersion,      /* Major version number for the release */
        uint16_t *minorVersion,      /* Minor version number for the release */
        uint16_t *patchNumer,        /* Patch version for the release */
        uint16_t *buildNumer         /* The build number for the release */
	);

/*Summary
  This function returns the current SYS_CLK frequency.

  Description
  The function can be called only after the CyU3PDeviceInit call
  has been invoked. The SYS_CLK frequency depends on the input clock fed
  to the device. Also the FSLC pins should be configured correctly. If the
  input clock to the device is 19.2 MHz or 38.4MHz then this value is 
  403.2MHz and if the input clock is 26MHz or 52MHz this value is 416MHz.

  Return value
  * CY_U3P_SUCCESS - If the call succeeds
  * CY_U3P_ERROR_NULL_POINTER - if NULL pointer was passed as parameter
  * CY_U3P_ERROR_NOT_CONFIGURED - if the device is not initialized

  See Also
  * CyU3PSysClockConfig_t
  * CyU3PFirmwareEntry
 */
extern CyU3PReturnStatus_t
CyU3PDeviceGetSysClkFreq (
        uint32_t *freq  /* Pointer to return the current SYS_CLK frequency. */
        );

/* Summary
   This function resets the FX3 device.

   Description
   This function is used to reset the FX3 device as a whole. Only a whole
   system reset is supported, as a CPU only reset will leave hardware blocks
   within the device in an inconsistent state. If the warm boot option is
   selected, the firmware in the FX3 RAM shall be maintained; otherwise it
   shall be discarded and freshly loaded. If the warm boot option is used,
   then it should be noted that the firmware should be capable of initializing
   any global variables explicitly.

   Return value
   None as this function does not return.
 */
extern void
CyU3PDeviceReset (
        CyBool_t isWarmReset    /* Whether this should be a warm reset or a cold reset.
                                   In the case of a warm reset, the previously loaded
                                   firmware will start executing again. In the case of a
                                   cold reset, the firmware download to FX3 needs to be
                                   performed again. */
        );

/* Summary
   Places the FX3 device in low power suspend mode.
 
   Description
   The function can be called only after initializing the device completely.
   The device will enter into the suspended mode until any of the wakeup sources
   are triggered. This function does not return until the device has already
   resumed normal operation. The CPU stops running and the device enters a low
   power state. CY_U3P_SYS_PPORT_WAKEUP_SRC_EN, CY_U3P_SYS_USB_WAKEUP_SRC_EN
   and CY_U3P_SYS_UART_WAKEUP_SRC_EN are the various wakeup sources.
 
   Return value
   * CY_U3P_SUCCESS - if the call was successful
   * CY_U3P_ERROR_INVALID_CALLER - if called from interrupt context
   * CY_U3P_ERROR_BAD_ARGUMENT - if the wakeup sources specified are invalid
   * CY_U3P_ERROR_ABORTED - if one of the wakeup source is already triggering
 */
extern CyU3PReturnStatus_t
CyU3PSysEnterSuspendMode (
        uint16_t wakeupFlags,            /* Bit mask representing the wakeup sources that are allowed to
                                            bring FX3 out of suspend mode. */
        uint16_t polarity,               /* Polarity of the Wakeup Sources. This field is valid only for the
                                            CY_U3P_SYS_UART_WAKEUP_SRC and CY_U3P_SYS_USB_VBUS_WAKEUP_SRC wakeup 
                                            sources.
                                            0 - Wakeup when the corresponding source goes low. 
                                            1 - Wakeup when the corresponding source goes high. */
        uint16_t *wakeupSource           /* Output parameter indicating the sources responsible for waking
                                            the FX3 from the Suspend mode. */
        );

/* Summary
   Places the FX3 device in low power standby mode.

   Description
   This function places the FX3 device in low power standby mode where the power consumption on the
   device is lowest. As the power to all the device blocks is removed when in standby mode; this
   function can only be called after shutting down (de-initializing) all FX3 blocks such as USB, GPIF,
   UART, I2C etc. Only the GPIO block can be left on, and its state will be restored when the device
   wakes up.

   On wakeup from standby, the device starts firmware execution from the original entry point. In this
   sense, the entry into and wakeup from standby mode is similar to a warm reset being applied to the
   FX3 device.

   Only P-Port and VBus based wakeup sources are supported to trigger a wake up of the FX3 device from
   standby mode. While the FX3 System RAM retains its content, the TCM blocks lose power while the device
   is in standby. This API backs up the content of the I-TCM region into a user specified buffer location
   before entering standby mode. The I-TCM contents are then automatically restored during re-initialization
   of the FX3 device.

   Return value
   * No return if the device is actually placed in standby mode.
   * CY_U3P_ERROR_BAD_ARGUMENT if the wakeup sources, polarity or buffer pointer provided is invalid.
   * CY_U3P_ERROR_INVALID_SEQUENCE if this API is called while any of the FX3 blocks is still active.
   * CY_U3P_ERROR_STANDBY_FAILED if the specified wakeup sources are already active.
 */
extern CyU3PReturnStatus_t
CyU3PSysEnterStandbyMode (
        uint16_t  wakeupFlags,          /* List of selected wakeup sources from standby. */
        uint16_t  polarity,             /* Polarity of the wakeup source which causes the device to wakeup. */
        uint8_t  *bkp_buff_p            /* Pointer to buffer where the I-TCM content can be backed up.
                                           Should be located in the SYSMEM and be able to hold 18 KB of data. */
        );

/* Summary
   Configure the watchdog reset control.

   Description
   The FX3 device implements a watchdog timer that can be used to reset the
   device when it is not responsive. This function is used to enable the watchdog
   feature and to set the period for this watchdog timer.

   Return Value
   None

   See Also
   * CyU3PSysWatchDogClear
 */
extern void
CyU3PSysWatchDogConfigure (
        CyBool_t enable,                /* Whether the watch dog should be enabled. */
        uint32_t period                 /* Period in milliseconds. Is valid only for enable calls. */
        );

/* Summary
   Clear the watchdog timer to prevent a device reset.

   Description
   This function is used to clear the watchdog timer so as to prevent it from
   resetting the FX3 device. This function should be called more frequently than
   the specified watchdog timer period to avoid unexpected resets.

   Return Value
   None

   See Also
   * CyU3PSysWatchDogConfigure
 */
extern void
CyU3PSysWatchDogClear (
        void);

/* Summary
   Configures the IO matrix for the device.

   Description
   The FX3 device has 61 IO pins that can be configured to function in a
   variety of modes. The functional mode for each of these pins (or groups of
   pins) should be set based on the desired system level functionality.

   The processor port (P-port) of the device can be configured to as a GPIF
   interface. The mode selection for this interface should be based on the means
   of connecting the external device / processor. Some pins are not used depending
   upon the configuration used. The free pins can be used as GPIOs.
   If 16 bit GPIF interface is used, then the DQ[15:0], CTL[4:0] and PMODE pins
   are reserved for their default usage. CTL[12:5] are not locked and can be
   configured as GPIO. If any of the CTL[12:5] lines are used for GPIF interface
   they should not be configured as GPIOs. Similarly if some lines of CTL[4:0]
   are not used for GPIF they can be overridden to be GPIOs using
   CyU3PDeviceGpioOverride call. If 32 bit GPIF interface is used all of DQ[31:0]
   is reserved for GPIF interface.

   The serial peripheral interfaces have various configurations that can be used
   depending on the type of interfaces used. The default mode of operation has
   all the low performance (serial) peripherals enabled. I2C lines are not
   multiplexed and is available in all configurations except when used as GPIOs.
   If a peripheral is marked as not used (CyFalse), then those IO lines can be
   configured as GPIO.

   Please refer to the FX3 device data sheet for the complete list of
   supported modes on each of the device interfaces.

   IOs that are not configured for peripheral interfaces can be used as GPIOs. This
   selection has to be explicitly made. Otherwise these lines will be left tri-stated.
   The GPIOs available are further classified as simple (set / get a value or receive
   a GPIO interrupt); or as complex (PWM, timer, counter etc). The selection of this
   is made via four 32-bit bitmasks where each IO is represented with (1 << IO number).

   There are only eight complex GPIOs possible. The restriction is applied based on
   % 8. So only one of 0, 8, 16, ...  can be configured as complex GPIO. Similarly
   only one of 1, 9, 17 ... can be configured as complex GPIO.

   This function is recommmended to be called after the CyU3PDeviceInit call from
   the main () function. IO matrix is not recommended to be dynamically changed. This
   API can be invoked when the peripherals affected are disabled. If the API is invoked
   after initialization, then it is expected that the external devices attached are also
   capable of this dynamic reconfiguration. This function provides error checks but is
   not completely fool proof. The actual IO configuration selected should make sure that
   there are no conflicts. For example, CTL[12:5] are allowed as GPIOs by this
   function. But if the GPIF configuration is using any of these lines and these
   lines are also configured as GPIO, then the IO will behave as the GPIOs and not
   as part of GPIF. So care should be taken during hardware design.

   None of the IOs are expected to be used before this call. Even if the configuration
   to be used is same as default, the call still has to be made to initialize the IOs.
   The default configuration for p-port will depend on the boot-mode used. If p-port
   boot is selected, then the  boot-loader will configure it accordingly. GPIF will
   be left configured at 16-bit mode, the LPP mode will be CY_U3P_IO_MATRIX_LPP_DEFAULT
   and none of the GPIOs will be enabled.

   Return value
   * CY_U3P_SUCCESS - when the IO configuration is successful
   * CY_U3P_ERROR_BAD_ARGUMENT - if some configuration value is invalid
   * CY_U3P_ERROR_NOT_SUPPORTED - if the FX3 part in use does not support any of the selected features

   See Also
   * CyU3PIoMatrixConfig_t
   * CyU3PDeviceGpioOverride
   * CyU3PDeviceGpioRestore
 */
extern CyU3PReturnStatus_t
CyU3PDeviceConfigureIOMatrix (
        CyU3PIoMatrixConfig_t *cfg_p /* Pointer to Configuration parameters.*/
	);

/* Summary
   This function is used to override an IO as a GPIO.

   Description
   This is an override mechanism and can be used to enable any IO line as
   simple / complex GPIO. This should be done with caution as a wrong setting
   can cause damage to hardware.

   The API is expected to be invoked before the corresponding peripheral module
   is enabled. The specific GPIO configuration has to be still done after this call.

   The CyU3PDeviceConfigureIOMatrix API checks for validity of the configuration,
   whereas this API does not. Use this API with great caution.

   For example if SPI SSN is not used, then it can be configured as a GPIO and
   used even though SPI module is being used. CyU3PDeviceConfigureIOMatrix fails
   the configuration of this pin as GPIO but this function allows the configuration.
   The flip side is that there is no error check and if the configuration does not
   match the hardware configuration, it can result in unexpected behavior and
   hardware damage.

   Return Value
   * CY_U3P_SUCCESS if the operation is successful
   * CY_U3P_ERROR_BAD_ARGUMENT if the GPIO specified is invalid

   See Also
   * CyU3PDeviceGpioRestore
   * CyU3PDeviceConfigureIOMatrix
 */
extern CyU3PReturnStatus_t
CyU3PDeviceGpioOverride (
        uint8_t gpioId,        /* The IO to be converted to GPIO. */
        CyBool_t isSimple      /* CyTrue: simple GPIO, CyFalse: complex GPIO */
        );

/*Summary
  This function is used to restore IO to normal mode.

  Description
  IOs that are overridden to be GPIO can be restored to normal mode of
  operation by invoking this API.

  Return Value
  * CY_U3P_SUCCESS if the operation is successful
  * CY_U3P_ERROR_BAD_ARGUMENT if the GPIO specified is invalid

  See Also
  * CyU3PDeviceGpioOverride
  * CyU3PDeviceConfigureIOMatrix
 */
extern CyU3PReturnStatus_t
CyU3PDeviceGpioRestore (
        uint8_t gpioId  /* The GPIO to be restored. */
        );

/* Summary
   Set the IO drive strength for UART, SPI and I2S interfaces.

   Description
   The default IO drive strength for the interfaces are set to
   CY_U3P_DS_THREE_QUARTER_STRENGTH. I2C has a separate API for
   setting the drive strength. Refer to CyU3PSetI2cDriveStrength() API.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_BAD_ARGUMENT   - If the Drive strength requested is invalid

   See Also
   * CyU3PDriveStrengthState_t
   * CyU3PSetI2cDriveStrength
 */

extern CyU3PReturnStatus_t
CyU3PSetSerialIoDriveStrength (
        CyU3PDriveStrengthState_t driveStrength /* Serial IO drive strength */
        );

/********************************** Debug *********************************/

/*@@Logging Support

   Summary
   This section documents the APIs that are provided to facilitate logging of
   firmware activity for debug purposes

   Description
   The FX3 API library includes a provision to log firmware actions and send
   them out to a selected target such as the UART console. The logs can be
   generated by the API library or the drivers themselves, or be messages sent
   by the application logic.

  The log messages are classified into two types:
    1. Codified messages: These messages contain only a two byte message ID and
       a four byte parameter. This kind of messages are used to compress the log
       output and require an external decoder to interpret the logs based on the
       message ID. All logs generated by the FX3 firmware framework and library
       will be of this kind.
    2. Verbose messages: These are free-form string messages. A function with a
       signature similar to printf is provided for generating these messages.

   The log messages are further classified based on priority levels ranging from
   0 to 9. 0 is the highest priority and 9 is the lowest. The logger
   implementation allows the user to set a priority threshold value at runtime,
   and only messages with a higher priority will be logged.

   All log messages will include a source ID which identifies the thread that
   generated the message, the priority assigned to the message and the message
   ID. In case the message is a codified one, the message ID can range from
   0x0000 to 0xFFFE; and the message will also contain a 4 byte parameter field.

   If the message is a verbose one, the message ID will be set to 0xFFFF and
   the parameter will indicate the length of the message string. The next length
   bytes will form the actual text of the string.
 */

/*@@Logging Data Types
   This section documents the data types used for data logging.
 */

/*@@Logging Functions
   This section documents the functions defined as part of the firmware
   logger function.
 */

/* Summary
   Initialize the firmware logging functionality.

   Description
   This function initializes the firmware logging functionality and creates a DMA
   channel to send the log traces out through the selected DMA socket.

   Return Values
   * CY_U3P_SUCCESS  - when successfully initialized.
   * CY_U3P_ERROR_ALREADY_STARTED - when the logger is already initialized.
   * CY_U3P_ERROR_BAD_ARGUMENT - when bad socket ID is passed in as parameter.
   * Other DMA specific error codes are also returned.

   See Also
   * CyU3PDebugDeInit
 */
extern CyU3PReturnStatus_t
CyU3PDebugInit (
        CyU3PDmaSocketId_t destSckId,           /* Socket through which the logs are to be output. */
        uint8_t traceLevel                      /* Priority level beyond which logs should be output. */
        );

/* Summary
   De-initialize the logging function.

   Description
   This function de-initializes the firmware logging function
   and destroys the DMA channel created to output the logs.

   Return Value
   * CY_U3P_SUCCESS - When successful.
   * CY_U3P_ERROR_NOT_STARTED - When the logger is not started.

   See Also
   * CyU3PDebugInit
 */
extern CyU3PReturnStatus_t
CyU3PDebugDeInit (
        void);

/* Summary
   Initializes the SYS_MEM based logger facility.

   Description
   This function cannot be used when the CyU3PDebugInit is invoked. The SYS_MEM
   logging is faster as the writes are done directly to the system memory. But
   the limitation is that only CyU3PDebugLog function can be used. The logger
   can be cleared by invoking CyU3PDebugLogClear.

   Return Values
   * CY_U3P_SUCCESS  - when successfully initialized.
   * CY_U3P_ERROR_ALREADY_STARTED - The logger is already started.
   * CY_U3P_ERROR_NULL_POINTER - If NULL buffer pointer is passed as argument.
   * CY_U3P_ERROR_BAD_ARGUMENT - If any of the arguments passed is invalid.

   See Also
   * CyU3PDebugSysMemDeInit
   * CyU3PDebugLog
   * CyU3PDebugLogClear
 */
extern CyU3PReturnStatus_t
CyU3PDebugSysMemInit (
        uint8_t *buffer,        /* The buffer memory to be used for writing the log. */
        uint16_t size,          /* The size of memory available for logging. */
        CyBool_t isWrapAround,  /* CyTrue - wrap up and start logging from beginning
                                   CyFalse - Stop logging on reaching the last memory address. */
        uint8_t traceLevel      /* Priority level beyond which logs should be output. */
        );

/* Summary
   Disables the SYS_MEM logger.

   Description
   The function de-initializes the SYS_MEM logger. The normal logger can now be
   initialized using CyU3PDebugInit call.

   Return Values
   * CY_U3P_SUCCESS  - when successfully initialized.
   * CY_U3P_ERROR_NOT_STARTED - when the logger is not yet started.

   See Also
   * CyU3PDebugSysMemInit
   * CyU3PDebugLog
   * CyU3PDebugLogClear
 */
extern CyU3PReturnStatus_t
CyU3PDebugSysMemDeInit (
        void);

/* Summary
   Inhibits the preamble bytes preceding the print message.

   Description
   The CyU3PDebugPrint function internally sends 8 byte preamble data preceding 
   the actual message. This preamble data is used for tool to decode the message in 
   appropriate manner. It can be called to enable and disable sending of the
   preamble data. This is useful in the case where the debug data is visually
   interpreted (and not decoded by a tool).

   Return Value
   * None

   See Also
   * CyU3PDebugPrint
*/
extern void
CyU3PDebugPreamble (
        CyBool_t sendPreamble    /* CyFalse : disable preamble data before actual message,
                                    CyTrue  : Send preamble data before actual message */
        );

/* Summary
   Free form message logging function.

   Description
   This function can be used by the application code to log free form message
   strings, and causes the output message to be written to the UART immediately.
  
   The function supports a subset of the output conversion specifications
   supported by the printf function. The 'c', 'd', 'u', 'x' and 's' conversion
   specifications are supported. There is no support for float or double formats,
   or for flags, precision or type modifiers.

   Note
   If the full functionality supported by printf such as all conversion specifications,
   flags, precision, type modifiers etc. are required; please use the standard C library
   functions to print the formatted message into a string, and then use this function
   to send the string out through the UART port.

   Return Values
   * CY_U3P_SUCCESS  - if the Debug print is successful
   * CY_U3P_ERROR_NOT_STARTED - if the debug module has not been initialized
   * CY_U3P_ERROR_INVALID_CALLER - if called from an interrupt
   * CY_U3P_ERROR_BAD_ARGUMENT - if the total length of the formatted string exceeds the debug buffer size.

   See Also
   * CyU3PDebugInit
   * CyU3PDebugLog
   * CyU3PDebugSetTraceLevel
   * CyU3PDebugPreamble
 */
extern CyU3PReturnStatus_t
CyU3PDebugPrint (
        uint8_t priority,       /* Priority level for this message. */
        char   *message,        /* Format specifier string. */
        ...                     /* Variable argument list. */
        );

/* Summary
   Log a codified message.

   Description
   This function is used to output a codified log message which contains a two
   byte message ID and a four byte parameter. The message ID is expected to be
   in the range 0x0000 to 0xFFFE.

   The log messages are written to a log buffer. This log buffer will be written
   out to the UART when it is full.

   The CyU3PDebugLogFlush can be used to flush the contents of the log buffer to 
   the UART.

   The CyU3PDebugLogClear can be used to clear the contents of the log buffer.

   Return Values
   * CY_U3P_SUCCESS  - if the Debug log is successful
   * CY_U3P_ERROR_NOT_STARTED - if the debug module has not been initialized
   * CY_U3P_ERROR_FAILURE - if the logging fails

   See Also
   * CyU3PDebugPrint
   * CyU3PDebugLogFlush
   * CyU3PDebugLogClear
   * CyU3PDebugSetTraceLevel
 */
extern CyU3PReturnStatus_t
CyU3PDebugLog (
        uint8_t priority,       /* Priority level for the message. */
        uint16_t message,       /* Message ID for the message. */
        uint32_t parameter      /* Parameter associated with the message. */
        );

/* Summary
   Flush any pending logs out of the internal buffers.

   Description
   All log messages are collected into internal memory buffers on the
   FX3 device and sent out to the target when the buffer is full or when
   a free form message is committed. This function is used to force the
   logger to send out any pending logs messages to the target and flush
   its internal buffers.

   Return Values
   * CY_U3P_SUCCESS  - if the Debug log flush is successful
   * CY_U3P_ERROR_NOT_STARTED - if the debug module has not been initialized
   * CY_U3P_ERROR_INVALID_CALLER - if called from an interrupt

   See Also
   * CyU3PDebugLogClear
 */
extern CyU3PReturnStatus_t
CyU3PDebugLogFlush (
        void);

/* Summary
   Drop any pending logs in the internal buffers.
  
   Description
   This function is used to ask the logger to drop any pending logs
   in its internal buffers.

   Return Values
   * CY_U3P_SUCCESS  - if the Debug log flush is successful
   * CY_U3P_ERROR_NOT_STARTED - if the debug module has not been initialized
   * CY_U3P_ERROR_INVALID_CALLER - if called from an interrupt

   See Also
   * CyU3PDebugLogFlush
 */
extern CyU3PReturnStatus_t
CyU3PDebugLogClear (
        void);

/* Summary
   This function sets the priority threshold above which debug traces will be logged.

   Description
   The logger can suppress log messages that have a priority level lower than a user
   specified threshold.  This function is used to set the priority threshold below
   which logs will be suppressed.

   Return Values
   * None

   See Also
   * CyU3PDebugPrint
   * CyU3PDebugLog

 */
extern void
CyU3PDebugSetTraceLevel (
        uint8_t level             /* Lowest debug trace priority level that is enabled. */
        );

/* Summary
   Enable log messages from specified threads.

   Description
   Thread ID of 0 means interrupt context. Only threadIds 1 - 15, and interrupt
   context can be controlled by this API call. This call is intended for only
   controlling logs from library threads. By default logs from all library
   threads are disabled.

   Return Values
   * None

   See Also
   * CyU3PDebugDisable
 */
extern void
CyU3PDebugEnable (
        uint16_t threadMask       /* ThreadID mask whose logs are to be enabled. 1 means enabled. */
        );

/* Summary
   Disable log messages from all library threads.

   Description
   The API returns the previous threadMask set, so that it can
   be used for re-enabling the logs.

   Return Values
   The previous threadMask

   See Also
   * CyU3PDebugEnable
 */
extern uint16_t
CyU3PDebugDisable (
        void);

/* Summary
   Check whether a specific pin has been selected as a simple GPIO.

   Description
   This function checks whether a specific FX3 pin has been selected to function as a simple GPIO.
   The IO Matrix configuration is queried for this information.

   Return Value
   * CyTrue if the pin is selected as a simple GPIO, CyFalse otherwise.

   See Also
   * CyU3PDeviceConfigureIOMatrix
   * CyU3PIsGpioComplexIOConfigured
 */
extern CyBool_t 
CyU3PIsGpioSimpleIOConfigured (
        uint32_t gpioId                 /* Pin number to be queried. */
        );

/* Summary
   Check whether a specific pin has been selected as a complex GPIO.

   Description
   This function is used to check whether a specific FX3 pin has been selected to function
   as a complex GPIO, while setting up the IO matrix.

   Return Value
   * CyTrue if the pin has been selected as a complex GPIO, CyFalse otherwise.

   See Also
   * CyU3PIsGpioSimpleIOConfigured
   * CyU3PDeviceConfigureIOMatrix
 */
extern CyBool_t 
CyU3PIsGpioComplexIOConfigured (
        uint32_t gpioId);

/* Summary
   Check whether a specified GPIO ID is valid on the current FX3 part.

   Description
   Different parts in the FX3 family support different numbers of GPIOs. This function is used to
   check whether a specific GPIO number is valid on the current part.

   Return Value
   * CyTrue if the GPIO ID is valid, CyFalse otherwise.
 */
extern CyBool_t 
CyU3PIsGpioValid (
        uint8_t gpioId          /* GPIO number to be checked for validity. */
        );

/* Summary
   Check whether a specific serial peripheral has been enabled in the IO Matrix.

   Description
   This function checks whether a specific serial peripheral interface has been enabled
   in the active IO matrix.

   Return Value
   * CyTrue if specified lpp is enabled, CyFalse otherwise.

   See Also
   * CyU3PDeviceConfigureIOMatrix
   * CyU3PLppModule_t
 */
extern CyBool_t 
CyU3PIsLppIOConfigured (
        CyU3PLppModule_t lppModule      /* Serial interface to be queried. */
        );

/* Summary
   This function is used to get the part number of the FX3 part in use.

   Description
   The EZ-USB FX3 family has multiple parts which support various sets of features.
   This function can be used to query the part number of the current device so as to
   check whether specific functionality is supported or not.

   Return Value
   Part number of the FX3 device in use.

   See Also
   * CyU3PPartNumber_t
 */
extern CyU3PPartNumber_t
CyU3PDeviceGetPartNumber (
        void);

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3P_SYSTEM_H_ */

/*[]*/

