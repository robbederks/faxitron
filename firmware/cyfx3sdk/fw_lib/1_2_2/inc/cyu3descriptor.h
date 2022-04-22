/*
 ## Cypress USB 3.0 Platform header file (cyu3descriptor.h)
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

#ifndef _INCLUDED_CYU3DESCRIPTOR_H_
#define _INCLUDED_CYU3DESCRIPTOR_H_

#include "cyu3types.h"
#include "cyu3externcstart.h"

/*
   Summary
   Descriptors are data structures that keep track of the source/destinations
   and the memory buffers for a data transfer through the West Bridge
   device.  The firmware maintains a queue of descriptors that are allocated
   as required and used by the corresponding firmware modules.  This file
   defines the data structures and the interfaces for descriptor management.
 */

/**************************************************************************
 ******************************* Data types *******************************
 **************************************************************************/

/* Summary
   Descriptor data structure.

   Description
   This data structure contains the fields that make up a DMA descriptor on
   the device.

   The following members are composed of the corresponding fields.
   The fields are defined in sock_regs.h. Refer to sock_regs.h for
   more details about these fields.

   buffer:
        (CY_U3P_BUFFER_ADDR_MASK)

   sync:
        (CY_U3P_EN_PROD_INT | CY_U3P_EN_PROD_EVENT | CY_U3P_PROD_IP_MASK |
        CY_U3P_PROD_SCK_MASK | CY_U3P_EN_CONS_INT | CY_U3P_EN_CONS_EVENT |
        CY_U3P_CONS_IP_MASK | CY_U3P_CONS_SCK_MASK)

   chain:
        (CY_U3P_WR_NEXT_DSCR_MASK | CY_U3P_RD_NEXT_DSCR_MASK)

   size:
        (CY_U3P_BYTE_COUNT_MASK | CY_U3P_BUFFER_SIZE_MASK | CY_U3P_BUFFER_OCCUPIED |
        CY_U3P_BUFFER_ERROR | CY_U3P_EOP | CY_U3P_MARKER)

   See Also
   * CyU3PDmaDscrGetConfig
   * CyU3PDmaDscrSetConfig
 */
typedef struct CyU3PDmaDescriptor_t
{
    uint8_t    *buffer;    /* Pointer to buffer used. */
    uint32_t    sync;      /* Consumer, Producer binding. */
    uint32_t    chain;     /* Next descriptor links. */
    uint32_t    size;      /* Current and maximum sizes of buffer. */
} CyU3PDmaDescriptor_t;

/**************************************************************************
 ********************** Global variable declarations **********************
 **************************************************************************/

extern CyU3PDmaDescriptor_t *glDmaDescriptor; /* Pointer to DMA descriptors */

/**************************************************************************
 ********************************* Macros *********************************
 **************************************************************************/

/* This is the location of the descriptor pool. These are special memory
   locations used for hardware configuration and this value must not be
   modified.
 */
#define CY_U3P_DMA_DSCR0_LOCATION   (0x40000000)

/* This is the number of descriptor in the pool. This value should not be
   modified.
 */
#define CY_U3P_DMA_DSCR_COUNT       (512)

/* This is the size of the descriptor in memory. This value should not be
   modified.
 */
#define CY_U3P_DMA_DSCR_SIZE        (16)

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/************************ Descriptor Pool Functions ***********************/

/*@@Descriptor Functions
   This section documents the utility functions that work with the DMA descriptors.
   These functions are supposed to be internal functions for the libraries' use
   and are not expected to be called directly by the user application.

   If an application chooses to call these functions, extreme care must be taken
   to validate the parameters being passed as these functions do not perform 
   any error checks. Passing incorrect/invalid parameters can result in
   unpredictable behavior. In particular, the buffer address in the DMA 
   descriptor needs to be in system memory area of 512KB. Any other buffer
   address can cause the entire DMA engine to freeze, requiring a reset.
*/

/* Summary
   Create (init) the free descriptor list.

   Description
   This function initializes the free descriptor list, and marks all of the
   descriptors as available. This function is invoked internal to the library
   and is not expected to be called explicitly.

   Return value
   * None

   See Also
   * CyU3PDmaDscrListDestroy
 */
extern void
CyU3PDmaDscrListCreate (
        void);

/* Summary
   Destroy (de-init) the free descriptor list.

   Description
   This function de-initializes the free descriptor list, and marks all of the
   descriptors as non-available. This function is invoked internal to the
   library and is not expected to be called explicitly.

   Return value
   * None

   See Also
   * CyU3PDmaDscrListCreate
 */
extern void
CyU3PDmaDscrListDestroy (
        void);

/* Summary
   Get a descriptor number from the free list.

   Description
   This function searches the free list for the first available descriptor,
   and returns the index. This function is used by the DMA channel APIs.
   These can be used to customize descriptors and do advanced DMA operations.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_BAD_ARGUMENT - if a NULL pointer is passed
   * CY_U3P_ERROR_FAILURE - if no free descriptor is found

   See Also
   * CyU3PDmaDscrPut
   * CyU3PDmaDscrGetFreeCount
 */
