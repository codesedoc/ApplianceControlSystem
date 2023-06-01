#include "infraredEmitter.h"
#include "led.h"
#include "delay.h"
#include "usart.h"


void recJudge(u16 time);

//������ʱ��TIM6���ڶ�ʱЭ�鶨ʱ�����ߵδ�ʱ������ͨ�ö�ʱ��TIM2���ڲ���38kHZ��PWM�ز�
void InfraredEmitterInit(void){
	u32 TimeClk;//���ڱ��涨ʱ�����ڲ�ʱ��
	u16 prescaler;//����Ԥ��Ƶ����ֵ
	
	RCC_ClocksTypeDef RCC_Clocks;
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;	
	
	//ʹ�ܶ�Ӧio��ʱ�ӣ�TIM��CH2��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);//ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//ʹ��TIM2ʱ��
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//����IO�ڸ����������ģʽ
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;//ѡ��IO�ڵ�1������
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//IO��Ƶ��50MHz
	GPIO_Init(GPIOA,&GPIO_InitStruct);//IO��ʼ���⺯��
	
	//��ȡϵͳ�͸�������ʱ��
	RCC_GetClocksFreq(&RCC_Clocks);
	if(RCC_Clocks.SYSCLK_Frequency/RCC_Clocks.PCLK1_Frequency>1)
		TimeClk=RCC_Clocks.PCLK1_Frequency*2;
	else
		TimeClk=RCC_Clocks.PCLK1_Frequency;
	prescaler=TimeClk/10000;
	
	
	
	//��ʼ��TIM2ʱ����Ԫ
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV4;
	TIM_TimeBaseInitStruct.TIM_Period=26-1;//�趨�������Զ���װֵ PWM����Ϊ26us�ɵ�38KHz�ز�
	TIM_TimeBaseInitStruct.TIM_Prescaler=prescaler/100-1;//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);//��ʱ��ʱ����ʼ���⺯��
	//��ʼ��TIM2ʱ�Ƚ����ͨ��
	//TIM_OCInitStruct.TIM_OCIdleState=
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//���ñȽ����ΪPWM1ģʽ
	//TIM_OCInitStruct.TIM_OCNIdleState=
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//���ñȽ�������Ըߵ�ƽ��Ч
	//TIM_OCInitStruct.TIM_OutputNState=
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//���ñȽ����״̬ʹ��
	TIM_OCInitStruct.TIM_Pulse=13;//���ñȽϼĴ�����ֵΪ13
	TIM_OC2Init(TIM2,&TIM_OCInitStruct);//��ʱ�������ʼ���⺯��
	
	//TIM4��CH4ͨ����ӦIO��ʼ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //ʹ��PORTBʱ�� 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	//TIM4 ʱ��ʹ�� 

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//PB9 ���� 
 	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD; //�������� 
 	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//IOת��Ƶ��50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStruct);//IO��ʼ���⺯��
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	//��λGPIOB.9
	
	TIM_TimeBaseInitStruct.TIM_Period = 60000; //�趨�������Զ���װֵ ���60ms���  
	TIM_TimeBaseInitStruct.TIM_Prescaler =(72-1); 	//Ԥ��Ƶ��,1M�ļ���Ƶ��,1us��1.	   	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct); //����ָ���Ĳ�����ʼ��TIMx
	//����ͨ��4��Ϊ�������벢ʹ�ܸ�ͨ�����˲�0x3�����أ������أ�����ѡ����������ӳ�䣬Ԥ��������1
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_4;  // ѡ������� IC4ӳ�䵽TI4��
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;	//�����ز���
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //���������Ƶ,����Ƶ 
	TIM_ICInitStruct.TIM_ICFilter = 0x03;//IC4F=0011 ���������˲��� 8����ʱ��ʱ�������˲�
	TIM_ICInit(TIM4, &TIM_ICInitStruct);//��ʼ����ʱ�����벶��ͨ��
	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//��������ж� ,����CC4IE�����ж�	
  
  	//��������¼�����
	TIM_UpdateDisableConfig(TIM4,DISABLE);
	//���ø����¼�����ԴֻΪ���
	TIM_UpdateRequestConfig(TIM4,TIM_UpdateSource_Regular);
	
	//����Ƕ�������жϿ�������ʹ��TIM2��4�ж�
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�Ϊ1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;//��Ӧ���ȼ�Ϊ1
	NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	//ʹ�ܼ�����
	//TIM_Cmd(TIM4,ENABLE);
	//ʧ��TIM2��CH2�����
	TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);
	
	banFraRecive();
}
u8 time4Ch4ICPolarity=0;
u8 RecBitCountBefore=0;
u8 RecBitCountRight=0;

