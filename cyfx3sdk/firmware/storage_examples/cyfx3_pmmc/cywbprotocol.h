/* Cypress West Bridge API header file (cywbprotocol.h)
 ## ===========================
 ##
 ##  Copyright Cypress Semiconductor Corporation, 2009-2011,
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

#ifndef _INCLUDED_CYWBPROTOCOL_H_
#define _INCLUDED_CYWBPROTOCOL_H_

#include "cywbtypes.h"

/*
 * Constants defining the per context buffer sizes
 */
#ifndef __doxygen__
#define CY_CTX_GEN_MAX_DATA_SIZE                (24)
#define CY_CTX_STR_MAX_DATA_SIZE                (16)
#define CY_CTX_USB_MAX_DATA_SIZE                (16)
#define CY_CTX_LPP_MAX_DATA_SIZE                (20)
#define CY_CTX_DBG_MAX_DATA_SIZE                (16)
#endif

/*@@Common Responses
    Summary
    The response types that are common to all contexts include:
     * CY_RESP_SUCCESS_FAILURE
     * CY_RESP_INVALID_REQUEST
     * CY_RESP_INVALID_LENGTH
     * CY_RESP_NO_SUCH_ADDRESS
 */

/* Summary
   This response indicates a command has been processed and returned a status.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = all
    * Response Code = 0

   D0
    * 0 = success (CY_WB_ERROR_SUCCESS)
    * non-zero = error code

   Description
   This response indicates that a request was processed and no data
   was generated as a result of the request beyond a single 16 bit
   status value.  This response contains the 16 bit data value.
 */
#define CY_RESP_SUCCESS_FAILURE                                 (0)

/* Summary
   This response indicates an invalid request was sent

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = all
    * Response Code = 1

   D0
    * MailBox contents for invalid request

   Description
   This response is returned when a request is sent that contains an invalid
   context or request code.
 */
#define CY_RESP_INVALID_REQUEST                                 (1)

/* Summary
   This response indicates a request of invalid length was sent

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = all
    * Response Code = 2

   D0
    * MailBox contents for invalid request
    * Length for invalid request

   Description
   The software API and firmware sends requests across the P Port to West Bridge
   interface on different contexts.  Each contexts has a maximum size of the
   request packet that can be received.  The size of a request can be determined
   during the first cycle of a request transfer.  If the request is larger
   than can be handled by the receiving context this response is returned.  Note
   that the complete request is received before this response is sent, but that
   the request is dropped after this response is sent.
 */
#define CY_RESP_INVALID_LENGTH                                  (2)


/* Summary
   This response indicates a request was made to an invalid storage address.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   2

   MailBox0
    * Context = all
    * Response Code = 3

   D0
   Bits 15 - 12 : Portion of address that is invalid
                  0 = Port number
                  1 = Device Index
                  2 = Unit Index
                  3 = Block Address
   Bits 11 -  8 : Port number 
                  0 = S0 port
                  1 = S1 port
   Bits  7 -  0 : Zero based unit index

   D1
   Upper 16 bits of block address

   D2
   Lower 16 bits of block address

   Description
   This response indicates a request to an invalid storage media
   address
 */
#define CY_RESP_NO_SUCH_ADDRESS                                 (3)

/******************************************************************************/

/* Summary
   The general (device) context is used to send/receive commands and data affecting
   the functioning of the device as a whole.
 */
#define CY_RQT_GENERAL_RQT_CONTEXT                              (0)

/*@@Device requests
    Summary
    The general device requests from API to firmware include:
     * CY_RQT_GET_FIRMWARE_VERSION
     * CY_RQT_CONTROL_FIRMWARE_HEARTBEAT
     * CY_RQT_ENTER_STANDBY_MODE
     * CY_RQT_ENTER_SUSPEND_MODE
     * CY_RQT_IO_CONFIGURE
 */


/* Summary
   This command returns the firmware version number, media types supported and
   debug/release mode information.

   Direction
   P Port Processor-> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 0
    * Request Code = 0

   Description
   The response contains the 16-bit major version, the 16-bit minor version,
   the 16 bit build number, media types supported and release/debug mode
   information.

   Responses
    * CY_RESP_FIRMWARE_VERSION
 */
#define CY_RQT_GET_FIRMWARE_VERSION                             (0)


/* Summary
   This command enables/disables the periodic heartbeat message from the West Bridge firmware to
   the processor.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 0
    * Request code = 1

   D0
   Enable/Disable flag

   Description
   This command enables/disables the periodic heartbeat message from the West Bridge firmware to
   the processor. The heartbeat message is left disabled by default as it can lead to a fall
   in performance on the P port interface.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_CONTROL_FIRMWARE_HEARTBEAT                       (1)

/* Summary
   This command requests the West Bridge firmware to place the device in standby mode.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 0
    * Request code = 2

   D0
   Bit 0: 1 - Enable P-port as Wakeup Source
          0 - Disable P-port as Wakeup Source
   Bit 1: 1 - Enable UART as Wakeup Source
          0 - Disable UART as Wakeup Source
   Bit 2: 1 - Enable USB VBUS as Wakeup Source
          0 - Disable USB VBUS as Wakeup Source
   Bit 3: 1 - Enable USB Bus Activity as Wakeup Source
          0 - Disable USB Bus Activity as Wakeup Source
   Bit 4: 1 - Enable OTG ID as Wakeup Source
          0 - Disable OTG ID as Wakeup Source
   Bit 5: 1 - Enable SDIO as Wakeup Source
          0 - Disable SDIO as Wakeup Source
   Bit 6: 1 - Enable Storage as Wakeup Source
          0 - Disable Storage as Wakeup Source

   D1
   Bit 1: 1 - Wakeup when the CTS line of UART goes high
          0 - Wakeup when the CTS line of UART goes low
   Bit 2: 1 - Wakeup when the VBUS goes high
          0 - Wakeup when the VBUS goes low

   Description
   This command is sent by the processor to the West Bridge to request the device to
   be placed in the low power standby state.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_ENTER_STANDBY_MODE                               (2)

/* Summary
   Requests the firmware to go into suspend mode.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 0
    * Request code = 3

   D0
   Bit 0: 1 - Enable P-port source to wakeup system from suspend.
          0 - Disable P-port source to wakeup system from suspend.
   Bit 1: 1 - Enable UART source to wakeup system from suspend.
          0 - Disable UART source to wakeup system from suspend.
   Bit 2: 1 - Enable USB VBUS source to wakeup system from suspend.
          0 - Disable USB VBUS source to wakeup system from suspend.
   Bit 3: 1 - Enable USB Bus Activity to wakeup system from suspend.
          0 - Disable USB Bus Activity to wakeup system from suspend.
   Bit 4: 1 - Enable OTG ID to wakeup system from suspend.
          0 - Disable OTG ID to wakeup system from suspend.
   Bit 5: 1 - Enable SDIO source to wakeup system from suspend.
          0 - Disable SDIO source to wakeup system from suspend.
   Bit 6: 1 - Enable Storage source to wakeup system from suspend.
          0 - Disable Storage source to wakeup system from suspend.

   D1
   Bit 1: 1 - Wakeup when the CTS line of UART goes high
          0 - Wakeup when the CTS line of UART goes low
   Bit 2: 1 - Wakeup when the VBUS goes high
          0 - Wakeup when the VBUS goes low

   Other Bits - Zero

   Description
   This command is sent by the processor to the West Bridge to request the device
   to be placed in suspend mode. The firmware will complete any pending/cached storage
   operations before going into the low power state.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_ENTER_SUSPEND_MODE                               (3)

/* Summary
   Configure the IO connections of the West Bridge device.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   4

   MailBox0
    * Context = 0
    * Request code = 4

   D0
   Bits  8 - 15 : P-port configuration value
   Bit   0      : Carkit enable flag

   D1
   Bits  8 - 15 : S0 port mode selection
   Bits  0 -  7 : S1 port mode selection
                  * 0 = 8 bit MMC mode
                  * 1 = 4 bit SD, SDIO mode
                  * 2 = DQ mode (part of P-port)
                  * 3 = GPIO mode (not P or S port)

   D2
   Bit   0      : I2C enable flag
   Bit   1      : UART enable flag
   Bit   2      : SPI enable flag
   Bit   3      : I2S enable flag

   D3-D6
   Simple GPIO selection, GPIO[0] up to GPIO[63].

   D7-D10
   Complex GPIO selection, GPIO[0] up to GPIO[63].

   Responses
    * CY_RESP_SUCCESS_FAILURE

   Description
   This request is sent to configure the IO matrix of the West Bridge device
   to enable/disable the peripherals of interest.
 */
