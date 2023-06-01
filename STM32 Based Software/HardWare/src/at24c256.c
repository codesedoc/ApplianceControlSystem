#include "at24c256.h"
#include "iic.h"
#include "delay.h"

/********************/
//初始化函数
/********************/
void at24c256Init(void){
	iicInit();
	//at24c256Writedata((u8 *)"0",0,0);
}

/********************/
//初始化函数
/********************/
u8 at24c256CheckBusy(void){
	u8 result=0;
	iicSendStart();
	result=iicSendAddr(0xa0,0);
	iicSendStop();
	
	return result;
}

/********************/
//写数据函数
//*sendBuf：发送缓冲区起始地址
//leng：数据长度
//dataAddr：at24c256写数据起始地址值
/********************/
void at24c256Writedata(u8 *sendBuf,u8 leng,u16 dataAddr){
//	u16 i=0;//临时变量
//	u8 dataAddrHight=dataAddr>>8;//取得数据起始地址高8位
//	u8 dataAddrLow=dataAddr&0x00ff;//取得数据起始地址低8位
//	if(iicCheckBusy()!=IIC_IDLE)//检测总线忙
//	{
//		return;
//	}
//	iicSendStart();//发送起始信号
//	iicSendAddr(0xa0,0);//发送从设备地址和写标志位
//	iicSendOneByte(dataAddrHight);//发送数据起始地址高8位
//	iicSendOneByte(dataAddrLow);//发送数据起始地址低8位
//	for(i=0;i<leng;i++)
//	{
//		iicSendOneByte(sendBuf[i]);//逐字节发送数据
//	}
//	iicSendStop();//发送终止信号
	
	u8 i=0;//临时变量
	u16 dataAddrTemp=dataAddr;
	for(i=0;i<leng;i++)
	{
		u8 dataAddrHight=dataAddrTemp>>8;//取得数据起始地址高8位
		u8 dataAddrLow=dataAddrTemp&0x00ff;//取得数据起始地址低8位
	
		if(iicCheckBusy()!=IIC_IDLE)//检测总线忙
		{
			return;
		}
		iicSendStart();//发送起始信号
		iicSendAddr(0xa0,0);//发送从设备地址和写标志位
		iicSendOneByte(dataAddrHight);//发送数据起始地址高8位
		iicSendOneByte(dataAddrLow);//发送数据起始地址低8位
		iicSendOneByte(sendBuf[i]);//逐字节发送数据
		iicSendStop();//发送终止信号
		delay_ms(100);
		dataAddrTemp++;
	}
}

/********************/
//立即读函数
//*receiveBuf：接收缓冲区起始地址
//leng：数据长度
/********************/
void at24c256ReadImmediate(u8 *receiveBuf,u8 leng){
	u16 i,isACK=0;//临时变量
	if(iicCheckBusy()!=IIC_IDLE)//检测忙状态
	{
		return;
	}
	iicSendStart();//发送起始信号
	iicSendAddr(0xa0,1);//发送设备起始设备
	isACK=0;
	for(i=0;i<leng;i++)//根据发送的数据下标决定是否发送ACK
	{
		if(i+1<leng)
			isACK=1;
		else
			isACK=0;
		receiveBuf[i]=iicReceiveOneByte(isACK);//将数据暂存入数据接收缓冲区
	}
	iicSendStop();//发送终止信号
}

/********************/
//随机读函数
//*receiveBuf：接收缓冲区起始地址
//leng：数据长度
//dataAddr：at24c256写数据起始地址值
/********************/
void at24c256ReadRandom(u8 *receiveBuf,u8 leng,u16 dataAddr){
	//at24c256Writedata((u8 *)"0",0,dataAddr);
	//at24c256ReadImmediate(receiveBuf,leng);
	
	u16 i=0,isACK=0;//临时变量
	u8 dataAddrHight=dataAddr>>8;//取得数据起始地址高8位
	u8 dataAddrLow=dataAddr&0x00ff;//取得数据起始地址低8位
	if(iicCheckBusy()!=IIC_IDLE)//检测总线忙
	{
		return;
	}
	iicSendStart();//发送起始信号
	iicSendAddr(0xa0,0);//发送从设备地址和写标志位
	iicSendOneByte(dataAddrHight);//发送数据起始地址高8位
	iicSendOneByte(dataAddrLow);//发送数据起始地址低8位
	delay_ms(1);
	iicSendStart();//发送起始信号
	iicSendAddr(0xa0,1);//发送设备起始设备
	isACK=0;
	for(i=0;i<leng;i++)//根据发送的数据下标决定是否发送ACK
	{
		if(i+1<leng)
			isACK=1;
		else
			isACK=0;
		receiveBuf[i]=iicReceiveOneByte(isACK);//将数据暂存入数据接收缓冲区
	}
	iicSendStop();//发送终止信号
}

