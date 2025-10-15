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
        
        // �жϵ�ǰҪ���͵�λ�� dat�ĵ�7-iλ������i=0ʱ��ȡ��7λ��
        if (dat & (0x80 >> i)) {  // 0x80��10000000������iλȡ��Ӧλ
            SDA_High();  // ��λ��1��SDA=1
        } else {
            SDA_Low();   // ��λ��0��SDA=0
        }
        
        // ����ѡ����΢С��ʱ��ȷ��SDA�ȶ���
        SCL_High();  // SCL=1���ӻ���ȡSDA�ϵ�λ
        // ����ѡ����΢С��ʱ��ȷ���ӻ����꣩
        SCL_Low();   // SCL���ͣ�׼����һλ
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
