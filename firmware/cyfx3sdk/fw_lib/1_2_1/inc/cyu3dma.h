/*
 ## Cypress USB 3.0 Platform header file (cyu3dma.h)
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

#ifndef _INCLUDED_CYU3DMA_H_
#define _INCLUDED_CYU3DMA_H_

#include "cyu3os.h"
#include "cyu3externcstart.h"

/**************************************************************************
 ******************************* Data types *******************************
 **************************************************************************/

/*@@DMA Data Types
   This section documents the data types that are defined and used as part
   of the DMA manager module.
*/

/* Summary
   DMA socket IDs for all sockets in the device.

   Description
   This is a software representation of all sockets on the device.
   The socket ID has two parts IP number and socket number. Each
   peripheral (IP) has a fixed ID. LPP is 0, PIB is 1 USB egress
   is 3 and USB ingress is 4.
   
   Each peripheral has a number of sockets. The LPP sockets are fixed
   and has to be used as defined. The PIB sockets 0-15 can be used as
   both producer and consumer but the PIB sockets 16-31 are strictly
   producer sockets. The UIB sockets are defined as 0-15 producer and
   0-15 consumer sockets. The CPU sockets are virtual representations
   and cannot be used for CPU-CPU transfers.

   See Also
   * CyU3PDmaChannelConfig_t
   * CyU3PDmaMultiChannelConfig_t
   * CyU3PDmaChannelCreate
   * CyU3PDmaMultiChannelCreate
 */
typedef enum CyU3PDmaSocketId_t
{
    CY_U3P_LPP_SOCKET_I2S_LEFT = 0x0000,        /* Left channel output to I2S port. */
    CY_U3P_LPP_SOCKET_I2S_RIGHT,                /* Right channel output to I2S port. */
    CY_U3P_LPP_SOCKET_I2C_CONS,                 /* Outgoing data to I2C slave. */
    CY_U3P_LPP_SOCKET_UART_CONS,                /* Outgoing data to UART peer. */
    CY_U3P_LPP_SOCKET_SPI_CONS,                 /* Outgoing data to SPI slave. */
    CY_U3P_LPP_SOCKET_I2C_PROD,                 /* Incoming data from I2C slave. */
    CY_U3P_LPP_SOCKET_UART_PROD,                /* Incoming data from UART peer. */
    CY_U3P_LPP_SOCKET_SPI_PROD,                 /* Incoming data from SPI slave. */


    CY_U3P_PIB_SOCKET_0 = 0x0100,               /* P-port socket number 0. */
    CY_U3P_PIB_SOCKET_1,                        /* P-port socket number 1. */
    CY_U3P_PIB_SOCKET_2,                        /* P-port socket number 2. */
    CY_U3P_PIB_SOCKET_3,                        /* P-port socket number 3. */
    CY_U3P_PIB_SOCKET_4,                        /* P-port socket number 4. */
    CY_U3P_PIB_SOCKET_5,                        /* P-port socket number 5. */
    CY_U3P_PIB_SOCKET_6,                        /* P-port socket number 6. */
    CY_U3P_PIB_SOCKET_7,                        /* P-port socket number 7. */
    CY_U3P_PIB_SOCKET_8,                        /* P-port socket number 8. */
    CY_U3P_PIB_SOCKET_9,                        /* P-port socket number 9. */
    CY_U3P_PIB_SOCKET_10,                       /* P-port socket number 10. */
    CY_U3P_PIB_SOCKET_11,                       /* P-port socket number 11. */
    CY_U3P_PIB_SOCKET_12,                       /* P-port socket number 12. */
    CY_U3P_PIB_SOCKET_13,                       /* P-port socket number 13. */
    CY_U3P_PIB_SOCKET_14,                       /* P-port socket number 14. */
    CY_U3P_PIB_SOCKET_15,                       /* P-port socket number 15. */
    CY_U3P_PIB_SOCKET_16,                       /* P-port socket number 16. */
    CY_U3P_PIB_SOCKET_17,                       /* P-port socket number 17. */
    CY_U3P_PIB_SOCKET_18,                       /* P-port socket number 18. */
    CY_U3P_PIB_SOCKET_19,                       /* P-port socket number 19. */
    CY_U3P_PIB_SOCKET_20,                       /* P-port socket number 20. */
    CY_U3P_PIB_SOCKET_21,                       /* P-port socket number 21. */
    CY_U3P_PIB_SOCKET_22,                       /* P-port socket number 22. */
    CY_U3P_PIB_SOCKET_23,                       /* P-port socket number 23. */
    CY_U3P_PIB_SOCKET_24,                       /* P-port socket number 24. */
    CY_U3P_PIB_SOCKET_25,                       /* P-port socket number 25. */
    CY_U3P_PIB_SOCKET_26,                       /* P-port socket number 26. */
    CY_U3P_PIB_SOCKET_27,                       /* P-port socket number 27. */
    CY_U3P_PIB_SOCKET_28,                       /* P-port socket number 28. */
    CY_U3P_PIB_SOCKET_29,                       /* P-port socket number 29. */
    CY_U3P_PIB_SOCKET_30,                       /* P-port socket number 30. */
    CY_U3P_PIB_SOCKET_31,                       /* P-port socket number 31. */


    CY_U3P_UIB_SOCKET_CONS_0 = 0x0300,          /* U-port output socket number 0. */
    CY_U3P_UIB_SOCKET_CONS_1,                   /* U-port output socket number 1. */
    CY_U3P_UIB_SOCKET_CONS_2,                   /* U-port output socket number 2. */
    CY_U3P_UIB_SOCKET_CONS_3,                   /* U-port output socket number 3. */
    CY_U3P_UIB_SOCKET_CONS_4,                   /* U-port output socket number 4. */
    CY_U3P_UIB_SOCKET_CONS_5,                   /* U-port output socket number 5. */
    CY_U3P_UIB_SOCKET_CONS_6,                   /* U-port output socket number 6. */
    CY_U3P_UIB_SOCKET_CONS_7,                   /* U-port output socket number 7. */
    CY_U3P_UIB_SOCKET_CONS_8,                   /* U-port output socket number 8. */
    CY_U3P_UIB_SOCKET_CONS_9,                   /* U-port output socket number 9. */
    CY_U3P_UIB_SOCKET_CONS_10,                  /* U-port output socket number 10. */
    CY_U3P_UIB_SOCKET_CONS_11,                  /* U-port output socket number 11. */
    CY_U3P_UIB_SOCKET_CONS_12,                  /* U-port output socket number 12. */
    CY_U3P_UIB_SOCKET_CONS_13,                  /* U-port output socket number 13. */
    CY_U3P_UIB_SOCKET_CONS_14,                  /* U-port output socket number 14. */
    CY_U3P_UIB_SOCKET_CONS_15,                  /* U-port output socket number 15. */


    CY_U3P_UIB_SOCKET_PROD_0 = 0x400,           /* U-port input socket number 0. */
    CY_U3P_UIB_SOCKET_PROD_1,                   /* U-port input socket number 1. */
    CY_U3P_UIB_SOCKET_PROD_2,                   /* U-port input socket number 2. */
    CY_U3P_UIB_SOCKET_PROD_3,                   /* U-port input socket number 3. */
    CY_U3P_UIB_SOCKET_PROD_4,                   /* U-port input socket number 4. */
    CY_U3P_UIB_SOCKET_PROD_5,                   /* U-port input socket number 5. */
    CY_U3P_UIB_SOCKET_PROD_6,                   /* U-port input socket number 6. */
    CY_U3P_UIB_SOCKET_PROD_7,                   /* U-port input socket number 7. */
    CY_U3P_UIB_SOCKET_PROD_8,                   /* U-port input socket number 8. */
    CY_U3P_UIB_SOCKET_PROD_9,                   /* U-port input socket number 9. */
    CY_U3P_UIB_SOCKET_PROD_10,                  /* U-port input socket number 10. */
    CY_U3P_UIB_SOCKET_PROD_11,                  /* U-port input socket number 11. */
    CY_U3P_UIB_SOCKET_PROD_12,                  /* U-port input socket number 12. */
    CY_U3P_UIB_SOCKET_PROD_13,                  /* U-port input socket number 13. */
    CY_U3P_UIB_SOCKET_PROD_14,                  /* U-port input socket number 14. */
    CY_U3P_UIB_SOCKET_PROD_15,                  /* U-port input socket number 15. */


    CY_U3P_CPU_SOCKET_CONS = 0x3F00,            /* Socket through which the FX3 CPU receives data. */
    CY_U3P_CPU_SOCKET_PROD                      /* Socket through which the FX3 CPU produces data. */

} CyU3PDmaSocketId_t;

/* Summary
   Various DMA single channel types.

   Description
   A DMA channel needs to be created before any DMA operation
   can be made. The following are the different types of DMA
   single (one to one) channels. The single channel types are
   the simple cases and all APIs for this has a CyU3PDmaChannel
   prefix.

   See Also
   * CyU3PDmaChannelCreate
 */
typedef enum CyU3PDmaType_t
{
    CY_U3P_DMA_TYPE_AUTO = 0,           /* Auto mode DMA channel. */
    CY_U3P_DMA_TYPE_AUTO_SIGNAL,        /* Auto mode with produce event signalling. */
    CY_U3P_DMA_TYPE_MANUAL,             /* Manual mode DMA channel. */
    CY_U3P_DMA_TYPE_MANUAL_IN,          /* Manual mode producer socket to CPU. */
    CY_U3P_DMA_TYPE_MANUAL_OUT,         /* Manual mode CPU to consumer socket. */
    CY_U3P_DMA_NUM_SINGLE_TYPES         /* Number of single DMA channel types. */

} CyU3PDmaType_t;

/* Summary
   Various DMA multichannel types.

   Description
   A DMA channel needs to be created before any DMA operation
   can be made. The following are the different types of DMA
   multichannels. The multiple socket channel types are special
   cases and separate create calls must be made for this.
   All APIs for multi socket channels are different and has the
   CyU3PDmaMultiChannel prefix.

   See Also
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaEnableMulticast
 */
