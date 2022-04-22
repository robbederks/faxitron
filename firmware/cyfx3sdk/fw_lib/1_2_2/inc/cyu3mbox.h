/*
 ## Cypress West Bridge Benicia header file (cyu3mbox.h)
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

#ifndef _INCLUDED_CYU3_MBOX_H_
#define _INCLUDED_CYU3_MBOX_H_

#include "cyu3types.h"
#include "cyu3externcstart.h"

/*@@Mailbox Handler
   Summary
   The mailbox handler is responsible for sending/receiving short messages
   from an external processor through the mailbox registers.

   Description
   The FX3 device implements a set of mailbox registers that can be used to
   exchange short general purpose messages between FX3 and the external device
   connected on the P-port. Messages of upto 8 bytes can be sent in each
   direction at a time.

   The mailbox handler is responsible for handling mailbox data in both directions.
 */

/**************************************************************************
 ******************************* Data Types *******************************
 **************************************************************************/

/*@@Mailbox Data Types
   This section documents the data types defined as part of the mailbox handler.
 */

/* Summary
   Structure that holds a packet of mailbox data.

   Description
   The FX3 device has 8 byte mailbox register that can be used when the
   P-port mode is enabled. This structure represents the eight bytes to be
   written to or read from the corresponding mailbox registers.
 */
typedef struct CyU3PMbox
{
    uint32_t w0;                                /* Contents of the lower mailbox register. */
    uint32_t w1;                                /* Contents of the upper mailbox register. */
} CyU3PMbox;

/* Summary
   Type of function to be called to notify about a mailbox related interrupt.

   Description
   This type is the prototype for a callback function that will be called to
   notify the application about a mailbox interrupt. The mboxEvt parameter
   will identify the type of interrupt. If a read interrupt is received, the
   mailbox registers have to be read; and if a write interrupt is received,
   any pending data can be written to the registers.
 */
typedef void (*CyU3PMboxCb_t)(
        CyBool_t mboxEvt                        /* Mailbox event type: 0=Read, 1=Write */
        );

/**************************************************************************
 *************************** Function prototypes **************************
 **************************************************************************/

/*@@Mailbox Functions
   This section documents the functions defined as part of the mailbox handler.
 */

/* Summary
   Initialize the mailbox handler.
  
   Description
   Initiate the mailbox related structures and register a callback function that will
   be called on every mailbox related interrupt.
   
   See Also
   * CyU3PMboxDeInit
 */
extern void
CyU3PMboxInit (
        CyU3PMboxCb_t callback                  /* Callback function to be called on interrupt. */
        );

/* Summary
   De-initialize the mailbox handler.
  
   Description
   Destroys the mailbox related structures.
  
   See Also
   * CyU3PMboxInit
 */
extern void
CyU3PMboxDeInit (
        void);

/* Summary
   Reset the mailbox handler.
  
   Description
   Clears the data structures and state related to mailbox handler. Can be called for error
   recovery.
  
   See Also
   * CyU3PMboxInit
 */
extern void 
CyU3PMboxReset (
        void);

/* Summary
   This function sends a mailbox message to the external processor.
  
   Description
   This function writes 8 bytes of data to the outgoing mailbox registers after ensuring that
   the external processor has read out the previous message.
  
   See Also
   * CyU3PMboxRead
 */
extern CyU3PReturnStatus_t
CyU3PMboxWrite (
        CyU3PMbox *mbox         /* Pointer to mailbox message data. */
        );

/* Summary
   This function reads an incoming mailbox message.
 
   Description
   This function is used to read the contents of an incoming mailbox message. This needs
   to be called in response to a read event callback.
  
   See Also
   * CyU3PMboxWrite
 */
extern CyU3PReturnStatus_t
CyU3PMboxRead (
        CyU3PMbox *mbox         /* Pointer to buffer to hold the incoming message data. */
        );

/* Summary
   Wait until the Mailbox register to send messages to P-port is free.

   Description
   This function waits until the mailbox register used to send messages to the
   processor/device connected on FX3's P-port is free. This is expected to be used
   in cases where the application needs to ensure that the last message that was
   sent out has been read by the external processor or device.

   Return Value
   * CY_U3P_SUCCESS              - If the operation is successful
   * CY_U3P_ERROR_TIMEOUT        - If the wait times out.
 */
extern CyU3PReturnStatus_t
CyU3PMboxWait (
	void);

#include "cyu3externcend.h"

#endif /* _INCLUDED_CYU3_MBOX_H_ */

/*[]*/

