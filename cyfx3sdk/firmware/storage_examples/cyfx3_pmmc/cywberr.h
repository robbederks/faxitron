/* Cypress West Bridge API header file (cywberr.h)
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

#ifndef _INCLUDED_CYWBERR_H_
#define _INCLUDED_CYWBERR_H_

/*@@West Bridge Errors
  Summary
  This section lists the error codes for the West Bridge API.

  Description
  All of the West Bridge API functions return a status value that indicates
  the execution status of the requested operation.  The error codes listed here
  are used to indicate various error conditions that may be encountered while
  executing the West Bridge APIs.
 */

/* Summary
   The function completed sucessfully.
 */
#define CY_WB_SUCCESS                                                           (0)

/* Summary
   This is a synonym for the CY_WB_SUCCESS return status.
 */
#define CY_WB_ERROR_SUCCESS                                                     (0)

/* Summary
   A function trying to acquire a resource was unable to do so.

   Description
   This code indicates that a resource that the API was trying to claim
   could not be claimed.
 */
#define CY_WB_ERROR_NOT_ACQUIRED                                                (1)

/* Summary
   A function trying to acquire a resource was unable to do so.

   Description
   The West Bridge API provides the capability to assign the storage media to
   either the West Bridge device or the USB port.  This error indicates the
   P port was trying to release a storage media and was not able to do
   so.  This generally means it was not owned by the P port processor.
 */
#define CY_WB_ERROR_NOT_RELEASED                                                (2)

/* Summary
   The West Bridge firmware is not loaded.

   Description
   Most of the API functions that are part of the West Bridge API rely on firmware
   running on the West Bridge device.  This error code is returned when one of
   these functions is called and the firmware has not yet been loaded.
 */
#define CY_WB_ERROR_NO_FIRMWARE                                                 (3)

/* Summary
   A timeout occurred waiting on a response from the West Bridge device

   Description
   When requests are made of the West Bridge device, a response is expected
   within a given timeframe.  If a response is not received within the
   given timeframe, a timeout error occurs.
 */
#define CY_WB_ERROR_TIMEOUT                                                     (4)

/* Summary
   A request to download firmware was made while not in the CONFIG mode

   Description
   Firmware is downloaded via the cy_wb_dev_download_firmware() function.  This
   function can only be called while in the CONFIG mode.  This error indicates
   that the cy_wb_dev_download_firmware() call was made while not in the CONFIG
   mode.
 */
#define CY_WB_ERROR_NOT_IN_CONFIG_MODE                                          (5)

/* Summary
   This error is returned if the firmware size specified is too invalid.

   Description
   If the size of the firmware to be downloaded into West Bridge is invalid, this
   error is issued.  Invalid firmware sizes are those greater than 24K or a
   size of zero.
 */
#define CY_WB_ERROR_INVALID_SIZE                                                (6)

/* Summary
   This error is returned if a request is made to acquire a resource that has
   already been acquired.

   Description
   This error is returned if a request is made to acquire a resource that has
   already been acquired.
 */
#define CY_WB_ERROR_RESOURCE_ALREADY_OWNED                                      (7)

/* Summary
   This error is returned when a request is made for a device that does not exist

   Description
   This error is returned when a request is made that references a storage device
   that does not exist.  This error is returned when the device index is not present
   in the current system, or if the device index exceeds 15.
 */
#define CY_WB_ERROR_NO_SUCH_DEVICE                                              (8)

/* Summary
   This error is returned when a request is made for a unit that does not exist

   Description
   This error is returned when a request is made that references a storage unit
   that does not exist.  This error is returned when the unit index is not present
   in the current system, or if the unit index exceeds 255.
 */
#define CY_WB_ERROR_NO_SUCH_UNIT                                                (9)

/* Summary
   This error is returned when a request is made for a block that does not exist

   Description
   This error is returned when a request is made that references a storage block
   that does not exist.  This error is returned when the block address reference
   an address beyond the end of the unit selected.
 */
#define CY_WB_ERROR_INVALID_BLOCK                                               (10)

/* Summary
   This error is returned when West Bridge is already in the standby state
   and an attempt is made to put West Bridge into this state again.

   Description
   This error is returned when West Bridge is already in the standby state
   and an attempt is made to put West Bridge into this state again.
 */
#define CY_WB_ERROR_ALREADY_IN_STANDBY                                          (11)

