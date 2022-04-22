/*
 ## Cypress USB 3.0 Platform header file (cyu3os.h)
 ## ===========================
 ##
 ##  Copyright Cypress Semiconductor Corporation, 2010-2011,
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

#ifndef _INCLUDED_CYU3P_OS_H_
#define _INCLUDED_CYU3P_OS_H_

#include "cyu3types.h"
#include "cyu3externcstart.h"

/*
   Summary
   The file define the RTOS abstraction layer for FX3 devices.
 */

/**************************************************************************
 ******************************* Macros ***********************************
 **************************************************************************/

/*@@RTOS Constants
   Summary
   The RTOS wrappers used by the FX3 firmware library define and make use of the
   following constants.

   Description
   The following constants are special parameter values passed to some OS
   functions to specify the desired behaviour.

   * CYU3P_NO_WAIT - This is a special value for waitOption which indicates that
     the function should not wait / block the thread and should immediately return.
     This is used for OS functions like CyU3PMutexGet and also for various firmware
     API functions which internally wait for mutexes or events.

   * CYU3P_WAIT_FOREVER - This is a special value for waitOption which indicates that
     the function should wait until the particular mutex or event has been flagged.
     This is used for OS functions like CyU3PMutexGet and also for various firmware
     API functions which internally wait for mutexes and events.

   * CYU3P_EVENT_AND - This is a special value for getOption / setOption in the
     CyU3PEventGet and CyU3PEventSet functions which specifies the bit-wise operation
     to be performed on the event flag. This variable is used when the mask
     and the flag have to be ANDed without modification to the actual flags in the
     case of CyU3PEventGet.

   * CYU3P_EVENT_AND_CLEAR - This is a special value for getOption in the CyU3PEventGet
     function which specifies the bit operation to be performed on the event flag. This
     option is used when the mask and the flag have to be ANDed and the flag cleared.

   * CYU3P_EVENT_OR - This is a special value for the getOption / setOption in
     CyU3PEventGet and CyU3PEventSet functions which specifies the bit operation to be
     performed on the event flag. This option is used when the mask and the flag have
     to be ORed without modification to the flags.

   * CYU3P_EVENT_OR_CLEAR - This is a special value for the getOption in CyU3PEventGet
     function which specifies the bit operation to be performed on the event flag.
     This option is used when the mask and the flag have to be ORed and the flag
     cleared.

   * CYU3P_NO_TIME_SLICE - This is a special value for the timeSlice option in
     CyU3PThreadCreate function. The value specifies that the thread shall not be
     pre-empted based on the timeSlice value provided.

   * CYU3P_AUTO_START - This is a special value for the autoStart option in
     CyU3PThreadCreate function. The value specifies that the thread should be
     started immediately without waiting for a CyU3PThreadResume call.

   * CYU3P_DONT_START - This is a special value for the autoStart option in
     CyU3PThreadCreate function. The value specifies that the thread should be suspended
     on create and shall be started only after a subsequent CyU3PThreadResume call.

   * CYU3P_AUTO_ACTIVATE - This is a special value for the timerOption parameter in
     CyU3PTimerCreate function. This value specifies that the timer shall be automatically
     started after create.

   * CYU3P_NO_ACTIVATE - This is a special value for the timerOption parameter in
     CyU3PTimerCreate function. This value specifies that the timer shall not be activated
     on create and needs to be explicitly activated.

   * CYU3P_INHERIT - This is a special value for the priorityInherit parameter of the
     CyU3PMutexCreate function. This value specifies that the mutex supports priority
     inheritance.

   * CYU3P_NO_INHERIT - This is a special value for the priorityInherit parameter of the
     CyU3PMutexCreate function. This value specifies that the mutex does not support
     priority inheritance.
 */

/**************************************************************************
 ******************************* Data Types *******************************
 **************************************************************************/

/*@@RTOS Data Types
   Summary
   The RTOS wrappers used by the FX3 firmware library for data types.
*/

/*
   Summary
   Byte pool structure.

   Description
   This is the RTOS defined byte pool structure for RTOS internal use.
   The pointer to this structure is used as handle for all RTOS APIs.

   See Also
   * CyU3PBytePoolCreate
   * CyU3PBytePoolDestroy
   * CyU3PByteAlloc
   * CyU3PByteFree
   */
typedef struct CyU3PBytePool     CyU3PBytePool;

/*
   Summary
   Block pool structure.

   Description
   This is the RTOS defined block pool structure for RTOS internal use.
   The pointer to this structure is used as handle for all RTOS APIs.

   See Also
   * CyU3PBlockPoolCreate
   * CyU3PBlockPoolDestroy
   * CyU3PBlockAlloc
   * CyU3PBlockFree
   */
typedef struct CyU3PBlockPool    CyU3PBlockPool;

/*
   Summary
   Thread structure.

   Description
   This is the RTOS defined thread structure for RTOS internal use.
   The pointer to this structure is used as handle for all RTOS APIs.

   See Also
   * CyU3PThreadCreate
   * CyU3PThreadDestroy
   * CyU3PThreadIdentify
   * CyU3PThreadInfoGet
   * CyU3PThreadSleep
   * CyU3PThreadSuspend
   * CyU3PThreadResume
   * CyU3PThreadWaitAbort
   * CyU3PThreadRelinquish
   * CyU3PThreadPriorityChange
   * CyU3PThreadStackErrorNotify
   */
typedef struct CyU3PThread       CyU3PThread;

/* Summary
   Thread entry function type.

   Description
   This type represents the entry function for an RTOS thread created by a FX3
   firmware application. The threadArg argument is a context input that is specified
   by the user when creating the thread.

   See Also
   * CyU3PThread
   * CyU3PThreadCreate
 */
typedef void (*CyU3PThreadEntry_t) (
        uint32_t threadArg              /* User specified context argument. */
        );

/*
   Summary
   Queue structure.

   Description
   This is the RTOS defined queue structure for RTOS internal use.
   The pointer to this structure is used as handle for all RTOS APIs.

   See Also
   * CyU3PQueueCreate
   * CyU3PQueueDestroy
   * CyU3PQueueSend
   * CyU3PQueuePrioritySend
   * CyU3PQueueReceive
   * CyU3PQueueFlush
   */
typedef struct CyU3PQueue        CyU3PQueue;

/*
   Summary
   Mutex structure.

   Description
   This is the RTOS defined mutex structure for RTOS internal use.
   The pointer to this structure is used as handle for all RTOS APIs.

   See Also
   * CyU3PMutexCreate
   * CyU3PMutexDestroy
   * CyU3PMutexPut
   * CyU3PMutexGet
   */
typedef struct CyU3PMutex        CyU3PMutex;

/*
   Summary
   Semaphore structure.

   Description
   This is the RTOS defined semaphore structure for RTOS internal use.
   The pointer to this structure is used as handle for all RTOS APIs.

   See Also
   * CyU3PSemaphoreCreate
   * CyU3PSemaphoreDestroy
   * CyU3PSemaphoreGet
   * CyU3PSemaphorePut
   */
typedef struct CyU3PSemaphore    CyU3PSemaphore;

/*
   Summary
   Event structure.

   Description
   This is the RTOS defined event structure for RTOS internal use.
   The pointer to this structure is used as handle for all RTOS APIs.

   See Also
   * CyU3PEventCreate
   * CyU3PEventDestroy
   * CyU3PEventSet
   * CyU3PEventGet
   */
typedef struct CyU3PEvent        CyU3PEvent;