typedef enum CyU3PDmaMultiType_t
{
    CY_U3P_DMA_TYPE_AUTO_MANY_TO_ONE = (CY_U3P_DMA_NUM_SINGLE_TYPES), /* Auto mode many to one interleaved DMA channel. */
    CY_U3P_DMA_TYPE_AUTO_ONE_TO_MANY,   /* Auto mode one to many interleaved DMA channel. */
    CY_U3P_DMA_TYPE_MANUAL_MANY_TO_ONE, /* Manual mode many to one  interleaved DMA channel. */
    CY_U3P_DMA_TYPE_MANUAL_ONE_TO_MANY, /* Manual mode one to many interleaved DMA channel. */
    CY_U3P_DMA_TYPE_MULTICAST,          /* Multicast mode with one producer and multiple consumers for the same
                                           buffer. This is a manual channel. Please note that the
                                           CyU3PDmaEnableMulticast function needs to be called before any
                                           multicast DMA channels are created. */
    CY_U3P_DMA_NUM_TYPES                /* Number of DMA channel types. */
} CyU3PDmaMultiType_t;

/* Summary
   Different DMA channel states.

   Description
   The following are the different states that a DMA channel can be in.
   All states until CY_U3P_DMA_NUM_STATES are channel states whereas
   those after it are virual state values returned on the GetStatus calls.

   See Also
   * CyU3PDmaChannelGetStatus
 */
typedef enum CyU3PDmaState_t
{
    CY_U3P_DMA_NOT_CONFIGURED = 0, /* DMA channel is unconfigured. This state occurs only 
                                      when using stale channel structure. This channel state
                                      is set to this when a channel is destroyed. */
    CY_U3P_DMA_CONFIGURED,         /* DMA channel has been configured successfully. The channel
                                      reaches this state through the following conditions:
                                      1. Channel is successfully created.
                                      2. Channel is reset.
                                      3. A finite transfer has been successfully completed.
                                         A GetStatus call in this case will return a virtual
                                         CY_U3P_DMA_XFER_COMPLETED state.
                                      4. One of the override modes have completed successfully.
                                         A GetStatus call in this case will return a virtual
                                         CY_U3P_DMA_SEND_COMPLETED or CY_U3P_DMA_RECV_COMPLETED state.
                                      */
    CY_U3P_DMA_ACTIVE,             /* Channel has active transaction going on.
                                      This state is reached when a SetXfer call is invoked and the
                                      transfer is on-going. */
    CY_U3P_DMA_PROD_OVERRIDE,      /* The channel is working in producer socket override mode.
                                      This state is reached when a SetupSend call is invoked and the
                                      transfer is on-going. */
    CY_U3P_DMA_CONS_OVERRIDE,      /* Channel is working in consumer socket override mode.
                                      This state is reached when a SetupRecv call is invoked and the
                                      transfer is on-going. */
    CY_U3P_DMA_ERROR,              /* Channel has encountered an error. This state is reached when
                                      a DMA hardware error is detected. */
    CY_U3P_DMA_IN_COMPLETION,      /* Waiting for all data to drain out. This state is reached when
                                      transfer has completed from the producer side, but waiting for
                                      the consumer to drain all data. */
    CY_U3P_DMA_ABORTED,            /* The channel is in aborted state. This state is reached when a
                                      Abort call is made. */
    CY_U3P_DMA_NUM_STATES,         /* Number of states. This is not a valid state. This is just a 
                                      place holder. */
    CY_U3P_DMA_XFER_COMPLETED,     /* This is virtual state returned by GetStatus function. 
                                      The actual state is CY_U3P_DMA_CONFIGURED. This is just the value
                                      returned on GetStatus call after completion of finite transfer. */
    CY_U3P_DMA_SEND_COMPLETED,     /* This is virtual state returned by GetStatus function.
                                      The actual state is CY_U3P_DMA_CONFIGURED. This is just the value
                                      returned on GetStatus call after completion of producer override mode. */
    CY_U3P_DMA_RECV_COMPLETED      /* This is virtual state returned by GetStatus function.
                                      The actual state is CY_U3P_DMA_CONFIGURED. This is just the value
                                      returned on GetStatus call after completion of consumer override mode. */
} CyU3PDmaState_t;

/* Summary
   Different DMA transfer modes.

   Description
   The following are the different types of DMA transfer modes.
   The default mode of operation is byte mode. The buffer mode
   is useful only when there are variable data sized transfers
   and when firmware handling is required after a finite number
   of buffers.

   See Also
   * CyU3PDmaChannelConfig_t
   * CyU3PDmaMultiChannelConfig_t
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaMultiChannelUpdateMode
 */
typedef enum CyU3PDmaMode_t
{
    CY_U3P_DMA_MODE_BYTE = 0, /* Transfer is based on byte count. This is the default mode of operation.
                                 The transfer count is done based on number of bytes received / sent. */
    CY_U3P_DMA_MODE_BUFFER,   /* Transfer is based on buffer count. The transfer count is based on the 
                                 number of buffers generated or consumed. This is useful only when the
                                 data size is variable but there should be finite handling after N buffers. */
    CY_U3P_DMA_NUM_MODES      /* Count of DMA modes. This is just a place holder and not a valid mode. */
} CyU3PDmaMode_t;

/* Summary
   DMA channel callback input values.

   Description
   These are the parameters with which a DMA channel callback will be made.
   This is used to identify the DMA event. This is also used to generate
   the notification bit mask used for DMA channel configuration.

   See Also
   * CyU3PDmaCallback_t
   * CyU3PDmaChannelConfig_t
   * CyU3PDmaMultiChannelConfig_t
   * CyU3PDmaChannelCreate
   * CyU3PDmaMultiChannelCreate
 */
typedef enum CyU3PDmaCbType_t
{
    CY_U3P_DMA_CB_XFER_CPLT  = (1 << 0), /* Transfer has been completed. This event is generated when
                                            a finite transfer queued with SetXfer is completed. */
    CY_U3P_DMA_CB_SEND_CPLT  = (1 << 1), /* SendBuffer call has been completed. This event is generated
                                            when the data queued with SendBuffer has been successfully sent. */
    CY_U3P_DMA_CB_RECV_CPLT  = (1 << 2), /* ReceiverBuffer call has been completed. This event is generated
                                            when data is received successfully on the producer socket. */
    CY_U3P_DMA_CB_PROD_EVENT = (1 << 3), /* Buffer received from producer. This event is generated when
                                            a buffer is generated by the producer socket when a transfer
                                            is queued with SetXfer. */
    CY_U3P_DMA_CB_CONS_EVENT = (1 << 4), /* Buffer consumed by the consumer. This event is generated when
                                            a buffer is sent out by the consumer socket when a transfer
                                            is queued with SetXfer. */
    CY_U3P_DMA_CB_ABORTED    = (1 << 5), /* This event is generated when the Abort API is invoked. */
    CY_U3P_DMA_CB_ERROR      = (1 << 6), /* This event is generated when the hardware detects an error. */
    CY_U3P_DMA_CB_PROD_SUSP  = (1 << 7), /* This event is generated when the producer socket is suspended. */
    CY_U3P_DMA_CB_CONS_SUSP  = (1 << 8)  /* This event is generated when the consumer socket is suspended. */
} CyU3PDmaCbType_t;

/* Summary
   DMA socket suspend options.

   Description
   The producer and consume sockets can be suspended on various options.
   Each of the suspend option behaves differently.

   CY_U3P_DMA_SCK_SUSP_NONE
   No suspend option is applied. This is the default for all
   channels created.
   
   CY_U3P_DMA_SCK_SUSP_EOP
   This option will suspend the socket after the current buffer
   with end of packet signalling. The buffer will not be stopped.
   This can be used if EOP is rare and has special value. This cannot
   be used for making any data modifications. This option is sticky and
   will not change until explicitly changed.

   CY_U3P_DMA_SCK_SUSP_CUR_BUF
   This option is used to suspend the socket after the current
   buffer handling is completed. The buffer cannot be stopped or
   modified. This can be used to suspend the socket at a defined
   point to be resumed later. This option is is valid for only the
   current buffer and the socket option will change back to 
   CY_U3P_DMA_SCK_SUSP_NONE on resume.

   CY_U3P_DMA_SCK_SUSP_CONS_PARTIAL_BUF
   This option is valid only for consumer socket. The socket will
   be suspended before the data is transferred. This allows for
   data manipulation. This allows only very restricted access to buffer.

   In case of multi channels, only the single socket side can be suspended.
   For a many to one channel, the producer sockets cannot be suspended
   and for one to many channel, the consumer sockets cannot be suspended.

   See Also
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaMultiChannelCreate
 */
typedef enum CyU3PDmaSckSuspType_t
{
    CY_U3P_DMA_SCK_SUSP_NONE = 0,        /* Socket will not be suspended. */
    CY_U3P_DMA_SCK_SUSP_EOP,             /* Socket will be suspended after handling buffer with EOP. */ 
    CY_U3P_DMA_SCK_SUSP_CUR_BUF,         /* Socket will be suspended after the current buffer is completed. */
    CY_U3P_DMA_SCK_SUSP_CONS_PARTIAL_BUF /* Consumer socket will be suspended before handling a partial buffer. */
} CyU3PDmaSckSuspType_t;

/* Summary
   DMA channel buffer data structure.

   Description
   The data structure is used to describe the status of a buffer.
   It holds the address, size, valid count, and the status information.
   It is used in callbacks and DMA APIs to idenitfy the data to be sent
   / received using DMA.

   See Also
   * CyU3PDmaCBInput_t
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForRecvBuffer
 */
typedef struct CyU3PDmaBuffer_t
{
    uint8_t *buffer;    /* Pointer to the buffer                */
    uint16_t count;     /* Byte count of valid data in buffer   */
    uint16_t size;      /* Buffer size                          */
    uint16_t status;    /* Buffer status. This is a four bit data field defined by 
                           CY_U3P_DMA_BUFFER_STATUS_MASK. This holds information like
                           whether the buffer is occupied, whether the buffer holds the
                           end of packet and whether the buffer encountered a DMA error. */
} CyU3PDmaBuffer_t;

/* Summary
   DMA channel callback input.

   Description
   The callback function registered for each DMA channel shall
   be used for various notifications and the input values for
   each notifications shall be passed to the callback via this
   union.

   See Also
   * CyU3PDmaBuffer_t
   * CyU3PDmaCallback_t
 */