/* Summary
   This error is returned when a module is being started that has
   already been started.

   Description
   This error is returned when a module is being started and that module
   has already been started.  This error does not occur with the cy_wb_storage_start()
   or cy_wb_usb_start() functions as the storage and USB modules are reference counted.
 */
#define CY_WB_ERROR_ALREADY_RUNNING                                             (12)

/* Summary
   This error is returned when a module is being stopped that has
   already been stopped.

   Description
   This error is returned when a module is being stopped and that module
   has already been stopped.  This error does not occur with the cy_wb_storage_stop()
   or cy_wb_usb_stop() functions as the storage and USB modules are reference counted.
 */

#define CY_WB_ERROR_NOT_RUNNING                                                 (13)

/* Summary
   This error is returned when the caller tries to release a media that has already been
   released.

   Description
   This error is returned when the caller tries to release a media that has already been
   released.
 */
#define CY_WB_ERROR_MEDIA_NOT_CLAIMED                                           (14)

/* Summary
   This error is returned when canceling trying to cancel an asynchronous operation when
   an async operation is not pending.

   Description
   This error is returned when a call is made to a function to cancel an asynchronous operation
   and there is no asynchronous operation pending.
 */
#define CY_WB_ERROR_NO_OPERATION_PENDING                                        (15)

/* Summary
   This error is returned when an invalid endpoint number is provided to an API call.

   Description
   This error is returned when an invalid endpoint number is specified in an API call.  The
   endpoint number may be invalid because it is greater than 15, or because it was a reference
   to an endpoint that is invalid for West Bridge (2, 4, 6, or 8).
 */
#define CY_WB_ERROR_INVALID_ENDPOINT                                            (16)

/* Summary
   This error is returned when an invalid descriptor type is specified in an API call.

   Description
   This error is returned when an invalid descriptor type is specified in an API call.
 */
#define CY_WB_ERROR_INVALID_DESCRIPTOR                                          (17)

/* Summary
   This error is returned if trying to set a USB descriptor when in the P port enumeration mode.

   Description
   This error is returned if trying to set a USB descriptor when in the P port enumeration mode.
 */
#define CY_WB_ERROR_BAD_ENUMERATION_MODE                                        (18)

/* Summary
   This error is returned when the endpoint configuration specified is not valid.

   Description
   This error is returned when the endpoint configuration specified is not valid.
 */
#define CY_WB_ERROR_INVALID_CONFIGURATION                                       (19)

/* Summary
   This error is returned when the API cannot verify it is connected to an West Bridge device.

   Description
   When the API is initialized, the API tries to read the ID register from the West Bridge
   device.  The value from this ID register should match the value expected before
   communications with West Bridge are established.  This error means that the contents of
   the ID register cannot be verified.
 */
#define CY_WB_ERROR_NO_DEVICE                                                   (20)

/* Summary
   This error is returned when an API function is called and cy_wb_dev_configure()
   has not been called to configure West Bridge for the current environment.

   Description
   This error is returned when an API function is called and cy_wb_dev_configure()
   has not been called to configure West Bridge for the current environment.
 */
#define CY_WB_ERROR_NOT_CONFIGURED                                              (21)

/* Summary
   This error is returned when West Bridge cannot allocate memory required for
   internal API operations.

   Description
   This error is returned when West Bridge cannot allocate memory required for
   internal API operations.
 */
#define CY_WB_ERROR_OUT_OF_MEMORY                                               (22)

/* Summary
   This error is returned when a module is being started that has
   already been started.

   Description
   This error is returned when a module is being started and that module
   has already been started.  This error does not occur with the cy_wb_storage_start()
   or cy_wb_usb_start() functions as the storage and USB modules are reference counted.
 */
#define CY_WB_ERROR_NESTED_SLEEP                                                (23)

/* Summary
   This error is returned when an operation is attempted on a socket that has
   been disabled.

   Description
   This error is returned when an operation is attempted on a socket that has
   been disabled.
 */
#define CY_WB_ERROR_SOCKET_DISABLED                                             (24)

/* Summary
   This error is returned when a call is made to an API function when the device is in
   standby.

   Description
   When the West Bridge device is in standby, the only two API functions that can be called
   are cy_wb_dev_in_standby() and cy_wb_dev_leave_standby().  Calling any other API function
   will result in this error.
 */
#define CY_WB_ERROR_IN_STANDBY                                                  (25)