#define CY_RQT_IO_CONFIGURE                                     (4)

/******************************************************************************/

/*@@Device events
    Summary
    The general events from firmware to API include:
     * CY_EVT_INITIALIZATION_COMPLETE
     * CY_EVT_OUT_OF_SUSPEND
     * CY_EVT_DEBUG_MESSAGE
     * CY_EVT_FW_HEARTBEAT
 */

/* Summary
   This event indicates that the firmware is up and ready for communications with the
   P port processor.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 0
    * Request Code = 128

   D0
   Bits  8 - 15 : Major Version
   Bits  0 -  7 : Minor Version

   D1
   Bits  8 - 15 : Patch level
   Bits  0 -  7 : Unused

   D2
   Bits  0 - 15 : Build number

   Description
   When the West Bridge firmware is loaded it being by performing initialization.  Initialization must
   be complete before West Bridge is ready to accept requests from the P port processor.  This request
   is sent from West Bridge to the P port processor to indicate that initialization is complete.
 */
#define CY_EVT_INITIALIZATION_COMPLETE                          (128)

/* Summary
   Indicates that the device has left suspend mode.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 0
    * Request code = 129

    D0 - Wakeup source responsible for waking Benicia from suspend mode.

   Description
   This message is sent by the West Bridge to the Processor to indicate that the device
   has woken up from suspend mode, and is ready to accept new requests.
 */
#define CY_EVT_OUT_OF_SUSPEND                                   (129)

/* Summary
   A debug message output from the West Bridge firmware.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 0
    * Request code = 130

   D0,D1,D2
   Debug state values

   Description
   This request is a general purpose logging mechanism for the West Bridge firmware.
   Firmware can send out a set of 6 byte values to the processor for tracking.
 */
#define CY_EVT_DEBUG_MESSAGE                                    (130)

/* Summary
   A heartbeat message sent from the West Bridge firmware.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 0
    * Request code = 131

   Description
   This request is sent by the West Bridge firmware to indicate that it is functioning
   correctly. The periodicity of the message is set at about once every 10 ms.
 */
#define CY_EVT_FW_HEARTBEAT                                     (131)

/******************************************************************************/

/*@@Device responses
    Summary
    The device responses from the device include:
     * CY_RESP_FIRMWARE_VERSION
 */

/* Summary
   This response indicates success and contains the firmware version number, media types supported
   by the firmware and release/debug mode information.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 0
    * Response Code = 4

   D0
   Bits  8 - 15 : Major Version
   Bits  0 -  7 : Minor Version

   D1
   Bits  8 - 15 : Patch level
   Bits  0 -  7 : Unused

   D2
   Bits  0 - 15 : Build number

   Description
   This response is sent to return the firmware version number to the requestor.
 */
#define CY_RESP_FIRMWARE_VERSION                                (4)

/******************************************************************************/

/* Summary
   The storage context is used for requests, events and responses relating to the
   storage devices connected to West Bridge.
 */
#define CY_RQT_STORAGE_RQT_CONTEXT                              (1)

/* Summary
   Macros to extract the port, device and unit fields from the unified 16 bit
   address used in the MailBox messages.
 */
#define cy_wb_stor_get_port_from_address(addr)        (cy_wb_stor_port_t)(((addr) >> 12) & 0x0F)
#define cy_wb_stor_get_dev_from_address(addr)         (uint32_t)(((addr) >> 8) & 0x0F)
#define cy_wb_stor_get_unit_from_address(addr)        (uint8_t)((addr) & 0xFF)

/* Summary
   Create the storage volume address by combining the port, device and unit
   numbers.
 */
extern uint16_t
cy_wb_storage_create_address (
        cy_wb_stor_port_t port,            /* Port number. */
        uint32_t       device,          /* Device number. */
        uint8_t        unit             /* Unit (partition) number. */
        );

/*@@Storage requests
    Summary
    The storage commands include:
     * CY_RQT_START_STORAGE
     * CY_RQT_STOP_STORAGE
     * CY_RQT_SD_INTERFACE_CONTROL
     * CY_RQT_STOR_SET_OWNER
     * CY_RQT_STOR_GET_OWNER
     * CY_RQT_QUERY_PORT
     * CY_RQT_QUERY_DEVICE
     * CY_RQT_QUERY_UNIT
     * CY_RQT_STOR_READ_P0_U0
     * CY_RQT_STOR_READ_P0_U1
     * CY_RQT_STOR_READ_P0_U2
     * CY_RQT_STOR_READ_P0_U3
     * CY_RQT_STOR_READ_P1_U0
     * CY_RQT_STOR_READ_P1_U1
     * CY_RQT_STOR_READ_P1_U2
     * CY_RQT_STOR_READ_P1_U3
     * CY_RQT_STOR_WRITE_P0_U0
     * CY_RQT_STOR_WRITE_P0_U1
     * CY_RQT_STOR_WRITE_P0_U2
     * CY_RQT_STOR_WRITE_P0_U3
     * CY_RQT_STOR_WRITE_P1_U0
     * CY_RQT_STOR_WRITE_P1_U1
     * CY_RQT_STOR_WRITE_P1_U2
     * CY_RQT_STOR_WRITE_P1_U3
     * CY_RQT_SD_REGISTER_READ
     * CY_RQT_PARTITION_STORAGE
     * CY_RQT_PARTITION_ERASE
     * CY_RQT_ERASE_BLOCKS
     * CY_RQT_ABORT_P2S_XFER
 */

