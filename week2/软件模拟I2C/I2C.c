#include "I2C.h"

// 延时宏定义
#define I2C_DELAY_MS 2

/**
 * @brief  I2C起始条件（标准时序+延时）
 * 时序：SDA高 → SCL高（保持2ms） → SDA低 → SCL低
 */
void I2C_Start(void) {
    SDA_High();          // SDA置高（空闲状态）
    SCL_High();          // SCL置高
    delay_ms(I2C_DELAY_MS);  // 高电平保持2ms，确保从机识别
    
    SDA_Low();           // SCL高电平时SDA拉低，产生起始信号
    delay_ms(I2C_DELAY_MS);  // 保持起始信号2ms
    
    SCL_Low();           // SCL拉低，准备发送数据
}

/**
 * @brief  I2C终止条件（标准时序+延时）
 * 时序：SDA低 → SCL高（保持2ms） → SDA高 → SCL低
 */
void I2C_Stop(void) {
    SDA_Low();           // SDA置低（数据传输状态）
    SCL_High();          // SCL置高
    delay_ms(I2C_DELAY_MS);  // 高电平保持2ms，确保从机识别
    
    SDA_High();          // SCL高电平时SDA拉高，产生终止信号
    delay_ms(I2C_DELAY_MS);  // 保持终止信号2ms
    
    SCL_Low();           // SCL拉低，总线稳定
}

/**
 * @brief  I2C发送一个字节（8Bit，标准时序+延时）
 * @param  dat: 待发送的8位数据（0~255）
 * 时序：SCL低电平时准备数据 → SCL高电平（保持2ms）从机读取 → 逐位发送（bit7先送）
 */
void I2C_SendByte(unsigned char dat) {
    unsigned char i;
    for (i = 0; i < 8; i++) {  // 循环8次，发送8位
        SCL_Low();              // SCL拉低，允许SDA变化
        delay_ms(1);            // 数据稳定延时
        
        // 提取当前位（从最高位bit7开始）
        if (dat & (0x80 >> i)) {
            SDA_High();         // 该位为1，SDA置高
        } else {
            SDA_Low();          // 该位为0，SDA置低
        }
        
        delay_ms(1);            // 数据稳定后再拉高SCL
        SCL_High();             // SCL拉高，从机读取当前位
        delay_ms(I2C_DELAY_MS);  // 高电平保持2ms，确保从机读完
        
        SCL_Low();              // SCL拉低，准备下一位
    }
}

/**
 * @brief  I2C接收从机应答信号（标准时序+延时）
 * @retval 0: 应答（Ack），1: 非应答（Nack）
 * 时序：主机释放SDA → SCL高（保持2ms） → 读取SDA状态
 */
unsigned char I2C_ReceiveAck(void) {
    unsigned char ack;
    
    SCL_Low();          // SCL拉低，准备接收
    SDA_High();         // 主机释放SDA，由从机控制
    delay_ms(1);        // 总线稳定延时
    
    SCL_High();         // SCL拉高，读取应答位
    delay_ms(I2C_DELAY_MS);  // 高电平保持2ms，等待从机输出应答
    
    ack = SDA_Read();   // 读取SDA电平（0=Ack，1=Nack）
    SCL_Low();          // SCL拉低，结束应答读取
    delay_ms(1);        // 稳定延时
    
    return ack;
}

/**
 * @brief  I2C发送数据完整流程（题目要求+时序延时）
 * @param  dat: 待发送的8位数据
 * 流程：Start → SendByte → ReceiveAck → Stop
 */
void I2C_SendData(unsigned char dat) {
    I2C_Start();         // 1. 起始条件
    I2C_SendByte(dat);   // 2. 发送一个字节
    I2C_ReceiveAck();    // 3. 接收应答
    I2C_Stop();          // 4. 终止条件
}
