
                        Cypress Semiconductor
                           EZ-USB FX3 SDK
                           Version 1.3.3

  OpenOCD binary for Linux 64 bit
  -------------------------------

  This folder provides an openocd binary for 64-bit Linux based
  platforms. The binary is based on the OpenOCD 0.8.0 release version
  and is designed to work with the Cypress CY7C65215 (USB Serial)
  device as a debug probe.

  This binary can be used with the CYUSB3KIT-003 development kit from
  Cypress, which has an onboard CY7C65215 which provides the JTAG debug
  probe and USB to UART capabilities.

  This binary has the following dependencies:

  1. libcyusbserial library: This is part of the Cypress USB-Serial SDK
     which can be obtained from http://www.cypress.com/?docID=45728.
     A pre-compiled version of this library is provided here along with
     the openocd binary.

  2. libusb-1.0 library: This is the standard LibUsb-1.0 library compiled
     for Linux. The latest version of libusb-1.0 can be downloaded from
     http://sourceforge.net/projects/libusb/files/libusb-1.0/ and compiled.
     A pre-compiled version of this library is provided here along with
     the openocd binary.

  3. libusb-0.1 library: This is the standard LibUsb-0.1 library compiled
     for Linux. The sources for this library can be downloaded from
     http://sourceforge.net/projects/libusb/files/libusb-0.1%20%28LEGACY%29/
     and compiled. A pre-compiled version of this library is provided here
     along with the openocd binary.

  Instructions for use:
  ---------------------
  1. Run the script.sh as root (sudo bash script.sh) to copy the libraries
     required by openocd to the relevant system folders.
  2. Follow instructions in the EzUsbSuite_QSG.pdf document to setup the
     JTAG configuration for an FX3 firmware project.

  Notes:
  ------
  1. This binary only supports debugging using the Cypress CY7C65215
     device as the debug probe. If other (standard) debug probes are being
     used, please download and compile the latest OpenOCD sources.

  2. The CY7C65215 part implements the JTAG protocol by controller
     bound bit-banging; and is therefore quite slow when performing
     a firmware download. A 150 KB firmware download to the FX3 device
     could take about 30 seconds using this connection. However, all
     OpenOCD functionality such as breakpoints, memory and register views
     etc. are supported.

[EOF]

