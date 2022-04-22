
                        CYPRESS SEMICONDUCTOR CORPORATION
                                    FX3S SDK

  HID CLASS (Mouse) EXAMPLE
  -------------------------

  This example illustrates the use of the FX3 firmware APIs to implement
  a HID class device which imitates mouse cursor movement.

  The application issues multiple input reports to the HID host, whenever it
  encounters an interrupt through switch press which is located on the FX3
  SuperSpeed Explorer Kit. The application also demonstrates the HID implementation
  using existing FX3 APIs.

  No actual sensors or real mouse control is present, because the application
  is designed to work on the kit without any additional hardware.

  Files:

    * cyfx_gcc_startup.S : Start-up code for the ARM-9 core on the FX3S device.
      This assembly source file follows the syntax for the GNU assembler.

    * cyfx3_hid.h        : Constant definitions for the HID class implementation.

    * cyhid_dscr.c       : C source file containing the USB descriptors that are
      used by this firmware example.

    * cyfxtx.c           : ThreadX RTOS wrappers and utility functions required
      by the FX3S API library.

    * cyfx3_hid.c        : Main C source file that implements the HID function.

    * makefile           : GNU make compliant build script for compiling this
      example.

[]