/*
   Summary
   Timer structure.

   Description
   This is the RTOS defined timer structure for RTOS internal use.
   The pointer to this structure is used as handle for all RTOS APIs.

   See Also
   * CyU3PTimerCreate
   * CyU3PTimerDestroy
   * CyU3PTimerStart
   * CyU3PTimerStop
   * CyU3PTimerModify
   */
typedef struct CyU3PTimer        CyU3PTimer;

/*
   Summary
   Type of callback function called on timer expiration.

   Description
   Type of callback functions that are registered as part of timer creation and are
   invoked when the specified time has elapsed. The timerArg argument is a context
   input that is specified by the user when creating the timer.

   See Also
   * CyU3PTimer
   * CyU3PTimerCreate
 */
typedef void (*CyU3PTimerCb_t) (
        uint32_t timerArg               /* User specified context argument. */
        );

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/*************************** Exception vectors ****************************/

/*@@Exceptions and Interrupts
   Summary
   This section describes the operating modes, exceptions and interrupts in
   the FX3 firmware.

   The FX3 firmware generally executes in the Supervisor (SVC) mode of ARM
   processors, which is a privileged mode. The User and FIQ modes are
   currently unused by the FX3 firmware. The IRQ mode is used for the initial
   part of interrupt handler execution and the System mode is used for handling
   the second halves of long interrupts in a nested manner.

   The Abort and Undefined modes are only executed when the ARM CPU encounters
   an execution error such as an undefined instruction, or a instruction/data
   access abort.

   The FX3 firmware makes use of the full ARM instruction set.
 */

/*@@Interrupt Handling
   Summary
   This section provides information on FX3 device interrupts.

   Description
   The FX3 device has an internal PL192 Vectored Interrupt
   Controller which is used for managing all interrupts raised
   by the device. The drivers for various hardware blocks in the
   FX3 firmware library register interrupt handlers for all
   interrupt sources. Event callbacks are raised to the user
   firmware for all relevant interrupt conditions.
   
   Each interrupt source has a 4 bit priority value associated
   with it. These priority settings are unused as of now; and
   interrupt priority is enforced through nesting and
   pre-emption.
   
   The RTOS used by the FX3 firmware allows interrupts to be
   nested, and this mechanism is used to allow higher priority
   interrupts to pre-empt lower priority ones. Thus the
   interrupts are classified into two groups: high priority ones
   that cannot be pre-empted and low priority ones that can be
   pre-empted.
   
   The high priority (non pre-emptable interrupts) are:
     * USB interrupt
     * DMA interrupt for USB sockets
     * DMA interrupt for GPIF sockets
     * DMA interrupt for Serial peripheral sockets
   
   The low priority (pre-emptable interrupts) are:
     * System control interrupt (used for suspend/wakeup)
     * OS scheduler interrupt (timer based)
     * GPIF interrupt
     * SPI interrupt
     * I2C interrupt
     * I2S interrupt
     * UART interrupt
     * GPIO interrupt

   The respective interrupt handlers in the drivers are responsible for
   enabling/disabling these interrupt sources at the appropriate time.
   Since disabling one or more of these interrupts at arbitrary times
   can cause system errors and crashes; user accessible functions to
   control these interrupts individually are not provided.

   The FX3 SDK only provides APIs that can disable and re-enable all
   interrupt sources so as to ensure atomicity of critical code sections.
  */

/*@@Exception Handler Functions
   Summary
   This section provides information on ARM Exception handlers in the
   FX3 firmware.

   Description
   Exceptions such as data or instruction abort, and undefined instruction
   may happen if the firmware image is corrupted during loading or at runtime.
   Since these are unexpected conditions, the FX3 firmware library does not
   provide any specific code to handle them. Default handlers for these
   conditions are provided in the cyfxtx.c file, and these can be modified
   by the users to match their requirements (for example, reset the FX3 device
   and restart operation).
 */

/*
   Summary
   The undefined instruction exception handler.

   Description
   This function gets invoked when the ARM CPU encounters an
   undefined instruction. This happens when the firmware loses
   control and jumps to unknown locations. This does not occur
   as a part of normal operation sequence. This is a fatal error.

   Return Value
   None

   See Also
   * CyU3PPrefetchHandler
   * CyU3PAbortHandler
   */
extern void
CyU3PUndefinedHandler (
        void);

/*
   Summary
   The pre-fetch error exception handler.

   Description
   This function gets invoked when the ARM CPU encounters an
   instruction pre-fetch error. Since there are no virtual
   memory use case, this is an unknown memory access error.
   This does not occur as a part of normal operation sequence.
   This is a fatal error for this platform.

   Return Value
   None

   See Also
   * CyU3PUndefinedHandler
   * CyU3PAbortHandler
   */
extern void
CyU3PPrefetchHandler (
        void);

/*
   Summary
   The abort error exception handler.

   Description
   This function gets invoked when the ARM CPU encounters an
   data pre-fetch abort error.  Since there are no virtual
   memory use case, this is an unknown memory access error.
   This does not occur as a part of normal operation sequence.
   This is a fatal error for this platform.

   Return Value
   None

   See Also
   * CyU3PUndefinedHandler
   * CyU3PPrefetchHandler
   */
extern void
CyU3PAbortHandler (
        void);

/**************************** Kernel functions ****************************/

/*@@Kernel Functions
   Summary
   The following functions represent the interface between
   the RTOS kernel and the firmware application code.
 */

/* Summary
   RTOS kernel entry function.

   Description
   The function call is expected to initialize the RTOS. This function
   needs to be invoked as the last function from the main () function.
   It is expected that the firmware shall always function in the SVC mode.

   Return Value
   None - Function is a non returning function.

   See Also
   * CyU3PApplicationDefine
 */
extern void
CyU3PKernelEntry (
        void);

/* Summary
   The driver specific OS primitives and threads shall be
   created  in this function.

   Description
   This function shall be implemented by the firmware library
   and needs to be invoked from the kernel during initialization.
   This is where all the threads and OS primitives for all module
   drivers shall be created. Though some OS calls can be safely
   made at this point; scheduling, wait options and sleep functions
   are not expected to be functional at this point.

   Return Value
   None

   See Also
   * CyU3PKernelEntry
 */
extern void
CyU3PApplicationDefine (
        void);

/* Summary
   Initialize the OS scheduler timer.

   Description
   This function is implemented by the firmware library and is
   invoked from the kernel during initialization. The function is
   not expected to be invoked by the FX3 application unless the OS
   timer tick interval needs to be modified. It is recommended to
   run the OS timer tick at 1ms (default) interval for fast and
   accurate response. If the timer tick needs to be modified, the
   API can be invoked only after the RTOS has been initialized.

   The OS_TIMER_TICK shall be defined by this function and all wait
   options will be calculated based on this. By default, the OS timer
   tick is configured to be 1ms. The timer interval value specified 
   can 1ms to 1000ms. Any other value will set the tick timer to 1ms.

   The clock is derived off the 32KHz standby clock. The actual time
   interval can have an error of upto +/- 4%. The time interval is
   accurate only as long as the interrupts are running.

   Return Value
   None
 */
extern void
CyU3POsTimerInit (
        uint16_t intervalMs /* OS Timer tick interval in millisecond. */
        );

/* Summary
   The OS scheduler.

   Description
   This function needs to be implemented in the RTOS kernel and this
   function shall be invoked each time the OS timer count value
   overflows. The timer interrupt handler is implemented as part of
   the firmware library and shall invoke this function.

   Return Value
   None
 */
extern void
CyU3POsTimerHandler (
        void);

