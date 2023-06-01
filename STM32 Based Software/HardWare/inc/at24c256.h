#ifndef __AT24C256_H
#define __AT24C256_H

#include "sys.h"

//发送数据最大长度（字节单位）
#define AT24C256_SEND_LENG_MAX 16
//接受数据最大长度（字节单位）
#define AT24C256_RECEIVE_LENG_MAX 10

//初始化
void at24c256Init(void);
//写数据
void at24c256Writedata(u8 *sendBuf,u8 leng,u16 dataAddr);
//立即读数据
void at24c256ReadImmediate(u8 *receiveBuf,u8 leng);
//随机读数据
void at24c256ReadRandom(u8 *receiveBuf,u8 leng,u16 dataAddr);

#endif