typedef union CyU3PDmaCBInput_t
{
    CyU3PDmaBuffer_t buffer_p; /* Buffer details. */
} CyU3PDmaCBInput_t;

/* Forward declaration to make compiler happy */
typedef struct CyU3PDmaChannel CyU3PDmaChannel;
typedef struct CyU3PDmaMultiChannel CyU3PDmaMultiChannel;

/* Summary
   DMA channel callback function.

   Description
   The callback function that has to be registered
   with every DMA channel. If no callback function is registered
   or the corresponding notification events are not registered,
   the callback will not be made.

   No blocking calls must be made from the callback functions.
   If data processing is required, it must be done outside
   of the callback function.

   In case of produce events, the application is expected to be
   fast enough to handle the incoming data rate. The input parameter can
   be stale if the handling of the buffer is not done in a timely fashion.
   In case of AUTO_SIGNAL channels, the input parameter points to
   the latest produced buffer. If the handling is delayed, the producer socket
   can potentially overwrite this buffer.

   In case of MANUAL or MANUAL_IN channels, the input parameter
   points to first buffer left to be committed to the consumer socket.
   If the buffer is not committed before the next callback, then the input
   parameter shall be stale data. If data processing has to be done in
   MANUAL / MANUAL_IN channels, the CyU3PDmaChannelGetBuffer function must be
   used and the callback function must be just used as a notification.

   See Also
   * CyU3PDmaCBInput_t
   * CyU3PDmaChannelConfig_t
   * CyU3PDmaChannelCreate
 */
typedef void (*CyU3PDmaCallback_t) (
        CyU3PDmaChannel *handle,        /* Handle to the DMA channel. */
        CyU3PDmaCbType_t type,          /* The type of callback notification being generated. */
        CyU3PDmaCBInput_t *input        /* Union that contains data related to the notification.
                                           The input parameter will be a pointer to a CyU3PDmaBuffer_t variable
                                           in the cases where the callback type is CY_U3P_DMA_CB_RECV_CPLT or
                                           CY_U3P_DMA_CB_PROD_EVENT. */
        );

/* Summary
   Multi socket DMA channel callback function.

   Description
   The callback function that has to be registered
   with every DMA channel. If no callback function is registered
   or the corresponding notification events are not registered,
   the callback will not be made.

   No blocking calls must be made from the callback functions.
   If data processing is required, it must be done outside
   of the callback function.

   In case of produce events, the application is expected to be
   fast enough. The input parameter can potentially be stale, if the
   handling of the buffer is not done in a timely fashion.

   In case of MANUAL channels, the input parameter
   points to first buffer left to be committed to the consumer socket.
   If the buffer is not committed before the next callback, then the input
   parameter shall be stale data. If data processing has to be done,
   the CyU3PDmaMultiChannelGetBuffer function must be used and the callback function
   must be just used as a notification.

   See Also
   * CyU3PDmaCBInput_t
   * CyU3PDmaMultiChannelConfig_t
   * CyU3PDmaMultiChannelCreate
 */
typedef void (*CyU3PDmaMultiCallback_t) (
        CyU3PDmaMultiChannel *handle,   /* Handle to the multi-socket DMA channel. */
        CyU3PDmaCbType_t type,          /* The callback type. */
        CyU3PDmaCBInput_t *input        /* Pointer to a union that contains the callback related data.
                                           Will point to a valid CyU3PDmaBuffer_t variable if the callback
                                           type is CY_U3P_DMA_CB_RECV_CPLT or CY_U3P_DMA_CB_PROD_EVENT. */
        );

/* Summary
   DMA channel parameter structure.

   Description
   This is the channel configuration structure which needs to
   be provided during channel initialization.

   The size field is the total buffer that needs to be allocated for
   DMA operations. This field has restrictions for DMA operations.

   The size of the buffer as seen by the producer socket should always be
   a multiple of 16 bytes; ie, (size - prodHeader - prodFooter) must be a multiple of 16 bytes.

   If the data cache is enabled, the buffers should be 32 byte aligned and a 
   multiple of 32 byte. This is because the cache line is 32 byte long.
   The buffers allocated by CyU3PDmaBufferAlloc function takes care of this.
   The size field need not be a multiple of 32 as the DMA API allocates 
   channel memory from using the CyU3PDmaBufferAlloc call.

   The offsets prodHeader, prodFooter and consHeader are used to
   do header addition and removal. These are valid only for manual channels,
   and should be zero for auto channels.

   The buffer address seen by the producer = (buffer + prodHeader).
   The buffer size seen by the producer    = (size - prodHeader - prodFooter).
   The buffer address seen by the consumer = (buffer + consHeader).
   The buffer size seen by the consumer    = (buffer - consHeader).

   For header addition to the buffer generated by the producer, the prodHeader should
   be the length of the header to be added and the other offsets should be zero.
   Once the buffer is generated, the header can be modified manually by the CPU
   and committed using CommitBuffer call.

   For footer addition to the buffer generated by the producer, the prodFooter should
   be the length of the footer to be added and the other offsets should be zero.
   Once the buffer is generated, the footer can be added and committed using the 
   CommitBuffer call.

   For header deletion from the buffer generated by the producer, the consHeader should
   be the length of the header to be removed and the other offsets should be zero.
   Once the buffer is generated, the buffer can be committed to the consumer socket
   with only change to the size of the data to be transmitted using the CommitBuffer call.

   More complex modes can be deviced by enabling more than one offset.

   The prodAvailCount count should always be zero. This is used only for very specific
   use case where there should always be free buffers. Since there is no current use case
   for such a channel, this field should always be zero.

   See Also
   * CyU3PDmaChannelCreate
 */
typedef struct CyU3PDmaChannelConfig_t
{
    uint16_t size;                /* The buffer size associated with the channel. */
    uint16_t count;               /* Number of buffers to be allocated for the channel. The count can
                                     be zero for MANUAL, MANUAL_OUT and MANUAL_IN channels if the channel
                                     is intended to operate only in override mode and no buffer need to be
                                     allocated for the channel. The count cannot be zero for AUTO and
                                     AUTO_SIGNAL channels. */
    CyU3PDmaSocketId_t prodSckId; /* The producer (ingress) socket ID. */
    CyU3PDmaSocketId_t consSckId; /* The consumer (egress) socket ID. */
    uint16_t prodAvailCount;      /* Minimum available empty buffers before producer is active. By default,
                                     this should be zero. A non-zero value will activate this feature.
                                     The producer socket will not receive data into memory until the specified
                                     number of free buffers are available. This feature should be used only for
                                     very specific use cases where there is a requirement that there should
                                     always be free buffers during the transfer. */
    uint16_t prodHeader;          /* The producer socket header offset. */
    uint16_t prodFooter;          /* The producer socket footer offset. */
    uint16_t consHeader;          /* The consumer socket header offset. */
    CyU3PDmaMode_t dmaMode;       /* Mode of DMA operation. */
    uint32_t notification;        /* Registered notifications. This is a bit map based on CyU3PDmaCbType_t.
                                     This defines the events for which the callback is triggered. */
    CyU3PDmaCallback_t cb;        /* Callback function which gets invoked on DMA events. */
} CyU3PDmaChannelConfig_t;

/* Macro defining the minimum required sockets for multi socket DMA channels.
   Since for single socket, single socket DMA channel can be used, the minimum
   number on multi side is 2.
 */
#define CY_U3P_DMA_MIN_MULTI_SCK_COUNT        (2)

/* Macro defining the maximum allowed socket for multi socket DMA channels.
   Since the PIB has only four threads, the maximum allowed
   socket on one side is four.
 */
#define CY_U3P_DMA_MAX_MULTI_SCK_COUNT        (4)

/* Summary
   Multi socket DMA channel parameter structure.

   Description
   This is the channel configuration structure which needs to
   be provided during multi socket channel initialization.

   In case of many to one operations, there shall be 'validSckCount'
   number of producer sockets and only one consumer socket.
   The producer sockets needs to be updated in the required order
   of operation. The first buffer shall be taken from the prodSckId[0]
   and second from prodSckId[1] and so on. If only two producer sockets are used,
   then only prodSckId[0], prodSckId[1] and consSckId[0] shall be considered.

   In case of one to many operations, there shall be only one producer socket
   and 'validSckCount' number of consumer sockets. The consumer sockets needs
   to be updated in the required order of operation. The first buffer shall be
   send to the consSckId[0] and second from consSckId[1] and so on.

   The size field is the total buffer that needs to be allocated for
   DMA operations. This field has restrictions for DMA operations.

   The size of the buffer as seen by the producer socket should always be
   a multiple of 16 bytes; ie, (size - prodHeader - prodFooter) must be a multiple of 16 bytes.

   If the data cache is enabled, the buffers should be 32 byte aligned and a 
   multiple of 32 byte. This is because the cache line is 32 byte long.
   The buffers allocated by CyU3PDmaBufferAlloc function takes care of this.
   The size field need not be a multiple of 32 as the DMA API allocates 
   channel memory from using the CyU3PDmaBufferAlloc call.

   The offsets prodHeader, prodFooter and consHeader are used to
   do header addition and removal. These are valid only for manual channels,
   and should be zero for auto channels.

   The buffer address seen by the producer = (buffer + prodHeader).
   The buffer size seen by the producer    = (size - prodHeader - prodFooter).
   The buffer address seen by the consumer = (buffer + consHeader).
   The buffer size seen by the consumer    = (buffer - consHeader).

   For header addition to the buffer generated by the producer, the prodHeader should
   be the length of the header to be added and the other offsets should be zero.
   Once the buffer is generated, the header can be modified manually by the CPU
   and committed using CommitBuffer call.

   For footer addition to the buffer generated by the producer, the prodFooter should
   be the length of the footer to be added and the other offsets should be zero.
   Once the buffer is generated, the footer can be added and committed using the 
   CommitBuffer call.

   For header deletion from the buffer generated by the producer, the consHeader should
   be the length of the header to be removed and the other offsets should be zero.
   Once the buffer is generated, the buffer can be committed to the consumer socket
   with only change to the size of the data to be transmitted using the CommitBuffer call.

   More complex modes can be deviced by enabling more than one offset.

   The prodAvailCount count should always be zero. This is used only for very specific
   use case where there should always be free buffers. Since there is no current use case
   for such a channel, this field should always be zero.
   
   See Also
   * CyU3PDmaMultiChannelCreate
*/
typedef struct CyU3PDmaMultiChannelConfig_t
{
    uint16_t size;              /* The buffer size associated with the channel. */
    uint16_t count;             /* Number of buffers to be allocated for each socket of the channel. 
                                   For one to many and many to one channels, there will be twice the
                                   number of buffers as specified in the count and for multicast it
                                   will have the same number of buffers as specified in count. The
                                   count cannot be zero. */
    uint16_t validSckCount;     /* Number of sockets in the multi-socket operation. */
    CyU3PDmaSocketId_t prodSckId[CY_U3P_DMA_MAX_MULTI_SCK_COUNT]; /* The producer (ingress) socket ID. */
    CyU3PDmaSocketId_t consSckId[CY_U3P_DMA_MAX_MULTI_SCK_COUNT]; /* The consumer (egress) socket ID. */
    uint16_t prodAvailCount;    /* Minimum available empty buffers before producer is active. By default,
                                   this should be zero. A non-zero value will activate this feature.
                                   The producer socket will not receive data into memory until the specified
                                   number of free buffers are available. This feature should be used only for
                                   very specific use cases where there is a requirement that there should
                                   always be free buffers during the transfer. */
    uint16_t prodHeader;        /* The producer socket header offset. */
    uint16_t prodFooter;        /* The producer socket footer offset. */
    uint16_t consHeader;        /* The consumer socket header offset. */
    CyU3PDmaMode_t dmaMode;     /* Mode of DMA operation */
    uint32_t notification;      /* Registered notifications. This is a bit map based on CyU3PDmaCbType_t.
                                   This defines the events for which the callback is triggered. */
    CyU3PDmaMultiCallback_t cb; /* Callback function which gets invoked on multi socket DMA events. */
} CyU3PDmaMultiChannelConfig_t;

