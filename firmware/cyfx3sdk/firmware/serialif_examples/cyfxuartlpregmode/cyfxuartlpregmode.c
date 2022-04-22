/*
 ## Cypress USB 3.0 Platform source file (cyfxuartlpregmode.c)
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

/* This file implements a simple UART (register mode) loopback application example */

/*
   This example illustrates a loopback mechanism between the UART RX and UART TX 
   through CPU. CPU receives a byte sent from the UART and sends the same byte
   back to UART which results in the loopback.
 */


#include <cyu3system.h>
#include <cyu3os.h>
#include <cyu3error.h>
#include <cyu3uart.h>

#define CY_FX_UARTLP_THREAD_STACK      (0x0400) /* UART application thread stack size */
#define CY_FX_UARTLP_THREAD_PRIORITY   (8)      /* UART application thread priority */
#define CY_FX_UARTLT_BUF_SIZE          (512)    /* Size of UART data buffer. */
#define UART_RECEIVE_DATA_EVT          (1 << 0) /* Flag for data received event. */

CyU3PThread UartLpAppThread;	                /* UART Example application thread structure */
CyU3PEvent  UartLpAppEvent;                     /* Event flags group used by the application. */
static          uint8_t  *UartDataBuffer = 0;   /* Buffer used for received UART data. */
static volatile uint32_t  UartDataCount  = 0;   /* Amount of data in the buffer. */
static          uint32_t  UartDataWrPtr  = 0;   /* Write pointer into the UART buffer. */
static          uint32_t  UartDataRdPtr  = 0;   /* Read pointer into the UART buffer. */

/* Application error handler. */
void
CyFxAppErrorHandler (
        CyU3PReturnStatus_t apiRetStatus    /* API return status */
        )
{
    /* Application failed with the error code apiRetStatus */

    /* Add custom debug or recovery actions here */

    /* Loop indefinitely */
    for (;;)
    {
        /* Thread sleep : 100 ms */
        CyU3PThreadSleep (100);
    }
}

/* UART event callback function. Schedules a UART receive call when the RX_DATA event is received. */
static void
CyFxUartLpApplnCallback (
        CyU3PUartEvt_t   evType,
        CyU3PUartError_t evStatus)
{
    uint8_t  rxData;
    uint32_t count;
    CyU3PReturnStatus_t status;

    if (evType == CY_U3P_UART_EVENT_RX_DATA)
    {
        do
        {
            /* Keep reading the incoming data into the buffer, as long as we have space. */
            count = CyU3PUartReceiveBytes (&rxData, 1, &status);
            if (count != 0)
            {
                /* Just drop the data if the buffer is already full. */
                if (UartDataCount < CY_FX_UARTLT_BUF_SIZE)
                {
                    UartDataBuffer[UartDataWrPtr] = rxData;
                    UartDataWrPtr++;
                    if (UartDataWrPtr == CY_FX_UARTLT_BUF_SIZE)
                        UartDataWrPtr = 0;
                    UartDataCount++;
                }
            }
        } while (count != 0);

        CyU3PEventSet (&UartLpAppEvent, UART_RECEIVE_DATA_EVT, CYU3P_EVENT_OR);
    }
}

/* This function initializes the UART module */
void
CyFxUartLpApplnInit (void)
{
    CyU3PUartConfig_t uartConfig;
    CyU3PReturnStatus_t apiRetStatus = CY_U3P_SUCCESS;

    /* Initialize the UART module */
    apiRetStatus = CyU3PUartInit ();
    if (apiRetStatus != CY_U3P_SUCCESS)
    {
        /* Error handling */
        CyFxAppErrorHandler(apiRetStatus);
    }

    /* Configure the UART:
       Baud-rate = 115200, One stop bit, No parity, Flow control enabled.
     */
    CyU3PMemSet ((uint8_t *)&uartConfig, 0, sizeof(uartConfig));
    uartConfig.baudRate = CY_U3P_UART_BAUDRATE_115200;
    uartConfig.stopBit = CY_U3P_UART_ONE_STOP_BIT;
    uartConfig.parity = CY_U3P_UART_NO_PARITY;
    uartConfig.flowCtrl = CyFalse;
    uartConfig.txEnable = CyTrue;
    uartConfig.rxEnable = CyTrue;
    uartConfig.isDma = CyFalse; /* Register mode */

    /* Set the UART configuration */
    apiRetStatus = CyU3PUartSetConfig (&uartConfig, CyFxUartLpApplnCallback);
    if (apiRetStatus != CY_U3P_SUCCESS )
    {
        /* Error handling */
        CyFxAppErrorHandler(apiRetStatus);
    }
}

