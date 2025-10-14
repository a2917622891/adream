#include <stdio.h>

int main() {
    int input;
    while (1) {
        printf("请输入-1、0或1：");
        scanf("%d", &input);
        
        if (input == -1) {
            printf("程序退出\n");
            break;
        } else if (input == 0) {
            printf("helloworld\n");
        } else if (input == 1) {
            printf("HELLOWORLD\n");
        } else {
            printf("输入无效，请重新输入\n");
        }
    }
    
    return 0;
}
    