/* Summary
   DMA channel structure.

   Description
   Every DMA channel shall have a channel structure associated with it for 
   storing info and status. This structure is updated and maintained by the
   library APIs and should not be modified by the application.

   See Also
   * CyU3PDmaChannelConfig_t
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
struct CyU3PDmaChannel
{
    uint32_t state;             /* The current state of the DMA channel */
    uint16_t type;              /* The type of the DMA channel */
    uint16_t size;              /* The buffer size associated with the channel*/
    uint16_t count;             /* Number of buffers for the channel */
    uint16_t prodAvailCount;    /* Minimum available buffers before producer is active. */
    uint16_t firstProdIndex;    /* Head for the normal descriptor chain list */
    uint16_t firstConsIndex;    /* Head for the manual mode consumer descriptor chain list */
    uint16_t prodSckId;         /* The producer (ingress) socket ID */
    uint16_t consSckId;         /* The consumer (egress) socket ID */
    uint16_t overrideDscrIndex; /* Internal data - Descriptor for override modes. */
    uint16_t currentProdIndex;  /* Internal data - Producer descriptor for the latest
                                   buffer produced. */
    uint16_t currentConsIndex;  /* Internal data - Consumer descriptor for the latest
                                   buffer produced. */
    uint16_t commitProdIndex;   /* Internal data - Producer descriptor for the buffer
                                   to be consumed. */
    uint16_t commitConsIndex;   /* Internal data - Consumer descriptor for the buffer
                                   to be consumed. */
    uint16_t activeProdIndex;   /* Internal data - Active producer descriptor. */
    uint16_t activeConsIndex;   /* Internal data - Active consumer descriptor. */
    uint16_t prodHeader;        /* The producer socket header offset */
    uint16_t prodFooter;        /* The producer socket footer offset */
    uint16_t consHeader;        /* The consumer socket header offset */
    uint16_t dmaMode;           /* Mode of DMA operation */
    uint16_t prodSusp;          /* Internal data - producer suspend option. */
    uint16_t consSusp;          /* Internal data - consumer suspend option. */
    uint16_t discardCount;      /* Inernal data - number of pending discards. */
    uint32_t notification;      /* Registered notifications */
    uint32_t xferSize;          /* Internal data - current xfer size */
    CyBool_t isDmaHandleDCache; /* Whether to do internal DMA cache handling. */
    CyU3PMutex lock;            /* Internal data - lock for the channel */
    CyU3PEvent flags;           /* Internal data - event flags for the channel */
    CyU3PDmaCallback_t cb;      /* Callback function which gets invoked on DMA events */
};

/* Summary
   Multi socket DMA channel structure.

   Description
   Every DMA channel shall have a channel structure associated with it for 
   storing info and status. This structure is updated and maintained by the
   library APIs and should not be modified by the application.
   This structure is for multi-socket operations.

   See Also
   * CyU3PDmaMultiChannelConfig_t
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelConfig_t
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
   * CyU3PDmaMultiChannelCacheControl
 */
struct CyU3PDmaMultiChannel
{
    uint32_t state;             /* The current state of the DMA channel */
    uint16_t type;              /* The type of the DMA channel */
    uint16_t size;              /* The buffer size associated with the channel*/
    uint16_t count;             /* Number of buffers for the channel */
    uint16_t validSckCount;     /* Number of sockets in the multi-socket operation. */
    uint16_t firstProdIndex[CY_U3P_DMA_MAX_MULTI_SCK_COUNT];    /*
                                   Head for the normal descriptor chain list */
    uint16_t firstConsIndex[CY_U3P_DMA_MAX_MULTI_SCK_COUNT];    /*
                                   Head for the manual mode consumer descriptor chain list */
    uint16_t prodSckId[CY_U3P_DMA_MAX_MULTI_SCK_COUNT];         /*
                                   The producer (ingress) socket ID */
    uint16_t consSckId[CY_U3P_DMA_MAX_MULTI_SCK_COUNT];         /*
                                   The consumer (egress) socket ID */
    uint16_t overrideDscrIndex; /* Internal data - Descriptor for override modes. */
    uint16_t currentProdIndex;  /* Internal data - Producer descriptor for the latest
                                   buffer produced. */
    uint16_t currentConsIndex;  /* Internal data - Consumer descriptor for the latest
                                   buffer produced. */
    uint16_t commitProdIndex;   /* Internal data - Producer descriptor for the buffer
                                   to be consumed. */
    uint16_t commitConsIndex;   /* Internal data - Consumer descriptor for the buffer
                                   to be consumed. */
    uint16_t activeProdIndex[CY_U3P_DMA_MAX_MULTI_SCK_COUNT];   /* Internal data - 
                                   Active producer descriptor. */
    uint16_t activeConsIndex[CY_U3P_DMA_MAX_MULTI_SCK_COUNT];   /* Internal data - 
                                   Active consumer descriptor. */
    uint16_t prodHeader;        /* The producer socket header offset */
    uint16_t prodFooter;        /* The producer socket footer offset */
    uint16_t consHeader;        /* The consumer socket header offset */
    uint16_t prodAvailCount;    /* Minimum available buffers before producer is active. */
    uint16_t dmaMode;           /* Mode of DMA operation */
    uint16_t prodSusp;          /* Internal data - producer suspend option. */
    uint16_t consSusp;          /* Internal data - consumer suspend option. */
    uint16_t bufferCount[CY_U3P_DMA_MAX_MULTI_SCK_COUNT];       /* Inernal data - number of
                                   active buffers available for consumer. */
    uint16_t discardCount[CY_U3P_DMA_MAX_MULTI_SCK_COUNT];      /* Inernal data - number of
                                   buffers to be discarded. */
    uint32_t notification;      /* Registered notifications. */
    uint32_t xferSize;          /* Internal data - current xfer size */
    CyBool_t isDmaHandleDCache; /* Whether to do internal DMA cache handling. */
    CyU3PMutex lock;            /* Internal data - lock for the channel */
    CyU3PEvent flags;           /* Internal data - event flags for the channel */
    CyU3PDmaMultiCallback_t cb; /* Callback function which gets invoked on DMA events */
};

/* Summary
   DMA socket control structure.

   Description
   The structure holds information about each socket.
   This is an internal data structure and should not
   be modified outside of the library.
 */
typedef union
{
    void *handle;                       /* Dummy handle for read operations. */
    CyU3PDmaChannel *singleHandle;      /* Handle to the associated single socket DMA channel */
    CyU3PDmaMultiChannel *multiHandle;  /* Handle to the associated multi socket DMA channel */
} CyU3PDmaSocketCtrl;

/**************************************************************************
 ********************** Global variable declarations **********************
 **************************************************************************/

/* Summary
   Array of socket control structure.

   Description
   Internal global variable used inside the APIs.
 */
extern CyU3PDmaSocketCtrl *glDmaSocketCtrl[];

/* Summary
   The variable determines whether DMA APIs should handle D-cache.

   Description
   This variable is updated and used by API and should not be used by the application.
 */
extern CyBool_t glDmaHandleDCache;

/**************************************************************************
 ********************************* Macros *********************************
 **************************************************************************/

/* The is a marker that is provided by s/w and can be observed by the IP.
   This bit is read only as far as the user application is concerned.
   It is used to indicate that a buffer has been marked for discarding.
   This bit has no effect on the hardware but is used by the DMA APIs.
 */
#define CY_U3P_DMA_BUFFER_MARKER                (1u << 0)

/* This is a marker indicating this descriptor refers to the last buffer of a packet
   or transfer. Packets/transfers may span more than one buffer. The producing
   IP provides this marker by providing the EOP signal to its DMA adapter.
   The consuming IP observes this marker by inspecting its EOP return signal
   from its own DMA adapter.
 */
#define CY_U3P_DMA_BUFFER_EOP                   (1u << 1)

/* Indicates the buffer data is valid (0) or in error (1). */
#define CY_U3P_DMA_BUFFER_ERROR                 (1u << 2)

/* Indicates the buffer is in use (1) or empty (0). */
#define CY_U3P_DMA_BUFFER_OCCUPIED              (1u << 3)

