#include "led.h"

void LedInit(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能对应IO口时钟
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;//选择13号引脚
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//设置IO频率为50MHz
	GPIO_Init(GPIOC,&GPIO_InitStruct);//初始化IO口的库函数
	LED=0; //默认熄灭LED
}
 
