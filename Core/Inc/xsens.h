#ifndef __XSENS_H__
#define __XSENS_H__

#include "main.h"

#define XSENS_AHRS_ID       0x70

enum XSENS_REQ 
{
    IdReq = 0x75,
    AccReq,
    GyroReq,
    MagReq,
    FullDataReq
};

void I2Creset();
uint8_t CheckSum(uint8_t* Data, uint8_t size);
uint8_t CheckDataHealth(uint8_t* Data, uint8_t size);
void flushBuf();
void PrepareXsensData();
void ReadAHRS();
HAL_StatusTypeDef SendAHRSData();
HAL_StatusTypeDef ReceiveI2CData();
void ResetUART();

#endif