/* The combined status field mask. */
#define CY_U3P_DMA_BUFFER_STATUS_MASK           (0x000F)

/* The combined status field mask which can be modified by
   user application. */
#define CY_U3P_DMA_BUFFER_STATUS_WRITE_MASK     (0x000E)

/* Start address of the allowed buffer memory area for DMA operations. */
#define CY_U3P_DMA_BUFFER_AREA_BASE     (uint8_t *)(0x40000000)

#ifdef CYU3P_FPGA
  /* FPGA has only 256KB memory. So limit has to be set
   * correctly. */
  #define CY_U3P_DMA_BUFFER_AREA_LIMIT    (uint8_t *)(0x40040000)
#else 
/* End address of the allowed buffer memory area for DMA operations. */
  #define CY_U3P_DMA_BUFFER_AREA_LIMIT    (uint8_t *)(0x40080000)
#endif

/* Maximum allowed size for a single DMA buffer. */
#define CY_U3P_DMA_MAX_BUFFER_SIZE      (0xFFF0)

/* Maximum number of available buffers for a DMA channel.
   This is significant if prodAvailCount is non-zero. */
#define CY_U3P_DMA_MAX_AVAIL_COUNT      (31)

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/* Summary
   Queue a task message to the DMA module.

   Description
   The function shall be used to send task requests to the DMA
   module. It is an internal function and should not be invoked.

   Return value
   * None
 */
extern CyU3PReturnStatus_t
CyU3PDmaMsgSend (
        uint32_t *msg,                  /* Pointer to the message data. */
        uint32_t waitOption,            /* Timeout value for send operation. Can be CYU3P_NO_WAIT or
                                           CYU3P_WAIT_FOREVER. */
        CyBool_t priority               /* Whether this message should be queued at the head or the tail. */
        );

/******************* Single channel Function prototypes *******************/

/*@@DMA Functions
   This section documents the functions that are provided as part of the DMA
   manager module in the FX3 firmware library.
*/

/*@@Channel Functions
   This section documents the functions that operate on DMA channels.
*/

/* Summary
   Returns handle associated with the socket ID.

   Description
   This function is used to identify if there is any DMA channel
   associated with a socket. The function should be used only with
   single DMA channels.

   Return value
   * Handle of the channel associated with the specified socket. A NULL return indicates
   that an invalid socket ID was passed.

   See Also
   * CyU3PDmaChannel
   * CyU3PDmaSocketId_t
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PDmaChannel *
CyU3PDmaChannelGetHandle (
        CyU3PDmaSocketId_t sckId        /* The socket ID for the socket whose channel handle is required. */
        );

/* Summary
   Create a DMA channel.

   Description
   A DMA channel requires a producer socket, a consumer socket,
   a set of buffers and a callback function. This function
   must not be called when in DMA callback.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if any of the configuration parameters are invalid
   * CY_U3P_ERROR_MEMORY_ERROR - if the memory required for the channel could not be allocated

   See Also
   * CyU3PDmaType_t
   * CyU3PDmaChannel
   * CyU3PDmaChannelConfig_t
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelCreate (
        CyU3PDmaChannel *handle,                /* Pointer to channel structure that should be initialized. */
        CyU3PDmaType_t type,                    /* Type of DMA channel desired. */
        CyU3PDmaChannelConfig_t *config         /* Channel configuration parameters. */
        );

/* Summary
   Destroy a DMA channel.

   Description
   This should be called once the DMA channel is no longer required.
   This function must not be called when in DMA callback.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelDestroy (
        CyU3PDmaChannel *handle                 /* Pointer to DMA channel structure to be de-initialized. */
        );

/* Summary
   Update the DMA mode for the channel.

   Description
   The DMA mode is specified during creation of the channel and
   if this needs to be changed, then the API needs to be invoked.
   This API call can be made only when the DMA channel is in
   configured state (when there are no transfers setup).

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if DMA mode is invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_INVALID_SEQUENCE - if the DMA channel is not in the Configured state
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMode_t
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelUpdateMode (
        CyU3PDmaChannel *handle,                /* Handle to DMA channel to be modified. */
        CyU3PDmaMode_t dmaMode                  /* Desired DMA operating mode. Can be byte mode or buffer mode. */
        );

/* Summary
   Set a transfer on the DMA channel.

   Description
   The function starts a transaction the selected DMA channel.
   It should be invoked only when the channel is in CY_U3P_DMA_CONFIGURED
   state.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if the channel count is zero
   * CY_U3P_ERROR_ALREADY_STARTED - if the channel is active
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelSetXfer (
        CyU3PDmaChannel *handle,                /* Handle to the channel to be modified. */
        uint32_t count                          /* The desired transaction size in units corresponding to the selected
                                                   DMA mode.  Channel will revert to idle state when the specified
                                                   size of data has been transferred.  Can be set to zero to request an
                                                   infinite data transfer. */
        );

/* Summary
   Wraps up the current active buffer for the channel from the producer side.

   Description
   The function affects only the producer socket and does not change the
   behavior of the consumer socket. The function will only wrap up the current
   buffer and the producer socket will move to the next available buffer
   and this call does not suspend or disable the producer socket.
   The call can result in data loss or unaligned packet boundaries.

   The function is not intended for MANUAL_OUT channels. Also the function
   can be called only when the channel is in active mode or in consumer override
   mode.

   NOTE: This API will not work on p-port sockets if the data is less than
         48 bytes. Use GPIF state-machine to trigger the buffer wrap-up 
         in this case.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_INVALID_SEQUENCE - if the DMA channel is not in the required state
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelSetWrapUp (
        CyU3PDmaChannel *handle                /* Handle to the channel to be modified. */
        );

/* Summary
   Set the suspend options for the sockets.

   Description
   The function sets the suspend options for the sockets. The sockets
   are by default set to SUSP_NONE option. The API can be called only when
   the channel is in configured state or in active state. The suspend options
   are applied only in the active mode (SetXfer mode) and is a don't care
   override modes of operations.

   For manual channels, this is largely not required as each buffer needs to 
   be manually committed. But these options are still valid and can be used.
   For MANUAL_IN channel, only the producer socket option is considered
   and the consumer option is expected to be SUSP_NONE. Similarly for
   MANUAL_OUT channel, producer option is expected to be SUSP_NONE.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the channel type/suspend options are invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_INVALID_SEQUENCE - if the DMA channel is not in the required state
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelSetSuspend (
        CyU3PDmaChannel *handle,                /* Handle to the channel to be modified. */
        CyU3PDmaSckSuspType_t prodSusp,         /* Suspend option for the producer socket. */
        CyU3PDmaSckSuspType_t consSusp          /* Suspend option for the consumer socket. */
        );

/* Summary
   Resume a suspended DMA channel.

   Description
   The function can be called to resume a suspended
   channel. It can only be called when the channel is active
   after a SetXfer call. Producer and consumer suspends can
   be individually cleared.

   For a MANUAL_IN channel, only producer socket can be cleared
   and for a MANUAL_OUT channel only a consumer socket can be cleared.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the channel type is invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_STARTED - if the DMA channel was not started
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelResume (
        CyU3PDmaChannel *handle,                /* Handle to the channel to be resumed. */
        CyBool_t        isProdResume,           /* Whether to resume the producer socket. */
        CyBool_t        isConsResume            /* Whether to resume the consumer socket. */
        );

/* Summary
   Aborts a DMA channel.

   Description
   The function shall abort both the producer and consumer.
   The data in transition is lost and any active transaction cannot
   be resumed. This function leaves the channel in an aborted
   state and requires a reset before the channel can be used
   again.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelAbort (
        CyU3PDmaChannel *handle                 /* Handle to the channel to be aborted. */
        );

/* Summary
   Aborts and resets a DMA channel.

   Description
   The function shall abort both the producer and consumer.
   This function also resets the channel and flushes all buffers.
   The data in transition is lost and any active transaction cannot
   be resumed. But new transactions can be setup.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelReset (
        CyU3PDmaChannel *handle                 /* Handle to the channel to be reset. */
        );

/* Summary
   Send an external buffer to the consumer.

   Description
   This function is an override on the normal behavior of the
   channel and can send an external buffer to the consumer.
   This function can be called only from the CY_U3P_DMA_CONFIGURED state.

   The buffers used for DMA operations are expected to be allocated using
   CyU3PDmaBufferAlloc call. If this is not the case, then the buffer has
   to be over allocated in such a way that the full buffer should
   be 32 byte aligned and should be a multiple of 32 bytes. This is to
   satisfy the 32 byte cache lines.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the buffer parameters are invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_ALREADY_STARTED - if the DMA channel is already started
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaBuffer_t
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelSetupSendBuffer (
        CyU3PDmaChannel *handle,                /* Handle to the DMA channel to be modified. */
        CyU3PDmaBuffer_t *buffer_p              /* Pointer to structure containing address, size and status of the
                                                   DMA buffer to be sent out. */
        );

/* Summary
   Receive data to an external buffer.

   Description
   This function is an override on the normal behavior of the
   channel and can receive data from producer to an external buffer.
   This function can be called only from the CY_U3P_DMA_CONFIGURED state.

   The buffer that is passed as parameter for receiving the data has the
   following restrictions:

   1. The buffer should have a size multiple of 16 bytes.
      
   2. If the data cache is enabled then, the buffer should be 32 byte aligned and
      a multiple of 32 bytes. This is to match the 32 byte cache line. 32 byte
      check is not enforced by the API as the buffer can be over-allocated.
      All buffers used for DMA are expected to be allocated using the CyU3PDmaBufferAlloc
      call. This takes care of the 32 byte alignment and size restrictions.
      If for any reason, the buffer is not allocated using this API, then 
      cache restrictions should be taken care of by the application.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the buffer parameters are invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_ALREADY_STARTED - if the DMA channel is already started
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaBuffer_t
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelSetupRecvBuffer (
        CyU3PDmaChannel *handle,                /* Handle to the DMA channel to be modified. */
        CyU3PDmaBuffer_t *buffer_p              /* Pointer to structure containing the address and size of the buffer
                                                   to be filled up with received data. */
        );