/* Summary
   This command requests initialization of the storage stack.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request Code = 16

   D0
   Bits  8 - 15 : Storage write socket number
   Bits  0 -  7 : Storage read socket number

   Responses
    * CY_RESP_SUCCESS_FAILURE

   Description
   This command is required before any other storage related command
   can be send to the West Bridge firmware.
 */
#define CY_RQT_START_STORAGE                                    (16)

/* Summary
   This command requests shutdown of the storage stack.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request Code = 17

   Responses
    * CY_RESP_SUCCESS_FAILURE

   Description
   Shut down the storage stack. This request is made in preparation to the
   West Bridge device being shut down or suspended.
 */
#define CY_RQT_STOP_STORAGE                                     (17)

/* Summary
   This request is sent by the Processor to enable/disable the handling of SD card
   detection and SD card write protection by the firmware.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   2

   MailBox0
    * Context = 2
    * Request code = 18

   D0
   Bits  8 - 15 : GPIO used for voltage selection on S1 port.
   Bits  0 -  7 : GPIO used for voltage selection on S0 port.

   D1
   Bit       10 : Polarity of voltage control GPIO for S1.
   Bit        9 : Enable write protect check on S1 port.
   Bit        8 : Enable card detection on S1 port.
   Bit        2 : Polarity of voltage control GPIO for S0.
   Bit        1 : Enable write protect check on S0 port.
   Bit        0 : Enable card detection on S0 port.

   Description
   This request is sent by the Processor to enable/disable the handling of SD card
   detection and SD card write protection by the firmware.
 */
#define CY_RQT_SD_INTERFACE_CONTROL                             (18)

/* Summary
   This command sets the ownership of the specified storage parition(s).

   Direction
   P Port Processor -> FX3S

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request Code = 26

    D0
    Bits 8 - 15 : Bitmap of ownership for port 1 partitions
                  0 - BMC
                  1 - USB
    Bits 0 - 7  : Bitmap of ownership for port 0 partitions
                  0 - BMC
                  1 - USB

   Responses
    * CY_RESP_SUCCESS_FAILURE
    * CY_RESP_SET_OWNER
 */
#define CY_RQT_STOR_SET_OWNER                                   (19)

/* Summary
   Get the current ownership for the storage partitions.

   Direction
   P Port Processor -> FX3S

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request code = 75

   Responses
    * CY_RESP_SET_OWNER

   Description
   This command gets the current ownership of the storage partitions.
 */
#define CY_RQT_STOR_GET_OWNER					(20)

/* Summary
   This command queries the number and type of devices connected on a storage
   port.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request Code = 21

   D0
   Bits 12 - 15 : Port index
   Bits  0 - 11 : Not Used

   Responses
    * CY_RESP_PORT_DESCRIPTOR
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_QUERY_PORT                                       (21)

/* Summary
   This command queries a given device to determine information about the number
   of logical units (partitions) on the given device.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request Code = 22

   D0
   Bits 12 - 15 : Port index
   Bits  8 - 11 : Zero based device index
   Bits  0 -  7 : Not Used

   Responses
    * CY_RESP_DEVICE_DESCRIPTOR
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_QUERY_DEVICE                                     (22)

/* Summary
   This command queries a given device to determine information about the size
   and location of a logical unit located on a physical device.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request Code = 23

   D0
   Bits 12 - 15 : Port index
   Bits  8 - 11 : Zero based device index
   Bits  0 -  7 : Zero based unit index

   Responses
    * CY_RESP_UNIT_DESCRIPTOR
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_QUERY_UNIT                                       (23)

/* Summary
   Read commands targeted to specific storage units.

   Direction
   P Port Processor -> FX3S

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request code = 24 - 31

   D0
   Number of blocks

   D1
   Lower 16 bits of block address

   D2
   Upper 16 bits of block address

   Responses
    * CY_RESP_SUCCESS_FAILURE

   Description
   This command is used to request to read the data from a storage partition.
 */
#define CY_RQT_STOR_READ_P0_U0                              (24)
#define CY_RQT_STOR_READ_P0_U1                              (25)
#define CY_RQT_STOR_READ_P0_U2                              (26)
#define CY_RQT_STOR_READ_P0_U3                              (27)
#define CY_RQT_STOR_READ_P1_U0                              (28)
#define CY_RQT_STOR_READ_P1_U1                              (29)
#define CY_RQT_STOR_READ_P1_U2                              (30)
#define CY_RQT_STOR_READ_P1_U3                              (31)

/* Summary
   Write commands targeted to specific storage units.

   Direction
   P Port Processor -> FX3S

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request code = 32 - 39

   D0
   Number of blocks

   D1
   Lower 16 bits of block address

   D2
   Upper 16 bits of block address

   Responses
    * CY_RESP_SUCCESS_FAILURE

   Description
   This command is used to request to write new data into a storage partition.
 */
#define CY_RQT_STOR_WRITE_P0_U0                             (32)
#define CY_RQT_STOR_WRITE_P0_U1                             (33)
#define CY_RQT_STOR_WRITE_P0_U2                             (34)
#define CY_RQT_STOR_WRITE_P0_U3                             (35)
#define CY_RQT_STOR_WRITE_P1_U0                             (36)
#define CY_RQT_STOR_WRITE_P1_U1                             (37)
#define CY_RQT_STOR_WRITE_P1_U2                             (38)
#define CY_RQT_STOR_WRITE_P1_U3                             (39)

/* Summary
   Request from the processor to read a register on the SD card, and return the
   contents.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request code = 40

   D0
   Bits 12 - 15 : Port index
   Bits  8 - 11 : Zero based device index
   Bits  0 -  7 : Type of register to read

   Description
   This request is sent by the processor to instruct the West Bridge to read a register
   on the SD card, and send the contents back through the CY_RESP_SD_REGISTER_DATA
   response.
 */
#define CY_RQT_SD_REGISTER_READ                                 (40)

/* Summary
   Divide a storage device into two partitions (logical units).

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request code = 41

   D0
   Bits 12 - 15 : Port number
   Bits  8 - 11 : Device number
   Bits  0 -  7 : Not used

   D1
   Size of partition 0 (MS word)

   D2
   Size of partition 0 (LS word)

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_PARTITION_STORAGE                                (41)

/* Summary
   Remove the partition table and unify all partitions on a storage device.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request code = 42

   D0
   Bits 12 - 15 : Port number
   Bits  8 - 11 : Device number

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_PARTITION_ERASE                                  (42)

/* Summary
   Request to erase a set of blocks on a storage partition.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   2

   MailBox0
    * Context = 2
    * Request code = 43

   D0
   Bits 12 - 15 : Port index
   Bits  8 - 11 : Zero based device index
   Bits  0 -  7 : Zero based unit index

   D1
   Upper 16 bits of start erase unit

   D2
   Lower 16 bits of start erase unit

   D3
   Number of erase units to erase.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_ERASE_BLOCKS                                     (43)

/* Summary
   This command cancels the processing of a P2S operation in firmware.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 1
    * Request Code = 44

   Responses
    * CY_RESP_SUCCESS_FAILURE

   Description
   This command asks the firmware to abort any ongoing P2S data transfers
   triggered through the API.
 */
