#include "i2c.h"
#include "usart.h"
#include "xsens.h"

uint8_t RxData[69];
uint8_t Xsens[69];
uint8_t XsensAcc[15];
uint8_t XsensGyro[15];
uint8_t XsensMag[15];
uint8_t RxI2C[1];
uint8_t Config[5];
uint8_t ACK[5];
uint8_t BaudMsg[5];
uint8_t IdBuf[1];

void ResetUART()
{
  HAL_UART_DeInit(&huart4);
  HAL_UART_Init(&huart4);
  HAL_UART_Receive_IT(&huart4, RxData, sizeof(RxData));
}
void I2Creset()
{
  HAL_I2C_DeInit(&hi2c1);
  HAL_I2C_Init(&hi2c1);
  HAL_I2C_EnableListen_IT(&hi2c1);
}

uint8_t CheckSum(uint8_t* Data, uint8_t size)
{
  uint16_t sum = 0;
  for (int i = 0; i < size; i++)
    sum += Data[i];
  return (sum - 0xFA) & 0xFF;
}

uint8_t CheckDataHealth(uint8_t* Data, uint8_t size)
{
  if (Data[0] == 0xFA && Data[1] == 0xFF && CheckSum(Data, size) == 0)
    return 1;
  else
    return 0;
}

void flushBuf()
{
  for (int i = 0; i < sizeof(RxData); i++)
    RxData[i] = 0;
}

void PrepareXsensData()
{
  
  for (int i = 0; i < sizeof(RxData); i++)
    Xsens[i] = RxData[i];
  for (int i = 0; i < sizeof(XsensAcc); i++)
    XsensAcc[i] = RxData[i+23];
  for (int i = 0; i < sizeof(XsensGyro); i++)
    XsensGyro[i] = RxData[i+38];
  for (int i = 0; i < sizeof(XsensMag); i++)
    XsensMag[i] = RxData[i+53];
}

void ReadAHRS()
{
  IdBuf[0] = XSENS_AHRS_ID;
  if (HAL_UART_Receive_IT(&huart4, RxData, sizeof(RxData)) != HAL_OK)
    ResetUART();
  HAL_Delay(6);
  if (CheckDataHealth(RxData, sizeof(RxData)))
    PrepareXsensData();
  flushBuf();
}

HAL_StatusTypeDef SendAHRSData()
{
  if (RxI2C[0] == IdReq)
    return HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, IdBuf, sizeof(IdBuf), I2C_FIRST_AND_LAST_FRAME);
  else if(RxI2C[0] == AccReq)
    return HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, XsensAcc, sizeof(XsensAcc), I2C_FIRST_AND_LAST_FRAME);
  else if(RxI2C[0] == GyroReq)
    return HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, XsensGyro, sizeof(XsensGyro), I2C_FIRST_AND_LAST_FRAME);
  else if(RxI2C[0] == MagReq)
    return HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, XsensMag, sizeof(XsensMag), I2C_FIRST_AND_LAST_FRAME);
  else
    return HAL_I2C_Slave_Seq_Transmit_IT(&hi2c1, Xsens, sizeof(Xsens), I2C_FIRST_AND_LAST_FRAME);
}

HAL_StatusTypeDef ReceiveI2CData()
{
    return HAL_I2C_Slave_Seq_Receive_IT(&hi2c1, RxI2C, sizeof(RxI2C), I2C_FIRST_AND_LAST_FRAME);
}