/* Summary
   This function saves the active thread context when entering
   the IRQ context.

   Description
   This function shall be invoked by the IRQ handlers as the first
   call and saves the current thread state before handling the IRQ.
   This function should be used if the device does not have a VIC.
   The CyU3PIrqVectoredContextSave function should be used if the
   device includes a VIC.

   Return Value
   None

   See Also
   * CyU3PIrqContextRestore
   * CyU3PIrqVectoredContextSave
   * CyU3PFiqContextSave
 */
extern void
CyU3PIrqContextSave (
        void);

/* Summary
   This function saves the thread context when entering the IRQ context.

   Description
   This shall be invoked by the IRQ handlers as the first call and saves
   the current thread state before handling the IRQ. This function should
   be used if the device uses a VIC and has vectored interrupt handlers.

   Return Value
   None

   See Also
   * CyU3PIrqContextRestore
   * CyU3PIrqContextSave
   * CyU3PFiqContextSave
 */
extern void
CyU3PIrqVectoredContextSave (
        void);

/* Summary
   This function restores the thread context after handling an IRQ interrupt.

   Description
   This shall be invoked by the IRQ handlers as the last call. This is a
   non returning function and restores control to the new thread according
   to the scheduling mechanism of the RTOS. This function can be used in
   devices with or without a VIC.

   Return Value
   None

   See Also
   * CyU3PIrqContextSave
   * CyU3PIrqVectoredContextSave
 */
extern void
CyU3PIrqContextRestore (
        void);

/* Summary
   This function saves the active thread context when entering the FIQ context.

   Description
   This shall be invoked by the FIQ handler as the first call and saves the
   current thread state before handling the FIQ.

   Return Value
   None

   See Also
   * CyU3PFiqContextRestore
 */
extern void
CyU3PFiqContextSave (
        void);

/* Summary
   This function restores the thread context after handling an FIQ interrupt.

   Description
   This shall be invoked by the FIQ handlers as the last call. This is a
   non-returning call and restores the thread state according to the
   scheduling mechanism of the RTOS.

   Return Value
   None

   See Also
   * CyU3PFiqContextSave
 */
extern void
CyU3PFiqContextRestore (
        void);

/* Summary
   This function switches the ARM mode from IRQ to SYS to allow nesting of
   interrupts.

   Description
   Nesting of interrupts on an ARMv5 controller requires that the current
   interrupt handler switch the ARM execution mode to SYS mode. This function
   is used to do this switching. In case interrupt nesting is not required
   for this platform, this function can be a No-Operation.

   Return Value
   None

   See Also
   * CyU3PIrqNestingStop
 */
extern void
CyU3PIrqNestingStart (
        void);

/* Summary
   This function switches the ARM mode from SYS to IRQ at the end of a interrupt
   handler.

   Description
   Nesting of interrupts on an ARMv5 controller requires that the ARM execution
   mode be switched to SYS mode at the beginning of the handler and back to IRQ
   mode at the end. This function is used to do the switch the mode back to IRQ
   at the end of an interrupt handler. In case interrupt nesting is not required
   for this platform, this function can be a No-operation.

   Return Value
   None

   See Also
   * CyU3PIrqNestingStart
 */
extern void
CyU3PIrqNestingStop (
        void);

/**************************** Memory functions ****************************/

/*@@Memory Functions
   Summary
   This section documents the functions that the FX3 firmware provides for
   dynamic memory management. These are implemented as wrappers on top of the
   corresponding RTOS services.

   Description
   The FX3 firmware makes use of a set of memory management services
   that are provided by the RTOS used. The firmware library also provides a set
   of high level dynamic memory management functions that are wrappers on top
   of the corresponding RTOS services.

   Two flavors of memory allocation services are provided.
   * Byte Pool: This is a generic memory pool similar to a fixed sized heap.
     Memory buffers of any size can be allocated from a byte pool.
   * Block pool: This is a memory pool that is optimized for handling buffers
     if a fixed size only. The block pool has lesser memory overhead per buffer
     allocated and can be used in cases where the application requires a large
     number of buffers of a specific size.

   The firmware library or application is not expected to use the compiler
   provided heap for memory allocation, to avoid portability issues across
   different compilers and tool chains. The library code expects that the
   following functions are provided by the application environment for use
   as a general purpose heap.

   * CyU3PMemInit
   * CyU3PMemAlloc
   * CyU3PMemFree
   * CyU3PDmaBufferInit
   * CyU3PDmaBufferDeInit
   * CyU3PDmaBufferAlloc
   * CyU3PDmaBufferFree
   * CyU3PFreeHeaps

   These functions can be implemented using the above mentioned byte pool and
   block pool functions. Implementations of these functions in source form are
   provided for reference, and can be modified as required by the application.
 */

/* Summary
   Initialize the custom heap manager for OS specific allocation calls

   Description
   This function creates a memory pool that can be used in place of the system
   heap. All dynamic memory allocation for OS data structures, thread stacks, and
   firmware data buffers should be allocated out of this memory pool. The size and
   location of this memory pool needs to be adjusted as per the user requirements
   by modifying this function. User application must not invoke this call.

   Return value
   None.

   See Also
   * CyU3PMemAlloc
   * CyU3PMemFree
 */
extern void
CyU3PMemInit (
        void);

/* Summary
   Allocate memory from the dynamic memory pool.

   Description
   This function is the malloc equivalent for allocating memory from the pool
   created by the CyU3PMemInit function. This function needs to be implemented
   as part of the RTOS porting to the FX3 device. This function needs to
   be capable of allocating memory even if called from an interrupt handler.

   Return value
   Pointer to the allocated buffer, or NULL in case of alloc failure.

   See Also
   * CyU3PMemInit
   * CyU3PMemFree
 */
extern void *
CyU3PMemAlloc (
        uint32_t size                   /* The size of the buffer to be allocated in bytes. */
        );

/* Summary
   Free memory allocated from the dynamic memory pool.

   Description
   This function is the free equivalent that frees memory allocated through
   the CyU3PMemAlloc function. This function is also expected to be able to
   free memory in an interrupt context.

   Return value
   None

   See Also
   * CyU3PMemInit
   * CyU3PMemAlloc
 */
extern void
CyU3PMemFree (
         void *mem_p                    /* Pointer to memory buffer to be freed. */
         );

/* Summary
   Fill a region of memory with a specified value.

   Description
   This function is a memset equivalent and is used by the firmware
   library code. It can also be used by the application firmware.

   Return value
   None
 */
extern void
CyU3PMemSet (
        uint8_t *ptr,                   /* Pointer to the buffer to be filled. */
        uint8_t data,                   /* Value to fill the buffer with. */
        uint32_t count                  /* Size of the buffer in bytes. */
        );

/* Summary
   Copy data from one memory location to another.

   Description
   This is a memcpy equivalent function that is provided and used by the
   firmware library. The implementation does not handle the case of
   overlapping buffers.

   Return value
   None
 */
extern void
CyU3PMemCopy (
        uint8_t *dest,                  /* Pointer to destination buffer. */
        uint8_t *src,                   /* Pointer to source buffer. */
        uint32_t count                  /* Size of the buffer to be copied. */
        );

/* Summary
   Compare the contents of two memory buffers.

   Description
   This is a memcmp equivalent function that does a byte by byte
   comparison of two memory buffers. This function is provided
   and used by the firmware library.

   Return value
   Zero    : s1 == s2
   Negative: s1 < s2
   Positive: s1 > s2
 */
extern int32_t
CyU3PMemCmp (
        const void* s1,                 /* Pointer to first memory buffer. */
        const void* s2,                 /* Pointer to second memory buffer. */
        uint32_t n                      /* Size of buffers to compare in bytes. */
        );