#define CY_RQT_ABORT_P2S_XFER                                   (44)

/******************************************************************************/

/*@@Storage Events
    Summary
    The storage related events include:
     * CY_EVT_MEDIA_CHANGED
     * CY_EVT_FIRMWARE_CLAIM
     * CY_EVT_FIRMWARE_RELEASE
 */

/* Summary
   This event is sent when the West Bridge device detects a change in the status
   of the media.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request Code = 136

   D0
   Bits 12 - 15 : Port index
   Bits  8 - 11 : Device index

   D1
   Bit       0  : Action
             0  = Inserted
             1  = Removed

    Description
    When the media manager detects the insertion or removal of a media from the West Bridge
    port, this request is sent from the West Bridge device to the P Port processor to inform
    the processor of the change in status of the media.  This request is sent for both an
    insert operation and a removal operation.
 */
#define CY_EVT_MEDIA_CHANGED                                    (136)

/* Summary
   This event is sent when the USB module wishes to claim storage partitions.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request Code = 137

   D0
   Bits  8 - 15 : Units that need to be released on port 1.
   Bits  0 -  7 : Units that need to be released on port 0.

   Description
   When the USB cable is attached to the West Bridge device, West Bridge will enumerate the storage
   devices per the USB initialization of West Bridge.  In order for West Bridge to respond to requests
   received via USB for the mass storage devices, the USB module must claim the storeage.  This
   request is a request to the P port processor to release the storage partitions.  The storage will
   not be visible on the USB host, until it has been released by the processor.
 */
#define CY_EVT_FIRMWARE_CLAIM                                   (137)

/* Summary
   This event is sent when the storage device has been released by USB, and is ready
   for access by the processor.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Request Code = 138

   D0
   Bits  8 - 15 : Units that have been claimed on port 1.
   Bits  0 -  7 : Units that have been claimed on port 0.

   Description
   When the P port asks for control of a particular unit, West Bridge may be able to release the
   storage immediately.  West Bridge may also need to complete the flush of buffers before releasing
   the unit.  In the latter case, West Bridge will indicated a release is not possible immediately
   and West Bridge will send this event to the P port when the release has been completed.
 */
#define CY_EVT_FIRMWARE_RELEASE                                 (138)

/******************************************************************************/

/*@@Storage responses
    Summary
    The storage responses include:
     * CY_RESP_SET_OWNER
     * CY_RESP_PORT_DESCRIPTOR
     * CY_RESP_DEVICE_DESCRIPTOR
     * CY_RESP_UNIT_DESCRIPTOR
     * CY_RESP_SD_REGISTER_DATA
 */

/* Summary
   Based on the request sent, the state of a given media was changed as indicated
   by this response.

   Direction
   FX3S -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Response Code = 33

   D0
   Bits 8 - 15 : Bitmap of current ownership for port 1 partitions
                 0 - BMC
                 1 - USB
   Bits 0 -  7 : Bitmap of current ownership for port 0 partitions
                 0 - BMC
                 1 - USB
 */
#define CY_RESP_SET_OWNER                                       (8)

/* Summary
   This response gives the number of physical devices connected to a storage
   port. For the current family of devices, this is either 0 or 1.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 2
    * Response Code = 9

   D0
   Bits 12 - 15 : Port index

   D1
   Number of devices
 */
#define CY_RESP_PORT_DESCRIPTOR                                 (9)

/* Summary
   This response gives description of a physical device.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   2

   MailBox0
    * Context = 2
    * Response Code = 10

   D0
   Bits 12 - 15 : Port index
   Bits  8 - 11 : Zero based device index
   Bits  0 -  7 : Type of media present on port
                  *  0 = NONE
                  *  1 = MMC
                  *  2 = SD Flash
                  *  3 = SDIO
                  *  4 = CEATA

   D1
   Block Size in bytes

   D2
   Bits      15 : Is device removable
   Bits       9 : Is device password locked
   Bits       8 : Is device writeable
   Bits  0 -  7 : Number Of Units

   D3
   ERASE_UNIT_SIZE high 16 bits

   D4
   ERASE_UNIT_SIZE low 16 bits
 */
#define CY_RESP_DEVICE_DESCRIPTOR                               (10)

/* Summary
   This response gives description of a unit on a physical device.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   3

   MailBox0
    * Context = 2
    * Response Code = 11

   D0
   Bits 12 - 15 : Port index
   Bits  8 - 11 : Zero based device index
   Bits  0 -  7 : Zero based unit index

   D1
   Sector Size in bytes

   D2
   Start sector Low 16 bits

   D3
   Start sector High 16 bits

   D4
   Unit Size Low 16 bits

   D5
   Unit Size High 16 bits
 */
#define CY_RESP_UNIT_DESCRIPTOR                                 (11)

/* Summary
   Contents of a register on the SD/MMC card connected to West Bridge.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   Variable

   MailBox0
    * Context = 2
    * Request code = 12

   D0
   Length of data in bytes

   D1 - Dn
   The register contents

   Description
   This is the response to a CY_RQT_SD_REGISTER_READ request.
 */
#define CY_RESP_SD_REGISTER_DATA                                (12)

/******************************************************************************/

/* Summary
   The USB context is used for messages relating to USB configuration and data
   transfers.
 */
#define CY_RQT_USB_RQT_CONTEXT                                  (2)

/*@@USB requests
    Summary
    The USB requests include:
     * CY_RQT_START_USB
     * CY_RQT_STOP_USB
     * CY_RQT_SET_CONNECT_STATE
     * CY_RQT_GET_CONNECT_STATE
     * CY_RQT_SET_USB_CONFIG
     * CY_RQT_STALL_ENDPOINT
     * CY_RQT_GET_STALL
     * CY_RQT_SET_ENDPOINT_CONFIG
     * CY_RQT_ENDPOINT_SET_NAK
     * CY_RQT_GET_ENDPOINT_NAK
     * CY_RQT_ACK_SETUP_PACKET
     * CY_RQT_USB_REMOTE_WAKEUP
     * CY_RQT_CANCEL_ASYNC_TRANSFER
     * CY_RQT_USB_ENABLE_MSC
     *
 */

