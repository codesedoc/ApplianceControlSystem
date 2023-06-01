#ifndef __AT24C256_H
#define __AT24C256_H

#include "sys.h"

//����������󳤶ȣ��ֽڵ�λ��
#define AT24C256_SEND_LENG_MAX 16
//����������󳤶ȣ��ֽڵ�λ��
#define AT24C256_RECEIVE_LENG_MAX 10

//��ʼ��
void at24c256Init(void);
//д����
void at24c256Writedata(u8 *sendBuf,u8 leng,u16 dataAddr);
//����������
void at24c256ReadImmediate(u8 *receiveBuf,u8 leng);
//���������
void at24c256ReadRandom(u8 *receiveBuf,u8 leng,u16 dataAddr);

#endif
