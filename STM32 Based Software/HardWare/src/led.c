#include "led.h"

void LedInit(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ�ܶ�ӦIO��ʱ��
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//�������ģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_13;//ѡ��13������
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//����IOƵ��Ϊ50MHz
	GPIO_Init(GPIOC,&GPIO_InitStruct);//��ʼ��IO�ڵĿ⺯��
	LED=0; //Ĭ��Ϩ��LED
}
 
