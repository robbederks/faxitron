/*
 ## Cypress USB 3.0 Platform header file (cyu3socket.h)
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

#ifndef _INCLUDED_CYU3SOCKET_H_
#define _INCLUDED_CYU3SOCKET_H_

#include "cyu3types.h"
#include "cyu3descriptor.h"
#include "cyu3externcstart.h"

/**************************************************************************
 ******************************* Data types *******************************
 **************************************************************************/

/* Summary
   DMA IP block IDs.

   Description
   Each IP block has an ID which is required in identifying
   its DMA sockets. The socket ID is made of the block ID and
   the socket number.

   See Also
   * CyU3PDmaSocketId_t
 */
typedef enum CyU3PBlockId_t
{
    CY_U3P_LPP_IP_BLOCK_ID = 0,         /* LPP block contains UART, I2C, I2S and SPI. */
    CY_U3P_PIB_IP_BLOCK_ID = 1,         /* P-port interface block. */
    CY_U3P_RES_IP_BLOCK_ID = 2,         /* Reserved block ID. */
    CY_U3P_UIB_IP_BLOCK_ID = 3,         /* USB interface block for egress sockets. */
    CY_U3P_UIBIN_IP_BLOCK_ID = 4,       /* USB interface block for ingress sockets. */
    CY_U3P_NUM_IP_BLOCK_ID = 5,         /* Sentinel value. Count of valid DMA IPs. */
    CY_U3P_CPU_IP_BLOCK_ID = 0x3F       /* Special value used to denote CPU as DMA endpoint. */
} CyU3PBlockId_t;

/* Summary
   DMA socket register structure.

   Description
   Each hardware block on the FX3 device implements a number of DMA sockets through
   which it handles data transfers with the external world. Each DMA socket serves as
   an endpoint for an independent data stream going through the hardware block.
   
   Each socket has a set of registers associated with it that reflect the configuration
   and status information for that socket. The CyU3PDmaSocket structure is a replica
   of the config/status registers for a socket and is designed to perform socket configuration
   and status checks directly from firmware.

   See the sock_regs.h header file for the definitions of the fields that make up each
   of these registers.

   See Also
   * CyU3PDmaSocketConfig_t
 */
typedef struct CyU3PDmaSocket_t
{
    uvint32_t dscrChain;               /* The descriptor chain associated with the socket */
    uvint32_t xferSize;                /* The transfer size requested for this socket. The size can
                                          be specified in bytes or in terms of number of buffers,
                                          depending on the UNIT (bit no. 29) field in the status value. */
    uvint32_t xferCount;               /* The completed transfer count for this socket. The count is
                                          also specified in bytes or in terms of number of buffers
                                          depending on the UNIT field in the status value. */
    uvint32_t status;                  /* Socket configuration and status register. */
    uvint32_t intr;                    /* Interrupt status register. */
    uvint32_t intrMask;                /* Interrupt mask register. */
    uvint32_t unused2[2];              /* Reserved register space. */
    CyU3PDmaDescriptor_t activeDscr;   /* Active descriptor information. See cyu3descriptor.h for definition. */
    uvint32_t unused19[19];            /* Reserved register space. */
    uvint32_t sckEvent;                /* Generate event register. */
} CyU3PDmaSocket_t;

