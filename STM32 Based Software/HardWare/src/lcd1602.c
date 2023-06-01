#include "lcd1602.h"
#include "delay.h"

void setGPIO_Struct(GPIO_InitTypeDef GPIO_InitStruct);
void setDataPortOutput(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	setGPIO_Struct(GPIO_InitStruct);	
}
void setDataPortInput(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	setGPIO_Struct(GPIO_InitStruct);
	
}
void setGPIO_Struct(GPIO_InitTypeDef GPIO_InitStruct){
	GPIO_InitStruct.GPIO_Pin=LCD1602_D0_PIN;
	GPIO_Init(LCD1602_D0_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=LCD1602_D1_PIN;
	GPIO_Init(LCD1602_D1_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=LCD1602_D2_PIN;
	GPIO_Init(LCD1602_D2_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=LCD1602_D3_PIN;
	GPIO_Init(LCD1602_D3_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=LCD1602_D4_PIN;
	GPIO_Init(LCD1602_D4_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=LCD1602_D5_PIN;
	GPIO_Init(LCD1602_D5_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=LCD1602_D6_PIN;
	GPIO_Init(LCD1602_D6_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=LCD1602_D7_PIN;
	GPIO_Init(LCD1602_D7_GPIO,&GPIO_InitStruct);
}



u8 readDataPort(void){
	u8 result;
	setDataPortInput();
	result<<=1;
	if(GPIO_ReadInputDataBit(LCD1602_D7_GPIO,LCD1602_D7_PIN)!=0){
		result|=1;
	}
	
	result<<=1;
	if(GPIO_ReadInputDataBit(LCD1602_D6_GPIO,LCD1602_D6_PIN)!=0){
		result|=1;
	}
	
	result<<=1;
	if(GPIO_ReadInputDataBit(LCD1602_D5_GPIO,LCD1602_D5_PIN)!=0){
		result|=1;
	}
	
	result<<=1;
	if(GPIO_ReadInputDataBit(LCD1602_D4_GPIO,LCD1602_D4_PIN)!=0){
		result|=1;
	}
	
	result<<=1;
	if(GPIO_ReadInputDataBit(LCD1602_D3_GPIO,LCD1602_D3_PIN)!=0){
		result|=1;
	}
	
	result<<=1;
	if(GPIO_ReadInputDataBit(LCD1602_D2_GPIO,LCD1602_D2_PIN)!=0){
		result|=1;
	}
	
	result<<=1;
	if(GPIO_ReadInputDataBit(LCD1602_D1_GPIO,LCD1602_D1_PIN)!=0){
		result|=1;
	}
	
	result<<=1;
	if(GPIO_ReadInputDataBit(LCD1602_D0_GPIO,LCD1602_D0_PIN)!=0){
		result|=1;
	}
	return result;
}

void writeDataPort(u8 data){
	u8 dataTemp=data;
	setDataPortOutput();
	if(dataTemp&0x01)
		GPIO_SetBits(LCD1602_D0_GPIO,LCD1602_D0_PIN);
	else
		GPIO_ResetBits(LCD1602_D0_GPIO,LCD1602_D0_PIN);
	dataTemp>>=1;
	
	if(dataTemp&0x01)
		GPIO_SetBits(LCD1602_D1_GPIO,LCD1602_D1_PIN);
	else
		GPIO_ResetBits(LCD1602_D1_GPIO,LCD1602_D1_PIN);
	dataTemp>>=1;
	
	
	if(dataTemp&0x01)
		GPIO_SetBits(LCD1602_D2_GPIO,LCD1602_D2_PIN);
	else
		GPIO_ResetBits(LCD1602_D2_GPIO,LCD1602_D2_PIN);
	dataTemp>>=1;
	
	
	if(dataTemp&0x01)
		GPIO_SetBits(LCD1602_D3_GPIO,LCD1602_D3_PIN);
	else
		GPIO_ResetBits(LCD1602_D3_GPIO,LCD1602_D3_PIN);
	dataTemp>>=1;
	
	
	if(dataTemp&0x01)
		GPIO_SetBits(LCD1602_D4_GPIO,LCD1602_D4_PIN);
	else
		GPIO_ResetBits(LCD1602_D4_GPIO,LCD1602_D4_PIN);
	dataTemp>>=1;
	
	
	if(dataTemp&0x01)
		GPIO_SetBits(LCD1602_D5_GPIO,LCD1602_D5_PIN);
	else
		GPIO_ResetBits(LCD1602_D5_GPIO,LCD1602_D5_PIN);
	dataTemp>>=1;
	
	
	if(dataTemp&0x01)
		GPIO_SetBits(LCD1602_D6_GPIO,LCD1602_D6_PIN);
	else
		GPIO_ResetBits(LCD1602_D6_GPIO,LCD1602_D6_PIN);
	dataTemp>>=1;
	
	
	if(dataTemp&0x01)
		GPIO_SetBits(LCD1602_D7_GPIO,LCD1602_D7_PIN);
	else
		GPIO_ResetBits(LCD1602_D7_GPIO,LCD1602_D7_PIN);
	dataTemp>>=1;
}

u8 dis1[16]={"   Welcome To   "};     //显示两行
u8 dis2[16]={"   chzu         "};

//**************************************************************************************************
//向LCD写一命令
//**************************************************************************************************
void wcode(u8 t)
{
  LCD1602_RS=0;           // 写的是命令
	delay_us(50);
  LCD1602_RW=0;           // 写状态
	delay_us(50);
  LCD1602_E=1;            //使能
  delay_us(50);
  writeDataPort(t);           //写入命令 
  delay_ms(100);      //等待写入,如果时间太短，会导致液晶无法显示
  LCD1602_E=0;            //数据的锁定
	delay_us(50);
}
//**************************************************************************************************
//向LCD写一数据
//**************************************************************************************************
void wdata(u8 t)
{
  LCD1602_RS=1;          // 写的是数据
	delay_us(50);
  LCD1602_RW=0;          // 写状态
	delay_us(50);
  LCD1602_E=1;           //使能
	delay_us(50);
  writeDataPort(t);      //写入数据
  delay_ms(100);     //等待写入,如果时间太短，会导致液晶无法显示
  LCD1602_E=0;           //数据的锁定
	delay_us(50);
}
//**************************************************************************************************
//LCD显示第一行
//**************************************************************************************************
void xian1()
{
  u8 i;
  wcode(0x80);          //设置第一行显示地址
  for(i=0;i<16;i++)     //循环16次，写完1行
    {
      wdata(dis1[i]);   //写入该行数据
    }
}
//**************************************************************************************************
//LCD显示第二行
//**************************************************************************************************
void xian2()
{
   u8 i;
   wcode(0xc0);        //设置第二行显示地址
   for(i=0;i<16;i++)   //循环16次，写完1行
    {
      wdata(dis2[i]);  //写入该行数据
    }
}
//**************************************************************************************************
//LCD 初始化
//**************************************************************************************************
void Lcd1602_Init(void){	
	GPIO_InitTypeDef GPIO_InitStruct;
	//使能对应io口时钟
	RCC_APB2PeriphClockCmd(LCD1602_GPIOR_RCC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	
	GPIO_InitStruct.GPIO_Pin=LCD1602_RS_PIN;
	GPIO_Init(LCD1602_RS_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=LCD1602_RW_PIN;
	GPIO_Init(LCD1602_RW_GPIO,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin=LCD1602_E_PIN;
	GPIO_Init(LCD1602_E_GPIO,&GPIO_InitStruct);
	
	setDataPortOutput();
	
	
	wcode(0x01);   //清屏
	wcode(0x06);   //输入方式控制,增量光标不移位
	wcode(0x0e);   //显示开关控制
	wcode(0x38);   //功能设定:设置16x2显示，5x7显示,8位数据接口   
}

void Lcd1602Test(void){
	xian1();	       //显示第一行	   
	xian2();		   //显示第二行	
}
