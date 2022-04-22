
                        CYPRESS SEMICONDUCTOR CORPORATION
                                    FX3 SDK

  FX3G2 UVC Demo Firmware Example
  -------------------------------

  This example implements a UVC 1.1 compliant firmware demo application for the Cypress EZ-USB FX2G2
  device. The FX2G2 device is a USB 2.0 only version of the EZ-USB FX3 device, and has the computing
  capabilities and supports all interfaces of the FX3 device except the USB SuperSpeed PHY and device
  logic.
  
  Firmware applications for the FX2G2 device are built using the FX3 SDK, and using the same firmware
  libraries and APIs. As FX2G2 is a USB 2.0 only device, applications targeted for FX2G2 do not need
  to implement USB 3.0 specific features such as SuperSpeed descriptors, USB Low Power Mode (LPM)
  handling etc. FX2G2 applications can be developed and tested using the Cypress FX3 development
  kits such as CYUSB3KIT-001 and CYUSB3KIT-003.
  
  This application is designed to work on these kits without any external devices, and therefore does
  not use an actual image source. Instead, the video stream is generated within the FX2G2 device memory
  and sent up to the USB host in the UVC defined data format.

  The application demonstrates USB Video Streaming using Bulk or Isochronous endpoints in USB Hi-Speed
  and Full Speed Modes. The YUY2 uncompressed image format is used, and a constant pink image pattern
  is transmitted continuously.

  The example works at the following settings:

  1) USB Hi-Speed:
        A) Bulk: 640 * 480, 16 bits per pixel YUY2 image at about 66 frames per second.
        B) Isochronous: 640 * 480, 16 bits per pixel YUY2 image at about 30 frames per second.

  2) USB Full Speed:
        A) Bulk: 320 * 240, 16 bits per pixel YUY2 image at about 7 frames per second.
        B) Isochronous: 320 * 240, 16 bits per pixel YUY2 image at about 5 frames per second.
  
  The application uses a Bulk Endpoint for streaming by default. An Isochronous endpoint can be
  selected by enabling the USE_ISOCHRONOUS_ENDPOINT definition in the cyfx2g2_uvc.h header file,
  and re-compiling the application.

  Application Files:
    * cyfx_gcc_startup.S:       Start-up code for the ARM-9 core on the FX2G2 device.
                                This assembly source file follows the syntax for the GNU assembler.

    * cyfxtx.c          :       ThreadX RTOS wrappers and utility functions required
                                by the FX3/FX2G2 API library.

    * cyfx2g2_uvc.h     :       Constant definitions used in the application.

    * cyfx2g2_uvc.c     :       C source file implementing the UVC logic.

    * cyfx2g2_dscr.c    :       C source file containing the USB descriptors and UVC setting structures
                                used by the application.

    * makefile          :       GNU make compliant build script for compiling this example.

[]

