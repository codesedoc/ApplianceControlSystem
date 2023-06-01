#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "independentKey.h"
#include "infraredEmitter.h"
#include "hc05.h"
#include "usart3.h"
#include "at24c256.h"
#include "application.h"
#include "lcd1602.h"
#include "oled.h"
int main(void)
{
	//用NVIC将系统终端优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//各部分初始化
	delay_init();
	LedInit(); 
	uart_init(9600);
	at24c256Init();
	IndependentKeyInit();
	InfraredEmitterInit();	
	OLED_Init();
	//oled清屏操作
	OLED_Clear(); 
	//串口提示系统正常
	printf("Hello!\r\n"); 
	while(1) 		 
	{
		appplicationRun();
	}	
}