/* Summary
   This command requests initialization of the USB stack.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request Code = 48

   D0
   Bits  8 - 15 : Socket to use for EP0-OUT
   Bits  0 -  7 : Socket to use for EP0-IN

   D1
   Bit        0 : Super speed (USB 3.0) enable
   Bit        1 : Use Vbatt to power UIB regulator
   Bit        2 : MSC Composite Interface Enbl/Dsbl

   Description
   This command is required before any other USB related command can be
   sent to the West Bridge firmware.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_START_USB                                        (48)

/* Summary
   This command requests shutdown of the USB stack.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request Code = 49

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_STOP_USB                                         (49)

/* Summary
   This command requests that the USB pins be connected or disconnected to/from
   the West Bridge device.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request Code = 50

   D0
   Desired Connect State
    * 0 = DISCONNECTED
    * 1 = CONNECTED

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_SET_CONNECT_STATE                                (50)

/* Summary
   This command requests the connection state of the West Bridge USB pins.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request Code = 51

   Responses
    * CY_RESP_CONNECT_STATE
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_GET_CONNECT_STATE                                (51)

/* Summary
   This request enables/disables the mass storage interface.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request Code = 52

   D0
   Storage partitions to enumerate
   Bits  0 -  7 : Bitmap of partitions on the S0 port to be enumerated
   Bits  8 - 15 : Bitmap of partitions on the S1 port to be enumerated

   D1
   Write enable settings for the mass storage LUNs
   Bits  0 -  7 : Write enables for each of the S0 units.
   Bits  8 - 15 : Write enables for each of the S1 units.

   D2
   Bits  8 - 15 : Reserved
   Bits  0 -  7 : Mass Storage Interface Number

   Description
   This indicates how enumeration should be handled.  Enumeration can be handled by the
   West Bridge device or by the P port processor.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_SET_USB_CONFIG                                   (52)

/* Summary
   This request stalls the given endpoint.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request Code = 53

   D0
   Endpoint Number

   D1
   Bit 0 : Stall / Clear stall on endpoint.
   Bit 1 : Clear data toggles when clearing stall.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_STALL_ENDPOINT                                   (53)

/* Summary
   This request retrieves the stall status of the requested endpoint.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request Code = 54

   D0
   Endpoint number

   Responses
    * CY_RESP_ENDPOINT_STALL
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_GET_STALL                                        (54)


/* Summary
   This request is sent from the P port processor to the West Bridge device to
   physically configure an endpoint in the device.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   2

   MailBox0
    * Context = 3
    * Request Code = 55

   D0
   Bits  8 - 15 : EP number to configure (including direction)
   Bit   7      : Enable/disable flag for the EP
   Bits  0 -  6 : P port socket to use for EP

   D1
   Bits 14 - 15 : Endpoint type
                  * 0 = Invalid
                  * 1 = Isochronous
                  * 2 = Bulk
                  * 3 = Interrupt
   Bits  0 - 13 : Maximum packet size

   D2
   Bits  0 -  7 : Maximum burst length in packets.
   Bits  8 -  9 : Mult setting for ISO endpoints.
   Bit       15 : Whether this is a high bandwidth endpoint.

   D3
   Bits  0 -  7 : Polling rate for interrupt / ISO endpoints.

   Responses
    * CY_RESP_ENDPOINT_CONFIG
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_SET_ENDPOINT_CONFIG                              (55)

/* Summary
   This request sets the NAK bit on an endpoint.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request Code = 56

   D0
   Endpoint Number

   D1
    * 1 = NAK Endpoint
    * 0 = Clear NAK

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_ENDPOINT_SET_NAK                                 (56)

/* Summary
   This request retrieves the NAK config status of the requested endpoint.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request Code = 57

   D0
   Endpoint number

   Responses
    * CY_RESP_ENDPOINT_NAK
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_GET_ENDPOINT_NAK                                 (57)

/* Summary
   This request asks West Bridge to complete the status phase for a setup request.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox
    * Context = 3
    * Request Code = 58

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_ACK_SETUP_PACKET                                 (58)

/* Summary
   Request the West Bridge to signal remote wakeup to the USB host.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request code = 59

   Description
   Request from the processor to West Bridge, to signal remote wakeup to the USB host.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_USB_REMOTE_WAKEUP                                (59)

/* Summary
   Request to cancel an asynchronous USB write from the processor side.

   Direction
   P Port processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request code = 60

   D0
   Endpoint number

   Description
   This is a request to the firmware to update internal state so that a pending
   write on an endpoint can be cancelled.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_CANCEL_ASYNC_TRANSFER                            (60)

/* Summary
   This command requests the West Bridge device to enable/disable MSC configuration 
   along with any other composite requests from external Application Processor.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request Code = 61

   D0
   Desired Connect State
    * 0 = DISABLE
    * 1 = ENABLE

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_USB_ENABLE_MSC                                (61)

/* Summary
   Request identifying the direction and size of the EP0 data transfer required.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request code = 62

   D0
   Direction of data transfer
    * 0 = OUT (read) data transfer
    * 1 = IN  (write) data transfer

   D1
   Length of the transfer in bytes

   Description
   This is a header packet that notifies the firmware about the direction and size
   of the EP0 data transfer requested by the application.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_USB_EP0_XFER                                 (62)

/******************************************************************************/

/*@@USB events
    Summary
    The USB related events from firmware to API include:
     * CY_EVT_USB_EVENT
     * CY_EVT_USB_SETUP_REQUEST
 */

/* Summary
   This event is sent to the P port processor when a USB event occurs
   and needs to be relayed to the P port.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Request Code = 144

   D0
   Event Type
    *  0 = USB Connect
    *  1 = USB Disconnect
    *  2 = USB Suspend
    *  3 = USB Resume
    *  4 = USB Reset
    *  5 = USB Set Configuration
    *  6 = USB Speed change
    *  7 = USB Set Interface
    *  8 = USB SET_SEL request
    *  9 = Reserved
    *  10 = Reserved
    *  11 = VBus detected event
    *  12 = VBus removed event

   D1
   If Event Type is USB Speed change
    * 0 = Full Speed
    * 1 = High Speed

   If Event Type is USB Set Configuration
    * The number of the configuration to use (may be zero to unconfigure)

   If Event Type is USB connect
    * Bit 0 : Connection speed
    * 1 = Super Speed
    * 0 = Legacy speed (HS/FS)
 */
#define CY_EVT_USB_EVENT                                        (144)

/* Summary
   Event from the West Bridge indicating a new USB setup request to be handled.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   2

   MailBox0
    * Context = 3
    * Request code = 145

   D0-D3
   Eight bytes of setup request

   Description
   This event indicates that the West Bridge device has received a new USB setup
   request that needs to be processed with the user application's help. The event
   data includes the 8 bytes of setup request data.
 */
#define CY_EVT_USB_SETUP_REQUEST                                (145)

/******************************************************************************/

/*@@USB responses
    Summary
    The USB responses include:
     * CY_RESP_ENDPOINT_STALL
     * CY_RESP_CONNECT_STATE
     * CY_RESP_ENDPOINT_NAK
     * CY_RESP_ENDPOINT_CONFIG
 */

