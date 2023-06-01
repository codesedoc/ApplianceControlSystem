#ifndef __IIC_H
#define __IIC_H

#include "sys.h"

//SCL线映射
#define SCL_PIN GPIO_Pin_11
#define SCL_IN PAin(11)
#define SCL_OUT PAout(11)
//SDL线映射
#define SDL_PIN GPIO_Pin_12
#define SDL_IN PAin(12)
#define SDL_OUT PAout(12)

//定义引脚输入输出标志宏
#define IIC_PIN_IN 0
#define IIC_PIN_OUT 1

//定义总线忙状态
#define IIC_BUSY 1
#define IIC_IDLE 0

//初始化函数
void iicInit(void);
//检测总线忙状态函数
u8 iicCheckBusy(void);
//发送开始信号函数
void iicSendStart(void);
//发送终止信号函数
void iicSendStop(void);
//发送从设备地址函数
u8 iicSendAddr(u8 addr,u8 wr);
//发送一字节数据函数
u8 iicSendOneByte(u8 data);
//接受一字节数据函数
u8 iicReceiveOneByte(u8 isAck);

#endif