/*@@Buffer Functions
   This section documents the functions that get or free buffers for DMA operations.
*/

/* Summary
   Initialize the buffer allocator.

   Description
   This function is the allocates memory required by the DMA engine. There are
   restrictions for the memory to be used with DMA. The buffer should be 32 byte
   aligned and should be a multiple of 32 bytes. This function needs to be
   implemented as part of the RTOS porting to the FX3 device. The function 
   shall be invoked on DMA module initialization. This function must not be
   invoked by user application.

   Return value
   * None

   See Also
   * CyU3PDmaBufferDeInit
   * CyU3PDmaBufferAlloc
   * CyU3PDmaBufferFree
 */
extern void
CyU3PDmaBufferInit (
        void);

/* Summary
   De-initialize the buffer allocator.

   Description
   This function de-initializes the DMA buffer manager. The function shall be 
   invoked on DMA module de-init. This is provided in source format so that it
   can be modified as per user requirement. This function must not be invoked
   by user application.

   Return value
   * None

   See Also
   * CyU3PDmaBufferInit
   * CyU3PDmaBufferAlloc
   * CyU3PDmaBufferFree
 */
extern void
CyU3PDmaBufferDeInit (
        void);

/* Summary
   Allocate the required amount of buffer memory.

   Description
   This function is used to allocate memory required for DMA operations.
   It is called from the DMA module whenever a channel is created. It can
   also be used by user application for allocating memory which can be used
   for DMA operations. This function needs to be implemented as part of RTOS
   porting and is provided in source format. It can be modified, to suit the
   user application. The buffers allocated must be 32 byte aligned and should
   be a multiple of 32 bytes.

   Return value
   * Pointer to the allocated buffer. Return NULL in case of error.

   See Also
   * CyU3PDmaBufferInit
   * CyU3PDmaBufferDeInit
   * CyU3PDmaBufferFree
 */
extern void *
CyU3PDmaBufferAlloc (
        uint16_t size                   /* The size of the buffer required. */
        );

/* Summary
   Free the previously allocated buffer area.

   Description
   This function frees the memory allocated. Care should be taken so that
   buffers allocated by the DMA module is not freed explicitly by the user
   application. This can lead corruption of DMA channels. User application
   can invoke this function for those buffers explicitly allocated. The
   function is also invoked when ever a DMA channel destroy call is made.
   The function is provided in source format and can be modified if required
   by the user application.

   Return value
   * None

   See Also
   * CyU3PDmaBufferInit
   * CyU3PDmaBufferDeInit
   * CyU3PDmaBufferAlloc
 */
extern void
CyU3PDmaBufferFree (
        void *buffer                    /* Address of buffer to be freed. */
        );

/* Summary
   Free up the custom heap manager and the buffer allocator.

   Description
   This function is called in preparation to a reset of the FX3 device and is
   intended to allow the user to free up the custom heap manager and the buffer
   allocator that are created in the CyU3PMemInit and the CyU3PDmaBufferInit
   functions.

   Return value
   None

   See Also
   * CyU3PMemInit
   * CyU3PDmaBufferInit
 */
extern void
CyU3PFreeHeaps (
        void);

/* Summary
   This function creates and initializes a memory byte pool.

   Description
   This function is a wrapper around the byte pool service provided by the
   RTOS and creates a fixed size general purpose heap structure from which
   memory buffers of various sizes can be allocated. The size and location
   of the memory region to be used as the heap storage are passed in as
   parameters.

   Return value
   CY_U3P_SUCCESS if pool was successfully initialized.
   Other RTOS specific error codes in case of failure.

   See Also
   * CyU3PBytePoolDestroy
   * CyU3PByteAlloc
   * CyU3PBlockFree
 */
extern uint32_t
CyU3PBytePoolCreate (
        CyU3PBytePool *pool_p,          /* Handle to the byte pool structure. The memory for the
                                           structure is to be allocated by the caller. This function
                                           only initializes the structure with the pool information. */
        void *poolStart,                /* Pointer to memory region provided for the byte pool. This
                                           address needs to be 16 byte aligned. */
        uint32_t poolSize               /* Size of the memory region provided for the byte pool. This
                                           size needs to be a multiple of 16 bytes. */
        );

/* Summary
   De-initialize and free up a memory byte pool.

   Description
   This function cleans up the previously created byte pool pointed to
   by the pool_p structure. Memory allocated for the pool can be reused
   after this call returns.

   Return value
   CY_U3P_SUCCESS on success, other RTOS error codes on failure.

   See Also
   * CyU3PBytePoolCreate
   * CyU3PByteAlloc
   * CyU3PBlockFree
 */
extern uint32_t
CyU3PBytePoolDestroy (
        CyU3PBytePool *pool_p           /* Handle to the byte pool to be freed up */
        );

/* Summary
   Allocate memory from a byte pool.

   Description
   This function is used to allocate memory buffers from a previously
   created memory byte pool. The waitOption specifies the timeout duration
   after which the memory allocation should be failed. It is permitted to use
   this function from an interrupt context as long as the waitOption is set to
   zero or CYU3P_NO_WAIT.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PBytePoolCreate
   * CyU3PBytePoolDestroy
   * CyU3PByteFree
 */
extern uint32_t
CyU3PByteAlloc (
        CyU3PBytePool *pool_p,          /* Handle to the byte pool to allocate memory from. */
        void **mem_p,                   /* Output variable that points to the allocated memory buffer. */
        uint32_t memSize,               /* Size of memory buffer required in bytes. */
        uint32_t waitOption             /* Timeout for memory allocation operation in terms of
                                           OS timer ticks. Can be set to CYU3P_NO_WAIT or
                                           CYU3P_WAIT_FOREVER. */
        );

/* Summary
   Frees memory allocated by the CyU3PByteAlloc function.

   Description
   This function frees memory allocated from a byte pool using the
   CyU3PByteAlloc function. This function can also be invoked from an
   interrupt context.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PBytePoolCreate
   * CyU3PBytePoolDestroy
   * CyU3PByteAlloc
 */
extern uint32_t
CyU3PByteFree (
        void *mem_p                     /* Pointer to memory buffer to be freed */
        );

/* Summary
   Creates and initializes a memory block pool.

   Description
   This function initializes a memory block pool from which buffers of a fixed
   size can be dynamically allocated. The size of the buffer is specified as a
   parameter to this pool create function.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PBlockPoolDestroy
   * CyU3PBlockAlloc
   * CyU3PBlockFree
 */
extern uint32_t
CyU3PBlockPoolCreate (
        CyU3PBlockPool *pool_p,         /* Handle to block pool structure. The caller needs to
                                           allocate the structure, and this function only initializes
                                           the fields of the structure. */
        uint32_t blockSize,             /* Size of memory blocks that this pool will manage. The block
                                           size needs to be a multiple of 16 bytes. */
        void *poolStart,                /* Pointer to memory region provided for the block pool. */
        uint32_t poolSize               /* Size of memory region provided for the block pool. */
        );

/* Summary
   De-initialize a block memory pool.

   Description
   This function de-initializes a memory block pool. The memory region used
   by the block pool can be re-used after this function returns.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PBlockPoolCreate
   * CyU3PBlockAlloc
   * CyU3PBlockFree
 */
extern uint32_t
CyU3PBlockPoolDestroy (
        CyU3PBlockPool *pool_p          /* Handle to the block pool to be destroyed. */
        );