/* Summary
   DMA socket configuration structure.

   Description
   DMA socket structure represents all DMA ingress and egress transactions.
   This structure should not be modified by outside code.

   The following members are composed of the corresponding fields.
   The fields are defined in sock_regs.h. Refer to sock_regs.h for
   more details about these fields.

   dscrChain:
        (CY_U3P_DSCR_LOW_MASK | CY_U3P_DSCR_COUNT_MASK | CY_U3P_DSCR_NUMBER_MASK)

   xferSize:
        (CY_U3P_TRANS_SIZE_MASK)

   xferCount:
        (CY_U3P_TRANS_COUNT_MASK)

   status:
        (CY_U3P_GO_ENABLE | CY_U3P_GO_SUSPEND | CY_U3P_UNIT | CY_U3P_WRAPUP |
        CY_U3P_SUSP_EOP | CY_U3P_SUSP_TRANS | CY_U3P_SUSP_LAST | CY_U3P_SUSP_PARTIAL |
        CY_U3P_EN_CONS_EVENTS | CY_U3P_EN_PROD_EVENTS | CY_U3P_TRUNCATE |
        CY_U3P_ENABLED | CY_U3P_SUSPENDED | CY_U3P_ZLP_RCVD | CY_U3P_STATE_MASK |
        CY_U3P_AVL_ENABLE | CY_U3P_AVL_MIN_MASK | CY_U3P_AVL_COUNT_MASK)

   intr:
        (CY_U3P_LAST_BUF | CY_U3P_PARTIAL_BUF | CY_U3P_TRANS_DONE | CY_U3P_ERROR |
        CY_U3P_SUSPEND | CY_U3P_STALL | CY_U3P_DSCR_NOT_AVL | CY_U3P_DSCR_IS_LOW |
        CY_U3P_CONSUME_EVENT | CY_U3P_PRODUCE_EVENT)

   intrMask:
        (CY_U3P_LAST_BUF | CY_U3P_PARTIAL_BUF | CY_U3P_TRANS_DONE | CY_U3P_ERROR |
        CY_U3P_SUSPEND | CY_U3P_STALL | CY_U3P_DSCR_NOT_AVL | CY_U3P_DSCR_IS_LOW |
        CY_U3P_CONSUME_EVENT | CY_U3P_PRODUCE_EVENT)

   See Also
   * CyU3PDmaSocket_t
   * CyU3PDmaSocketSetConfig
   * CyU3PDmaSocketGetConfig
 */
typedef struct CyU3PDmaSocketConfig_t
{
    uint32_t dscrChain; /* The descriptor chain associateed with the socket     */
    uint32_t xferSize;  /* Transfer size for the socket.                        */
    uint32_t xferCount; /* Transfer status for the socket.                      */
    uint32_t status;    /* Socket status register.                              */
    uint32_t intr;      /* Interrupt status.                                    */
    uint32_t intrMask;  /* Interrupt mask.                                      */
} CyU3PDmaSocketConfig_t;

/* Summary
   DMA socket interrupt handler callback function.

   Description
   When a custom DMA interface needs to be built, the channel interface cannot
   be used and CyU3PDmaSocketRegisterCallback function needs to be invoked to set this.
   The function returns the interrupt status of the socket triggering the interrupt.
 */
typedef void (*CyU3PDmaSocketCallback_t) (
        uint16_t sckId,         /* ID of the socket triggering the interrupt. */
        uint32_t status         /* Socket interrupt status. */
        );

/**************************************************************************
 ********************************* Macros *********************************
 **************************************************************************/

/* The socket number range for the different IP blocks. */

#define CY_U3P_DMA_LPP_MIN_CONS_SCK     (0)    /* The min consumer socket number for Serial IOs. */
#define CY_U3P_DMA_LPP_MAX_CONS_SCK     (4)    /* The max consumer socket number for Serial IOs. */
#define CY_U3P_DMA_LPP_MIN_PROD_SCK     (5)    /* The min producer socket number for Serial IOs. */
#define CY_U3P_DMA_LPP_MAX_PROD_SCK     (7)    /* The max producer socket number for Serial IOs. */
#define CY_U3P_DMA_LPP_NUM_SCK          (8)    /* The number of sockets for Serial IOs. */

#define CY_U3P_DMA_PIB_MIN_CONS_SCK     (0)    /* The min consumer socket number for GPIF-II. */
#define CY_U3P_DMA_PIB_MAX_CONS_SCK     (15)   /* The max consumer socket number for GPIF-II. */
#define CY_U3P_DMA_PIB_MIN_PROD_SCK     (0)    /* The min producer socket number for GPIF-II. */
#define CY_U3P_DMA_PIB_MAX_PROD_SCK     (31)   /* The max producer socket number for GPIF-II. */
#define CY_U3P_DMA_PIB_NUM_SCK          (32)   /* The number of sockets for GPIF-II. */

#define CY_U3P_DMA_UIB_MIN_CONS_SCK     (0)    /* The min consumer socket number for USB egress EPs. */
#define CY_U3P_DMA_UIB_MAX_CONS_SCK     (15)   /* The max consumer socket number for USB egress EPs. */
#define CY_U3P_DMA_UIB_NUM_SCK          (16)   /* The number of sockets for USB egress EPs. */

