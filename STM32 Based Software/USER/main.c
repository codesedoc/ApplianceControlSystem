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
	//��NVIC��ϵͳ�ն����ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//�����ֳ�ʼ��
	delay_init();
	LedInit(); 
	uart_init(9600);
	at24c256Init();
	IndependentKeyInit();
	InfraredEmitterInit();	
	OLED_Init();
	//oled��������
	OLED_Clear(); 
	//������ʾϵͳ����
	printf("Hello!\r\n"); 
	while(1) 		 
	{
		appplicationRun();
	}	
}