/* Summary
   Allocate a memory buffer from a block pool.

   Description
   This function allocates a memory buffer from a block pool. The size of the
   buffer is fixed during the pool creation. The waitOption parameter specifies
   the timeout duration before the alloc call is failed.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PBlockPoolCreate
   * CyU3PBlockPoolDestroy
   * CyU3PBlockFree
 */
extern uint32_t
CyU3PBlockAlloc (
        CyU3PBlockPool *pool_p,         /* Handle to the memory block pool. */
        void **block_p,                 /* Output variable that will be filled with a pointer
                                           to the allocated buffer. */
        uint32_t waitOption             /* Timeout duration in timer ticks. */
        );

/* Summary
   Frees a memory buffer allocated from a block pool.

   Description
   This function frees a memory buffer allocated through the CyU3PBlockAlloc
   call.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PBlockPoolCreate
   * CyU3PBlockPoolDestroy
   * CyU3PBlockAlloc
 */
extern uint32_t
CyU3PBlockFree (
        void *block_p                   /* Pointer to buffer to be freed. */
        );

/**************************** Thread functions ****************************/

/*@@Thread Functions
   Summary
   This section documents the thread functions that are provided as part of
   the FX3 firmware.

   Description
   The FX3 firmware provides a set of thread functions that can be
   used by the application to create and manage threads. These functions are
   wrappers around the corresponding RTOS services.

   The firmware framework itself consists of a number of threads that run
   the  drivers for various peripheral blocks on the device. It is expected
   that these driver threads will have higher priorities than any threads
   created by the firmware application.

   Pre-emptive scheduling is typically used in the firmware, and time slices
   are typically not used. This means that thread switches will only happen
   when the active thread is blocked.
 */

/* Summary
   This function creates a new thread.

   Description
   This function creates a new application thread with the specified parameters.
   This function call must be made only after the RTOS kernel has been started.

   The application threads are expected to be created in the CyFxApplicationDefine
   function. This function is expected to implemented in the application specific
   code and shall be invoked by the library after all the device modules have been
   initialized.

   The application threads should take only priority values from 7 to 15. The
   higher priorities (0 - 6) are reserved for the driver threads.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PThreadDestroy
   * CyU3PThreadIdentify
   * CyU3PThreadInfoGet
   * CyU3PThreadPriorityChange
   * CyU3PThreadRelinquish
   * CyU3PThreadSleep
   * CyU3PThreadSuspend
   * CyU3PThreadResume
   * CyU3PThreadWaitAbort
   * CyU3PThreadStackErrorNotify
 */
extern uint32_t
CyU3PThreadCreate (
        CyU3PThread        *thread_p,           /* Pointer to the thread structure. Memory for this
                                                   structure has to be allocated by the caller. */
        char               *threadName,         /* Name string to associate with the thread. All threads
                                                   should be named with the "Thread_Number:Description"
                                                   convention. */
        CyU3PThreadEntry_t  entryFn,            /* Thread entry function. */
        uint32_t            entryInput,         /* Parameter to be passed into the thread entry function. */
        void               *stackStart,         /* Start address of the thread stack. */
        uint32_t            stackSize,          /* Size of the thread stack in bytes. */
        uint32_t            priority,           /* Priority to be assigned to this thread. */
        uint32_t            preemptThreshold,   /* Threshold value for thread pre-emption. Only threads
                                                   with higher priorities than this value will be allowed
                                                   to pre-empt this thread. */
        uint32_t            timeSlice,          /* Maximum execution time allowed for this thread in timer
                                                   ticks. It is recommended that time slices be disabled by
                                                   specifying CYU3P_NO_TIME_SLICE as the value. */
        uint32_t            autoStart           /* Whether this thread should be suspended or started
                                                   immediately. Can be set to CYU3P_AUTO_START or
                                                   CYU3P_DONT_START. */
        );

/* Summary
   Free up and remove a thread from the RTOS scheduler.

   Description
   This function removes a previously created thread from the
   scheduler, and frees up the resources associated with it.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PThreadCreate
   * CyU3PThreadIdentify
   * CyU3PThreadInfoGet
   * CyU3PThreadPriorityChange
   * CyU3PThreadRelinquish
   * CyU3PThreadSleep
   * CyU3PThreadSuspend
   * CyU3PThreadResume
   * CyU3PThreadWaitAbort
   * CyU3PThreadStackErrorNotify
 */
extern uint32_t
CyU3PThreadDestroy (
        CyU3PThread *thread_ptr         /* Pointer to the thread structure. */
        );

/* Summary
   Get the thread structure corresponding to the current thread.

   Description
   This function returns a pointer to the thread structure corresponding
   to the active thread, or NULL if called from interrupt context.

   Return value
   Pointer to the thread structure of the currently running thread

   See Also
   * CyU3PThreadCreate
   * CyU3PThreadDestroy
   * CyU3PThreadInfoGet
   * CyU3PThreadPriorityChange
   * CyU3PThreadRelinquish
   * CyU3PThreadSleep
   * CyU3PThreadSuspend
   * CyU3PThreadResume
   * CyU3PThreadWaitAbort
   * CyU3PThreadStackErrorNotify
 */
extern CyU3PThread *
CyU3PThreadIdentify (
        void);

/* Summary
   Extract information regarding a specified thread.

   Description
   This function is used to extract information about a thread whose
   pointer is provided. This function is used by the debug mechanism
   in the firmware library to extract information about the source thread
   which is queuing log messages.

   The output variables can be set to NULL if the corresponding
   return value is not required.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PThreadCreate
   * CyU3PThreadDestroy
   * CyU3PThreadIdentify
   * CyU3PThreadPriorityChange
   * CyU3PThreadRelinquish
   * CyU3PThreadSleep
   * CyU3PThreadSuspend
   * CyU3PThreadResume
   * CyU3PThreadWaitAbort
   * CyU3PThreadStackErrorNotify
 */
extern uint32_t
CyU3PThreadInfoGet (
    CyU3PThread *thread_p,              /* Pointer to thread to be queried. */
    uint8_t **name_p,                   /* Output variable to be filled with the
                                           thread's name string. */
    uint32_t *priority,                 /* Output variable to be filled with the
                                           thread priority. */
    uint32_t *preemptionThreshold,      /* Output variable to be filled with the
                                           pre-emption threshold. */
    uint32_t *timeSlice                 /* Output variable to be filled with the
                                           time slice value. */
    );

/* Summary
   Change the priority of a thread.

   Description
   This function can be used to change the priority of a specified thread.
   Though this is not expected to be used commonly, it can be used for
   temporary priority changes to prevent deadlocks.

   Parameters
   thread_p  : Pointer to the thread structure.
   newPriority: The new priority value to be set.
   oldPriority: Pointer to a uint32_t to get the previous priority as return value.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PThreadCreate
   * CyU3PThreadDestroy
   * CyU3PThreadIdentify
   * CyU3PThreadInfoGet
   * CyU3PThreadRelinquish
   * CyU3PThreadSleep
   * CyU3PThreadSuspend
   * CyU3PThreadResume
   * CyU3PThreadWaitAbort
   * CyU3PThreadStackErrorNotify
 */
extern uint32_t
CyU3PThreadPriorityChange (
        CyU3PThread *thread_p,          /* Pointer to thread to be modified. */
        uint32_t newPriority,           /* Priority value to assign to the thread. */
        uint32_t *oldPriority           /* Output variable that will hold the original priority. */
        );

