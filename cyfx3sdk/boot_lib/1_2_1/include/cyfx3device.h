/*
 ## Cypress USB 3.0 Platform header file (cyfx3device.h)
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

#ifndef __CYFX3DEVICE_H__
#define __CYFX3DEVICE_H__

#include <cyu3types.h>
#include <cyfx3error.h>

#include <cyu3externcstart.h>

/*@@FX3 Boot APIs
   Summary
   The FX3 software boot configures minimal functionality of the FX3 device for a secondary image.
   This boot code does not use an RTOS and the image footprint is significantly smaller as 
   compared to the final application image.

   Description
   The software boot supports the following interfaces on the FX3 device

   1. USB
   2. SPI
   3. I2C
   4. UART
   5. GPIO

   A set of APIs are provided to configure these interfaces. The final image can be downloaded
   from the USB, SPI or I2C.

   The boot library (cyfx3_boot.a) is present at $FX3_INSTALL_PATH\\firmware\\boot_fw\\lib
*/

/*@@FX3 Boot Device Interface
   The boot device interface initializes the FX3 device and configures the system clock.
*/

/*@@FX3 Boot Device Data Types
   This section documents the data types which are part of the FX3 Boot Device APIs.
*/

/* Summary:
   ITCM and SYSMEM memory address range.

   Description:
   The first 256 bytes of the ITCM are reserved for the exception vectors.
   The first 8K of the SYSMEM is reserved for the DMA descriptors.

   See Also:
   cyfx3.scat file.
*/

#define CY_FX3_BOOT_ITCM_BASE           (0x00000000u)
#define CY_FX3_BOOT_ITCM_END            (CY_FX3_BOOT_ITCM_BASE + 0x4000u)
#define CY_FX3_BOOT_SYSMEM_BASE         (0x40000000u)
#define CY_FX3_BOOT_SYSMEM_END          (CY_FX3_BOOT_SYSMEM_BASE + 0x80000u)
#define CY_FX3_BOOT_SYSMEM_BASE1        (CY_FX3_BOOT_SYSMEM_BASE + 0x2000u)

/* Summary:
   Wait options to be used in Dma data transfer related APIs.
 */
#define CY_FX3_BOOT_NO_WAIT 			(0x00)        /* Return Immediately. */
#define CY_FX3_BOOT_WAIT_FOREVER 		(0xFFFFFFFF)  /* Wait until the transfer done interrupt
                                                         or the error interrupt gets set.
                                                       */

/* Summary:
   Clock source for a peripheral block.

   Description:
   The peripheral blocks can take various clock values based on the system
   clock supplied. This is all derived from the SYS_CLK_PLL supplied to the
   device.

 */
typedef enum CyFx3BootSysClockSrc_t
{
    CY_FX3_BOOT_SYS_CLK_BY_16 = 0,   /* SYS_CLK divided by 16. */
    CY_FX3_BOOT_SYS_CLK_BY_4,        /* SYS_CLK divided by 4. */
    CY_FX3_BOOT_SYS_CLK_BY_2,        /* SYS_CLK divided by 2. */
    CY_FX3_BOOT_SYS_CLK,             /* SYS_CLK. */
    CY_FX3_BOOT_NUM_CLK_SRC          /* Number of clock source enumerations. */
} CyFx3BootSysClockSrc_t;

/* Summary
   Enumeration of EZ-USB FX3 part numbers.

   Description
   There are multiple EZ-USB FX3 parts which support varying feature sets. Please refer
   to the device data sheets or the Cypress device catalogue for information on the
   features supported by each FX3 part.

   This enumerated type lists the various valid part numbers in the EZ-USB FX3 family.

   See Also
   * CyFx3BootGetPartNumber
 */
typedef enum CyFx3PartNumber_t
{
    CYPART_USB3014 = 0,                 /* CYUSB3014: 512 KB RAM; GPIF can be 32 bit; UART, SPI and I2S supported. */
    CYPART_USB3012,                     /* CYUSB3012: 256 KB RAM; GPIF can be 32 bit; UART, SPI and I2S supported. */
    CYPART_USB3013,                     /* CYUSB3013: 512 KB RAM; GPIF supports 16 bit; no UART, SPI or I2S. */
    CYPART_USB3011                      /* CYUSB3011: 256 KB RAM; GPIF supports 16 bit; no UART, SPI or I2S. */
} CyFx3PartNumber_t;

/* Summary:
   Defines the IO matrix configuration parameters

   Description:
   This structure defines all the IO configuration parameters that
   are required to be set at startup.

   See Also
   * CyFx3BootDeviceConfigureIOMatrix
 */
