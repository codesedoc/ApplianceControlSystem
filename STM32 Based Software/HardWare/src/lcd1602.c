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

u8 dis1[16]={"   Welcome To   "};     //��ʾ����
u8 dis2[16]={"   chzu         "};

//**************************************************************************************************
//��LCDдһ����
//**************************************************************************************************
void wcode(u8 t)
{
  LCD1602_RS=0;           // д��������
	delay_us(50);
  LCD1602_RW=0;           // д״̬
	delay_us(50);
  LCD1602_E=1;            //ʹ��
  delay_us(50);
  writeDataPort(t);           //д������ 
  delay_ms(100);      //�ȴ�д��,���ʱ��̫�̣��ᵼ��Һ���޷���ʾ
  LCD1602_E=0;            //���ݵ�����
	delay_us(50);
}
//**************************************************************************************************
//��LCDдһ����
//**************************************************************************************************
void wdata(u8 t)
{
  LCD1602_RS=1;          // д��������
	delay_us(50);
  LCD1602_RW=0;          // д״̬
	delay_us(50);
  LCD1602_E=1;           //ʹ��
	delay_us(50);
  writeDataPort(t);      //д������
  delay_ms(100);     //�ȴ�д��,���ʱ��̫�̣��ᵼ��Һ���޷���ʾ
  LCD1602_E=0;           //���ݵ�����
	delay_us(50);
}
//**************************************************************************************************
//LCD��ʾ��һ��
//**************************************************************************************************
void xian1()
{
  u8 i;
  wcode(0x80);          //���õ�һ����ʾ��ַ
  for(i=0;i<16;i++)     //ѭ��16�Σ�д��1��
    {
      wdata(dis1[i]);   //д���������
    }
}
//**************************************************************************************************
//LCD��ʾ�ڶ���
//**************************************************************************************************
void xian2()
{
   u8 i;
   wcode(0xc0);        //���õڶ�����ʾ��ַ
   for(i=0;i<16;i++)   //ѭ��16�Σ�д��1��
    {
      wdata(dis2[i]);  //д���������
    }
}
//**************************************************************************************************
//LCD ��ʼ��
//**************************************************************************************************
void Lcd1602_Init(void){	
	GPIO_InitTypeDef GPIO_InitStruct;
	//ʹ�ܶ�Ӧio��ʱ��
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
	
	
	wcode(0x01);   //����
	wcode(0x06);   //���뷽ʽ����,������겻��λ
	wcode(0x0e);   //��ʾ���ؿ���
	wcode(0x38);   //�����趨:����16x2��ʾ��5x7��ʾ,8λ���ݽӿ�   
}

void Lcd1602Test(void){
	xian1();	       //��ʾ��һ��	   
	xian2();		   //��ʾ�ڶ���	
}
