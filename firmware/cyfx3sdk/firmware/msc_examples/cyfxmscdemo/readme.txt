
                        CYPRESS SEMICONDUCTOR CORPORATION
                                    FX3S SDK

  USB Mass Storage Class Example
  ------------------------------

  This example illustrates how to implement a USB Mass Storage Class (Bulk Only
  Transport) device using the FX3 APIs.

  This implementation makes use of a 32 KB region of the FX3 device RAM as the
  storage medium, and shows how the Mass Storage Class commands (SCSI Transparent
  command set) can be handled.

  The implementation for the READ_CAPACITY, READ_10 and WRITE_10 commands can
  be updated if you are creating a MSC device using a real storage medium like
  a FLASH memory device.

  Files:

    * cyfx_gcc_startup.S : Start-up code for the ARM-9 core on the FX3S device.
      This assembly source file follows the syntax for the GNU assembler.

    * cyfxmscdemo.h      : Constant definitions for the mass storage class
      implementation.

    * cyfxmscdscr.c      : C source file containing the USB descriptors that are
      used by this firmware example.

    * cyfxtx.c           : ThreadX RTOS wrappers and utility functions required
      by the FX3S API library.

    * cyfxmscdemo.c      : Main C source file that implements the mass storage
      function.

    * makefile           : GNU make compliant build script for compiling this
      example.

[]