/* Summary
   This error is returned when an API call is made with an invalid handle value.

   Description
   This error is returned when an API call is made with an invalid handle value.
 */
#define CY_WB_ERROR_INVALID_HANDLE                                              (26)

/* Summary
   This error is returned when an invalid response is returned from the West Bridge device.

   Description
   Many of the API calls result in requests made to the West Bridge device.  This error occurs
   when the response from West Bridge is invalid and generally indicates that the West Bridge device
   should be reset.
 */
#define CY_WB_ERROR_INVALID_RESPONSE                                            (27)

/* Summary
   This error is returned from the callback function for any asynchronous read or write
   request that is canceled.

   Description
   When asynchronous requests are canceled, this error is passed to the callback function
   associated with the request to indicate that the request has been canceled
 */
#define CY_WB_ERROR_CANCELED                                                    (28)

/* Summary
   This error is returned when the call to create sleep channel fails
   in the HAL layer.

   Description
   This error is returned when the call to create sleep channel fails
   in the HAL layer.
 */
#define CY_WB_ERROR_CREATE_SLEEP_CHANNEL_FAILED                                 (29)

/* Summary
   This error is returned when the call to cy_wb_dev_leave_standby
   is made and the device is not in standby.

   Description
   This error is returned when the call to cy_wb_dev_leave_standby
   is made and the device is not in standby.
 */
#define CY_WB_ERROR_NOT_IN_STANDBY                                              (30)

/* Summary
   This error is returned when the call to destroy sleep channel fails
   in the HAL layer.

   Description
   This error is returned when the call to destroy sleep channel fails
   in the HAL layer.
 */
#define CY_WB_ERROR_DESTROY_SLEEP_CHANNEL_FAILED                                (31)

/* Summary
   This error occurs when an operation is requested on an endpoint that has
   a currently pending async operation.

   Description
   There can only be a single asynchronous pending operation on a given endpoint and
   while the operation is pending on other operation can occur on the endpoint.  In
   addition, the device cannot enter standby while any asynchronous operations are
   pending.
 */
#define CY_WB_ERROR_ASYNC_PENDING                                               (32)

/* Summary
   This error is returned when a call to cy_wb_storage_cancel_async() or
   cy_wb_usb_cancel_async() is made when no asynchronous request is pending.

   Description
   This error is returned when a call to cy_wb_storage_cancel_async() or
   cy_wb_usb_cancel_async() is made when no asynchronous request is pending.
 */
#define CY_WB_ERROR_ASYNC_NOT_PENDING                                           (33)

/* Summary
   This error is returned when a request is made to put the West Bridge device
   into standby mode while the USB stack is still active.

   Description
   This error is returned when a request is made to put the West Bridge device
   into standby mode while the USB stack is still active.  You must call the
   function cy_wb_usb_stop() in order to shut down the USB stack in order to go
   into the standby mode.
 */
#define CY_WB_ERROR_USB_RUNNING                                                 (34)

/* Summary
   A request for in the wrong direction was issued on an endpoint.

   Description
   This error is returned when a write is attempted on an OUT endpoint or
   a read is attempted on an IN endpoint.
 */
#define CY_WB_ERROR_USB_BAD_DIRECTION                                           (35)

/* Summary
   An invalid request was received

   Description
   This error is isused if an invalid request is issued.
 */
#define CY_WB_ERROR_INVALID_REQUEST                                             (36)

/* Summary
   An ACK request was requested while no setup packet was pending.

   Description
   This error is issued if CyWbUsbAckSetupPacket() is called when no setup packet is
   pending.
 */
#define CY_WB_ERROR_NO_SETUP_PACKET_PENDING                                     (37)

/* Summary
   A call was made to a API function that cannot be called from a callback.

   Description
   Only asynchronous functions can be called from within West Bridge callbacks.
   This error results when an invalid function is called from a callback.
 */
#define CY_WB_ERROR_INVALID_IN_CALLBACK                                         (38)

/* Summary
   The data supplied to the cy_wb_dev_download_firmware() call is not aligned on a
   WORD (16 bit) boundary.

   Description
   Many systems have problems with the transfer of data a word at a time when the
   data is not word aligned.  For this reason, we require that the firmware image
   be aligned on a word boundary and be an even number of bytes.  This error is returned
   if these conditions are not met.
 */
#define CY_WB_ERROR_ALIGNMENT_ERROR                                             (39)