/* Summary
   Relinquish control to the OS scheduler.

   Description
   This is a RTOS call for fair scheduling which relinquishes control
   to other ready threads that are at the same priority level. The 
   thread that relinquishes control remains in ready state and can be
   regain control if there are no other ready threads with the same
   priority level.

   This function is useful in threads which take very little time for
   operation and can let other threads to function within the same
   timer tick.

   Return value
   None

   See Also
   * CyU3PThreadCreate
   * CyU3PThreadDestroy
   * CyU3PThreadIdentify
   * CyU3PThreadInfoGet
   * CyU3PThreadPriorityChange
   * CyU3PThreadSleep
   * CyU3PThreadSuspend
   * CyU3PThreadResume
   * CyU3PThreadWaitAbort
   * CyU3PThreadStackErrorNotify
 */
extern void
CyU3PThreadRelinquish (
        void);

/* Summary
   Puts a thread to sleep for the specified timer ticks.

   Description
   This function puts the current thread to sleep for the
   specified number of timer ticks.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PThreadCreate
   * CyU3PThreadDestroy
   * CyU3PThreadIdentify
   * CyU3PThreadInfoGet
   * CyU3PThreadPriorityChange
   * CyU3PThreadRelinquish
   * CyU3PThreadSuspend
   * CyU3PThreadResume
   * CyU3PThreadWaitAbort
   * CyU3PThreadStackErrorNotify
 */
extern uint32_t
CyU3PThreadSleep (
        uint32_t timerTicks             /* Number of timer ticks to sleep. */
        );

/* Summary
   Suspends the specified thread.

   Description
   This function is used to suspend a thread that is in
   the ready state, and can be called from any thread.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PThreadCreate
   * CyU3PThreadDestroy
   * CyU3PThreadIdentify
   * CyU3PThreadInfoGet
   * CyU3PThreadPriorityChange
   * CyU3PThreadRelinquish
   * CyU3PThreadSleep
   * CyU3PThreadResume
   * CyU3PThreadWaitAbort
   * CyU3PThreadStackErrorNotify
 */
extern uint32_t
CyU3PThreadSuspend (
        CyU3PThread *thread_p           /* Pointer to the thread to be suspended. */
        );

/* Summary
   Resume operation of a thread that was previously suspended.

   Description
   This function is used to resume operation of a thread that was suspended
   using the CyU3PThreadSuspend call. Threads that are suspended because
   they are blocked on Mutexes or Events cannot be resumed using this call.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PThreadCreate
   * CyU3PThreadDestroy
   * CyU3PThreadIdentify
   * CyU3PThreadInfoGet
   * CyU3PThreadPriorityChange
   * CyU3PThreadRelinquish
   * CyU3PThreadSleep
   * CyU3PThreadSuspend
   * CyU3PThreadWaitAbort
   * CyU3PThreadStackErrorNotify
 */
extern uint32_t
CyU3PThreadResume (
        CyU3PThread *thread_p           /* Pointer to thread to be resumed. */
        );

/* Summary
   Returns a thread to ready state by aborting all waits on the thread.

   Description
   This function is used to restore a thread to ready state by aborting
   any waits that the thread is performing on Queues, Mutexes or Events.
   The wait operations will return with an error code that indicates that
   they were aborted.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PThreadCreate
   * CyU3PThreadDestroy
   * CyU3PThreadIdentify
   * CyU3PThreadInfoGet
   * CyU3PThreadPriorityChange
   * CyU3PThreadRelinquish
   * CyU3PThreadSleep
   * CyU3PThreadSuspend
   * CyU3PThreadResume
   * CyU3PThreadStackErrorNotify
 */
extern uint32_t
CyU3PThreadWaitAbort (
        CyU3PThread *thread_p           /* Pointer to the thread to restore. */
        );

#ifdef CYU3P_DEBUG

/* Summary
   Type of thread stack overflow handler function.

   Description
   This type represents the callback function that will be called when the RTOS detects a stack
   overflow condition on one of the user created threads.

   See Also
   * CyU3PThreadStackErrorNotify
 */
typedef void (*CyU3PThreadStackErrorHandler_t) (
        CyU3PThread *thread_p                   /* Pointer to thread on which stack overflow was detected. */
        );

/* Summary
   Registers a callback function that will be notified of
   thread stack overflows.

   Description
   This function is only provided in debug builds of the firmware
   and allows the application to register a callback function that
   will be called to notify when any thread in the system has
   encountered a stack overflow.

   This API must be called before any user thread is created.
   It is recommended to invoke this API from CyFxApplicationDefine()

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PThreadCreate
   * CyU3PThreadDestroy
   * CyU3PThreadIdentify
   * CyU3PThreadInfoGet
   * CyU3PThreadPriorityChange
   * CyU3PThreadRelinquish
   * CyU3PThreadSleep
   * CyU3PThreadSuspend
   * CyU3PThreadResume
   * CyU3PThreadWaitAbort
 */
extern uint32_t
CyU3PThreadStackErrorNotify (
        CyU3PThreadStackErrorHandler_t errorHandler     /* Pointer to thread stack error handler function. */
        );
#endif

/***************************** Queue functions ****************************/

/*@@Message Queue Functions
   Summary
   This section documents the message queue functions that are provided as
   part of the FX3 firmware.

   Description
   The FX3 firmware makes use of message queues for inter-thread data
   communication. A set of wrapper functions on top of the corresponding OS
   services are provided to allow the use of message queues from application
   threads.
 */

/* Summary
   Create a message queue.

   Description
   Create a message queue that can hold a specified number of messages of a
   specified size. The memory for the queue should be allocated and passed in
   by the caller.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PQueueDestroy
   * CyU3PQueueSend
   * CyU3PQueuePrioritySend
   * CyU3PQueueReceive
   * CyU3PQueueFlush
 */
extern uint32_t
CyU3PQueueCreate (
        CyU3PQueue *queue_p,            /* Pointer to the queue structure. This should
                                           be allocated by the caller and will be initialized
                                           by the queue create function. */
        uint32_t messageSize,           /* Size of each message in 4 byte words. Allowed values
                                           are from 1 to 16 (4 bytes to 64 bytes). */
        void *queueStart,               /* Pointer to memory buffer to be used for message storage. */
        uint32_t queueSize              /* Total size of the queue in bytes. */
        );

/* Summary
   Free up a previously created message queue.

   Description
   This function frees up a previously created message queue. Any
   function call that is waiting to send or receive messages on
   this queue will return with an error.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PQueueCreate
   * CyU3PQueueSend
   * CyU3PQueuePrioritySend
   * CyU3PQueueReceive
   * CyU3PQueueFlush
 */
extern uint32_t
CyU3PQueueDestroy (
        CyU3PQueue *queue_p             /* Pointer to the queue to be destroyed. */
        );

/* Summary
   Queue a new message on the specified message queue.

   Description
   This function waits on and adds a new message on to the specified
   message queue. The amount of time to wait to try to enqueue the message
   is specified as a parameter. In case this function is called from
   an interrupt handler, the time-out should be specified as CYU3P_NO_WAIT.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PQueueCreate
   * CyU3PQueueDestroy
   * CyU3PQueuePrioritySend
   * CyU3PQueueReceive
   * CyU3PQueueFlush
 */
extern uint32_t
CyU3PQueueSend (
        CyU3PQueue *queue_p,            /* Queue to add a new message to. */
        void *src_p,                    /* Pointer to buffer containing message. */
        uint32_t waitOption             /* Timeout value to wait on the queue. */
        );

/* Summary
   Add a new message at the head of a message queue.

   Description
   This function is used to send a high priority message to a
   message queue which will placed at the head of the queue.
   The duration to wait for a free location on the queue needs
   to be specified through the waitOption parameter.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PQueueCreate
   * CyU3PQueueDestroy
   * CyU3PQueueSend
   * CyU3PQueueReceive
   * CyU3PQueueFlush
 */
