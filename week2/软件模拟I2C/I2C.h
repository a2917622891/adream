#ifndef __I2C_H__  
#define __I2C_H__
#include "RM.h"


void I2C_Start(void); 
void I2C_Stop(void);
void I2C_SendByte(unsigned char dat);
unsigned char I2C_ReceiveAck(void); 
void I2C_SendData(unsigned char dat); 

#endif
