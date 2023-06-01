#ifndef __APPLICATION_H
#define __APPLICATION_H

#include "sys.h"

#define DEVICE_KEY_NULL 0//��־�豸û�а�������
#define DEVICE_KEY0 1//��־�豸����0����
#define DEVICE_KEY1 2//��־�豸����1����
#define DEVICE_KEY2 3//��־�豸����2����
#define DEVICE_KEY3 4//��־�豸����3����

#define DEVICE_STATUS_KEY DEVICE_KEY0 //�豸״̬����ӳ�����豸����0
#define DEVICE_SET DEVICE_KEY1 //�豸���ã����������ӳ�����豸����1
#define DEVICE_UP_KEY DEVICE_KEY2 //�豸���ϰ���ӳ�����豸����2
#define DEVICE_DOWN_KEY DEVICE_KEY3 //�豸���°���ӳ�����豸����3

#define DEVICE_WORK_STATUS 0//��־���ڹ���״̬
#define DEVICE_OPERATE_STATUS 1//��־���ڲ�����ѧϰ��״̬

#define HE_TOTLE_MAX 10//�豸���ܴ洢���ļҵ��������
#define HE_EAMAIN_MAX 10//�豸EEPROM�洢�ռ��Ԥ���ռ�
#define HE_KEY_TOTLE_MAX 30//ÿ���ҵ���������������ң�ذ�������

#define HE_KEY_LENG_MAX 8//ÿ��ң�ذ�����󳤶ȣ��ֽڵ�λ��
#define HE_KEY_REMAIN_MAX 5//ÿ���ҵ���������Ԥ���ռ䣨�ֽڵ�λ��
#define HE_ADDR_GAP (HE_KEY_TOTLE_MAX*HE_KEY_LENG_MAX+HE_KEY_REMAIN_MAX)//ÿ��ҵ������֮��Ŀռ��������ڼ����ȡʱ����ʼ��ַ��
#define HE_KEY_REMAIN_ADDR (HE_KEY_TOTLE_MAX*HE_KEY_LENG_MAX)//ÿ��ҵ������Ԥ���ռ���Ը�����ʼ��ַ��ƫ�ƣ����ڼ����ȡʱ����ʼ��ַ��

#define HE_KEY_TOTLE 27//��ϵͳ��Ĭ��ÿ���ҵ�ң�ذ�������
//Ӧ�ó������к���
void appplicationRun(void);
//�豸����״̬���к���
void deviceWorkStatue(void);
//�豸����״̬���к���
void deviceOperateStatue(void);
//����ҵ�ң�ؿ������Ӧ����ʾ��Ϣ��
//ѧϰ������ʱ�ǰ��մ˱��˳��ѧϰ
//�ֻ����͵İ���������Ҳ�ǰ��մ˱�˳����
struct HEKeyToCrtl{
	u8 ctrlCode;
	char indication[20];
};
extern struct HEKeyToCrtl MyHEKeyToCrtl[HE_KEY_TOTLE];//���ⲿ�ɷ��ʴ˱�
#endif