u8 ReceiveStartBit=0;
u8 ReceiveBitCount=0;
u8 ReceiveOtherBit=0;
u8 ReceiveConnectBit=0;
u8 ReceiveOverBit=0;
u8 ReceiveData[32]={0};

extern void learnKeyValue(u8 *value,u8 valueLength,u8 conStart);
void getGeliSendCode(u8 *recData);
	
void TIM4_IRQHandler(void){
	if(TIM_GetITStatus(TIM4,TIM_IT_CC4))
	{	
		TIM_ClearITPendingBit(TIM4,TIM_IT_CC4);//�������Ƚ��ж�
		if(time4Ch4ICPolarity==0){
			TIM_SetCounter(TIM4,0);//���ü�����Ϊ0
			//�����½���
			TIM_OC4PolarityConfig(TIM4,TIM_OCPolarity_Low);
			time4Ch4ICPolarity=1;
			//ʹ�ܼ�����
			TIM_Cmd(TIM4,ENABLE);
		}
		else{
			u16 time;
			time=TIM_GetCapture4(TIM4);//�ȵ����������
			recJudge(time);//����������о����ж�
			//�����Ͻ���
			TIM_OC4PolarityConfig(TIM4,TIM_OCPolarity_High);
			time4Ch4ICPolarity=0;
			//ʧ�ܼ�����
			TIM_Cmd(TIM4,DISABLE);
		}
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_Update))
	{	
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//��������ж�
		//ʧ�ܼ�����
		TIM_Cmd(TIM4,DISABLE);
		//�����Ͻ���
		TIM_OC4PolarityConfig(TIM4,TIM_OCPolarity_High);
		time4Ch4ICPolarity=0;
		
//		getGeliSendCode(ReceiveData);
//		cleanAllFraRecFlag();
		
		//ֻ������3�β���İ���ֵ����һ�²Ž���ѧϰ���ų������ź�
		if(RecBitCountBefore!=ReceiveBitCount){
			//ֻҪ��һ���Ե�ǰֵ��Ϊ�ο�������ۼƴ��������������ź�
			RecBitCountBefore=ReceiveBitCount;
			ReceiveBitCount=0;
			RecBitCountRight=0;
		}
		else{
			//�ۼ���ȷ����
			RecBitCountRight++;
			ReceiveBitCount=0;
			
		}
		if(RecBitCountRight>=2)
		{
			//�����ȷ�ۼƣ������´�ѧϰ����ֵ
			RecBitCountRight=0;
			//�ų��������
			if(RecBitCountBefore!=0)	
			{
				//����ѧϰ����ֵ
				learnKeyValue(ReceiveData,RecBitCountBefore,ReceiveStartBit/3); 
				//ѧϰ��֮����Ҫ������к������ѧϰ���,�����´�ѧϰ
				cleanAllFraRecFlag();
			}
		}
		 
	}
}
void cleanAllFraRecFlag(void){
	u8 i;
	ReceiveStartBit=0;
	ReceiveBitCount=0;
	ReceiveOtherBit=0;
	ReceiveConnectBit=0;
	RecBitCountBefore=0;
	ReceiveOverBit=0;
	RecBitCountRight=0;
	for(i=0;i<32;i++)
		ReceiveData[i]=0;
}
void allowFraRecive(){
	TIM_CCxCmd(TIM4,TIM_Channel_4,TIM_CCx_Enable);
}
void banFraRecive(){
	TIM_CCxCmd(TIM4,TIM_Channel_4,TIM_CCx_Disable);
}	