/* Entry function for the UartLpAppThread. */
void
UartLpAppThread_Entry (
        uint32_t input)
{
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;
    uint32_t            evStat;

    UartDataBuffer = (uint8_t *)CyU3PDmaBufferAlloc (CY_FX_UARTLT_BUF_SIZE);
    if (UartDataBuffer == 0)
        CyFxAppErrorHandler (CY_U3P_ERROR_MEMORY_ERROR);

    UartDataCount = 0;
    UartDataWrPtr = 0;
    UartDataRdPtr = 0;

    /* Initialize the UART example application */
    CyFxUartLpApplnInit();

    /* We set a timeout of 1 for receive data and 50000 for transmit data.
     * Then wait until the callback notifies us of incoming data.
     * Once all of the data has been fetched, we loop the data back on the transmit side.
     */
    CyU3PUartSetTimeout (1, 50000);

    for (;;)
    {
        status = CyU3PEventGet (&UartLpAppEvent, UART_RECEIVE_DATA_EVT, CYU3P_EVENT_OR_CLEAR, &evStat,
                CYU3P_WAIT_FOREVER);

        if (status == CY_U3P_SUCCESS)
        {
            while (UartDataCount > 0)
            {
                CyU3PUartTransmitBytes (UartDataBuffer + UartDataRdPtr, 1, &status);
                if (status == CY_U3P_SUCCESS)
                {
                    /* Move to the next byte. */
                    UartDataRdPtr++;
                    if (UartDataRdPtr == CY_FX_UARTLT_BUF_SIZE)
                        UartDataRdPtr = 0;
                    UartDataCount--;
                }
                else
                {
                    if (status != CY_U3P_ERROR_TIMEOUT)
                    {
                        /* Failed to transmit the data back. Return error. */
                        CyFxAppErrorHandler (status);
                    }
                }
            }
        }
    }
}

/* Application define function which creates the threads. */
void
CyFxApplicationDefine (
        void)
{
    void *ptr = NULL;
    uint32_t ret = CY_U3P_SUCCESS;

    /* Create the event flag used for receive data signaling. */
    ret = CyU3PEventCreate (&UartLpAppEvent);
    if (ret != CY_U3P_SUCCESS)
        goto StartupError;

    /* Allocate the memory for the thread stack. */
    ptr = CyU3PMemAlloc (CY_FX_UARTLP_THREAD_STACK);
    if (ptr == 0)
        goto StartupError;

    /* Create the thread for the application */
    ret = CyU3PThreadCreate (&UartLpAppThread,                  /* UART Example App Thread structure */
                          "21:UART_loopback_register_mode",     /* Thread ID and Thread name */
                          UartLpAppThread_Entry,                /* UART Example App Thread Entry function */
                          0,                                    /* No input parameter to thread */
                          ptr,                                  /* Pointer to the allocated thread stack */
                          CY_FX_UARTLP_THREAD_STACK,            /* UART Example App Thread stack size */
                          CY_FX_UARTLP_THREAD_PRIORITY,         /* UART Example App Thread priority */
                          CY_FX_UARTLP_THREAD_PRIORITY,         /* UART Example App Thread priority */
                          CYU3P_NO_TIME_SLICE,                  /* No time slice for the application thread */
                          CYU3P_AUTO_START                      /* Start the Thread immediately */
                          );

    if (ret == CY_U3P_SUCCESS)
        return;

StartupError:
    while (1);
}

/*
 * Main function
 */
int
main (void)
{
    CyU3PIoMatrixConfig_t io_cfg;
    CyU3PReturnStatus_t status = CY_U3P_SUCCESS;

    /* Initialize the device */
    status = CyU3PDeviceInit (0);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* Initialize the caches. Enable both Instruction and Data Caches. */
    status = CyU3PDeviceCacheControl (CyTrue, CyTrue, CyTrue);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* Configure the IO matrix for the device. On the FX3 DVK board, the COM port 
     * is connected to the IO(53:56). This means that either DQ32 mode should be
     * selected or lppMode should be set to UART_ONLY. Here we are choosing
     * UART_ONLY configuration. */
    CyU3PMemSet ((uint8_t *)&io_cfg, 0, sizeof(io_cfg));
    io_cfg.isDQ32Bit = CyFalse;
    io_cfg.s0Mode    = CY_U3P_SPORT_INACTIVE;
    io_cfg.s1Mode    = CY_U3P_SPORT_INACTIVE;
    io_cfg.useUart   = CyTrue;
    io_cfg.useI2C    = CyFalse;
    io_cfg.useI2S    = CyFalse;
    io_cfg.useSpi    = CyFalse;
    io_cfg.lppMode   = CY_U3P_IO_MATRIX_LPP_UART_ONLY;

    /* No GPIOs are enabled. */
    io_cfg.gpioSimpleEn[0]  = 0;
    io_cfg.gpioSimpleEn[1]  = 0;
    io_cfg.gpioComplexEn[0] = 0;
    io_cfg.gpioComplexEn[1] = 0;
    status = CyU3PDeviceConfigureIOMatrix (&io_cfg);
    if (status != CY_U3P_SUCCESS)
    {
        goto handle_fatal_error;
    }

    /* This is a non returnable call for initializing the RTOS kernel */
    CyU3PKernelEntry ();

    /* Dummy return to make the compiler happy */
    return 0;

handle_fatal_error:
    /* Cannot recover from this error. */
    while (1);
}

/* [ ] */