extern CyU3PReturnStatus_t
CyU3PDmaDscrGet (
        uint16_t *index_p       /* Output parameter which is filled with the free descriptor index. */
        );

/* Summary
   Add a descriptor number to the free list.

   Description
   This function marks a descriptor as available in the free list.
   This function is used by the DMA channel APIs. These can be used
   to customize descriptors and do advanced DMA operations.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_BAD_ARGUMENT - if an invalid index is passed

   See Also
   * CyU3PDmaDscrGet
   * CyU3PDmaDscrGetFreeCount
 */
extern CyU3PReturnStatus_t
CyU3PDmaDscrPut (
        uint16_t index          /* Descriptor index to be marked free. */
        );


/* Summary
   Get the number of free descriptors available.

   Description
   This function returns the number of descriptors available for
   use. It is used by the DMA channel APIs. These can be used to
   customize descriptors and do advanced DMA operations.

   Return value
   * The number of free descriptors available.

   See Also
   * CyU3PDmaDscrGet
   * CyU3PDmaDscrPut
 */
extern uint16_t
CyU3PDmaDscrGetFreeCount (
        void);

/*************************** Descriptor Functions *************************/

/* Summary
   Set the descriptor configuration.

   Description
   Since the descriptor cannot be set directly, load the required configuration
   into the parameter and configure the required descriptor. This function is 
   used by the DMA channel APIs. These can be used to customize descriptors and
   do advanced DMA operations.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_BAD_ARGUMENT - if an ivalid index or a NULL pointer is passed

   See Also
   * CyU3PDmaDscrGetConfig
 */
extern CyU3PReturnStatus_t
CyU3PDmaDscrSetConfig (
        uint16_t index,                 /* Index of the descriptor to be updated. */
        CyU3PDmaDescriptor_t *dscr_p    /* Pointer to descriptor structure containing the desired configuration. */
        );

/* Summary
   Get the descriptor configuration.

   Description
   Since the descriptor cannot be read directly, read the configuration
   into a descriptor structure. This function is used by the DMA channel
   APIs. These can be used to customize descriptors and do advanced DMA
   operations.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_BAD_ARGUMENT - if an ivalid index or a NULL pointer is passed

   See Also
   * CyU3PDmaDscrGetConfig
 */
extern CyU3PReturnStatus_t
CyU3PDmaDscrGetConfig (
        uint16_t index,                 /* Index of the descriptor to read. */
        CyU3PDmaDescriptor_t *dscr_p    /* Output parameter that will be filled with the descriptor values. */
        );

/* Summary
   Create a circular chain of descriptors.

   Description
   The function creates a chain of descriptors for DMA operations.
   Both the producer and consumer chains are created with the same
   values. The descriptor sync parameter is updated as provided.
   Buffer is allocated if the bufferSize variable is non zero.
   This function is used by the DMA channel APIs. These can be used
   to customize descriptors and do advanced DMA operations.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_MEMORY_ERROR - if the number of descriptors required in this chain are not available OR the buffers could not be allocated
   * CY_U3P_ERROR_BAD_ARGUMENT - if number of descriptors required is zero OR the index is NULL

   See Also
   * CyU3PDmaDscrChainDestroy
 */
extern CyU3PReturnStatus_t
CyU3PDmaDscrChainCreate (
        uint16_t *dscrIndex_p,          /* Output parameter that specifies the index of the head descriptor in the
                                           chain. */
        uint16_t count,                 /* Number of descriptors required in the chain. */
        uint16_t bufferSize,            /* Size of the buffers to be associated with each descriptor. If set to
                                           zero, no buffers will be allocated. */
        uint32_t dscrSync               /* The sync field to be set in all descriptors. Specifies the producer
                                           and consumer socket information. */
        );

/* Summary
   Frees the previously created chain of descriptors.

   Description
   The function fress the chain of descriptors. This function must be invoked
   only after suspending or disabling the sockets. The buffers pointed to by
   the descriptors can be optionally freed. This function is used by the DMA
   channel APIs. These can be used to customize descriptors and do advanced
   DMA operations.

   Return value
   * None

   See Also
   * CyU3PDmaDscrChainDestroy
 */
void
CyU3PDmaDscrChainDestroy (
        uint16_t dscrIndex,             /* Index of the head descriptor in the chain. */
        uint16_t count,                 /* Number of descriptors in the chain. */
        CyBool_t isProdChain,           /* Specifies whether to traverse the producer chain or the consumer chain
                                           to get the next descriptor. */
        CyBool_t freeBuffer             /* Whether the DMA buffers associated with the descriptors should be freed. */
        );

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3DESCRIPTOR_H_ */

/*[]*/