/* Summary
   A call was made to destroy the West Bridge device, but the USB stack or the storage
   stack was will running.

   Description
   Before calling cy_wb_destroy_handle() to destroy an West Bridge device created via a
   call to cy_wb_create_handle(), the USB and STORAGE stacks much be stopped via calls
   to cy_wb_usb_stop and cy_wb_storage_stop.  This error indicates that one of these two
   stacks have not been stopped.
 */
#define CY_WB_ERROR_STILL_RUNNING                                               (40)

/* Summary
   A call was made to the API for a function that is not yet supported.

   Description
   There are calls that are not yet supported that may be called through the API.  This
   is done to maintain compatibility in the future with the API.  This error is returned
   if you are asking for a capability that does not yet exist.
 */
#define CY_WB_ERROR_NOT_YET_SUPPORTED                                           (41)

/* Summary
   A NULL callback was provided where a non-NULL callback was required

   Description
   When async IO function are called, a callback is required to indicate that the
   IO has completed.  This callback must be non-NULL.
 */
#define CY_WB_ERROR_NULL_CALLBACK                                               (42)

/* Summary
   This error is returned when an operation is attempted that cannot be completed while
   the USB stack is connected to a USB host.

   Description
   This error is returned when an operation is attempted that cannot be completed while
   the USB stack is connected to a USB host.  In order to sucessfully complete the
   desired operation, cy_wb_usb_disconnect () must be called to disconnect from the host.
 */
#define CY_WB_ERROR_USB_CONNECTED                                               (43)

/* Summary
   This error is returned when a USB disconnect is attempted and the West Bridge device is
   not connected.

   Description
   This error is returned when a USB disconnect is attempted and the West Bridge device is
   not connected.
 */
#define CY_WB_ERROR_USB_NOT_CONNECTED                                           (44)

/* Summary
   This error is returned when an P2S storage operation attempted and data could not be
   read or written to the storage media.

   Description
   This error is returned when an P2S storage operation attempted and data could not be
   read or written to the storage media. If this error is recevied then a retry can be
   done.
 */
#define CY_WB_ERROR_MEDIA_ACCESS_FAILURE                                        (45)

/* Summary
   This error is returned when an P2S storage operation attempted and the media is write
   protected.

   Description
   This error is returned when an P2S storage operation attempted and the media is write
   protected.
 */
#define CY_WB_ERROR_MEDIA_WRITE_PROTECTED                                       (46)

/* Summary
   This error is returned when an attempt is made to cancel a request that has
   already been sent to the West Bridge.

   Description
   It is not possible to cancel an asynchronous storage read/write operation after
   the actual data transfer with the West Bridge has started. This error is returned
   if cy_wb_storage_cancel_async is called to cancel such a request.
 */
#define CY_WB_ERROR_OPERATION_IN_TRANSIT                                        (47)

/* Summary
   This error is returned when an invalid parameter is passed to one of the APIs.

   Description
   Some of the West Bridge APIs are applicable to only specific media types, devices
   etc. This error code is returned when a API is called with an invalid parameter
   type.
 */
#define CY_WB_ERROR_INVALID_PARAMETER                                           (48)

/* Summary
   This error is returned if an API is not supported in the current setup.

   Description
   Some of the West Bridge APIs work only with specific device types or firmware images.
   This error is returned when such APIs are called when the current device or firmware
   does not support the invoked API function.
 */
#define CY_WB_ERROR_NOT_SUPPORTED                                               (49)

/* Summary
   This error is returned when a call is made to one of the Storage or
   USB APIs while the device is in suspend mode.

   Description
   This error is returned when a call is made to one of the storage or
   USB APIs while the device is in suspend mode.
 */
#define CY_WB_ERROR_IN_SUSPEND                                                  (50)

/* Summary
   This error is returned when the call to cy_wb_dev_leave_suspend
   is made and the device is not in suspend mode.

   Description
   This error is returned when the call to cy_wb_dev_leave_suspend
   is made and the device is not in suspend mode.
 */
#define CY_WB_ERROR_NOT_IN_SUSPEND                                              (51)

/* Summary
   This error is returned when a command that is disabled by USB is called.

   Description
   The remote wakeup capability should be exercised only if enabled by the USB host.
   This error is returned when the cy_wb_usb_signal_remote_wakeup API is called when the
   feature has not been enabled by the USB host.
 */
#define CY_WB_ERROR_FEATURE_NOT_ENABLED                                         (52)

