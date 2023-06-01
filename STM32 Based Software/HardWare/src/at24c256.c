#include "at24c256.h"
#include "iic.h"
#include "delay.h"

/********************/
//��ʼ������
/********************/
void at24c256Init(void){
	iicInit();
	//at24c256Writedata((u8 *)"0",0,0);
}

/********************/
//��ʼ������
/********************/
u8 at24c256CheckBusy(void){
	u8 result=0;
	iicSendStart();
	result=iicSendAddr(0xa0,0);
	iicSendStop();
	
	return result;
}

/********************/
//д���ݺ���
//*sendBuf�����ͻ�������ʼ��ַ
//leng�����ݳ���
//dataAddr��at24c256д������ʼ��ֵַ
/********************/
void at24c256Writedata(u8 *sendBuf,u8 leng,u16 dataAddr){
//	u16 i=0;//��ʱ����
//	u8 dataAddrHight=dataAddr>>8;//ȡ��������ʼ��ַ��8λ
//	u8 dataAddrLow=dataAddr&0x00ff;//ȡ��������ʼ��ַ��8λ
//	if(iicCheckBusy()!=IIC_IDLE)//�������æ
//	{
//		return;
//	}
//	iicSendStart();//������ʼ�ź�
//	iicSendAddr(0xa0,0);//���ʹ��豸��ַ��д��־λ
//	iicSendOneByte(dataAddrHight);//����������ʼ��ַ��8λ
//	iicSendOneByte(dataAddrLow);//����������ʼ��ַ��8λ
//	for(i=0;i<leng;i++)
//	{
//		iicSendOneByte(sendBuf[i]);//���ֽڷ�������
//	}
//	iicSendStop();//������ֹ�ź�
	
	u8 i=0;//��ʱ����
	u16 dataAddrTemp=dataAddr;
	for(i=0;i<leng;i++)
	{
		u8 dataAddrHight=dataAddrTemp>>8;//ȡ��������ʼ��ַ��8λ
		u8 dataAddrLow=dataAddrTemp&0x00ff;//ȡ��������ʼ��ַ��8λ
	
		if(iicCheckBusy()!=IIC_IDLE)//�������æ
		{
			return;
		}
		iicSendStart();//������ʼ�ź�
		iicSendAddr(0xa0,0);//���ʹ��豸��ַ��д��־λ
		iicSendOneByte(dataAddrHight);//����������ʼ��ַ��8λ
		iicSendOneByte(dataAddrLow);//����������ʼ��ַ��8λ
		iicSendOneByte(sendBuf[i]);//���ֽڷ�������
		iicSendStop();//������ֹ�ź�
		delay_ms(100);
		dataAddrTemp++;
	}
}

/********************/
//����������
//*receiveBuf�����ջ�������ʼ��ַ
//leng�����ݳ���
/********************/
void at24c256ReadImmediate(u8 *receiveBuf,u8 leng){
	u16 i,isACK=0;//��ʱ����
	if(iicCheckBusy()!=IIC_IDLE)//���æ״̬
	{
		return;
	}
	iicSendStart();//������ʼ�ź�
	iicSendAddr(0xa0,1);//�����豸��ʼ�豸
	isACK=0;
	for(i=0;i<leng;i++)//���ݷ��͵������±�����Ƿ���ACK
	{
		if(i+1<leng)
			isACK=1;
		else
			isACK=0;
		receiveBuf[i]=iicReceiveOneByte(isACK);//�������ݴ������ݽ��ջ�����
	}
	iicSendStop();//������ֹ�ź�
}

/********************/
//���������
//*receiveBuf�����ջ�������ʼ��ַ
//leng�����ݳ���
//dataAddr��at24c256д������ʼ��ֵַ
/********************/
void at24c256ReadRandom(u8 *receiveBuf,u8 leng,u16 dataAddr){
	//at24c256Writedata((u8 *)"0",0,dataAddr);
	//at24c256ReadImmediate(receiveBuf,leng);
	
	u16 i=0,isACK=0;//��ʱ����
	u8 dataAddrHight=dataAddr>>8;//ȡ��������ʼ��ַ��8λ
	u8 dataAddrLow=dataAddr&0x00ff;//ȡ��������ʼ��ַ��8λ
	if(iicCheckBusy()!=IIC_IDLE)//�������æ
	{
		return;
	}
	iicSendStart();//������ʼ�ź�
	iicSendAddr(0xa0,0);//���ʹ��豸��ַ��д��־λ
	iicSendOneByte(dataAddrHight);//����������ʼ��ַ��8λ
	iicSendOneByte(dataAddrLow);//����������ʼ��ַ��8λ
	delay_ms(1);
	iicSendStart();//������ʼ�ź�
	iicSendAddr(0xa0,1);//�����豸��ʼ�豸
	isACK=0;
	for(i=0;i<leng;i++)//���ݷ��͵������±�����Ƿ���ACK
	{
		if(i+1<leng)
			isACK=1;
		else
			isACK=0;
		receiveBuf[i]=iicReceiveOneByte(isACK);//�������ݴ������ݽ��ջ�����
	}
	iicSendStop();//������ֹ�ź�
}