/* Summary
   Wait for the data to be filled.

   Description
   This function requires that the CyU3PDmaChannelSetupRecvBuffer API to be
   first called. It waits for the transfer to complete and returns the
   buffer status. If the transfer is not completed within the specified
   timeout, then the function returns with a timeout error and can be called
   again. The CyU3PDmaChannelWaitForCompletion also waits for this but
   does not return the buffer status.
   This function must not be called when in DMA callback.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the buffer parameters are invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_INVALID_SEQUENCE - if this sequence is not permitted
   * CY_U3P_ERROR_TIMEOUT - if the DMA transfer timed out
   * CY_U3P_ERROR_DMA_FAILURE - if the DMA transfer failed
   * CY_U3P_ERROR_ABORTED - if the DMA transfer was aborted
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired
   * CY_U3P_ERROR_NOT_STARTED - if the DMA channel is not started

   See Also
   * CyU3PDmaBuffer_t
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
CyU3PReturnStatus_t
CyU3PDmaChannelWaitForRecvBuffer (
        CyU3PDmaChannel *handle,                /* Handle to the DMA channel on which to wait. */
        CyU3PDmaBuffer_t *buffer_p,             /* Output parameter which will be filled up with the address, count
                                                   and status values of the DMA buffer into which data was received. */
        uint32_t waitOption                     /* Duration to wait for the receive completion. */
        );

/* Summary
   Get the current buffer pointer.

   Description
   This function acts differently for different types of DMA channels.

   For CY_U3P_DMA_TYPE_MANUAL and CY_U3P_DMA_TYPE_MANUAL_IN channels,
   the function returns the data buffer received from the producer socket.
   The wait_option specifies the timeout value. In case of the
   CY_U3P_DMA_TYPE_MANUAL_OUT type of DMA channel, the function returns the
   pointer to the buffer for the CPU to fill the data.

   The buffer pointer returned, points to the data produced by the producer
   socket. So the pointer and the count does not include the offsets assigned.

   For example, if there is a producer header offset of 12 bytes,
   the valid data from the producer header can be reached at buffer_p->buffer[-12].
   Care should be taken so that no more than allocated data is accessed.

   For a MANUAL_OUT channel with a consumer header offset,
   the buffer pointer returned points to the beginning of the
   actual buffer allocated as the CPU is the producer and there are
   no producer offsets. The count value shall be zero if there is no
   consumer header offset and shall be equal to the consumer header offset
   in case of a non-zero consumer offset. This is for ease of use and the
   user can know the consumer offset.

   For AUTO channels this is a special API to be called only when the consumer
   socket is in the suspended state. In all other cases this will return 
   CY_U3P_ERROR_INVALID_SEQUENCE. This API is used to look at the current
   consumer buffer in case of a SUSP_CONS_PARTIAL_BUF situation.

   This function must not be called when in DMA callback with a non-zero wait option.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the buffer parameters are invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_NOT_STARTED - if the DMA channel is not started
   * CY_U3P_ERROR_INVALID_SEQUENCE - if this sequence is not permitted
   * CY_U3P_ERROR_TIMEOUT - if the DMA transfer timed out
   * CY_U3P_ERROR_DMA_FAILURE - if the DMA transfer failed
   * CY_U3P_ERROR_ABORTED - if the DMA transfer was aborted
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaBuffer_t
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelGetBuffer (
        CyU3PDmaChannel *handle,                /* Handle to the DMA channel on which to wait. */
        CyU3PDmaBuffer_t *buffer_p,             /* Output parameter that will be filled with data about the
                                                   buffer that was obtained. */
        uint32_t waitOption                     /* Duration to wait before returning a timeout status. */
        );

/* Summary
   Commit the buffer to be sent to the consumer.

   Description
   The function is generally for the manual DMA channels.
   The function sends a buffer in the current DMA descriptor to the consumer
   socket. This is not valid for CY_U3P_DMA_TYPE_MANUAL_IN channels.
   The count of data provided is exact count that needs to be sent out of the
   consumer socket.

   This API is used for AUTO channels for special case handling.
   It should be called only when the consumer socket is in the suspended state.
   In all other cases this will return CY_U3P_ERROR_INVALID_SEQUENCE.
   This API is used to commit the current consumer buffer in case of a
   SUSP_CONS_PARTIAL_BUF situation.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the count is invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_NOT_STARTED - if the DMA channel is not started
   * CY_U3P_ERROR_INVALID_SEQUENCE - if this sequence is not permitted
   * CY_U3P_ERROR_DMA_FAILURE - if the DMA transfer failed
   * CY_U3P_ERROR_ABORTED - if the DMA transfer was aborted
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelCommitBuffer (
        CyU3PDmaChannel *handle,                /* Handle to the DMA channel to be modified. */
        uint16_t count,                         /* Size of data in the buffer being committed. The buffer
                                                   address is implicit and is fetched from the active descriptor
                                                   for the channel. */
        uint16_t bufStatus                      /* Current status (occupied and end of transfer bits) of the
                                                   buffer being committed. */
        );

/* Summary
   Discard buffer in the current DMA descriptor.

   Description
   The function is generally for the CY_U3P_DMA_TYPE_MANUAL and
   CY_U3P_DMA_TYPE_MANUAL_IN types of DMA channels to discard the
   current buffer.

   AUTO Channel special case:
   This API is also used for AUTO channels for special case handling.
   For AUTO channels, this API is used to discard the current consumer
   buffer only in case of a SUSP_CONS_PARTIAL_BUF situation. The discard
   buffer will function only when the consumer socket is in the suspended
   state.  If the socket is not suspended, this API will return
   CY_U3P_ERROR_INVALID_SEQUENCE.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_NOT_STARTED - if the DMA channel is not started
   * CY_U3P_ERROR_INVALID_SEQUENCE - if this sequence is not permitted
   * CY_U3P_ERROR_DMA_FAILURE - if the DMA transfer failed
   * CY_U3P_ERROR_ABORTED - if the DMA transfer was aborted
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelDiscardBuffer (
        CyU3PDmaChannel *handle                 /* Handle to the DMA channel to be modified. */
        );

/* Summary
   Wait for the current DMA transaction to complete.

   Description
   The function can be called from any thread and is invoked to wait
   until the current transfer is completed. This function is not supported
   for infinite transfers. This function waits for completion of the transfer
   and so is useful only for AUTO mode of operation and for external buffer
   operations. If DMA event notifications are enabled, then the notifications
   shall come before the function call returns.
   This function must not be called when in DMA callback.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_NOT_STARTED - if the DMA channel is not started
   * CY_U3P_ERROR_DMA_FAILURE - if the DMA transfer failed
   * CY_U3P_ERROR_ABORTED - if the DMA transfer was aborted
   * CY_U3P_ERROR_TIMEOUT - if the DMA transfer timed out
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetStatus
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaChannelWaitForCompletion (
        CyU3PDmaChannel *handle,                /* Handle to the DMA channel to wait on. */
        uint32_t waitOption                     /* Duration for which to wait. */
        );

/* Summary
   This functions returns the current channel status.

   Description
   The function can be used to keep track of the data transfers
   and even for transfer progress. It also returns the current channel state.
   In case of override modes, this function always returns the byte count.
   The count field is updated only at buffer boundaries and so does not give
   real time information.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaState_t
   * CyU3PDmaChannel
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
   * CyU3PDmaChannelUpdateMode
   * CyU3PDmaChannelGetHandle
   * CyU3PDmaChannelSetXfer
   * CyU3PDmaChannelGetBuffer
   * CyU3PDmaChannelCommitBuffer
   * CyU3PDmaChannelDiscardBuffer
   * CyU3PDmaChannelSetupSendBuffer
   * CyU3PDmaChannelSetupRecvBuffer
   * CyU3PDmaChannelWaitForCompletion
   * CyU3PDmaChannelWaitForRecvBuffer
   * CyU3PDmaChannelSetWrapUp
   * CyU3PDmaChannelSetSuspend
   * CyU3PDmaChannelResume
   * CyU3PDmaChannelAbort
   * CyU3PDmaChannelReset
   * CyU3PDmaChannelCacheControl
 */
CyU3PReturnStatus_t
CyU3PDmaChannelGetStatus (
        CyU3PDmaChannel *handle,                /* Handle to the DMA channel to query. */
        CyU3PDmaState_t *state,                 /* Output parameter that will be filled with state of the channel. */
        uint32_t *prodXferCount,                /* Output parameter that will be filled with transfer count on the
                                                   producer socket in DMA mode units. */
        uint32_t *consXferCount                 /* Output parameter that will be filled with transfer count on the
                                                   consumer socket in DMA mode units. */
        );

/* Summary
   This function enables / disables DMA API internal d-cache handling.

   Description
   This funtion provides control over DMA API internal d-cache handling
   feature per individual channel. The global behaviour is selected based
   on CyU3PDeviceCacheControl API. This API allows to override this option
   for a particular channel. It should be noted that the CyU3PDeviceCacheControl
   should be invoked before even initializing the RTOS whereas this API can
   be done after a channel is created. However the call should be made when
   the channel is idle (CONFIGURED state). This API makes sense only when
   the D-cache is enabled.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired
   * CY_U3P_ERROR_INVALID_SEQUENCE - if the DMA channel is not idle (configured state)

   See Also
   * CyU3PDmaChannel
   * CyU3PDeviceCacheControl
   * CyU3PDmaChannelCreate
   * CyU3PDmaChannelDestroy
 */
CyU3PReturnStatus_t
CyU3PDmaChannelCacheControl (
        CyU3PDmaChannel *handle,                /* Handle to the DMA channel. */
        CyBool_t isDmaHandleDCache              /* Whether to enable handling or not. */
        );

/******************* Multi channel Function prototypes ********************/

/*@@Multi-Channel Functions
   Multi-channel are special versions of DMA channels that involve multiple
   producers or multiple consumers for a data flow. Since these channels will
   only be used rarely, the operations on such channel are kept separated in a
   different section which can be removed from the firmware binary when not used.

   This section documents the functions that operate on multi-channels. These
   operations are similar to those supported on regular DMA channels.
*/

