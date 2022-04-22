/*
## Cypress USB 3.0 Platform header file (cyu3imagesensor.h)
## ===========================
##
##  Copyright Cypress Semiconductor Corporation, 2013,
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

#ifndef _INCLUDED_CYU3_CX3_IMAGESENSOR_H_
#define _INCLUDED_CYU3_CX3_IMAGESENSOR_H_

#include <cyu3types.h>
/* Enumeration defining Stream Format used for CyCx3_ImageSensor_Set_Format */
typedef enum CyU3PSensorStreamFormat_t
{
    SENSOR_YUV2 = 0,    /* UVC YUV2 Format*/
    SENSOR_RGB565,      /* RGB565 Format  */
    SENSOR_RGB888,      /* RGB-888 Format  */
    SENSOR_RAW8,        /* RAW 8 bit Format  */
    SENSOR_RAW10        /* RAW 10 bit Format */
}CyU3PSensorStreamFormat_t;


/* Initialize Image Sensor*/ 
extern void CyCx3_ImageSensor_Init(void);

/* Set Sensor to output 720p Stream at 55FPS*/
extern void CyCx3_ImageSensor_Set_720p55(void);

/* Set Sensor to output 1080p Stream at 30FPS*/
extern void CyCx3_ImageSensor_Set_1080p30(void);

/* Set Sensor to output QVGA Stream*/
extern void CyCx3_ImageSensor_Set_Qvga(void);

/* Set Sensor to output VGA Stream*/
extern void CyCx3_ImageSensor_Set_Vga(void);


/* Put Image Sensor to Sleep/Low Power Mode */
extern void CyCx3_ImageSensor_Cam_Power_Down(void);

/* Wake Image Sensor from Sleep/Low Power Mode to Active Mode */
extern void CyCx3_ImageSensor_Cam_Power_Up(void);

/* Set Image Sensor Data Format */
void CyCx3_ImageSensor_Set_Format(CyU3PSensorStreamFormat_t format);

/* Brightness Management Routines */
extern uint16_t CyCx3_ImageSensorGetBrightness(void);
extern void CyCx3_ImageSensorSetBrightness(uint16_t Brightness);

/* Contrast Management Routines */
extern uint16_t CyCx3_ImageSensorGetContrast(void);
extern void CyCx3_ImageSensorSetContrast(uint16_t Contrast);

/* Hue Management Routines */
extern int16_t CyCx3_ImageSensorGetHue(void);
extern void CyCx3_ImageSensorSetHue(int16_t Hue);

/* Saturation Management Routines */
extern int16_t CyCx3_ImageSensorGetSaturation(void);
extern void CyCx3_ImageSensorSetSaturation(int16_t Saturation);

/* Sharpness Management Routines */
extern int16_t CyCx3_ImageSensorGetSharpness(void);
extern void CyCx3_ImageSensorSetSharpness(int16_t Sharpness);

/* Gamma Management Routines */
extern int16_t CyCx3_ImageSensorGetGamma(void);
extern void CyCx3_ImageSensorSetGamma(int16_t Gamma);

/* Auto White Balance Management Routines */
extern uint8_t CyCx3_ImageSensorGetWhiteBalanceAuto(void);
extern void CyCx3_ImageSensoretWhiteBalanceAuto(uint8_t WhiteBalanceAuto);

/* Manual White Balance Management Routines */
extern int16_t CyCx3_ImageSensorGetWhiteBalanceManual(void);
extern void CyCx3_ImageSensorSetWhiteBalanceManual(int16_t WhiteBalance);

#endif /* _INCLUDED_CYU3_CX3_IMAGESENSOR_H_ */