/* Summary
   This error is returned when an Async storage read or write is called before a
   query device call is issued.

   Description
   In order for the SDK to properly set up a DMA the block size of a given media
   needs to be known. This is done by making a call to cy_wb_storage_query_device. This
   call only needs to be made once per device. If this call is not issued before
   an Async read or write is issued this error code is returned.
 */
#define CY_WB_ERROR_QUERY_DEVICE_NEEDED                                         (53)

/* Summary
   This error is returned when a call is made to USB or STORAGE Start or Stop before
   a prior Start or Stop has finished.

   Description
   The USB and STORAGE start and stop functions can only be called if a prior start
   or stop function call has fully completed. This means when an async EX call is made
   you must wait until the callback for that call has been completed before calling
   start or stop again.
 */
#define CY_WB_ERROR_STARTSTOP_PENDING                                           (54)

/* Summary
   This error is returned when a request is made for a port that does not exist

   Description
   This error is returned when a request is made that references a port number that
   does not exist.  This error is returned when the port number is not present in
   the current system, or if the port number given is not valid.

   See Also
   * CyWbDevSetTraceLevel
   * cy_wb_storage_claim
   * cy_wb_storage_release
   * cy_wb_storage_read
   * cy_wb_storage_write
   * cy_wb_storage_read_async
   * cy_wb_storage_write_async
 */
#define CY_WB_ERROR_NO_SUCH_PORT                                                (55)

/* Summary
   This error is returned when an invalid command is passed to the CyWbStorageSDIOSync ()
   function.

   Description
   This error indiactes an unknown Command type was passed to the SDIO command handler
   function.
 */
#define CY_WB_ERROR_INVALID_COMMAND                                             (56)

/* Summary
   This error is returned when an invalid function /uninitialized function is passed to an SDIO
   function.

   Description
   This error indiactes an unknown/uninitialized function number was passed to a SDIO
   function.
 */
#define CY_WB_ERROR_INVALID_FUNCTION                                            (57)

/* Summary
   This error is returned when an invalid block size is passed to cy_wb_sdio_set_blocksize ().

   Description
   This error is returned when an invalid block size (greater than maximum block size supported)is
   passed to cy_wb_sdio_set_blocksize ().
 */
#define CY_WB_ERROR_INVALID_BLOCKSIZE                                           (58)

/* Summary
   This error is returned when an tuple requested is not found.

   Description
   This error is returned when an tuple requested is not found.
 */
#define CY_WB_ERROR_TUPLE_NOT_FOUND                                             (59)

/* Summary
   This error is returned when an extended IO operation to an SDIO function is
   Aborted.
   Description
   This error is returned when an extended IO operation to an SDIO function is
 */
#define CY_WB_ERROR_IO_ABORTED                                                  (60)

/* Summary
   This error is returned when an extended IO operation to an SDIO function is
   Suspended.
   Description
   This error is returned when an extended IO operation to an SDIO function is
 */
#define CY_WB_ERROR_IO_SUSPENDED                                                (61)

/* Summary
   This error is returned when IO is attempted to a Suspended SDIO function.
   Description
 */
#define CY_WB_ERROR_FUNCTION_SUSPENDED                                          (62)

/* Summary
   This error is returned if an MTP function is called before MTPStart has completed.
   Description
   This error is returned if an MTP function is called before MTPStart has completed.
 */
#define CY_WB_ERROR_MTP_NOT_STARTED                                             (63)

/* Summary
   This error is returned by API functions that are not valid in MTP mode (cy_wb_storage_claim for example)
   Description
   This error is returned by API functions that are not valid in MTP mode (cy_wb_storage_claim for example)
 */
#define CY_WB_ERROR_NOT_VALID_IN_MTP                                            (64)

/* Summary
   This error is returned when an attempt is made to partition a storage device that is already
   partitioned.

   Description
   This error is returned when an attempt is made to partition a storage device that is already
   partitioned.
 */
#define CY_WB_ERROR_ALREADY_PARTITIONED                                         (65)

/* Summary
   This error is returned when a StorageWrite opperation is attempted during an ongoing MTP transfer.
   Description
   This error is returned when a StorageWrite opperation is attempted during an ongoing MTP transfer.
   A MTP transfer is initiated by a call to CyWbMTPInitSendObject or CyWbMTPInitGetObject and is not
   finished until the CyWbMTPSendObjectComplete or CyWbMTPGetObjectComplete event is generated.
 */