/* Summary
   Returns handle associated with the socket ID.

   Description
   This function is used to identify if there is any DMA channel
   associated with a socket. This function should be used only with
   multi DMA channels.

   Return value
   * Handle to the Multi-channel structure corresponding to the socket. A NULL return indicates
   * that an invalid socket ID was passed.

   See Also
   * CyU3PDmaSocketId_t
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PDmaMultiChannel *
CyU3PDmaMultiChannelGetHandle (
        CyU3PDmaSocketId_t sckId                /* ID of the socket whose channel handle is required. */
        );

/* Summary
   Create a DMA channel. This function is to be used for only multi socket
   DMA channels.

   Description
   A DMA channel requires producer socket(s), consumer socket(s),
   a set of buffers and a callback function.
   This function must not be called when in DMA callback.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if any of the configuration parameters are invalid
   * CY_U3P_ERROR_MEMORY_ERROR - if the memory required for the channel could not be allocated
   * CY_U3P_ERROR_INVALID_SEQUENCE - if a multicast channel is being created without calling CyU3PDmaEnableMulticast

   See Also
   * CyU3PDmaMultiType_t
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelConfig_t
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
   * CyU3PDmaMultiChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelCreate (
        CyU3PDmaMultiChannel *handle,           /* Pointer to multi-channel structure that is to be initialized. */
        CyU3PDmaMultiType_t type,               /* Type of DMA channel to be created. */
        CyU3PDmaMultiChannelConfig_t *config    /* Configuration information about the channel to be created. */
        );

/* Summary
   Destroy a DMA channel. This function is to be used for only multi socket
   DMA channels.

   Description
   This should be called once the DMA channel is no longer required.
   This function must not be called when in DMA callback.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
   * CyU3PDmaMultiChannelCacheControl
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelDestroy (
        CyU3PDmaMultiChannel *handle            /* Pointer to the multi-channel to be de-initialized. */
        );

/* Summary
   Update the DMA mode for the multi channel.

   Description
   The DMA mode is specified during creation of the channel and
   if this needs to be changed, then the API needs to be invoked.
   This API call can be made only when the DMA channel is in
   configured state (when there are no transfers setup).

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the DMA mode is invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_INVALID_SEQUENCE - if the DMA channel is not in the Configured state
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMode_t
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelUpdateMode (
        CyU3PDmaMultiChannel *handle,           /* Handle to the multi-channel to be modified. */
        CyU3PDmaMode_t dmaMode                  /* Desired DMA mode. */
        );

/* Summary
   Set a transfer on the DMA channel. This function should be used for only
   multi socket operations.

   Description
   The function starts a transaction the selected DMA channel.
   It should be invoked only when the channel is in CY_U3P_DMA_CONFIGURED
   state.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the multiSckOffset is invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_ALREADY_STARTED - if the DMA channel was already started
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelSetXfer (
        CyU3PDmaMultiChannel *handle,           /* Handle to the multi-channel to be modified. */
        uint32_t count,                         /* Size of the transfer to be started. Can be zero to denote
                                                   infinite data transfer. */
        uint16_t multiSckOffset                 /* Socket id to start the operation from. For a many to one channel,
                                                   this would be a producer socket offset; and for a one to many
                                                    channel, this would be a consumer socket offset. */
        );

/* Summary
   Wraps up the current active buffer for the channel from the producer side.

   Description
   The function affects only the producer socket and does not change the
   behavior of the consumer socket. The function will only wrap up the current
   buffer and the producer socket will move to the next available buffer
   and this call does not suspend or disable the producer socket.
   The call can result in data loss or unaligned packet boundaries.

   The function can be called only when the channel is in active mode
   or in consumer override mode.

   NOTE: This API will not work on p-port sockets if the data is less than
         48 bytes. Use GPIF state-machine to trigger the buffer wrap-up 
         in this case.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the multiSckOffset is invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_INVALID_SEQUENCE  - if the DMA channel is not in the required state
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelSetWrapUp (
        CyU3PDmaMultiChannel *handle,           /* Handle to the channel to be modified. */
        uint16_t multiSckOffset                 /* Socket id to wrapup. For a many to one channel,
                                                   this would be a producer socket offset; and for a one to many
                                                   channel, this would be always zero. */
        );

/* Summary
   Set the suspend options for the sockets.

   Description
   The function sets the suspend options for the sockets. The sockets
   are by default set to SUSP_NONE option. The API can be called only when
   the channel is in configured state or in active state. The suspend options
   are applied only in the active mode (SetXfer mode) and is a don't care
   override modes of operations.

   For manual channels, this is largely not required as each buffer needs to 
   be manually committed. But these options are still valid and can be used.
   Options can be set only for socket on the single side. For MANY_TO_ONE
   channel only consumer socket option can be used and for ONE_TO_MANY, only
   producer socket option can be used.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the suspend options are invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_INVALID_SEQUENCE  - if the DMA channel is not in the required state
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDmaSckSuspType_t
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelSetSuspend (
        CyU3PDmaMultiChannel *handle,           /* Handle to the multi-channel to be suspended. */
        CyU3PDmaSckSuspType_t prodSusp,         /* Suspend option for the producer socket. */
        CyU3PDmaSckSuspType_t consSusp          /* Suspend option for the consumer socket. */
        );

/* Summary
   Resume a suspended DMA channel.

   Description
   The function can be called to resume a suspended
   channel. It can only be called when the channel is active
   after a SetXfer call. Producer and consumer suspends can
   be individually cleared.

   For MANY_TO_ONE channel, only consumer side can be resumed
   and for ONE_TO_MANY channel, only producer side can be resumed.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the resume options are invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelResume (
        CyU3PDmaMultiChannel *handle,           /* Handle to the multi-channel to be resumed. */
        CyBool_t        isProdResume,           /* Whether to resume the producer socket. */
        CyBool_t        isConsResume            /* Whether to resume the consumer socket. */
        );

/* Summary
   Aborts a DMA channel. This function shall
   be used only with multi socket operations.

   Description
   The function shall abort both the producer and consumer.
   The data in transition is lost and any active transaction
   cannot be resumed. This function leaves the channel in an
   aborted state and requires a reset before the channel can
   be used again.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelAbort (
        CyU3PDmaMultiChannel *handle            /* Handle to the multi-channel to be aborted. */
        );

/* Summary
   Aborts and resets a DMA channel. This function shall
   be used only with multi socket operations.

   Description
   The function shall abort both the producer and consumer.
   This function also resets the channel and flushes all buffers.
   The data in transition is lost and any active transaction cannot
   be resumed. But new transactions can be setup.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelReset (
        CyU3PDmaMultiChannel *handle            /* Handle to the multi-channel to be reset. */
        );
                                                  

/* Summary
   Send an external buffer to the consumer.

   Description
   This function is an override on the normal behavior of the
   channel and can send an external buffer to the consumer.
   This function can be called only from the CY_U3P_DMA_CONFIGURED state.

   The buffers used for DMA operations are expected to be allocated using
   CyU3PDmaBufferAlloc call. If this is not the case, then the buffer has
   to be over allocated in such a way that the full buffer should
   be 32 byte aligned and should be a multiple of 32 bytes. This is to
   satisfy the 32 byte cache lines.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if an invalid parameters are passed
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_ALREADY_STARTED - if the DMA channel was already started
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaBuffer_t
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelSetupSendBuffer (
        CyU3PDmaMultiChannel *handle,           /* Handle to the DMA multi channel to be modified. */
        CyU3PDmaBuffer_t *buffer_p,             /* Pointer to structure containing address, size and status of the
                                                   DMA buffer to be sent out. */
        uint16_t multiSckOffset                 /* Consumer Socket id to send the data. For a many to one channel,
                                                   this would be a zero; and for a one to many channel, this would
                                                   be a consumer socket offset. */
        );

/* Summary
   Receive data to an external buffer.

   Description
   This function is an override on the normal behavior of the
   channel and can receive data from producer to an external buffer.
   This function can be called only from the CY_U3P_DMA_CONFIGURED state.

   The buffer that is passed as parameter for receiving the data has the
   following restrictions:

   1. The buffer should have a size multiple of 16 bytes.
      
   2. If the data cache is enabled then, the buffer should be 32 byte aligned and
      a multiple of 32 bytes. This is to match the 32 byte cache line. 32 byte
      check is not enforced by the API as the buffer can be over-allocated.
      All buffers used for DMA are expected to be allocated using the CyU3PDmaBufferAlloc
      call. This takes care of the 32 byte alignment and size restrictions.
      If for any reason, the buffer is not allocated using this API, then 
      cache restrictions should be taken care of by the application.
   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if an invalid parameters are passed
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_ALREADY_STARTED - if the DMA channel was already started
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaBuffer_t
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelSetupRecvBuffer (
        CyU3PDmaMultiChannel *handle,           /* Handle to the DMA multi channel to be modified. */
        CyU3PDmaBuffer_t *buffer_p,             /* Pointer to structure containing the address and size of the buffer
                                                   to be filled up with received data. */
        uint16_t multiSckOffset                 /* Producer Socket id to receive the data. For a one to many channel,
                                                   this would be a zero; and for a many to one channel, this would
                                                   be a producer socket offset. */
        );

/* Summary
   Wait for the data to be filled.

   Description
   This function requires that the CyU3PDmaMultiChannelSetupRecvBuffer API
   to be first called. It waits for the transfer to complete and returns the
   buffer status. If the transfer is not completed within the specified
   timeout, then the function returns with a timeout error and can be called
   again. The CyU3PDmaMultiChannelWaitForCompletion also waits for this but
   does not return the buffer status.
   This function must not be called when in DMA callback.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_INVALID_SEQUENCE  - if the DMA channel is not in the required state
   * CY_U3P_ERROR_DMA_FAILURE - if the DMA transfer failed
   * CY_U3P_ERROR_ABORTED - if the DMA transfer was aborted
   * CY_U3P_ERROR_TIMEOUT - if the DMA transfer timed out
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired
   * CY_U3P_ERROR_NOT_STARTED - if the DMA channel is not started

   See Also
   * CyU3PDmaBuffer_t
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
CyU3PReturnStatus_t
CyU3PDmaMultiChannelWaitForRecvBuffer (
        CyU3PDmaMultiChannel *handle,           /* Handle to the DMA channel on which to wait. */
        CyU3PDmaBuffer_t *buffer_p,             /* Output parameter which will be filled up with the address, count
                                                   and status values of the DMA buffer into which data was received. */
        uint32_t waitOption                     /* Duration to wait for the receive completion. */
        );

