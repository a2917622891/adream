#include <stdio.h>
typedef enum {
    GPIO_Speed_2MHz, 
    GPIO_Speed_10MHz,
    GPIO_Speed_50MHz 
} GPIO_SpeedTypeDef;

typedef struct {
    GPIO_SpeedTypeDef GPIO_Speed; 
} GPIO_InitTypeDef;

void GPIO_StructureInit(GPIO_InitTypeDef* GPIO_InitStruct) {
    if (GPIO_InitStruct != NULL) {
        GPIO_InitStruct->GPIO_Speed = GPIO_Speed_2MHz;
    }
}

int main() {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_StructureInit(&GPIO_InitStructure);
 printf("Ä¬ÈÏµÄGPIOËÙ¶ÈÎª: %d\n", GPIO_InitStructure.GPIO_Speed);
    
    return 0;
}
    