#define CY_U3P_DMA_UIBIN_MIN_PROD_SCK   (0)    /* The min producer socket number for USB ingress EPs. */
#define CY_U3P_DMA_UIBIN_MAX_PROD_SCK   (15)   /* The max producer socket number for USB ingress EPs. */
#define CY_U3P_DMA_UIBIN_NUM_SCK        (16)   /* The number of sockets for USB ingress EPs. */

#define CY_U3P_DMA_CPU_NUM_SCK          (2)    /* The number of sockets for CPU. */

#define CY_U3P_IP_BLOCK_POS     (8)       /* The ip block position. */
#define CY_U3P_IP_BLOCK_MASK    (0x3F)    /* The ip block mask. */
#define CY_U3P_DMA_SCK_MASK     (0xFF)    /* The DMA socket mask. */
#define CY_U3P_DMA_SCK_ID_MASK  (0x3FFF)  /* The DMA socket id mask. */

/* Get the socket number from the socket ID. */
#define CyU3PDmaGetSckNum(sckId) ((sckId) & CY_U3P_DMA_SCK_MASK)

/* Get the ip number from the socket ID. */
#define CyU3PDmaGetIpNum(sckId) \
    (((sckId) >> CY_U3P_IP_BLOCK_POS) & CY_U3P_IP_BLOCK_MASK)

/* Get the socket ID from the socket number and the ip number. */
#define CyU3PDmaGetSckId(ipNum,sckNum) (((sckNum) & CY_U3P_DMA_SCK_MASK) | \
        (((ipNum) & CY_U3P_IP_BLOCK_MASK) << CY_U3P_IP_BLOCK_POS))

/**************************************************************************
 ********************** Global variable declarations **********************
 **************************************************************************/

extern CyU3PDmaSocket_t *glDmaSocket []; /* Array of all sockets on the device */
extern CyU3PDmaSocketCallback_t glDmaSocketCB; /* The registered socket interrupt handler. */

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/*@@Socket Functions
   This section documents the functions that operate on the DMA sockets within
   the FX3 device. These functions are only for use of the DMA manager itself,
   and are not expected to be called directly by user applications.

   If an application chooses to call these functions, extreme care must be taken
   to validate the parameters being passed as these functions do not perform 
   any error checks. Passing incorrect/invalid parameters can result in
   unpredictable behavior.

   In particular, the Socket ID passed to these functions has to be validated by
   either of CyU3PDmaSocketIsValid(), CyU3PDmaSocketIsValidProducer() OR
   CyU3PDmaSocketIsValidConsumer() functions.
*/


/* Summary
   Validates the socket ID.

   Description
   The function validates if the given socket id exists on the device.
   CPU sockets are virtual sockets and so the function considers them as
   invalid. The function considers a socket invalid if the block
   itself is not started.

   Return value
   * CyTrue if the socket is valid, and CyFalse otherwise.

   See Also
   * CyU3PDmaSocketIsValidProducer
   * CyU3PDmaSocketIsValidConsumer
 */
CyBool_t
CyU3PDmaSocketIsValid (
        uint16_t sckId                  /* Socket id to be validated. */
        );

/* Summary
   Validates the socket ID as a producer.

   Description
   The function validates if the given socket id exists as a producer
   on the device. CPU sockets are virtual sockets and so the function
   considers them as invalid. The function considers a socket invalid
   if the block itself is not started.

   Return value
   * CyTrue if the socket is a valid producer, and CyFalse otherwise.

   See Also
   * CyU3PDmaSocketIsValid
   * CyU3PDmaSocketIsValidConsumer
 */
CyBool_t
CyU3PDmaSocketIsValidProducer (
        uint16_t sckId                  /* Socket id to be validated. */
        );

/* Summary
   Validates the socket ID as a producer.

   Description
   The function validates if the given socket id exists as a consumer
   on the device. CPU sockets are virtual sockets and so the function
   considers them as invalid. The function considers a socket invalid
   if the block itself is not started.

   Return value
   * CyTrue if the socket is a valid consumer, and CyFalse otherwise.

   See Also
   * CyU3PDmaSocketIsValid
   * CyU3PDmaSocketIsValidProducer
 */
