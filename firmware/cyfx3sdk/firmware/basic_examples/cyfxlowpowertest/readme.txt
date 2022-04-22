
                        CYPRESS SEMICONDUCTOR CORPORATION
                                    FX3 SDK

  Low Power Mode Test
  -------------------

  This example illustrates how the FX3 device can be configured to place 
  in the low power standby or suspend state. This example supports the
  use of the VBUS, UART_CTS and USB D+ or SSRX pins as wake-up sources from
  the low power state. Application also implements a Bulk Loopback function
  based on an AUTO DMA channel.

  Test Procedure for Low Power Test Cases:

  1. Stand-By with UART CTS as Wake Up Source
     Vendor Request Code : 0xE1
     Test Procedure      : Connect UART CTS Pin & FX3 DVK ground to common
     ground of external power supply and issue vendor request, device moves
     into standby state. Toggling UART CTS Pin to High will bring FX3 back
     from the stand-by state.

  2. Stand-By with VBUS as Wake Up Source
     Vendor Request Code : 0xE2
     Test Procedure      : Issue vendor request, device moves into standby state.
     Pull out cable and replug it back will bring FX3 back from the stand-by state.     

  3. Suspend with UART CTS as Wake Up Source
     Vendor Request Code : 0xE3
     Test Procedure      : Connect UART CTS Pin & FX3 DVK ground to common
     ground of external power supply and issue vendor request, device moves
     into suspend state. Toggling UART CTS Pin to High will bring FX3 back
     from the suspend state.

  4. Suspend with VBUS as Wake Up Source
     Vendor Request Code : 0xE4
     Test Procedure      : Issue vendor request and put system on sleep which will
     push device into suspend state. Pull out cable and Resume system, repluging
     USB cable will bring FX3 back from the stand-by state.

  5. Suspend with USB D+ or SSRX as Wake up Source
     Vendor Request Code : 0xE5
     Test Procedure      : Issue vendor request and put system on sleep which will
     push device into suspend state. Resuming system from sleep will bring FX3
     back from the suspend state.

  Logs on UART console can be observed for better clarity of Test case.

  Files:

    * cyfx_gcc_startup.S : Start-up code for the ARM-9 core on the FX3 device.
      This assembly source file follows the syntax for the GNU assembler.

    * cyfxlowpowertest.h : Constant definitions for the application.

    * cyfxlpdscr.c       : C source file containing the USB descriptors that
      are used by this firmware example. VID and PID is defined in this file.

    * cyfxtx.c           : ThreadX RTOS wrappers and utility functions required
      by the FX3 API library.

    * cyfxlowpowertest.c : Main C source file that implements the bulk loopback
      and the power management.

    * makefile           : GNU make compliant build script for compiling this
      example.

[]