#define CY_WB_ERROR_NOT_VALID_DURING_MTP                                        (66)

/* Summary
   This error is returned when an API call setting up a new data flow fails to get a socket.

   Description
   The West Bridge device has a number of sockets that can be used for independent data flows between
   the processor and the device. A number of API calls such as cy_wb_storage_start(), cy_wb_usb_start(),
   CyWbUsbSetEndpointConfig() need to set up new data flows. This error code is returned when
   one of these API fails to get a new socket that can be used for the data flow.
 */
#define CY_WB_ERROR_NO_SOCKET                                                   (67)

/* Summary
   This error is returned when a request to cancel a transfer has failed.

   Description
   This error is returned in response to a storage or Lpp cancel transfer request, if the
   API is unable to cancel the transfer as requested. In such a case, the only option to
   abort the transfer would be by resetting the West Bridge device.
 */
#define CY_WB_ERROR_ABORT_FAILED                                                (68)

/* Summary
   This error is returned from FW for error code which is not specific to any 
   paricular scenario.

   Description
   This error can be returned from any API. This informs that FW has faced an error.
   E.g: The Socket configuration failed. 
 */
#define CY_WB_ERROR_GENERAL_FW_FAILURE                                          (69)

/* Summary
   This error is returned when an API call is unable to get the lock to ensure mutual
   exclusion.

   Description
   The API used internal locks to ensure mutual exclusion in access to all of the
   API internal data structures. This error occurs when an API call is unable to
   procure the lock and it is unable to wait because it is in interrupt context.
 */
#define CY_WB_ERROR_MUTEX_GET_FAILED                                            (70)

/* Summary
   This error is returned when a bad descriptor index is used with the cy_wb_usb_set_descriptor
   API call.

   Description
   This error is returned if the cy_wb_usb_set_descriptor call attempts to set a USB string
   with index out of the supported range that can be remembered by firmware. It is
   required that the USB function drivers on the Processor keep track of all other
   descriptors.
 */
#define CY_WB_ERROR_BAD_INDEX                                                   (71)

/* Summary
   This error is returned when a lock/unlock operation on the storage card fails.

   Description
   This error is returned when a lock/unlock operation is performed with the wrong password.
 */
#define CY_WB_ERROR_MEDIA_LOCK_UNLOCK_FAILED                                    (72)

/* Summary
   This error is returned when a set/clear password on the storage card fails.

   Description
   This error is returned when the password specified doesn't match that of the stored
   password while performing a replacement or a removal of password operation.
 */
#define CY_WB_ERROR_MEDIA_SET_CLR_PWD_FAILED                                    (73)

/* Summary
   This error is returned when the force erase operation of the storage card fails.

   Description
   This error is returned if the force erase is performed on an unlocked card.
 */
#define CY_WB_ERROR_FORCE_ERASE_FAILED                                          (74)

/* Summary
   This error is returned when a read/write operation is performed on a locked card.

   Description
   This error is returned when a read/write operation is performed on a locked card.
 */
#define CY_WB_ERROR_MEDIA_LOCKED                                                (75)

/* Summary
   This error is returned when a MTP block table send is attempted without starting a
   Turbo transfer.

   Description
   MTP block tables can be sent to the firmware only after the Turbo transfer mode
   has been enabled by calling CyWbMTPInitSendObject or CyWbMTPInitGetObject. This
   error is returned if the cy_wb_mtp_send_block_table function is called without either
   of these functions being called.
 */
#define CY_WB_ERROR_NOT_IN_TURBO_MODE                                           (76)

/* Summary
   This error is returned when a USB host stack is accessed when in device mode.

   Description
   West Bridge has a single OTG USB port which can operate in host or device mode
   depending on the peripheral attached. If the peripheral attached is an USB host,
   or an OTG host, USB host stack cannot be used and this error is returned on any
   USB host API access.
 */
#define CY_WB_ERROR_NOT_IN_USB_HOST_MODE                                       (77)

/* Summary
   This error is returned when a USB host encounters an endpoint stall condition.

   Description
   When in host mode of operation, if a data transfer / request is stalled by the 
   attached usb peripheral, then this error code is returned.
 */
#define CY_WB_ERROR_STALLED                                                    (78)

#endif /* _INCLUDED_CYWBERR_H_ */

/*[]*/