void recJudge(u16 time){

	if(time>4200&&time<4700)
	{
		ReceiveStartBit++;//�ۼ���ʼ��
	}
	else if(time>1400&&time<1900)
	{
		//��λ���գ�ͨ����λ�ݴ�����ݽ���������
		ReceiveData[ReceiveBitCount/8]>>=1;
		ReceiveData[ReceiveBitCount/8]|=0x80;
		ReceiveBitCount++;//�ۼ�������
	}
	else if(time>300&&time<700)
	{
		//��λ���գ�ͨ����λ�ݴ�����ݽ���������
		ReceiveData[ReceiveBitCount/8]>>=1;
		ReceiveData[ReceiveBitCount/8]&=0x7f;
		ReceiveBitCount++;//�ۼ�������
	}
	else if(time>19000&&time<25000)
	{
		ReceiveConnectBit++;//�ۼ�������
	}
	
	else if(time>39000&&time<43000)
	{
		ReceiveOverBit++;
	}
	else
	{
		ReceiveOtherBit++;//�ۼ������루������Щң�غ�������ʽ����NECЭ�飩
	}
	
}


void getGeliSendCode(u8 *recData)
{
	u8 i=0,j=0;
	u32 dataTemp;
	printf("123456\r\n");
	for(i=0;i<134;i++){
		if(i%8==0)
		{
			dataTemp=recData[i/8];
		}
		if(j%8==0)
			printf("  ");

		if(i<128)
		{
			if(dataTemp&0x01)
				printf("%c",'1');
			else
				printf("%c",'0');
		}
		else{
			if(dataTemp&0x04)
				printf("%c",'1');
			else
				printf("%c",'0');
		}
			
		dataTemp>>=1;
	
		j++;	
		if(i==66)
		{	
			printf("\r\n");
			j=0;
		}
		
	}
	printf("\r\n");
}

//void getAllReceiveData(void)
//{    
//	u8 i=0;
//	if(ReceiveBitCount==134 || ReceiveBitCount==32)
//		LED=~LED;
//	getGetliSendCode(ReceiveData);	
//	ReceiveStartBit=0;
//	ReceiveBitCount=0;
//	ReceiveOtherBit=0;
//	ReceiveConnectBit=0;
//	RecBitCountBefore=0;
//	ReceiveOverBit=0;
//	for(i=0;i<32;i++)
//		ReceiveData[i]=0;
//}


#define INFRARED_START_CODE 0
#define INFRARED_HIGH 1
#define INFRARED_LOW 2
#define INFRARED_CONTINUOUS_CODE 3
#define INFRARED_CONNECT_CODE 4
//�����룬������necЭ��ֻ��Ӳ��ʵ������Ҫ�������½��أ����ڽ��ܶ˽������һλ��
#define INFRARED_OVER_CODE 5 
void SendOneBit(u8 bitKind){

	switch(bitKind){
		case INFRARED_START_CODE:
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Enable);//ʹ��TIM2ͨ��2���
			delay_ms(9);//��ʱ9ms
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);//ʧ��TIM2ͨ��2���
			delay_us(4500);//��ʱ4.5ms
			break;
		
		case INFRARED_HIGH:
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Enable);
			delay_us(560);
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);
			delay_us(1680);
			break;
		
		case INFRARED_LOW:
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Enable);
			delay_us(560);
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);
			delay_us(560);
			break;
		
		case INFRARED_CONTINUOUS_CODE:
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Enable);
			delay_ms(9);
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);
			delay_us(2250);
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Enable);
			delay_us(560);
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);
			break;
		
		case INFRARED_CONNECT_CODE:
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Enable);
			delay_us(560);
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);
			delay_ms(20);
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Enable);
			delay_us(560);
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);
			break;
		
		case INFRARED_OVER_CODE:
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Enable);
			delay_us(500);
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);
			delay_us(40000);
			break;
		
	}	
}