typedef struct CyFx3BootIoMatrixConfig_t
{
    CyBool_t isDQ32Bit;         /* CyTrue: The GPIF bus width is 32 bit
                                   CyFalse: The GPIF bus width is 16 bit */
    CyBool_t useUart;           /* CyTrue: The UART interface is to be used 
                                   CyFalse: The UART interface is not to be used */
    CyBool_t useI2C;            /* CyTrue: The I2C interface is to be used 
                                   CyFalse: The I2C interface is not to be used */
    CyBool_t useI2S;            /* CyTrue: The I2S interface is to be used 
                                   CyFalse: The I2S interface is not to be used 
                                   Booter doesn't support I2S feature as of now. */
    CyBool_t useSpi;            /* CyTrue: The SPI interface is to be used
                                   CyFalse: The SPI interface is not to be used */
    uint32_t gpioSimpleEn[2];   /* Bitmap variable that identifies pins that should 
                                   be configured as simple GPIOs. */
} CyFx3BootIoMatrixConfig_t;

/*@@FX3 Boot Device Functions
   These section documents the functions that are part of the FX3 Boot Device APIs.
*/

/* Summary:
   This function initializes the device.

   Description:
   The function is expected to be invoked as the first call from the main ()
   function. This function should be called only once.

   The setFastSysClk parameter is equivalent to the setSysClk400 parameter used
   in the main FX3 API library.
*/
extern void
CyFx3BootDeviceInit (
        CyBool_t setFastSysClk  /* Indicates whether the FX3 system clock should be
                                   set to faster than 400 MHz or not. Should be set to
                                   CyTrue if the GPIF will be used in Synchronous 32-bit
                                   mode at 100 MHz. */
        );

/* Summary:
   Function to transfer the control to the specified address.

   Description:
   This function is used to transfer the control to the next stage's program entry.
   All the Serial IOs (I2C, SPI, UART and GPIO) that have been initialized must be 
   de-initialized prior to calling this function.
*/
extern void 
CyFx3BootJumpToProgramEntry (
        uint32_t address     /* The program entry address */
        );

/* Summary:
   Configures the IO matrix for the device

   Description:
   The function configures the GPIOs to do specialized functions. Since the
   pins are multiplexed, the IO matrix should be configured before using any of
   the ports. This function must be called after the CyFx3BootDeviceInit call
   from the main() function. IO matrix cannot be dynamically changed and needs
   to be invoked during the device initialization.

   Note
   If the GPIF is 32bit, then SPI module cannot be used.

   Return value
   * CY_FX3_BOOT_SUCCESS - When the IO configuration is successful
   * CY_FX3_BOOT_ERROR_NOT_SUPPORTED - the FX3 part in use does not support the desired configuration
   * CY_FX3_BOOT_ERROR_BAD_ARGUMENT - If some configuration value is invalid

   See Also
   * CyFx3BootIoMatrixConfig_t
 */
extern CyFx3BootErrorCode_t
CyFx3BootDeviceConfigureIOMatrix (
        CyFx3BootIoMatrixConfig_t *cfg_p /* Pointer to Configuration parameters. */
        );

/* Summary
   Enable/disable the watchdog timer.

   Description
   The FX3 device implements a watchdog timer that can be used to reset the device
   when the CPU is not responsive. This function is used to enable the watchdog feature
   and to set the period for the timer.

   Return value
   None

   See Also
   * CyFx3BootWatchdogClear
 */
extern void
CyFx3BootWatchdogConfigure (
        CyBool_t enable,                /* Whether the watchdog timer is to be enabled or disabled. */
        uint32_t period                 /* Period for the timer in milliseconds. Used only for enable calls. */
        );

/* Summary
   Clear the watchdog timer to prevent device reset.

   Description
   This function is used to clear the watchdog timer so as to prevent the timer from
   resetting the device. This function needs to be called more often than the period of
   the watchdog timer.

   Return value
   None

   See Also
   * CyFx3BootWatchdogConfigure
 */
extern void
CyFx3BootWatchdogClear (
        void);

/* Summary
   This function is used to get the part number of the FX3 part in use.

   Description
   The EZ-USB FX3 family has multiple parts which support various sets of features.
   This function can be used to query the part number of the current device so as to
   check whether specific functionality is supported or not.

   Return Value
   Part number of the FX3 device in use.

   See Also
   * CyFx3PartNumber_t
 */
extern CyFx3PartNumber_t
CyFx3BootGetPartNumber (
        void);

/* Summary
   Request to keep the GPIO block powered ON across control transfer to the full firmware.

   Description
   All serial peripheral blocks on the FX3 device are normally reset when control of execution
   is transferred to the full firmware. This API is used to specify that the GPIO block should
   be left ON while jumping to the full firmware.

   Return Value
   None
 */
extern void
CyFx3BootRetainGpioState (
        void);

#include <cyu3externcend.h>
#endif /* __CYFX3DEVICE_H__ */