/* Summary
   Get the current buffer pointer. This function shall be used only with multi socket
   operations.

   Description
   This function is generally for manual mode operations.

   The function returns the data buffer received from the producer socket(s).
   In case of Many to one type of channels, this function receives buffers from each
   of the producer sockets in a round robin fashion. The starting point is decided by
   the multiSckOffset value specified during SetXfer phase.
   The wait_option specifies the timeout value.

   The buffer pointer returned, points to the buffer actually populated by 
   the producer socket and so does not include the producer offsets.
   The count returned also reports actual count of data received by the producer
   socket.

   For example, if there is a producer header offset of 12 bytes, the valid data starts
   at buffer_p->buffer[0] and the producer header can be accessed at buffer_p->buffer[-12].
   Care should be taken to only access memory locations actually allocated and not be
   access beyond producer header and footer region.

   For MANY_TO_ONE AUTO channel this is a special API to be called only when
   the consumer socket is in the suspended state. In all other cases this will
   return CY_U3P_ERROR_INVALID_SEQUENCE. This API is used to look at the current
   consumer buffer in case of a SUSP_CONS_PARTIAL_BUF situation.

   This function must not be called when in DMA callback with a non-zero wait option.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_NOT_STARTED - if the DMA channel is not started
   * CY_U3P_ERROR_INVALID_SEQUENCE  - if the DMA channel is not in the required state
   * CY_U3P_ERROR_DMA_FAILURE - if the DMA transfer failed
   * CY_U3P_ERROR_ABORTED - if the DMA transfer was aborted
   * CY_U3P_ERROR_TIMEOUT - if the DMA transfer timed out
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaBuffer_t
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelGetBuffer (
        CyU3PDmaMultiChannel *handle,           /* Handle to the multi-channel to be modified. */
        CyU3PDmaBuffer_t *buffer_p,             /* Output parameter that will be filled with address, size and status
                                                   of the buffer with received data. */
        uint32_t waitOption                     /* Duration for which to wait for data. */
        );

/* Summary
   Commit the buffer to be sent to the consumer. This function shall be used only
   with multi socket operations.

   Description
   The function is generally for the manual multi socket DMA channels.
   The function sends a buffer in the current DMA descriptor to the consumer
   socket(s). The count of data provided must be the amount of data required
   to be sent out of the consumer. This does not include the consumer offset.

   This API is used for MANY_TO_ONE AUTO channel for special case handling.
   It should be called only when the consumer socket is in the suspended state.
   In all other cases this will return CY_U3P_ERROR_INVALID_SEQUENCE.
   This API is used to commit the current consumer buffer in case of a
   SUSP_CONS_PARTIAL_BUF situation.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_NOT_STARTED - if the DMA channel is not started
   * CY_U3P_ERROR_INVALID_SEQUENCE  - if the DMA channel is not in the required state
   * CY_U3P_ERROR_DMA_FAILURE - if the DMA transfer failed
   * CY_U3P_ERROR_ABORTED - if the DMA transfer was aborted
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelUpdateMode
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelCommitBuffer (
        CyU3PDmaMultiChannel *handle,           /* Handle to the multi-channel to be modified. */
        uint16_t count,                         /* Size of the memory buffer being committed. The address of the
                                                   buffer is implicit and is taken from the active descriptor. */
        uint16_t bufStatus                      /* Status of the buffer being committed. */
        );

/* Summary
   Discard buffer in the current DMA descriptor. This functions should
   be used only with multi socket DMA channels.

   Description
   The function is generally for the manual mode multi socket DMA
   channels to discard the current buffer.

   AUTO Channel special case:
   This API is also used for AUTO channels for special case handling.
   For AUTO channels, this API is used to discard the current consumer
   buffer only in case of a SUSP_CONS_PARTIAL_BUF situation. The discard
   buffer will function only when the consumer socket is in the suspended
   state.  If the socket is not suspended, this API will return
   CY_U3P_ERROR_INVALID_SEQUENCE.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_NOT_STARTED - if the DMA channel is not started
   * CY_U3P_ERROR_INVALID_SEQUENCE  - if the DMA channel is not in the required state
   * CY_U3P_ERROR_DMA_FAILURE - if the DMA transfer failed
   * CY_U3P_ERROR_ABORTED - if the DMA transfer was aborted
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelGetHandle
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelSetWrapUp
   * CyU3PDmaMultiChannelSetSuspend
   * CyU3PDmaMultiChannelResume
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelDiscardBuffer (
        CyU3PDmaMultiChannel *handle            /* Handle to the multi-channel to be modified. */
        );

/* Summary
   Wait for the current DMA transaction to complete. This function should
   be used only for the multi socket DMA channels.

   Description
   The function can be called from any thread and is invoked to wait
   until the current transfer is completed. This function is not supported
   for infinite transfers. This function waits for completion of the transfer
   and so is useful only for AUTO mode of operations. If DMA event notifications
   are enabled, then the notifications shall come before the function call returns.
   This function must not be called when in DMA callback.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_NOT_SUPPORTED - if this operation is not supported for the DMA channel type
   * CY_U3P_ERROR_NOT_STARTED - if the DMA channel is not started
   * CY_U3P_ERROR_DMA_FAILURE - if the DMA transfer failed
   * CY_U3P_ERROR_ABORTED - if the DMA transfer was aborted
   * CY_U3P_ERROR_TIMEOUT - if the DMA transfer timed out
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelGetBuffer
   * CyU3PDmaMultiChannelCommitBuffer
   * CyU3PDmaMultiChannelDiscardBuffer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
   * CyU3PDmaMultiChannelGetStatus
 */
extern CyU3PReturnStatus_t
CyU3PDmaMultiChannelWaitForCompletion (
        CyU3PDmaMultiChannel *handle,           /* Handle to the multi-channel to wait on. */
        uint32_t waitOption                     /* Duration for which to wait. */
        );

/* Summary
   This functions returns the current multi channel status.

   Description
   The function can be used to keep track of the data transfers
   and even for transfer progress. It also returns the current channel state.
   In case of override modes, this function always returns the byte count.
   The count field is updated only at buffer boundaries and so does not give
   real time information. The function is for multi channels.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_BAD_ARGUMENT - if the sckIndex is invalid
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDmaState_t
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
   * CyU3PDmaMultiChannelSetXfer
   * CyU3PDmaMultiChannelSetupSendBuffer
   * CyU3PDmaMultiChannelSetupRecvBuffer
   * CyU3PDmaMultiChannelWaitForCompletion
   * CyU3PDmaMultiChannelWaitForRecvBuffer
   * CyU3PDmaMultiChannelAbort
   * CyU3PDmaMultiChannelReset
 */
CyU3PReturnStatus_t
CyU3PDmaMultiChannelGetStatus (
        CyU3PDmaMultiChannel *handle,           /* Handle to the DMA channel to query. */
        CyU3PDmaState_t *state,                 /* Output parameter that will be filled with state of the channel. */
        uint32_t *prodXferCount,                /* Output parameter that will be filled with transfer count on the
                                                   producer socket in DMA mode units. */
        uint32_t *consXferCount,                /* Output parameter that will be filled with transfer count on the
                                                   consumer socket in DMA mode units. */
        uint8_t   sckIndex                      /* The socket index for retreiving informtion on the multi socket side. */
        );

/* Summary
   This function enables / disables DMA API internal d-cache handling
   for multi-channels.

   Description
   This funtion provides control over DMA API internal d-cache handling
   feature per individual channel. The global behaviour is selected based
   on CyU3PDeviceCacheControl API. This API allows to override this option
   for a particular channel. It should be noted that the CyU3PDeviceCacheControl
   should be invoked before even initializing the RTOS whereas this API can
   be done after a channel is created. However the call should be made when
   the channel is idle (CONFIGURED state). This API makes sense only when
   the D-cache is enabled.

   Return value
   * CY_U3P_SUCCESS - if the function call is successful
   * CY_U3P_ERROR_NULL_POINTER - if any pointer passed as parameter is NULL
   * CY_U3P_ERROR_NOT_CONFIGURED - if the DMA channel was not configured
   * CY_U3P_ERROR_MUTEX_FAILURE - if the DMA channel mutex could not be acquired
   * CY_U3P_ERROR_INVALID_SEQUENCE - if the DMA channel is not idle (configured state)

   See Also
   * CyU3PDmaMultiChannel
   * CyU3PDeviceCacheControl
   * CyU3PDmaMultiChannelCreate
   * CyU3PDmaMultiChannelDestroy
 */
CyU3PReturnStatus_t
CyU3PDmaMultiChannelCacheControl (
        CyU3PDmaMultiChannel *handle,           /* Handle to the DMA channel. */
        CyBool_t isDmaHandleDCache              /* Whether to enable handling or not. */
        );

/* Summary
   Enable creation and management of DMA multicast channels.

   Description
   It is expected that DMA multicast channels will only rarely be used in FX3 applications. Since the
   multichannel creation code takes in the channel type as a parameter and then calls the appropriate
   handler functions, the code to setup and work with multicast channels gets linked into any FX3
   application that uses multichannels, leading to un-necessary loss of code space. This function is
   provided to prevent this memory loss.
   
   The multicast channel code will only be linked into the FX3 application if this function has been
   called. Therefore, this function needs to be called by the application before any multicast channels
   are created.

   Returns
   * None

   See Also
   * CyU3PDmaMultiChannelCreate
 */
extern void
CyU3PDmaEnableMulticast (
        void);

/* Internal Functions for the library - Do not add to documentation. */

/* Summary
   Specify that the AVL_EN bit needs to be set for a USB endpoint.
 */
void
CyU3PDmaSetUsbSocketMult (
        uint8_t ep                              /* Endpoint for which the AVL_EN is to be set. */
        );

/* Summary
   Check whether the AVL_EN needs to be set for this socket.
 */
CyBool_t
CyU3PDmaIsSockAvlEnReqd (
        uint16_t sckId                /* Socket id to be queried. */
        );

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3DMA_H_ */

/*[]*/
