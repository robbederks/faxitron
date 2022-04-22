
                        CYPRESS SEMICONDUCTOR CORPORATION
                                    FX3 SDK

  ASYNCHRONOUS SRAM MASTER EXAMPLE
  --------------------------------

  This example illustrates the use of the FX3 firmware APIs to implement an
  asynchronous SRAM Master. The example makes use of the CY7C1062DV33-10BGXI
  device that is connected to FX3 on the CYUSB3KIT-003 kit.

  The device enumerates as a vendor specific USB device with a pair of Bulk
  endpoints (1-OUT and 1-IN). The OUT endpoint acts as data WRITE to SRAM from
  the PC host and the IN endpoint acts as data READ from the SRAM to the PC Host.

  The READ and WRITE to/from SRAM is done using the GPIF state machine which is
  designed for FX3 to communicate with the SRAM. The state machine generates
  READ and WRITE signals which enables the SRAM to move data IN and OUT.

  The data is carried to and from the PC/ SRAM through 2 AUTO DMA channels between the
  FX3 GPIF socket and the USB socket. Data received on the OUT endpoint is written to
  the SRAM. A read operation is triggered by sending a USB vendor command; and FX3
  reads and transfers the contents of the SRAM to the USB host through the IN
  endpoint.

  Only 8 address pins of the CY7C1062DV33 SRAM are connected to FX3 on the CYUSB3KIT-003 kit.
  This means that only a 1 KB memory region (2 ^ 8 * 32 bits) can be addressed by FX3.
  Larger memories can be addressed by:
  1) Using a 16 bit data bus with a 16 bit address bus.
  2) Using other FX3 GPIOs as additional address pins.
  3) Using external IO expanders to connect to the additional address pins.

  Files:

    * cyfx_gcc_startup.S   : Start-up code for the ARM-9 core on the FX3 device.
      This assembly source file follows the syntax for the GNU assembler.

    * cyfxsrammaster.h     : Constant definitions for the SRAM FX3 READ /WRITE application.

    * cyfxgpif2config.h    : GPIF II Designer generated header file that implements
      the state machine used by this example.

    * cyfxsramusbdscr.c    : C source file containing the USB descriptors that
      are used by this firmware example. VID and PID is defined in this file.

    * cyfxtx.c             : ThreadX RTOS wrappers and utility functions required by the FX3 API library.

    * cyfxsrammaster.c     : Main C source file that implements the SRAM Master example.

    * makefile             : GNU make compliant build script for compiling this example.

[EOF]

