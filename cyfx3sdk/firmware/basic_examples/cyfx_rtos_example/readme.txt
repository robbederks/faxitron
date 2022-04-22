
                        CYPRESS SEMICONDUCTOR CORPORATION
                                    FX3 SDK

  RTOS Usage Example
  ------------------

  The FX3 SDK provides the fully functional ThreadX RTOS for usage in customer
  applications. While the ThreadX services (APIs) are mapped to Cypress provided
  Wrapper macros or functions for convenience, the original ThreadX features are
  not disabled and are all available for use.

  This example demonstrates the use of the ThreadX RTOS services like Threads,
  Mutexes, Semaphores and Event Flag Groups. The example also provides a way
  for the user to get visual indication of RTOS object state changes through
  GPIOs on the FX3 Development Kits.

  The application does not perform any actual data transfers, but makes use of
  multiple threads which operate on the other services like Mutexes, Semaphores
  and Event Flag Groups.

  The profiling enabled builds of the example (ProfileDebug or ProfileRelease)
  can be used to get indication of the ARM CPU loading as well as activity on
  each of the RTOS objects.

  Files:

    * cyfx_gcc_startup.S   : Start-up code for the ARM-9 core on the FX3 device.
      This assembly source file follows the syntax for the GNU assembler.

    * cyfx_rtos_eg.h       : Defines the thread stack size and priority constants.

    * cyfx_rtos_eg.c       : C source file that implements the application logic.

    * cyfxtx.c             : ThreadX RTOS wrappers and utility functions required
      by the FX3 API library.

    * makefile             : GNU make compliant build script for compiling this
      example.

[]