//����һ�ֽ�����
void InfraredSendData(u8 data){
	u8 i=0;
	//u8 addr=0x55;
	u8 sendTmep;
	//ʹ�ܼ�����
	TIM_Cmd(TIM2,ENABLE);
	//����ͬ����
	SendOneBit(INFRARED_START_CODE);
	//��λ���͵�ַ��1
	sendTmep=0x1c;
	for(i=0;i<8;i++)
	{
		if((sendTmep&0x01)==0x01)
			SendOneBit(INFRARED_HIGH);
		else
			SendOneBit(INFRARED_LOW);
		sendTmep=sendTmep>>1;
	}
	//��λ���͵�ַ��2
	sendTmep=0x61;
	for(i=0;i<8;i++)
	{
		if(sendTmep&0x01)
			SendOneBit(INFRARED_HIGH);
		else
			SendOneBit(INFRARED_LOW);
		sendTmep=sendTmep>>1;
	}
	//��λ��������
	sendTmep=0xdc;
	for(i=0;i<8;i++)
	{
		if(sendTmep&0x01)
			SendOneBit(INFRARED_HIGH);
		else
			SendOneBit(INFRARED_LOW);
		sendTmep=sendTmep>>1;
	}
	//��λ�����ݷ���
	sendTmep=0x4b;
	for(i=0;i<8;i++)
	{
		if(sendTmep&0x01)
			SendOneBit(INFRARED_HIGH);
		else
			SendOneBit(INFRARED_LOW);
		sendTmep=sendTmep>>1;
	}
	//����һ�������루Ӳ��ʵ���ϵ���Ҫ����Э���׼��
	SendOneBit(INFRARED_OVER_CODE);
	//ʧ�ܼ�����
	TIM_Cmd(TIM2,DISABLE);
}
void infraSendAnyLengData(u8 *dataGroup,u8 length){
	u8 i;
	u8 dataTemp;
	u8 remainder=length%8;
	TIM_Cmd(TIM2,ENABLE);
	SendOneBit(INFRARED_START_CODE);
	if(length==255)
		return;
	for(i=0;i<length;i++){
		if(i%8==0)
		{
			dataTemp=dataGroup[i/8];
		}
		if((length-i)>remainder)
		{
			if(dataTemp&0x01)
				SendOneBit(INFRARED_HIGH);
			else
				SendOneBit(INFRARED_LOW);
		}
		else{
			if(dataTemp&(1<<(remainder-1)))
				SendOneBit(INFRARED_HIGH);
			else
				SendOneBit(INFRARED_LOW);
		}
			
		dataTemp>>=1;
		
	}
	SendOneBit(INFRARED_OVER_CODE);
	//ʧ�ܼ�����
	TIM_Cmd(TIM2,DISABLE);
}