CyBool_t
CyU3PDmaSocketIsValidConsumer (
        uint16_t sckId                  /* Socket id to be validated. */
        );

/* Summary
   Sets the socket configuration.

   Description
   Sets the socket configuration.
   Socket structures must not be set directly and must be done only via APIs.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_BAD_ARGUMENT - if the sck_p pointer in NULL

   See Also
   * CyU3PDmaSocketGetConfig
 */
CyU3PReturnStatus_t
CyU3PDmaSocketSetConfig (
        uint16_t sckId,                 /* Socket id whose configuration is to be updated. */
        CyU3PDmaSocketConfig_t *sck_p   /* Pointer to structure containing socket configuration. */
        );

/* Summary
   Gets the socket configuration.

   Description
   Creates a copy of the socket structure and returns the current socket configuration.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_BAD_ARGUMENT - if the sck_p pointer in NULL

   See Also
   * CyU3PDmaSocketSetConfig
 */
CyU3PReturnStatus_t
CyU3PDmaSocketGetConfig (
        uint16_t sckId,                 /* Socket id whose configuration is to be retrieved. */
        CyU3PDmaSocketConfig_t *sck_p   /* Output parameter to be filled with the socket configuration. */
        );

/* Summary
   Sets the wrapup bit for the socket.

   Description
   The function wraps up the active buffer of the socket.
   The wrapup is set only if the socket is active. The function does
   not wait for the socket to wrap up. Data may be lost in the process.

   Return value
   * None

   See Also
   * CyU3PDmaSocketEnable
   * CyU3PDmaSocketDisable
 */
void
CyU3PDmaSocketSetWrapUp (
        uint16_t sckId                  /* Id of socket to be wrapped up. */
        );

/* Summary
   Disables the selected socket.

   Description
   The function disables the socket and returns only after the socket has been
   disabled. Data may be lost in the process.

   Return value
   * None

   See Also
   * CyU3PDmaSocketEnable
 */
void
CyU3PDmaSocketDisable (
        uint16_t sckId                  /* Id of socket to be disabled. */
        );

/* Summary
   Enables the selected socket.

   Description
   The socket shall start functioning immediately and the descriptors
   are expected to be setup previously.

   Return value
   * None

   See Also
   * CyU3PDmaSocketDisable
 */
void
CyU3PDmaSocketEnable (
        uint16_t sckId                  /* Id of socket to be enabled. */
        );

/* Summary
   Updates the options for the socket suspend.

   Description
   This API does not suspend the socket. It only updates
   the suspend conditions.

   Return value
   * None

   See Also
   * CyU3PDmaUpdateSocketResume
 */
void
CyU3PDmaUpdateSocketSuspendOption (
        uint16_t sckId,                 /* Id of socket to set the option. */
        uint16_t suspendOption          /* Suspend option. */
        );

/* Summary
   Resumes a socket from the suspended state.

   Description
   This function clears all suspend interrupts 
   and resumes the sockets. It also sets
   the interrupt masks as per the suspend option specified.

   Return value
   * None

   See Also
   * CyU3PDmaUpdateSocketSuspendOption
 */
void
CyU3PDmaUpdateSocketResume (
        uint16_t sckId,                 /* Id of socket to set the option. */
        uint16_t suspendOption          /* Suspend option. */
        );

/* Summary
   Send an event to the socket.

   Description
   Send a produce / consume event to the selected socket.

   Return value
   * None

   See Also
   * CyU3PDmaSocketResume
 */
void
CyU3PDmaSocketSendEvent (
        uint16_t sckId,                 /* Id of socket that should receive the event. */
        uint16_t dscrIndex,             /* The descriptor index to associate with the event. */
        CyBool_t isOccupied             /* Status of the buffer associated with the event. */
        );

/* Summary
   Register a callback handler for custom socket interrupts.

   Description
   The handler needs to be registered only if there are custom DMA
   operations built on socket APIs.

   Return value
   * None

   See Also
   * CyU3PDmaSocketCallback.
 */
void
CyU3PDmaSocketRegisterCallback (
        CyU3PDmaSocketCallback_t cb       /* Callback function pointer. Can be zero to unregister previous callback. */
        );


#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3SOCKET_H_ */

/*[]*/
