#include "I2C.h"
#include "RM.h" 
void I2C_Start(void) {
    SDA_High(); 
    SCL_High(); 
    SDA_Low(); 
    SCL_Low(); 
}
void I2C_Stop(void) {
    SDA_Low();
    SCL_High();
    SDA_High(); 
    SCL_Low();
}
void I2C_SendByte(unsigned char dat) {
    unsigned char i; 
    for (i = 0; i < 8; i++) { 
        SCL_Low(); 
        
        // 判断当前要发送的位（ dat的第7-i位：比如i=0时，取第7位）
        if (dat & (0x80 >> i)) {  // 0x80是10000000，右移i位取对应位
            SDA_High();  // 该位是1，SDA=1
        } else {
            SDA_Low();   // 该位是0，SDA=0
        }
        
        // （可选：加微小延时，确保SDA稳定）
        SCL_High();  // SCL=1，从机读取SDA上的位
        // （可选：加微小延时，确保从机读完）
        SCL_Low();   // SCL拉低，准备下一位
    }
}
unsigned char I2C_ReceiveAck(void) {
    unsigned char ack;
    
    SCL_Low(); 
    SDA_High(); 
    
    SCL_High(); 
    ack = SDA_Read();  
    SCL_Low();
    return ack; 
}
void I2C_SendData(unsigned char dat) {
    I2C_Start();       
    I2C_SendByte(dat);
    I2C_ReceiveAck();
    I2C_Stop();
}