//�յ����뷢�亯��
u8 airModel=2;
u8 airPower=0;
u8 airSpeed=1;
u8 airTempture=0;
u8 airUDSweep=0;
u8 airLRSweep=0;
u16 airFixValue=0x128;
u16 airFixValue2=0x138;
void airInfraredSendGroupData(u16 data,u8 leng){
	u8 j=0;
	for(j=0;j<leng;j++)
	{
		if(data&0x01)
			SendOneBit(INFRARED_HIGH);
		else
			SendOneBit(INFRARED_LOW);
		data>>=1;
	}
}
void airInfraredSendData(void){
	u8 i=0,t;
	
	u16 dataTemp;
	u8 dataLengthTemp=1;
	if(airPower==0)
		airPower=1;
	else
		airPower=0;
	//ʹ�ܼ�����
	TIM_Cmd(TIM2,ENABLE);
	for(t=0;t<2;t++){
		SendOneBit(INFRARED_START_CODE);
		for(i=0;i<68;){
			switch(i){
				case 0:
					dataLengthTemp=3;
					dataTemp=airModel;
					airInfraredSendGroupData(dataTemp,dataLengthTemp);
					i+=dataLengthTemp;
				break;
				
				case 3:
					if(airPower)
						SendOneBit(INFRARED_HIGH);
					else
						SendOneBit(INFRARED_LOW);
					i++;
				break;
				
				case 4:
					dataLengthTemp=2;
					dataTemp=airSpeed;
					airInfraredSendGroupData(dataTemp,dataLengthTemp);
					i+=dataLengthTemp;
				break;
				
				case 6:
					if(airUDSweep!=0 ||airLRSweep!=0)
						SendOneBit(INFRARED_HIGH);
					else
						SendOneBit(INFRARED_LOW);
					i++;
				break;
				
				case 8:
					dataLengthTemp=4;
					dataTemp=airTempture;
					airInfraredSendGroupData(dataTemp,dataLengthTemp);
					i+=dataLengthTemp;
				break;
				
				case 25:
					dataLengthTemp=10;
					if(t==0){
						dataTemp=airFixValue;
						}
					else{
						dataTemp=airFixValue2;
					}
					
					airInfraredSendGroupData(dataTemp,dataLengthTemp);
					i+=dataLengthTemp;
				break;
				
				case 35:
					SendOneBit(INFRARED_CONNECT_CODE);
					i++;
				break;
				
				
				
				case 36:
					if(airUDSweep)
						SendOneBit(INFRARED_HIGH);
					else
						SendOneBit(INFRARED_LOW);
					i++;
				break;
				
				case 40:
					if(airLRSweep)
						SendOneBit(INFRARED_HIGH);
					else
						SendOneBit(INFRARED_LOW);
					i++;
				break;
				
				
				case 64:
					dataLengthTemp=4;
					dataTemp=5+airModel-1+airTempture+airLRSweep;
					if(airPower==0){
						if(dataTemp&(1<<3)){
							dataTemp&=0x7;
						}
						else
							dataTemp|=0x8;
					}
					//ԭ������У����֮���������Ǹ�������ң�ص����ģ�������Ϊһ��Ҳ����
//					if(t){
//						//dataTemp--;
//					}
					airInfraredSendGroupData(dataTemp,dataLengthTemp);
					i+=dataLengthTemp;
				break;
				
				default:
					if(t==0){
						if(i==23||i==49 || i==21)
							SendOneBit(INFRARED_HIGH);
						else
							SendOneBit(INFRARED_LOW);
					}
					else{
						if(i==23||i==49|| i==21)
							SendOneBit(INFRARED_HIGH);
						else
							SendOneBit(INFRARED_LOW);
					}
					i++;
				break;	
			}
		}
		SendOneBit(INFRARED_OVER_CODE);
			
	}
	if(airPower)
		airTempture++;
	//ʧ�ܼ�����
	TIM_Cmd(TIM2,DISABLE);
}
void sendGLAirFraSign(u8 *airValues1,u8 *airValues2){
	u8 i,t;
	u8 dataTemp;
	u8 length=35;
	TIM_Cmd(TIM2,ENABLE);
	for(t=0;t<2;t++){
		SendOneBit(INFRARED_START_CODE);
		length=35;
		for(i=0;i<length;i++){
			if(i%8==0)
			{
				dataTemp=airValues1[i/8];
			}
			
			if(dataTemp&0x01)
				SendOneBit(INFRARED_HIGH);
			else
				SendOneBit(INFRARED_LOW);
			
			dataTemp>>=1;
			
		}
		SendOneBit(INFRARED_CONNECT_CODE);
		length=32;
		for(i=0;i<length;i++){
			if(i%8==0)
			{
				dataTemp=airValues2[i/8];
			}
			if(dataTemp&0x01)
				SendOneBit(INFRARED_HIGH);
			else
				SendOneBit(INFRARED_LOW);
			
			dataTemp>>=1;
			
		}
		
		SendOneBit(INFRARED_OVER_CODE);
	}
	
	//ʧ�ܼ�����
	TIM_Cmd(TIM2,DISABLE);
}
