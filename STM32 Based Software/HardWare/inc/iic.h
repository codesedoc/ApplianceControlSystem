#ifndef __IIC_H
#define __IIC_H

#include "sys.h"

//SCL��ӳ��
#define SCL_PIN GPIO_Pin_11
#define SCL_IN PAin(11)
#define SCL_OUT PAout(11)
//SDL��ӳ��
#define SDL_PIN GPIO_Pin_12
#define SDL_IN PAin(12)
#define SDL_OUT PAout(12)

//�����������������־��
#define IIC_PIN_IN 0
#define IIC_PIN_OUT 1

//��������æ״̬
#define IIC_BUSY 1
#define IIC_IDLE 0

//��ʼ������
void iicInit(void);
//�������æ״̬����
u8 iicCheckBusy(void);
//���Ϳ�ʼ�źź���
void iicSendStart(void);
//������ֹ�źź���
void iicSendStop(void);
//���ʹ��豸��ַ����
u8 iicSendAddr(u8 addr,u8 wr);
//����һ�ֽ����ݺ���
u8 iicSendOneByte(u8 data);
//����һ�ֽ����ݺ���
u8 iicReceiveOneByte(u8 isAck);

#endif
