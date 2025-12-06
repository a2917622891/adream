#ifndef __I2C_H__
#define __I2C_H__

#include "RM.h"  // 直接引用RM.h，获取SDA/SCL相关函数

// 函数声明（
void I2C_Start(void);        // 起始条件
void I2C_Stop(void);         // 终止条件
void I2C_SendByte(unsigned char dat);  // 发送一个字节（8Bit）
unsigned char I2C_ReceiveAck(void);    // 接收应答
void I2C_SendData(unsigned char dat);  // 完整流程：Start→SendByte→ReceiveAck→Stop

#endif
