#ifndef __APPLICATION_H
#define __APPLICATION_H

#include "sys.h"

#define DEVICE_KEY_NULL 0//标志设备没有按键按下
#define DEVICE_KEY0 1//标志设备按键0按下
#define DEVICE_KEY1 2//标志设备按键1按下
#define DEVICE_KEY2 3//标志设备按键2按下
#define DEVICE_KEY3 4//标志设备按键3按下

#define DEVICE_STATUS_KEY DEVICE_KEY0 //设备状态按键映射至设备按键0
#define DEVICE_SET DEVICE_KEY1 //设备设置（清除）按键映射至设备按键1
#define DEVICE_UP_KEY DEVICE_KEY2 //设备向上按键映射至设备按键2
#define DEVICE_DOWN_KEY DEVICE_KEY3 //设备向下按键映射至设备按键3

#define DEVICE_WORK_STATUS 0//标志处于工作状态
#define DEVICE_OPERATE_STATUS 1//标志处于操作（学习）状态

#define HE_TOTLE_MAX 10//设备所能存储最多的家电红外码组
#define HE_EAMAIN_MAX 10//设备EEPROM存储空间的预留空间
#define HE_KEY_TOTLE_MAX 30//每个家电红外码组包含最多的遥控按键数量

#define HE_KEY_LENG_MAX 8//每个遥控按键最大长度（字节单位）
#define HE_KEY_REMAIN_MAX 5//每个家电红外码组的预留空间（字节单位）
#define HE_ADDR_GAP (HE_KEY_TOTLE_MAX*HE_KEY_LENG_MAX+HE_KEY_REMAIN_MAX)//每组家电红外码之间的空间间隔（用于计算存取时的起始地址）
#define HE_KEY_REMAIN_ADDR (HE_KEY_TOTLE_MAX*HE_KEY_LENG_MAX)//每组家电红外码预留空间相对该组起始地址的偏移（用于计算存取时的起始地址）

#define HE_KEY_TOTLE 27//本系统的默认每个家电遥控按键总数
//应用程序运行函数
void appplicationRun(void);
//设备工作状态运行函数
void deviceWorkStatue(void);
//设备操作状态运行函数
void deviceOperateStatue(void);
//定义家电遥控控制码对应的提示信息表
//学习红外码时是按照此表的顺序学习
//手机发送的按键控制码也是按照此表顺序编的
struct HEKeyToCrtl{
	u8 ctrlCode;
	char indication[20];
};
extern struct HEKeyToCrtl MyHEKeyToCrtl[HE_KEY_TOTLE];//让外部可访问此表
#endif
