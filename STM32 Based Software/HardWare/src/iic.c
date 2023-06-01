#include "iic.h"
#include "delay.h"
//�ڲ�ʹ�ú���
void iicSetIOPinDir(u16 pin,u8 dir);

void iicSetIOPinDir(u16 pin,u8 dir){
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin=pin;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	//ʹ�ܶ�Ӧio��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	if(dir==IIC_PIN_OUT){
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;//�������			
	}
	else{
		GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;//��������
	}
	GPIO_Init(GPIOA,&GPIO_InitStruct);
}
/********************/
//��ʼ������
/********************/
void iicInit(void){
	iicSetIOPinDir(SCL_PIN,IIC_PIN_OUT);
	iicSetIOPinDir(SDL_PIN,IIC_PIN_OUT);
	SDL_OUT=1;
	SCL_OUT=1;
	delay_init();
}

/********************/
//�������æ״̬����
//����u8�������ݴ�������״̬
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
//���Ϳ�ʼ�źź���
/********************/
void iicSendStart(void){
	iicSetIOPinDir(SCL_PIN,IIC_PIN_OUT);//����SCL����Ϊ���ģʽ
	iicSetIOPinDir(SDL_PIN,IIC_PIN_OUT);//����SDL����Ϊ���ģʽ
	SCL_OUT=0;//����SCL����Ϊ���ģʽ
	SDL_OUT=1;//����SCL����Ϊ���ģʽ
	SCL_OUT=1;//����SCL����Ϊ���ģʽ
	delay_us(4);
	SDL_OUT=0;
	delay_us(4);
	SCL_OUT=0;	
}

/********************/
//������ֹ�źź���
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
//���ʹ��豸��ַ����
//addr:7λ��ֵַ
//wr����д��־λ��0д1��
/********************/
u8 iicSendAddr(u8 addr,u8 wr){
	u8 dataTemp=addr,result;
	dataTemp|=wr&0x01;
	result=iicSendOneByte(dataTemp);
	return result;
}

/********************/
//����һ�ֽ����ݺ���
//data��Ҫ���͵�һ�ֽ�����
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
//����һ�ֽ����ݺ���
//isAck���Ƿ���ӦACK��0��1��
//����u8���ͽ��ܵ�һ�ֽ�����
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
