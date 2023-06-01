#include "infraredEmitter.h"
#include "led.h"
#include "delay.h"
#include "usart.h"


void recJudge(u16 time);

//基本定时器TIM6用于定时协议定时（或者滴答定时器），通用定时器TIM2用于产生38kHZ的PWM载波
void InfraredEmitterInit(void){
	u32 TimeClk;//用于保存定时器的内部时钟
	u16 prescaler;//保存预分频器的值
	
	RCC_ClocksTypeDef RCC_Clocks;
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;	
	
	//使能对应io口时钟（TIM的CH2）
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA ,ENABLE);//使能GPIOA时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//使能TIM2时钟
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;//设置IO口复用推挽输出模式
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1;//选择IO口的1号引脚
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//IO口频率50MHz
	GPIO_Init(GPIOA,&GPIO_InitStruct);//IO初始化库函数
	
	//获取系统和各个总线时钟
	RCC_GetClocksFreq(&RCC_Clocks);
	if(RCC_Clocks.SYSCLK_Frequency/RCC_Clocks.PCLK1_Frequency>1)
		TimeClk=RCC_Clocks.PCLK1_Frequency*2;
	else
		TimeClk=RCC_Clocks.PCLK1_Frequency;
	prescaler=TimeClk/10000;
	
	
	
	//初始化TIM2时基单元
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV4;
	TIM_TimeBaseInitStruct.TIM_Period=26-1;//设定计数器自动重装值 PWM周期为26us可得38KHz载波
	TIM_TimeBaseInitStruct.TIM_Prescaler=prescaler/100-1;//预分频器,1M的计数频率,1us加1.	 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);//定时器时基初始化库函数
	//初始化TIM2时比较输出通道
	//TIM_OCInitStruct.TIM_OCIdleState=
	TIM_OCInitStruct.TIM_OCMode=TIM_OCMode_PWM1;//设置比较输出为PWM1模式
	//TIM_OCInitStruct.TIM_OCNIdleState=
	TIM_OCInitStruct.TIM_OCPolarity=TIM_OCPolarity_High;//设置比较输出极性高电平有效
	//TIM_OCInitStruct.TIM_OutputNState=
	TIM_OCInitStruct.TIM_OutputState=TIM_OutputState_Enable;//设置比较输出状态使能
	TIM_OCInitStruct.TIM_Pulse=13;//设置比较寄存器的值为13
	TIM_OC2Init(TIM2,&TIM_OCInitStruct);//定时器输出初始化库函数
	
	//TIM4—CH4通道对应IO初始化
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //使能PORTB时钟 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);	//TIM4 时钟使能 

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;//PB9 输入 
 	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD; //上拉输入 
 	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//IO转换频率50MHz
 	GPIO_Init(GPIOB, &GPIO_InitStruct);//IO初始化库函数
 	GPIO_SetBits(GPIOB,GPIO_Pin_9);	//置位GPIOB.9
	
	TIM_TimeBaseInitStruct.TIM_Period = 60000; //设定计数器自动重装值 最大60ms溢出  
	TIM_TimeBaseInitStruct.TIM_Prescaler =(72-1); 	//预分频器,1M的计数频率,1us加1.	   	
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStruct); //根据指定的参数初始化TIMx
	//配置通道4作为捕获输入并使能该通道：滤波0x3，边沿：上升沿，触发选择：引脚正常映射，预分屏器：1
	TIM_ICInitStruct.TIM_Channel = TIM_Channel_4;  // 选择输入端 IC4映射到TI4上
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM_ICInitStruct.TIM_ICFilter = 0x03;//IC4F=0011 配置输入滤波器 8个定时器时钟周期滤波
	TIM_ICInit(TIM4, &TIM_ICInitStruct);//初始化定时器输入捕获通道
	TIM_ITConfig( TIM4,TIM_IT_Update|TIM_IT_CC4,ENABLE);//允许更新中断 ,允许CC4IE捕获中断	
  
  	//允许更新事件产生
	TIM_UpdateDisableConfig(TIM4,DISABLE);
	//设置更新事件产生源只为溢出
	TIM_UpdateRequestConfig(TIM4,TIM_UpdateSource_Regular);
	
	//设置嵌套向量中断控制器，使能TIM2和4中断
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级为1
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;//响应优先级为1
	NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM4_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	//使能计数器
	//TIM_Cmd(TIM4,ENABLE);
	//失能TIM2—CH2的输出
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
		TIM_ClearITPendingBit(TIM4,TIM_IT_CC4);//清除捕获比较中断
		if(time4Ch4ICPolarity==0){
			TIM_SetCounter(TIM4,0);//设置计数器为0
			//设置下降沿
			TIM_OC4PolarityConfig(TIM4,TIM_OCPolarity_Low);
			time4Ch4ICPolarity=1;
			//使能计数器
			TIM_Cmd(TIM4,ENABLE);
		}
		else{
			u16 time;
			time=TIM_GetCapture4(TIM4);//等到捕获的脉宽
			recJudge(time);//根据脉宽进行具体判断
			//设置上降沿
			TIM_OC4PolarityConfig(TIM4,TIM_OCPolarity_High);
			time4Ch4ICPolarity=0;
			//失能计数器
			TIM_Cmd(TIM4,DISABLE);
		}
	}
	if(TIM_GetITStatus(TIM4,TIM_IT_Update))
	{	
		TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//清除更新中断
		//失能计数器
		TIM_Cmd(TIM4,DISABLE);
		//设置上降沿
		TIM_OC4PolarityConfig(TIM4,TIM_OCPolarity_High);
		time4Ch4ICPolarity=0;
		
//		getGeliSendCode(ReceiveData);
//		cleanAllFraRecFlag();
		
		//只有连续3次捕获的按键值长度一致才进行学习，排除干扰信号
		if(RecBitCountBefore!=ReceiveBitCount){
			//只要不一致以当前值作为参考，清楚累计次数，继续捕获信号
			RecBitCountBefore=ReceiveBitCount;
			ReceiveBitCount=0;
			RecBitCountRight=0;
		}
		else{
			//累计正确次数
			RecBitCountRight++;
			ReceiveBitCount=0;
			
		}
		if(RecBitCountRight>=2)
		{
			//清楚正确累计，方便下次学习按键值
			RecBitCountRight=0;
			//排除连按情况
			if(RecBitCountBefore!=0)	
			{
				//进行学习按键值
				learnKeyValue(ReceiveData,RecBitCountBefore,ReceiveStartBit/3); 
				//学习完之后需要清除所有红外编码学习标记,便于下次学习
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
		ReceiveStartBit++;//累计起始码
	}
	else if(time>1400&&time<1900)
	{
		//逐位接收，通过移位暂存的数据接收数组中
		ReceiveData[ReceiveBitCount/8]>>=1;
		ReceiveData[ReceiveBitCount/8]|=0x80;
		ReceiveBitCount++;//累计数据码
	}
	else if(time>300&&time<700)
	{
		//逐位接收，通过移位暂存的数据接收数组中
		ReceiveData[ReceiveBitCount/8]>>=1;
		ReceiveData[ReceiveBitCount/8]&=0x7f;
		ReceiveBitCount++;//累计数据码
	}
	else if(time>19000&&time<25000)
	{
		ReceiveConnectBit++;//累计连续码
	}
	
	else if(time>39000&&time<43000)
	{
		ReceiveOverBit++;
	}
	else
	{
		ReceiveOtherBit++;//累计其他码（可能有些遥控红外编码格式不是NEC协议）
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
//结束码，不属于nec协议只是硬件实现上需要个最后的下降沿，用于接受端接受最后一位数
#define INFRARED_OVER_CODE 5 
void SendOneBit(u8 bitKind){

	switch(bitKind){
		case INFRARED_START_CODE:
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Enable);//使能TIM2通道2输出
			delay_ms(9);//延时9ms
			TIM_CCxCmd(TIM2,TIM_Channel_2,TIM_CCx_Disable);//失能TIM2通道2输出
			delay_us(4500);//延时4.5ms
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

//发送一字节数据
void InfraredSendData(u8 data){
	u8 i=0;
	//u8 addr=0x55;
	u8 sendTmep;
	//使能计数器
	TIM_Cmd(TIM2,ENABLE);
	//发送同步码
	SendOneBit(INFRARED_START_CODE);
	//逐位发送地址码1
	sendTmep=0x1c;
	for(i=0;i<8;i++)
	{
		if((sendTmep&0x01)==0x01)
			SendOneBit(INFRARED_HIGH);
		else
			SendOneBit(INFRARED_LOW);
		sendTmep=sendTmep>>1;
	}
	//逐位发送地址码2
	sendTmep=0x61;
	for(i=0;i<8;i++)
	{
		if(sendTmep&0x01)
			SendOneBit(INFRARED_HIGH);
		else
			SendOneBit(INFRARED_LOW);
		sendTmep=sendTmep>>1;
	}
	//逐位发数据码
	sendTmep=0xdc;
	for(i=0;i<8;i++)
	{
		if(sendTmep&0x01)
			SendOneBit(INFRARED_HIGH);
		else
			SendOneBit(INFRARED_LOW);
		sendTmep=sendTmep>>1;
	}
	//逐位发数据反码
	sendTmep=0x4b;
	for(i=0;i<8;i++)
	{
		if(sendTmep&0x01)
			SendOneBit(INFRARED_HIGH);
		else
			SendOneBit(INFRARED_LOW);
		sendTmep=sendTmep>>1;
	}
	//发送一个结束码（硬件实现上的需要，非协议标准）
	SendOneBit(INFRARED_OVER_CODE);
	//失能计数器
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
	//失能计数器
	TIM_Cmd(TIM2,DISABLE);
}

//空调例码发射函数
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
	//使能计数器
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
					//原本两次校验码之间有区别是根据配套遥控得来的，现在设为一样也可以
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
	//失能计数器
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
	
	//失能计数器
	TIM_Cmd(TIM2,DISABLE);
}
