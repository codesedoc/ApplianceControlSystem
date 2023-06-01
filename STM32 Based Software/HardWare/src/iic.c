#include "iic.h"
#include "delay.h"
//内部使用函数
void iicSetIOPinDir(u16 pin,u8 dir);

void iicSetIOPinDir(u16 pin,u8 dir){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=pin;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	//使能对应io口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	if(dir==IIC_PIN_OUT){
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//推挽输出			
	}
	else{
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;//上拉输入
	}
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}
/********************/
//初始化函数
/********************/
void iicInit(void){
	iicSetIOPinDir(SCL_PIN,IIC_PIN_OUT);
	iicSetIOPinDir(SDL_PIN,IIC_PIN_OUT);
	SDL_OUT=1;
	SCL_OUT=1;
	delay_init();
}

/********************/
//检测总线忙状态函数
//返回u8类型数据代表总线状态
/********************/
u8 iicCheckBusy(void){
	iicSetIOPinDir(SCL_PIN,IIC_PIN_IN);
	iicSetIOPinDir(SDL_PIN,IIC_PIN_IN);
	if(SCL_IN==1&&SDL_IN==1){
		return IIC_IDLE;
	}
	else{
		return IIC_BUSY;
	}
	
}

/********************/
//发送开始信号函数
/********************/
void iicSendStart(void){
	iicSetIOPinDir(SCL_PIN,IIC_PIN_OUT);//设置SCL引脚为输出模式
	iicSetIOPinDir(SDL_PIN,IIC_PIN_OUT);//设置SDL引脚为输出模式
	SCL_OUT=0;//设置SCL引脚为输出模式
	SDL_OUT=1;//设置SCL引脚为输出模式
	SCL_OUT=1;//设置SCL引脚为输出模式
	delay_us(4);
	SDL_OUT=0;
	delay_us(4);
	SCL_OUT=0;	
}

/********************/
//发送终止信号函数
/********************/
void iicSendStop(void){
	iicSetIOPinDir(SCL_PIN,IIC_PIN_OUT);
	iicSetIOPinDir(SDL_PIN,IIC_PIN_OUT);
	SCL_OUT=0;
	SDL_OUT=0;
	delay_us(5);
	SCL_OUT=1;
	SDL_OUT=1;
	delay_us(3000);
}

/********************/
//发送从设备地址函数
//addr:7位地址值
//wr：读写标志位，0写1读
/********************/
u8 iicSendAddr(u8 addr,u8 wr){
	u8 dataTemp=addr,result;
	dataTemp|=wr&0x01;
	result=iicSendOneByte(dataTemp);
	return result;
}

/********************/
//发送一字节数据函数
//data：要发送的一字节数据
/********************/

u8 iicSendOneByte(u8 data){
	u8 i;
	u8 dataTemp=data,result;
	//u8 time;
	iicSetIOPinDir(SCL_PIN,IIC_PIN_OUT);
	iicSetIOPinDir(SDL_PIN,IIC_PIN_OUT);
	for(i=0;i<8;i++){
		SCL_OUT=0;
		if((dataTemp&0x80)>>7) 
			SDL_OUT=1;
		else
			SDL_OUT=0;
		dataTemp<<=1;
		
		delay_us(5);
		SCL_OUT=1;
		delay_us(5);
		SCL_OUT=0;		
	}
	iicSetIOPinDir(SDL_PIN,IIC_PIN_IN);
	SCL_OUT=0;
	delay_us(4);
	SCL_OUT=1;
	delay_us(4);
	if(SDL_IN){
		result=1;
	}
	else
	{
		result=0;
	}
	SCL_OUT=0;
	return result;
}

/********************/
//接受一字节数据函数
//isAck：是否响应ACK，0否1是
//返回u8类型接受的一字节数据
/********************/
u8 iicReceiveOneByte(u8 isAck){
	u8 i;
	u8 result=0;
	iicSetIOPinDir(SCL_PIN,IIC_PIN_OUT);
	iicSetIOPinDir(SDL_PIN,IIC_PIN_IN);
	for(i=0;i<8;i++){
		SCL_OUT=0;
		delay_us(4);
		SCL_OUT=1;
		delay_us(5);
		
		result<<=1;
		result|=SDL_IN&0x01;
		SCL_OUT=0;
	}
	iicSetIOPinDir(SDL_PIN,IIC_PIN_OUT);
	SCL_OUT=0;
	if(isAck){
		SDL_OUT=0;
	}
	else
		SDL_OUT=1;
	delay_us(4);
	SCL_OUT=1;
	delay_us(5);
	SCL_OUT=0;
	return result;
}
