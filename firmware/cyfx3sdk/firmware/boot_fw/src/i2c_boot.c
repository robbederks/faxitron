/*
## Cypress FX3 Boot Firmware Example Source file (i2c_boot.c)
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

#include <cyfx3i2c.h>
#include <cyfx3device.h>
#include <cyfx3utils.h>

/* Code to read and boot FX3 firmware from I2C EEPROM. */

#define I2C_BUS_TIMEOUT_VALUE           (0xFFFFFFFFu)
#define I2C_DMA_TIMEOUT_VALUE           (0xFFFFu)
#define I2C_FREQUENCY                   (400000u)

#define I2C_SLAVE_ADDRESS               (0xA0u)
#define I2C_CMD_WRITE                   (0)
#define I2C_CMD_READ                    (1)
#define I2C_START_BYTE2                 (0x0004)
#define I2C_RETRY_CNT                   (2)

#define FW_HEADER_SIZE                  (4)
#define SECTION_HEADER_SIZE             (8)

#define I2C_SLAVE_SPACE                 (0x10000)
#define GET_BYTE0(addr)                 ((uint8_t)((addr) & 0xFF))
#define GET_BYTE1(addr)                 ((uint8_t)(((addr) >> 8) & 0xFF))

static uint8_t  gI2cSlaveAddr = 0;
static uint32_t gI2cByteAddr = 0;
static uint32_t gReadBuf[32];

static CyBool_t
I2cInit (
        void)
{
    CyFx3BootErrorCode_t   status;
    CyFx3BootI2cConfig_t   i2cConfig;
    CyFx3BootI2cPreamble_t preamble;
    uint8_t               *rd_buf = (uint8_t *)gReadBuf;

    status = CyFx3BootI2cInit ();
    if (status != CY_FX3_BOOT_SUCCESS)
        return CyFalse;


    i2cConfig.busTimeout = I2C_BUS_TIMEOUT_VALUE;
    i2cConfig.dmaTimeout = I2C_DMA_TIMEOUT_VALUE;
    i2cConfig.isDma      = CyFalse;
    i2cConfig.bitRate    = I2C_FREQUENCY;
    status = CyFx3BootI2cSetConfig (&i2cConfig);
    if (status != CY_FX3_BOOT_SUCCESS)
        return CyFalse;

    preamble.buffer[0] = I2C_SLAVE_ADDRESS | I2C_CMD_WRITE;
    preamble.buffer[1] = 0;
    preamble.buffer[2] = 0;
    preamble.buffer[3] = I2C_SLAVE_ADDRESS | I2C_CMD_READ;
    preamble.length    = 4;
    preamble.ctrlMask  = I2C_START_BYTE2;

    status = CyFx3BootI2cReceiveBytes (&preamble, rd_buf, FW_HEADER_SIZE, I2C_RETRY_CNT);
    if (status == CY_FX3_BOOT_SUCCESS)
    {
        if ((rd_buf[0] == 'C') && (rd_buf[1] == 'Y') && (rd_buf[3] == 0xB0))
        {
            return CyTrue;
        }
    }

    return CyFalse;
}

static CyBool_t
LoadFwSection (
        void)
{
    CyFx3BootErrorCode_t   status;
    CyFx3BootI2cPreamble_t preamble;
    uint32_t               secLength, secAddress;
    uint32_t               tmp;
    uint8_t               *rd_buf = (uint8_t *)gReadBuf;

    preamble.buffer[0] = I2C_SLAVE_ADDRESS | (gI2cSlaveAddr << 1) | I2C_CMD_WRITE;
    preamble.buffer[1] = GET_BYTE1 (gI2cByteAddr);
    preamble.buffer[2] = GET_BYTE0 (gI2cByteAddr);
    preamble.buffer[3] = I2C_SLAVE_ADDRESS | (gI2cSlaveAddr << 1) | I2C_CMD_READ;
    preamble.length    = 4;
    preamble.ctrlMask  = I2C_START_BYTE2;

    status = CyFx3BootI2cReceiveBytes (&preamble, rd_buf, SECTION_HEADER_SIZE, I2C_RETRY_CNT);
    if (status == CY_FX3_BOOT_SUCCESS)
    {
        secLength  = gReadBuf[0] * sizeof (uint32_t);
        secAddress = gReadBuf[1];

        if (secLength == 0)
        {
            CyFx3BootJumpToProgramEntry (secAddress);
            return CyTrue;
        }

        gI2cByteAddr += SECTION_HEADER_SIZE;
        while (secLength != 0)
        {
            preamble.buffer[0] = I2C_SLAVE_ADDRESS | (gI2cSlaveAddr << 1) | I2C_CMD_WRITE;
            preamble.buffer[1] = GET_BYTE1 (gI2cByteAddr);
            preamble.buffer[2] = GET_BYTE0 (gI2cByteAddr);
            preamble.buffer[3] = I2C_SLAVE_ADDRESS | (gI2cSlaveAddr << 1) | I2C_CMD_READ;

            tmp = (secLength > 128) ? 128 : secLength;
            if ((tmp + gI2cByteAddr) > I2C_SLAVE_SPACE)
                tmp = I2C_SLAVE_SPACE - gI2cByteAddr;

            status = CyFx3BootI2cReceiveBytes (&preamble, rd_buf, tmp, I2C_RETRY_CNT);
            if (status != CY_FX3_BOOT_SUCCESS)
                return CyFalse;

            CyFx3BootMemCopy ((uint8_t *)secAddress, rd_buf, tmp);
            secAddress += tmp;
            secLength  -= tmp;
            gI2cByteAddr += tmp;

            if (gI2cByteAddr == I2C_SLAVE_SPACE)
            {
                gI2cSlaveAddr++;
                gI2cByteAddr = 0;
            }
        }

        return CyTrue;
    }

    return CyFalse;
}

CyBool_t
MyI2cBoot (
        void)
{
    /* Initialize the I2C interface and check for valid firmware signature. */
    if (!I2cInit ())
        return CyFalse;

    gI2cSlaveAddr = 0;
    gI2cByteAddr  = FW_HEADER_SIZE;

    /* Keep loading firmware as long we don't hit an error and the download is not complete. */
    while (LoadFwSection ());

    /* If boot is successful, control will never reach this point. */
    return CyFalse;
}

/*[]*/

