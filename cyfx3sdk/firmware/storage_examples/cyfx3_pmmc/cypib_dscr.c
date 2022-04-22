/*
 ## Cypress FX3S Example Application Source File (cypib_dscr.c)
 ## ===========================
 ##
 ##  Copyright Cypress Semiconductor Corporation, 2010-2018,
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

/* This file contains the MSC Interface specific configuration descriptors for
 * the PIB application example. These descriptors will be appended to the
 * configuration descriptor passed by Application Processor, if opted for MSC 
 * enable feature */

#include "cyfx3_pib.h"

/* Full Speed Configuration Descriptor */
uint8_t CyFxUSBFSBotIfaceDscr[] __attribute__ ((aligned (32))) =
{
    /* Interface Descriptor */
    0x09,                           /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,        /* Interface Descriptor type */
    CY_FX_USB_MSC_INTF,             /* Interface number */
    0x00,                           /* Alternate setting number */
    0x02,                           /* Number of end points */
    0x08,                           /* Interface class : Mass Storage Class */
    0x06,                           /* Interface sub class : SCSI Transparent Command Set */
    0x50,                           /* Interface protocol code : BOT */
    0x00,                           /* Interface descriptor string index */

    /* Endpoint Descriptor for Producer EP */
    0x07,                           /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,        /* Endpoint Descriptor Type */
    CY_FX_MSC_EP_BULK_OUT,          /* Endpoint address and description */
    CY_U3P_USB_EP_BULK,             /* Bulk End point Type */
    0x40,0x00,                      /* Max packet size = 64 bytes */
    0x00,                           /* Servicing interval for data transfers : NA for Bulk */

    /* Endpoint Descriptor for Consumer EP */
    0x07,                           /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,        /* Endpoint Descriptor Type */
    CY_FX_MSC_EP_BULK_IN,           /* Endpoint address and description */
    CY_U3P_USB_EP_BULK,             /* Bulk End point Type */
    0x40,0x00,                      /* Max packet size = 64 bytes */
    0x00                            /* Servicing interval for data transfers : NA for Bulk */
};

uint8_t CyFxUSBHSBotIfaceDscr[] __attribute__ ((aligned (32))) =
{
    /* Interface Descriptor */
    0x09,                           /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,        /* Interface Descriptor type */
    CY_FX_USB_MSC_INTF,             /* Interface number */
    0x00,                           /* Alternate setting number */
    0x02,                           /* Number of end points */
    0x08,                           /* Interface class : Mass Storage Class */
    0x06,                           /* Interface sub class : SCSI Transparent Command Set */
    0x50,                           /* Interface protocol code : BOT */
    0x00,                           /* Interface descriptor string index */

    /* Endpoint Descriptor for Producer EP */
    0x07,                           /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,        /* Endpoint Descriptor Type */
    CY_FX_MSC_EP_BULK_OUT,          /* Endpoint address and description */
    CY_U3P_USB_EP_BULK,             /* Bulk End point Type */
    0x00,0x02,                      /* Max packet size = 512 bytes */
    0x00,                           /* Servicing interval for data transfers : NA for Bulk */

    /* Endpoint Descriptor for Consumer EP */
    0x07,                           /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,        /* Endpoint Descriptor Type */
    CY_FX_MSC_EP_BULK_IN,           /* Endpoint address and description */
    CY_U3P_USB_EP_BULK,             /* Bulk End point Type */
    0x00,0x02,                      /* Max packet size = 512 bytes */
    0x00                            /* Servicing interval for data transfers : NA for Bulk */
};


uint8_t CyFxUSBSSBotIfaceDscr[] __attribute__ ((aligned (32))) =
{
    /* Interface Descriptor */
    0x09,                           /* Descriptor size */
    CY_U3P_USB_INTRFC_DESCR,        /* Interface Descriptor type */
    CY_FX_USB_MSC_INTF,             /* Interface number */
    0x00,                           /* Alternate setting number */
    0x02,                           /* Number of end points */
    0x08,                           /* Interface class */
    0x06,                           /* Interface sub class */
    0x50,                           /* Interface protocol code */
    0x00,                           /* Interface descriptor string index */

    /* Endpoint Descriptor for Producer EP */
    0x07,                           /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,        /* Endpoint Descriptor Type */
    CY_FX_MSC_EP_BULK_OUT,          /* Endpoint address and description */
    CY_U3P_USB_EP_BULK,             /* Bulk End point Type */
    0x00,0x04,                      /* Max packet size = 1024 bytes */
    0x00,                           /* Servicing interval for data transfers : NA for Bulk */

    /* Super Speed Endpoint Companion Descriptor for Producer EP */
    0x06,                           /* Descriptor size */
    CY_FX_SS_EP_COMPN_DSCR_TYPE,    /* SS Endpoint Companion Descriptor Type */
    CY_FX_MSC_EP_BURST_SIZE - 1,    /* Max no. of packets in a Burst. */
    0x00,                           /* Max streams for Bulk EP = 0 (No streams)*/
    0x00,0x00,                      /* Service interval for the EP : NA for Bulk */

    /* Endpoint Descriptor for Consumer EP */
    0x07,                           /* Descriptor size */
    CY_U3P_USB_ENDPNT_DESCR,        /* Endpoint Descriptor Type */
    CY_FX_MSC_EP_BULK_IN,              /* Endpoint address and description */
    CY_U3P_USB_EP_BULK,             /* Bulk End point Type */
    0x00,0x04,                      /* Max packet size = 1024 bytes */
    0x00,                           /* Servicing interval for data transfers : NA for Bulk */

    /* Super Speed Endpoint Companion Descriptor for Consumer EP */
    0x06,                           /* Descriptor size */
    CY_FX_SS_EP_COMPN_DSCR_TYPE,    /* SS Endpoint Companion Descriptor Type */
    CY_FX_MSC_EP_BURST_SIZE - 1,    /* Max no. of packets in a Burst. */
    0x00,                           /* Max streams for Bulk EP = 0 (No streams)*/
    0x00,0x00                       /* Service interval for the EP : NA for Bulk */
};


/*[]*/