/* Summary
   This response contains the stall status for the specified endpoint.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Response Code = 30

   D0
   Stall status
    * 0 = Active
    * 1 = Stalled
 */
#define CY_RESP_ENDPOINT_STALL                                  (30)

/* Summary
   This response contains the connected/disconnected state of the West Bridge USB pins.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Response Code = 31

   D0
   Connect state
    * 0 = Disconnected
    * 1 = Connected
 */
#define CY_RESP_CONNECT_STATE                                   (31)

/* Summary
   This response contains the current NAK status of a USB endpoint.

   Direction
   West Bridge -> P port processor

   Length
   1

   MailBox0
    * Context = 3
    * Response Code = 32

   D0
   The NAK status of the endpoint
   1 : NAK bit set
   0 : NAK bit clear
 */
#define CY_RESP_ENDPOINT_NAK                                    (32)

/* Summary
   This response provides the device level mapping in response to a
   CY_RQT_SET_ENDPOINT_CONFIG request.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 3
    * Response Code = 33

   D0
   Buffer size allocated for the endpoint

   D1
   Number of buffers allocated for the endpoint

   D2
   Bits  0 -  7 : USB socket number associated with the endpoint
 */
#define CY_RESP_ENDPOINT_CONFIG                                 (33)

/******************************************************************************/

/* Summary
   The LPP context is used for messages relating to the Low Performance
   Peripheral interfaces on the device.
 */
#define CY_RQT_LPP_RQT_CONTEXT                                  (5)

/*@@LPP Requests
    Summary
    The LPP related requests from the API to firmware include:
     * CY_RQT_LPP_START
     * CY_RQT_LPP_STOP
     * CY_RQT_GPIO_CONFIGURE
     * CY_RQT_GET_GPIO_STATE
     * CY_RQT_SET_GPIO_STATE
     * CY_RQT_SAMPLE_GPIO_TIMER
     * CY_RQT_PULSE_PIN
     * CY_RQT_UART_CONFIGURE
     * CY_RQT_UART_SENDBREAK
     * CY_RQT_I2S_CONFIGURE
     * CY_RQT_I2S_CONTROL
     * CY_RQT_SPI_CONFIGURE
     * CY_RQT_SPI_SET_SSN_LINE
     * CY_RQT_SPI_START_TRANSFER
     * CY_RQT_LPP_READ_BYTES
     * CY_RQT_LPP_WRITE_BYTES
     * CY_RQT_LPP_ABORT_XFER
     * CY_RQT_I2C_CONFIGURE
     * CY_RQT_I2C_READ_BYTES
     * CY_RQT_I2C_WRITE_BYTES
     * CY_RQT_I2C_START_TRANSFER
     * CY_RQT_I2C_POLL
 */