extern uint32_t
CyU3PQueuePrioritySend (
        CyU3PQueue *queue_p,            /* Pointer to the message queue structure. */
        void *src_p,                    /* Pointer to message buffer. */
        uint32_t waitOption             /* Timeout duration in timer ticks. */
        );

/* Summary
   Receive a message from a message queue.

   Description
   This function waits on and tries to receive a message from the
   specified message queue. The message from the head of the queue
   will be returned when the queue is non-empty. The timeout duration
   should be set to CYU3P_NO_WAIT if this call is being made from an
   interrupt handler.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PQueueCreate
   * CyU3PQueueDestroy
   * CyU3PQueueSend
   * CyU3PQueuePrioritySend
   * CyU3PQueueFlush
 */
extern uint32_t
CyU3PQueueReceive (
        CyU3PQueue *queue_p,            /* Pointer to the message queue. */
        void *dest_p,                   /* Pointer to buffer where the message should be
                                           copied. */
        uint32_t waitOption             /* Timeout duration in timer ticks. */
        );

/* Summary
   Flushes all messages on a queue.

   Description
   The function removes all waiting messages on the specified queue.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PQueueCreate
   * CyU3PQueueDestroy
   * CyU3PQueueSend
   * CyU3PQueuePrioritySend
   * CyU3PQueueReceive
 */
extern uint32_t
CyU3PQueueFlush (
        CyU3PQueue *queue_p             /* Pointer to the queue to be flushed. */
        );

/***************************** Mutex functions ****************************/

/*@@Mutex functions
   Summary
   This section documents the mutex functions that are provided as part of
   the FX3 firmware.

   Description
   The FX3 firmware provides a set of mutex functions that can be used
   for protection of global data structures in a multi-thread environment. These
   functions are all wrappers around the corresponding OS services.
 */

/* Summary
   Create a mutex variable.

   Description
   This function creates a mutex variable. The mutex data structure has to
   be allocated by the caller, and will be initialized by the function.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PMutexDestroy
   * CyU3PMutexGet
   * CyU3PMutexPut
 */
extern uint32_t
CyU3PMutexCreate (
        CyU3PMutex *mutex_p,            /* Pointer to Mutex data structure to be initialized. */
        uint32_t priorityInherit        /* Whether priority inheritance should be allowed for
                                           this mutex. Allowed values for this parameter are
                                           CYU3P_NO_INHERIT and CYU3P_INHERIT. */
        );

/* Summary
   Destroy a mutex variable.

   Description
   This function destroys a mutex that was created using the
   CyU3PMutexCreate API.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PMutexCreate
   * CyU3PMutexGet
   * CyU3PMutexPut
 */
extern uint32_t
CyU3PMutexDestroy (
        CyU3PMutex *mutex_p             /* Pointer to mutex to be destroyed. */
        );

/* Summary
   Get the lock on a mutex variable.

   Description
   This function is used to wait on a mutex variable and to get a lock on it.
   The maximum amount of time to wait is specified through the waitOption
   parameter.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PMutexCreate
   * CyU3PMutexDestroy
   * CyU3PMutexPut
 */
extern uint32_t
CyU3PMutexGet (
        CyU3PMutex *mutex_p,            /* Pointer to mutex to be acquired. */
        uint32_t waitOption             /* Timeout duration in timer ticks. */
        );

/* Summary
   Release the lock on a mutex variable.

   Description
   This function is used to release the lock on a mutex variable. The mutex
   can then be acquired by the highest priority thread from among those
   waiting for it.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PMutexCreate
   * CyU3PMutexDestroy
   * CyU3PMutexGet
 */
extern uint32_t
CyU3PMutexPut (
        CyU3PMutex *mutex_p             /* Pointer to mutex to be released. */
        );

/*************************** Semaphore functions **************************/

/*@@Semaphore Functions
   Summary
   This section documents the semaphore functions supported by the FX3
   firmware.

   Description
   In addition to mutexes used for ownership control of shared data and mutual
   exclusion, the FX3 firmware also provides counting semaphores that
   can be used for synchronization and signaling. Each semaphore is created with
   an initial count and the count is decremented on each successful get operation.
   The get operation is failed when the count reaches zero. Each put operation
   on the semaphore increments the associated count.
 */

/* Summary
   Create a semaphore object.

   Description
   This function creates a semaphore object with the specified initial count.
   The semaphore data structure has to be allocated by the caller and will be
   initialized by this function call.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PSemaphoreDestroy
   * CyU3PSemaphoreGet
   * CyU3PSemaphorePut
 */
extern uint32_t
CyU3PSemaphoreCreate (
        CyU3PSemaphore *semaphore_p,    /* Pointer to semaphore to be initialized. */
        uint32_t initialCount           /* Initial count to associate with semaphore. */
        );

/* Summary
   Destroy a semaphore object.

   Description
   This function destroys a semaphore object. All threads waiting to get
   the semaphore will receive an error code identifying that the object has
   been removed.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PSemaphoreCreate
   * CyU3PSemaphoreGet
   * CyU3PSemaphorePut
 */
extern uint32_t
CyU3PSemaphoreDestroy (
        CyU3PSemaphore *semaphore_p     /* Pointer to semaphore to be destroyed. */
        );

/* Summary
   Get an instance from the specified counting semaphore.

   Description
   This function is used to get an instance (i.e., decrement the count by one)
   from the specified counting semaphore. The maximum interval to wait for
   is specified using the waitOption parameter.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PSemaphoreCreate
   * CyU3PSemaphoreDestroy
   * CyU3PSemaphorePut
 */
extern uint32_t
CyU3PSemaphoreGet (
        CyU3PSemaphore *semaphore_p,    /* Pointer to semaphore to get. */
        uint32_t waitOption             /* Timeout duration in timer ticks. */
        );

/* Summary
   Release an instance of the specified counting semaphore.

   Description
   This function releases an instance (increments the count by one)
   of the specified counting semaphore. The semaphore put can be done
   from any thread and does not need to be done by the same thread as
   called the get function.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PSemaphoreCreate
   * CyU3PSemaphoreDestroy
   * CyU3PSemaphoreGet
 */
extern uint32_t
CyU3PSemaphorePut (
        CyU3PSemaphore *semaphore_p     /* Pointer to semaphore to put. */
        );

/***************************** Event functions ****************************/

/*@@Event Flag Functions
   Summary
   This section documents the event flag functions provided by the
   FX3 firmware.

   Description
   Event flags are an advanced means for inter-thread synchronization that
   is provided as part of the FX3 firmware. These functions are
   wrappers around the corresponding functionality provided by the RTOS.

   Event flag groups are groups of single bit flags or signals that can
   be used for inter-thread communication. Each flag in a event group can
   be individually signaled or waited upon by any given thread. It is
   possible for multiple threads to wait on different flags that are
   implemented by a single event group. This facility makes event flags
   a memory efficient means for inter-thread synchronization.

   Event flags are frequently used for synchronization between various
   driver threads in the FX3 device.
 */

/* Summary
   Create an event flag group.

   Description
   This function creates an event flag group consisting of 32 independent
   event flags. The application is free to use as many flags as required
   from this group. If more than 32 flags are required, multiple event flag
   groups have to be created.

   As with other OS service create functions, the caller is expected to
   allocate memory for the Event data structure.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PEventDestroy
   * CyU3PEventSet
   * CyU3PEventGet
 */
extern uint32_t
CyU3PEventCreate (
        CyU3PEvent *event_p             /* Pointer to event structure to be initialized. */
        );

