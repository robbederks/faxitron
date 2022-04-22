
                                CYPRESS SEMICONDUCTOR

                                   EZ-USB FX3 SDK

                                 BootGpifDemo example
                                 --------------------

  This directory contains an example application that shows the usage of the FX3 boot API
  to create a GPIF to USB data stream. The functionality provided here matches that of
  the GpifToUsb application based on the full FX3 API.

  The application makes use of a dummy GPIF-II configuration which continually samples
  data from the GPIF data pins and sends them out on USB EP 1-IN as full data packets.

  The application can be configured to function in an AUTO DMA mode as well as in a
  MANUAL DMA mode. The default option is the MANUAL DMA mode.

  Files:

  1. cyfx_gcc_startup.S  : Start-up code for the FX3 device.

  2. defines.h           : Definitions used in the application code.

  3. cyfxgpif2config.h   : GPIF-II configuration used in the application code.

  4. gpif_init.c         : GPIF-II initialization code.

  5. usb_descr.c         : USB descriptors used in the application.

  6. main.c              : Main application logic.

  7. makefile            : GNU make compatible build recipe.

  8. readme.txt          : This file.

#EOF#