/* Summary
   Request to start the LPP module.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 92

   Description
   Request the firmware to start the LPP module functionality. All LPP
   interfaces required need to be separately initialized after this.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_LPP_START                                        (92)

/* Summary
   Request to stop the LPP module.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 93

   Description
   Request the firmware to disable all LPP interfaces and shut down the
   corresponding module.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_LPP_STOP                                         (93)

/* Summary
   Request to configure a GPIO pin as a complex GPIO.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   2

   MailBox0
    * Context = 5
    * Request code = 94

   D0
   Bits 12 - 15 : Clock to associate with the timer for this pin.
    * 0 = Fast GPIO clock
    * 1 = Slow GPIO clock
    * 2 = Standby GPIO clock
    * 3 = Advance on positive edge
    * 4 = Advance on negative edge
    * 5 = Advance on any edge
   Bits  8 - 11 : Desired mode of operation
    * See SAS for mode information
   Bits  0 -  7 : Pin number to configure

   D1
   Upper 16 bits of period for the timer.

   D2
   Lower 16 bits of period for the timer.

   D3
   Upper 16 bits of threshold value for the timer.

   D4
   Lower 16 bits of threshold value for the timer.

   Description
   Some of the West Bridge device IOs can be configured to behave as
   complex GPIOs. This request is used to define the behavior for one of
   these pins.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_GPIO_CONFIGURE                                   (94)

/* Summary
   Request to get the current state of a GPIO pin.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 95

   D0
   Bits  0 -  7 : Pin number

   Description
   Request to the firmware to configure a GPIO pin as an input, and fetch
   its current value.

   Responses
    * CY_RESP_GPIO_STATE
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_GET_GPIO_STATE                                   (95)

/* Summary
   Request to update the current state of a GPIO pin.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 96

   D0
   Bits  0 -  7 : Pin number

   D1
   Bit   8      : Output enable
   Bit   0      : State to set if output enabled

   Description
   Request to configure the given GPIO as an output pin, and to update its
   current state.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_SET_GPIO_STATE                                   (96)

/* Summary
   Request to get the current timer count for a complex GPIO.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 97

   D0
   Bits  0 -  7 : Pin number

   Description
   Request to get the current timer count associated with a complex GPIO.

   Responses
    * CY_RESP_GPIO_TIMER_VALUE
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_SAMPLE_GPIO_TIMER                                (97)

/* Summary
   Request to drive a pulse out on a selected GPIO pin.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 98

   D0
   Bit   8      : Whether the pulse should be immediate
   Bits  0 -  7 : Pin number

   D1
   Upper word of pulse duration in timer ticks.

   D2
   Lower word of pulse duration in timer ticks.

   Description
   Request the firmware to cause a pulse to be driven on a selected GPIO
   pin. The duration of the pulse in timer ticks is specified as parameter.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_PULSE_PIN                                        (98)

/* Summary
   Request to configure the UART module.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   2

   MailBox0
    * Context = 5
    * Request code = 99

   D0
   Bit  15      : Enable UART transmission
   Bit  14      : Enable pass-through UART transmission
   Bits  8 - 13 : P-port socket for pass-through transmission
   Bit   7      : Enable UART reception
   Bit   6      : Enable pass-through UART reception
   Bits  0 -  5 : P-port socket for pass-through reception

   D1
   Upper 16 bits of baud rate

   D2
   Lower 16 bits of baud rate

   D3
   Bit  15      : Whether UART should be initialized in Block (DMA) mode
   Bits  8 - 14 : Desired parity setting
    * 0 = No parity
    * 1 = Even parity
    * 2 = Odd parity
    * 3 = Sticky parity - 0
    * 4 = Sticky parity - 1
   Bit   7      : Flow control enable flag.
   Bits  0 -  6 : Number of stop bits

   D4
   Bits  0 - 15 : DMA transfer block length.

   Description
   Request the configure the UART parameters and enable the UART.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_UART_CONFIGURE                                   (99)

/* Summary
   Configure the I2S interface.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   2

   MailBox0
    * Context = 5
    * Request code = 100

   D0
   Upper 16 bits of sample clock rate.

   D1
   Lower 16 bits of sample clock rate.

   D2
   Bit  15      : Whether to use LSB first bit ordering
   Bit  14      : Stereo mode enable
   Bits  8 - 11 : Sample bit width
    * 0 = 8 bit
    * 1 = 16 bit
    * 2 = 18 bit
    * 3 = 24 bit
    * 4 = 32 bit
   Bit   7      : Enable continuous transmission mode.
   Bits  0 -  6 : Type of padding to use
    * 0 = Normal
    * 1 = Left justified
    * 2 = Right justified

   D3
   Bits  8 - 15 : P-port socket for right channel
   Bits  0 -  7 : P-port socket for left channel

   D4
   Bits  0 - 15 : DMA transfer block length.

   Description
   Set the configuration parameters for and enable the I2S interface.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_I2S_CONFIGURE                                    (100)

/* Summary
   Request to control the I2S audio output.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 101

   D0
   Bit 8 : Pause the output
   Bit 0 : Mute the output

   Description
   Control the mute and pause settings for the I2S audio stream.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_I2S_CONTROL                                      (101)

/* Summary
   Configure the SPI interface.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   3

   MailBox0
    * Context = 5
    * Request code = 102

   D0
   Bit  14      : Enable pass-through SPI transmission
   Bits  8 - 13 : P-port socket for pass-through transmission
   Bit   6      : Enable pass-through SPI reception
   Bits  0 -  5 : P-port socket for pass-through reception

   D1
   Upper 16 bits of clock rate

   D2
   Lower 16 bits of clock rate

   D3
   Bit  15      : Whether SPI master should be set in block (DMA) mode of operation.
   Bit  14      : Send LSB first
   Bits 12 - 13 : Lag time (0: 0, 1: 0.5, 2: 1, 3: 1.5 clocks)
   Bits 10 - 11 : Lead time (0: 0, 1: 0.5, 2: 1, 3: 1.5 clocks)
   Bits  8 -  9 : SPI mode of operation
   Bits  0 -  7 : Desired word width

   D4
   Bits  0 -  7 : GPIO id to be used as slave select (SSN)

   D5
   Bits  0 - 15 : DMA transfer block length.

   Description
   Configure the SPI interface parameters and enable it.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_SPI_CONFIGURE                                    (102)

/* Summary
   Set the SPI SSN (chip select line).

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 103

   D0
   Bit 0 : Set the SSN Line High

   Description
   Control the SSN line for the SPI Interface.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_SPI_SET_SSN_LINE                                 (103)

/* Summary
   Request to start a block based read/write operation to a selected
   SPI slave.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 104

   D0
   Bit  15      : Type of operation to start
    * 0 = Read
    * 1 = Write
   Bits  0 - 14 : Number of blocks of data to transfer

   Description
   Request the firmware to initialise a block-based read/write operation
   from/to a selected LPP slave. The slave address and internal address
   parameters are passed in as parameters, along with the count of
   number of blocks to read/write.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_SPI_START_TRANSFER                               (104)

/* Summary
   Request to read a few bytes from a selected LPP peripheral.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 105

   D0
   Bits  8 - 15 : Target LPP peripheral
    * 0 = UART
    * 1 = I2C
    * 2 = I2S
    * 3 = SPI
   Bits  0 -  7 : Number of bytes of data to read.

   Description
   Request the firmware to read a few bytes of data from a selected LPP
   peripheral (using register mode), and return the data read.

   Responses
    * CY_RESP_LPP_DATA
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_LPP_READ_BYTES                                   (105)

/* Summary
   Request to write a few bytes to a selected LPP peripheral.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 106

   D0
   Bits  8 - 15 : Target LPP peripheral
    * 0 = UART
    * 1 = I2C
    * 2 = I2S
    * 3 = SPI
   Bits  0 -  7 : Number of bytes of data to write.

   D1
   Bits  0 -  7 : Byte 0 of the data
   Bits  8 - 15 : Byte 1 of the data

   D2
   Bits  0 -  7 : Byte 2 of the data
   Bits  8 - 15 : Byte 3 of the data

   Description
   Request the firmware to write a few bytes of data to a selected LPP
   peripheral.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_LPP_WRITE_BYTES                                  (106)

/* Summary
   Request to abort the ongoing transfers from/to an LPP peripheral.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 107

   D0
   Bits  8 - 15 : Target LPP peripheral
    * 0 = UART
    * 1 = I2C
    * 2 = I2S
    * 3 = SPI

   D1
   Bits  0 -  7 : P-port socket number to abort

   Description
   Request the firmware to abort all ongoing to transfers to a selected
   LPP peripheral.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_LPP_ABORT_XFER                                   (107)

/* Summary
   Configure the I2C peripheral interface.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   2

   MailBox0
    * Context = 5
    * Request code = 108

   D0
   Bit  15      : Enable debug mode functionality
   Bit  14      : Enable pass-through I2C transmission
   Bits  8 - 13 : P-port socket for pass-through transmission
   Bit   6      : Enable pass-through I2C reception
   Bits  0 -  5 : P-port socket for pass-through reception

   D1
   Upper 16 bits of clock rate

   D2
   Lower 16 bits of clock rate

   D3
   Bits  0 - 15 : DMA transfer block length in bytes.

   Description
   Set the configuration parameters for the I2C master interface on the
   West Bridge device.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_I2C_CONFIGURE                                    (108)

/* Summary
   Request to read a few bytes of data from a selected I2C slave.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   2

   MailBox0
    * Context = 5
    * Request code = 109

   D0
   Bits  0 -  7 : Number of bytes of data to read
   Bits  8 - 15 : Length of I2C preamble

   D1-D4
   Slave address value (first byte to last byte)

   D5
   Bits  0 -  7 : Bit mask representing start bit locations
   Bits  8 - 15 : Bit mask representing stop bit locations

   Description
   Request the firmware to read a few bytes of data from a selected
   I2C slave address, and return the data over the MailBoxes.

   Responses
    * CY_RESP_LPP_DATA
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_I2C_READ_BYTES                                   (109)

/* Summary
   Request to write a few bytes of data to a selected I2C slave.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   3

   MailBox0
    * Context = 5
    * Request code = 110

   D0
   Bits  0 -  7 : Number of bytes of data to write
   Bits  8 - 15 : Length of I2C preamble

   D1-D4
   Slave address value (first byte to last byte)

   D5
   Bits  0 -  7 : Bit mask representing start bit locations
   Bits  8 - 15 : Bit mask representing stop bit locations

   D6-D7
   Data values to write (MSB to LSB)

   Description
   Request the firmware to write a few bytes of data to a selected
   I2C slave address.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_I2C_WRITE_BYTES                                  (110)

/* Summary
   Request to start a block based read/write operation to a selected
   LPP slave.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   3

   MailBox0
    * Context = 5
    * Request code = 111

   D0
   Bit  15      : Type of operation to start
    * 0 = Read
    * 1 = Write
   Bits  0 - 14 : Number of blocks of data to transfer

   D1
   Bits  0 -  7 : Length of I2C preamble

   D2-D5
   Slave address value (first byte to last byte)

   D6
   Bits  0 -  7 : Bit mask representing start bit locations
   Bits  8 - 15 : Bit mask representing stop bit locations

   Description
   Request the firmware to initialise a block-based read/write operation
   from/to a selected LPP slave. The slave address and internal address
   parameters are passed in as parameters, along with the count of
   number of blocks to read/write.

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_I2C_START_TRANSFER                               (111)

/* Summary
   Poll the I2C slave until it returns the desired handshake.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   3

   MailBox0
    * Context = 5
    * Request code = 112

   D0
   Bit   0      : Handshake to wait for (0 = NAK, 1 = ACK)
   Bits  8 - 15 : Length of I2C preamble

   D1-D4
   Slave address value (first byte to last byte)

   D5
   Bits  0 -  7 : Bit mask representing start bit locations
   Bits  8 - 15 : Bit mask representing stop bit locations

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_I2C_POLL                                         (112)

/* Summary
   Send a break condition to the peer UART peripheral.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 113

   Responses
    * CY_RESP_SUCCESS_FAILURE
 */
