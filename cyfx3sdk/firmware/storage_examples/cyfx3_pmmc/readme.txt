
                        CYPRESS SEMICONDUCTOR CORPORATION
                                    FX3S SDK

  FX3S Co-processor Mode Example
  ------------------------------

  The FX3/FX3S device can be configured to function as a co-processor element
  that offloads USB and other peripheral interfaces in an embedded system. In
  such a case, the FX3/FX3S device can be controlled by the main processor
  through standard interfaces such as a MMC Slave interface or a RAM interface.

  This mode of operation is more suitable for the FX3S device which can provide
  storage (SD/eMMC flash control) functions in addition to USB and other serial
  peripheral functions.

  This example illustrates the use of the FX3/FX3S APIs to implement a co-processor
  device that can be controlled by the master processor through a MMC interface.
  The storage and USB functions on the FX3S device are operated under control
  of messages received from the master processor through the MMC interface.

  This example expects that the drivers/applications on the master processor are
  making use of the control APIs provided in source form by Cypress.

  Files:

    * cyfx_gcc_startup.S : Start-up code for the ARM-9 core on the FX3S device.
      This assembly source file follows the syntax for the GNU assembler.

    * cyfxtx.c           : ThreadX RTOS wrappers and utility functions required
      by the FX3S API library.

    * cywbtypes.h        : Data types defined for use by the applications/drivers
      running on the master processor.

    * cywberr.h          : Set of error codes defined for the applications/drivers
      running on the master processor.

    * cywbprotocol.h     : Defines the format of the control messages exchanged
      between FX3S and the master processor.

    * cyfx3_pibctxt.h    : Constant definitions and function signatures relating
      to the control message protocol between FX3S and the master processor.

    * cyfx3_pibctxt.c    : C source file implementing the control message exchange
      between FX3S and the master processor.

    * cypib_dscr.c       : USB descriptors used by the co-processor application.

    * cyfx3_pib.h        : Constants and function signatures relating to the
      co-processor implementation.

    * cyfx3_pib.c        : Main source file implementing the co-processor mode
      application.

    * makefile           : GNU make compliant build script for compiling this example.

[]