/* Summary
   Destroy an event flag group.

   Description
   This function frees up an event flag group. Any threads waiting on one or more
   flags in the group will be re-activated and will receive an error code from the
   event wait function.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PEventCreate
   * CyU3PEventSet
   * CyU3PEventGet
 */
extern uint32_t
CyU3PEventDestroy (
        CyU3PEvent *event_p             /* Pointer to event group to be destroyed. */
        );

/* Summary
   Update one or more flags in an event group.

   Description
   This function is used to set or clear one or more flags that are part of a
   specified event group. The parameters can be selected so as to set a number
   of specified flags, or to clear of number of specified flags.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PEventCreate
   * CyU3PEventDestroy
   * CyU3PEventGet
 */
extern uint32_t
CyU3PEventSet (
        CyU3PEvent *event_p,            /* Pointer to event group to update. */
        uint32_t rqtFlag,               /* Bit-mask that selects event flags of interest. */
        uint32_t setOption              /* Type of set operation to perform.
                                           * CYU3P_EVENT_AND: The rqtFlag will be ANDed with the current flags.
                                           * CYU3P_EVENT_OR: The rqtFlag will be ORed with the current flags. */
        );

/* Summary
   Wait for or get the status of an event flag group.

   Description
   This function is used to the get the current status of the flags in a
   event group. This can also be used to wait until one or more flags of
   interest have been signaled. The maximum amount of time to wait is
   specified through the waitOption parameter.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PEventCreate
   * CyU3PEventDestroy
   * CyU3PEventSet
 */
extern uint32_t
CyU3PEventGet (
        CyU3PEvent *event_p,            /* Pointer to event group to be queried. */
        uint32_t rqtFlag,               /* Bit-mask that selects event flags of interest.
                                           All bit positions corresponding to events of interest
                                           should be set to 1, and others should be cleared. */
        uint32_t getOption,             /* Type of operation to perform.
                                           * CYU3P_EVENT_AND: Use to wait until all selected flags are signaled.
                                           * CYU3P_EVENT_AND_CLEAR: Same as above, and clear the flags on read.
                                           * CYU3P_EVENT_OR: Wait until any of selected flags are signaled.
                                           * CYU3P_EVENT_OR_CLEAR: Same as above, and clear the flags on read. */
        uint32_t *flag_p,               /* Output parameter filled in with the state of the flags. */
        uint32_t waitOption             /* Timeout duration in timer ticks. */
        );

/***************************** Timer functions ****************************/

/*@@Timer Functions
   Summary
   This section documents the application timer functions that are provided
   by the FX3 firmware.

   Description
   Application timers are OS services that support the implementation of
   periodic tasks in the firmware application. Any number of application timers
   (subject to memory constraints) can be created by the application and the
   time intervals specified should be multiples of the OS timer ticks.

   The OS keeps track of the registered application timers and calls the
   application provided callback functions every time the corresponding
   interval has elapsed.
 */

/* Summary
   Create an application timer.

   Description
   This function creates an application timer than can be configured as a
   one-shot timer or as a auto-reload timer.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PTimerCb_t
   * CyU3PTimerDestroy
   * CyU3PTimerStart
   * CyU3PTimerStop
   * CyU3PTimerModify
   * CyU3PGetTime
   * CyU3PSetTime
 */
extern uint32_t
CyU3PTimerCreate (
        CyU3PTimer *timer_p,                    /* Pointer to the timer structure to be initialized. */
        CyU3PTimerCb_t expirationFunction,      /* Pointer to callback function called on timer expiration. */
        uint32_t expirationInput,               /* Parameter to be passed to the callback function. */
        uint32_t initialTicks,                  /* Initial value for the timer. This timer count will
                                                   be decremented once per timer tick and the callback
                                                   will be invoked once the count reaches zero. */
        uint32_t rescheduleTicks,               /* The reload value for the timer. If set to zero,
                                                   the timer will be a one-shot timer. */
        uint32_t timerOption                    /* Timer start control.
                                                   * CYU3P_AUTO_ACTIVATE: Start the timer immediately.
                                                   * CYU3P_NO_ACTIVATE: Wait until timer is started explicitly. */
        );

/* Summary
   Destroy an application timer object.

   Description
   This function destroys and application timer object.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PTimerCreate
   * CyU3PTimerStart
   * CyU3PTimerStop
   * CyU3PTimerModify
   * CyU3PGetTime
   * CyU3PSetTime
 */
extern uint32_t
CyU3PTimerDestroy (
        CyU3PTimer *timer_p             /* Pointer to the timer to be destroyed. */
        );

/* Summary
   Start an application timer.

   Description
   This function activates a previously stopped timer. This operation can
   be used to start a timer that was created with the CYU3P_NO_ACTIVATE option,
   or to re-start a one-shot or continuous timer that was stopped previously.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PTimerCreate
   * CyU3PTimerDestroy
   * CyU3PTimerStop
   * CyU3PTimerModify
   * CyU3PGetTime
   * CyU3PSetTime
 */
extern uint32_t
CyU3PTimerStart (
        CyU3PTimer *timer_p             /* Timer to be started. */
        );

/* Summary
   Stop operation of an application timer.

   Description
   This function can be used to stop operation of an application timer. The
   parameters associated with the timer can then be modified using the
   CyU3PTimerModify call.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PTimerCreate
   * CyU3PTimerDestroy
   * CyU3PTimerStart
   * CyU3PTimerModify
   * CyU3PGetTime
   * CyU3PSetTime
 */
extern uint32_t
CyU3PTimerStop (
        CyU3PTimer *timer_p             /* Pointer to timer to be stopped. */
        );

/* Summary
   Modify parameters of an application timer.

   Description
   This function is used to modify the periodicity of an application timer
   and can be called only after stopping the timer.

   Return value
   CY_U3P_SUCCESS (0) on success. Other error codes on failure.

   See Also
   * CyU3PTimerCreate
   * CyU3PTimerDestroy
   * CyU3PTimerStart
   * CyU3PTimerStop
   * CyU3PGetTime
   * CyU3PSetTime
 */
extern uint32_t
CyU3PTimerModify (
        CyU3PTimer *timer_p,            /* Pointer to the timer. */
        uint32_t initialTicks,          /* Initial count to be set for the timer. */
        uint32_t rescheduleTicks        /* Reload count for the timer. */
        );

/* Summary
   Get time from reset in terms of OS timer ticks.

   Description
   The function returns relative tick count value.

   Return value
   Current OS timer tick count. Starts from zero on reset.

   See Also
   * CyU3PTimerCreate
   * CyU3PTimerDestroy
   * CyU3PTimerStart
   * CyU3PTimerStop
   * CyU3PTimerModify
   * CyU3PSetTime
 */
extern uint32_t
CyU3PGetTime (
        void);

/* Summary
   Update the timer tick count.

   Description
   This function modifies the timer tick count that is started at system reset.
   This function should be called when the application gets defined as this
   affects all OS primitives.

   Return value
   None.

   See Also
   * CyU3PTimerCreate
   * CyU3PTimerDestroy
   * CyU3PTimerStart
   * CyU3PTimerStop
   * CyU3PTimerModify
   * CyU3PGetTime
 */
extern void
CyU3PSetTime (
        uint32_t newTime                /* New timer tick value to set. */
        );

/**************************************************************************/

/***** ThreadX RTOS *****/

#ifdef __CYU3P_TX__
#ifdef CYU3P_OS_DEFINED
#error Only one RTOS can be defined
#endif

#define CYU3P_OS_DEFINED

#include "cyu3tx.h"
#endif

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3P_OS_H_ */

/*[]*/