#define CY_RQT_UART_SENDBREAK                                   (113)

/******************************************************************************/

/*@@LPP events
    Summary
    The LPP related events from firmware to API include:
     * CY_EVT_GPIO_CHANGE
     * CY_EVT_UART_ERROR
     * CY_EVT_I2S_ERROR
     * CY_EVT_SPI_ERROR
     * CY_EVT_I2C_ERROR
 */

/* Summary
   Notification of a GPIO event of interest.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 168

   D0
   Bits  8 - 15 : GPIO pin id
   Bit   0      : Type of transition
    * 0 = Falling edge
    * 1 = Rising edge

   D1
   Upper word of time interval since last event

   D2
   Lower word of time interval since last event

   Description
   This event is sent when the firmware detects a change of interest
   in one of GPIOs that have been configured for interrupt/measurement.
 */
#define CY_EVT_GPIO_CHANGE                                      (168)

/* Summary
   Notification of an UART event.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 169

   D0
   Event type

   Description
   This event is sent when a UART function related error is detected
   by the firmware. The type of error is sent as part of the event data.
 */
#define CY_EVT_UART_ERROR                                       (169)

/* Summary
   Notification of an I2S event.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 170

   D0
   Event type

   Description
   This event is sent when a I2S function related error is detected
   by the firmware. The type of error is sent as part of the event data.
 */
#define CY_EVT_I2S_ERROR                                        (170)

/* Summary
   Notification of a SPI event.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 171

   D0
   Event type

   Description
   This event is sent when a SPI function related error is detected
   by the firmware. The type of error is sent as part of the event data.
 */
#define CY_EVT_SPI_ERROR                                        (171)

/* Summary
   Notification of an I2C event.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 172

   D0
   Event type

   Description
   This event is sent when a I2C function related error is detected
   by the firmware. The type of error is sent as part of the event data.
 */
#define CY_EVT_I2C_ERROR                                        (172)

/******************************************************************************/

/*@@LPP response
    Summary
    The LPP specific response types include:
     * CY_RESP_GPIO_STATE
     * CY_RESP_GPIO_TIMER_VALUE
     * CY_RESP_LPP_DATA
 */

/* Summary
   Response to a Get GPIO state request.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 44

   D0
   Bits  0 -  7 : GPIO pin id

   D1
   Bit   8      : Whether the pin is actively driven
   Bit   0      : Current value of the pin (0/1)

   Description
   This message is sent in response to a CY_RQT_GET_GPIO_STATE request.
 */
#define CY_RESP_GPIO_STATE                                      (44)

/* Summary
   This response indicates the current timer value associated with a GPIO.

   Direction
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 45

   D0
   Bits  0 -  7 : GPIO pin id

   D1
   Upper word of timer value

   D2
   Lower word of timer value

   Description
   This message is sent in response to a CY_RQT_SAMPLE_GPIO_TIMER request,
   and represents the current timer value associated with a GPIO.
 */
#define CY_RESP_GPIO_TIMER_VALUE                                (45)

/* Summary
   Response to LPP read bytes request.

   Direction
   P Port Processor -> West Bridge
   West Bridge -> P Port Processor

   Length (in transfers)
   1

   MailBox0
    * Context = 5
    * Request code = 46

   D0
   Bits  8 - 15 : Peripheral from which the data has been read
   Bits  0 -  7 : Number of bytes of valid data

   D1-D2
   The data bytes that have been read (MSB to LSB)

   Description
   This message is sent in response to CY_RQT_LPP_READ_BYTES or
   CY_RQT_I2C_READ_BYTES request, and contains the data values that
   have been read.
 */
#define CY_RESP_LPP_DATA                                        (46)

/******************************************************************************/

/* Summary
   The DBG context is reserved for API testing purposes.
 */
#define CY_RQT_DBG_RQT_CONTEXT                                  (6)

/*@@Debug requests
    Summary
    The debug requests from API to firmware include:
     * CY_RQT_DBG_ERROR_CREATE
     * CY_RQT_DBG_ERROR_CANCEL
 */


/* Summary
   This command requests West Bridge firmware to create a mailbox communication
   error on a subsequent API request.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 6
    * Request Code = 116

   D0
   Bits  0 -  3 : Mailbox context on which error is to be simulated.
    * 0 = General context
    * 1 = Storage context
    * 3 = USB context
    * 5 = LPP context
    * 6 = Invalid
    * 7 = Any of the valid contexts (0 to 5)

   D1
   Type of error to simulate
    * 0 = No error
    * 1 = Timeout error
    * 2 = Bad response

   D3
   Whether the error simulation should be repeated.
    * 0 = Simulate error once
    * 1 = Simulate error until cancelled

   Description
   This request is used to request firmware to simulate error conditions, so
   that the API response to these errors can be tested.
 */
#define CY_RQT_DBG_ERROR_CREATE                                 (116)

/* Summary
   This command requests West Bridge firmware to stop simulating mailbox
   communication errors on a specified context.

   Direction
   P Port Processor -> West Bridge

   Length (in transfers)
   1

   MailBox0
    * Context = 6
    * Request Code = 117

   D0
   Bits  0 -  3 : Mailbox context on which error is to be simulated.
    * 0 = General context
    * 1 = Storage context
    * 3 = USB context
    * 5 = LPP context
    * 6 = Invalid
    * 7 = All contexts

   Description
   This request is used to request firmware to stop simulating mailbox
   communication errors on one or all of the contexts.
 */
#define CY_RQT_DBG_ERROR_CANCEL                                 (117)

/******************************************************************************/

/* Summary
   Number of contexts used for MailBox communication.
 */
#define CY_RQT_CONTEXT_COUNT                                    (7)

#endif /* _INCLUDED_CYWBPROTOCOL_H_ */

