#include "independentKey.h"
#include "led.h"
#include "delay.h"
#include "infraredEmitter.h"
#include "application.h"

void IndependentKeyInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//使能对应io口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOE,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource2);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource3);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource4);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5);
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Rising;
	
	EXTI_Init(&EXTI_InitStruct);
	
	EXTI_InitStruct.EXTI_Line=EXTI_Line2|EXTI_Line3|EXTI_Line4|EXTI_Line5;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=2;
	
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI2_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStruct);	
	NVIC_InitStruct.NVIC_IRQChannel=EXTI9_5_IRQn;
	NVIC_Init(&NVIC_InitStruct);


}

extern u8 deviceKeyValue;
void EXTI2_IRQHandler(void){
	EXTI_ClearITPendingBit(EXTI_Line2);//清除中断标志位
		delay_ms(20);//延时消抖
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)!=0){
		return;//排除抖动信号
	}
	deviceKeyValue =DEVICE_STATUS_KEY;//设置状态切换键按下
}
void EXTI3_IRQHandler(void){
	EXTI_ClearITPendingBit(EXTI_Line3);
		delay_ms(20);//延时消抖
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)!=0){
		return;//排除抖动信号
	}
	deviceKeyValue =DEVICE_SET;//设置set键按下
}
void EXTI4_IRQHandler(void){
	EXTI_ClearITPendingBit(EXTI_Line4);//清除中断标志位
		delay_ms(20);//延时消抖
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)!=0){
		return;
	}
	deviceKeyValue =DEVICE_DOWN_KEY;//设置向下键按下
} 

void EXTI9_5_IRQHandler(void){
	EXTI_ClearITPendingBit(EXTI_Line5);//清除中断标志位
		delay_ms(20);//延时消抖
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)!=0){
		return;//排除抖动信号
	}
	deviceKeyValue =DEVICE_UP_KEY;//设置向上键按下
	airInfraredSendData();
}

u8 ReadDEVICE_SETKey(void){
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)!=0){
		return 0;
	}
	else
		return 1;
}

