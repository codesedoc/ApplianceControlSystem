#include "application.h"
#include "infraredEmitter.h"
#include "led.h"
#include "delay.h"
#include "at24c256.h"
#include "usart.h"
#include "hc05.h"
#include "usart3.h"
#include "independentKey.h"
#include "oled.h"
u8 deviceKeyValue=DEVICE_KEY_NULL;
u8 deviceStatus=DEVICE_WORK_STATUS; 
u8 operateHEtotle=0;
u8 operateHEkeyNum=0;
struct HEKeyToCrtl MyHEKeyToCrtl[HE_KEY_TOTLE]={ 
	{1,"Power"},{2,"Silence"},{3,"Num0"},{4,"Num1"},{5,"Num2"},{6,"Num3"},{7,"Num4"},
	{8,"Num5"},{9,"Num6"},{10,"Num7"},{11,"Num8"},{12,"Num9"},{13,"TV"},{14,"Many"},{15,"Up"},
	{16,"Down"},{17,"Ok"},{18,"Left"},{19,"Right"},{20,"Home"},{21,"Menu"},{22,"Set"},
	{23,"Back"},{24,"ChannelAdd"},{25,"ChannelSub"},{26,"VoiceAdd"},{27,"VoiceSub"}
};

void infraSendAndRecTest(u8 *dataGroup,u8 length);

void appplicationRun(void){
	//判断切换状态键是否按下
	if(deviceKeyValue==DEVICE_STATUS_KEY){
		//所有按键处理第一步，软件清除按键触发标志，防止一次按键多次相应
		deviceKeyValue=DEVICE_KEY_NULL;
		//根据不同的状态循环切换程序运行状态
		if(deviceStatus==DEVICE_WORK_STATUS)
			deviceStatus=DEVICE_OPERATE_STATUS;
		else
			deviceStatus=DEVICE_WORK_STATUS;
		}
	//根据不同的程序状态进入不同运行函数
	if(deviceStatus==DEVICE_WORK_STATUS){
		deviceWorkStatue();//进入工作状态函数
	}
	else{
		deviceOperateStatue();//进入操作（学习）状态函数
	}
}
//进入操作（学习）状态函数
void deviceOperateStatue(void){
	//u8 recSendBuf[AT24C256_SEND_LENG_MAX]={0};//
	u8 showTemp[2];
	printf("操作状态（OPERATE_STATUS）\r\n");//提示当前状态
	
	LED=1;//LED指示当前状态
	//读取已经保存所学过的设备总数，当达到最大值时不在增加。新学的设备一直覆盖最后一个
	//此函数第一个参数应该传入一个数组最安全，但是只要长度位1时，则直接把变量地址传入，不会出现异常（下同）
	at24c256ReadRandom(&operateHEtotle,1,0);
	operateHEkeyNum=0;//每次从第0个按键开始学习
	
	
	OLED_Clear();
	OLED_ShowString(12,0,"Learnning");
	OLED_ShowString(0,2,"Key:");
	OLED_ShowString(0,4,"Status:");
	OLED_ShowString(0,6,"Length:");
	
	showTemp[0]=operateHEtotle/10+'0';
	showTemp[1]=operateHEtotle%10+'0';
	OLED_ShowString(88,0,showTemp);
	
	OLED_ShowString(36,2,(u8*)MyHEKeyToCrtl[operateHEkeyNum].indication);
	OLED_ShowString(60,4,"wait");
	OLED_ShowString(60,6,"00");	
	
	//实时提示当前学习的按键编号对应的按键意思
	printf("当前学习第");printf("%s",showTemp);printf("号设备\r\n");
	printf("学习按键：");
	printf("%s\r\n",MyHEKeyToCrtl[operateHEkeyNum].indication);
	
	allowFraRecive();//只有在学习状态时才允许设备接受红外信号
	while(1){
		//当设备工作状态按下时变更设备状态
		if(deviceKeyValue==DEVICE_STATUS_KEY){
			deviceKeyValue=DEVICE_KEY_NULL;//清除按键状态
			deviceStatus=DEVICE_WORK_STATUS;//变换设备状态
		}
		//如果设备状态标志与当前不一致则退出当前状态维持的死循环
		if(deviceStatus!=DEVICE_OPERATE_STATUS){
			//当在变换状态前清除接受红外信号时所有的全局标记变量，一遍不影响下次学习（针对没有学习完就切换状态）
			cleanAllFraRecFlag();
			banFraRecive();//退出操作状态时禁止设备接受红外信号
			break;
		}
		//当设备设置按键按下时根据按下时间长短决定是否清除所有记忆。
		if(deviceKeyValue==DEVICE_SET){
			u8 time=0;
			deviceKeyValue=DEVICE_KEY_NULL;//清除按键状态
			//延迟3ms检测是否一直在按清除键
			while(time<30){
				time++;
				LED=~LED;
				delay_ms(100);
				if(!ReadDEVICE_SETKey())
					break;
			}
			LED=1;
			//长按此键则清除所有记忆
			if(time>=30){
				operateHEtotle=0;
				operateHEkeyNum=0;
				at24c256Writedata(&operateHEtotle,1,0);//保存最新的设备总数
				
				showTemp[0]=operateHEtotle/10+'0';
				showTemp[1]=operateHEtotle%10+'0';
				OLED_ShowString(88,0,showTemp);
				
				printf("已删除所有红外编码记忆\r\n");
			}	
		}
		//根据设备上下按键手动变换学习的按键编号
		if(deviceKeyValue==DEVICE_UP_KEY){
			deviceKeyValue=DEVICE_KEY_NULL;//清除按键状态
			operateHEkeyNum < HE_KEY_TOTLE-1 ? operateHEkeyNum++ : operateHEkeyNum;//当按键编号达到最大时不再增加，否则累加
			
			OLED_ShowString(36,2,"          ");
			OLED_ShowString(36,2,(u8*)MyHEKeyToCrtl[operateHEkeyNum].indication);
			
			//实时提示当前学习的按键编号对应的按键意思
			printf("学习按键：");
			printf("%s\r\n",MyHEKeyToCrtl[operateHEkeyNum].indication);
		}
		if(deviceKeyValue==DEVICE_DOWN_KEY){
			deviceKeyValue=DEVICE_KEY_NULL;//清除按键状态
			operateHEkeyNum > 0 ? operateHEkeyNum-- : operateHEkeyNum;//当按键编号达到最小时不再减少，否则累减
			
			OLED_ShowString(36,2,"          ");
			OLED_ShowString(36,2,(u8*)MyHEKeyToCrtl[operateHEkeyNum].indication);
			
			//实时提示当前学习的按键编号对应的按键意思
			printf("学习按键：");
			printf("%s\r\n",MyHEKeyToCrtl[operateHEkeyNum].indication);
		}
	}
}

//value：新学的按键值数组
//valueLength:新学的按键值长度
//valueLength:新学的按键的发送次数（一般没用）
void learnKeyValue(u8 *value,u8 valueLength,u8 conStart){
	u16 addr=0;//存放即将保存所学按键值数组的起始地址
	u16 addrOther=0;//存放即将保存所学按键拓展数据的起始地址
	u8 leng=0;//存放即将保存所学按键值数组的长度
	u8 i;//临时变量
	u8 showTemp[2];
	//按键起始地址=当前设备序号（等于设备总数值）*每个设备所占地址长度+按键编号*每个按键所占地址长度+设备起始预留地址长度
	addr=HE_ADDR_GAP*operateHEtotle+HE_KEY_LENG_MAX*operateHEkeyNum+HE_EAMAIN_MAX;
	//按键拓展数据的起始地址=按键起始地址+按键总数最大值*按键长度最大值
	addrOther=HE_ADDR_GAP*operateHEtotle+HE_EAMAIN_MAX+HE_KEY_LENG_MAX*HE_KEY_TOTLE_MAX;
	
	leng=valueLength/8;//得到数组的长度
	//向上去整，防止失真
	if(valueLength%8)
		leng++;
	
	//每个按键存储空间的最后一位保存当前按键长度，超过按键最大长度不可学习，并手动置位设备状态键，为了立即退出当前状态
	if(leng>=HE_KEY_LENG_MAX)
	{
		deviceKeyValue=DEVICE_STATUS_KEY;
		return;
	}
	at24c256Writedata(value,leng,addr);//向EEPROM中存按键值
	//保存按键值长度
	at24c256Writedata(&valueLength,1,addr+HE_KEY_LENG_MAX-1);
	//保存按键的发送次数
	at24c256Writedata(&conStart,1,addrOther);
	operateHEkeyNum++;//每次保存后累加按键编号
	//一旦按键学完后则自动保存并退出，此处手动置位设备状态键，为了立即退出当前状态
	//此处仅仅通过保存设备总数决定是否保存学习的按键值，实际上按键在每次检测正确时已经写入EEPROM中，
	//但如果此处设备总数不更新，则对于外部来说便不可存放当前学习按键值的存储空间,等效没有保存
	//所以通过控制这一个量的存储就形式上决定了一组按键值的存储
	if(operateHEkeyNum==HE_KEY_TOTLE){
		deviceKeyValue=DEVICE_STATUS_KEY;
		operateHEtotle<HE_TOTLE_MAX-1?operateHEtotle++:operateHEtotle;//当设备总数达到最大时不再增加，否则累加
		at24c256Writedata(&operateHEtotle,1,0);//保存最新的设备总数
	}
	printf("学习的红外码字节长度:%c\t",0x30+leng);
	printf("具体值为：");
	infraSendAndRecTest(value,leng);
	
	//每次学习完也要清除此次接受红外信号的标价变量
	cleanAllFraRecFlag();
	
	//屏幕显示学习成功
	OLED_ShowString(60,4,"		  ");
	OLED_ShowString(60,4,"ok!");
	showTemp[0]=valueLength/10+'0';showTemp[1]=valueLength%10+'0';
	OLED_ShowString(60,6,showTemp);
	//闪烁LED提示学习成功
	for(i=0;i<10;i++)
	{
		LED=~LED;
		delay_ms(100);
	}
	LED=1;//还原LED状态
	
	//屏幕显示最新提示
	OLED_ShowString(36,2,"          ");
	OLED_ShowString(36,2,(u8*)MyHEKeyToCrtl[operateHEkeyNum].indication);
	OLED_ShowString(60,4,"wait");
	OLED_ShowString(60,6,"00");
	
	//实时提示当前学习的按键编号对应的按键意思
	printf("学习按键：");
	printf("%s\r\n",MyHEKeyToCrtl[operateHEkeyNum].indication);
}
void sendFraSign(u8 hetotel,u8 keynum,u8 heNumAdd);
//进入工作状态函数
void deviceWorkStatue(void){
	u8 hETotle=0;
	u8 isHENumAdd=1;
	u8 showTemp[2];
	at24c256ReadRandom(&hETotle,1,0);
	printf("工作状态（WORK_STATUS）\r\n");//提示当前状态
	printf("共有:%c",0x30+hETotle);printf("个家电可用\r\n");
	printf("未匹配\r\n");
	OLED_Clear();
	
	OLED_ShowString(36,0,"Working");
	
	OLED_ShowString(16,2,"HE_Totle: ");
	showTemp[0]=hETotle/10+'0';
	showTemp[1]=hETotle%10+'0';
	OLED_ShowString(88,2,showTemp);
	
	LED=0;//LED指示当前状态
	if(HC05_Init())
	{	
		printf("蓝牙模块无法启动！");
		deviceStatus=DEVICE_OPERATE_STATUS;
		return;
	}
	if(HC05_Set_Cmd("AT+ROLE=0"))
		printf("蓝牙模块异常！");
	while(1){
		//当设备工作状态按下时变更设备状态
		if(deviceKeyValue==DEVICE_STATUS_KEY){
			deviceKeyValue=DEVICE_KEY_NULL;//清除按键状态
			deviceStatus=DEVICE_OPERATE_STATUS;//变换设备状态
		}
		//如果设备状态标志与当前不一致则退出当前状态维持的死循环
		if(deviceStatus!=DEVICE_WORK_STATUS){
			break;
		}
		
		if(deviceKeyValue==DEVICE_SET){
			u8 i;
			deviceKeyValue=DEVICE_KEY_NULL;//
			if(isHENumAdd)	
			{	
				isHENumAdd=0;
				printf("完成匹配\r\n");
			}
			else{
				isHENumAdd=1;
				printf("取消匹配\r\n");
			}
			for(i=0;i<6;i++)
			{
				LED=~LED;
				delay_ms(50);
			}
			LED=0;//还原LED状态
		}
		
		if(USART3_RX_STA&0X8000)//蓝牙对应的串口接收到一次蓝牙模块传来的数据了
		{
			u8 temp;
			temp=USART3_RX_STA&0X7FFF;//得到数据长度
			USART3_RX_STA=0;//清除串口3接收缓存长度，便于下次继续接收
			
			USART3_RX_BUF[temp]=0;		//加结束符	
			//数据头如果是1则代表发送的是电视机遥控按键信息，便进入发送函数
			if(USART3_RX_BUF[0]==1)
			{
				//得到按键控制码
				u8 ctrlCode=USART3_RX_BUF[1];
				u8 i;
				//通过按键控制码的到按键对应的编号
				for(i=0;i<HE_KEY_TOTLE;i++)
				{
					if(MyHEKeyToCrtl[i].ctrlCode==ctrlCode)
						break;
				}
				if(i<HE_KEY_TOTLE){
					//将对应按键编号的红外码发射出去
					sendFraSign(hETotle,i,isHENumAdd);
				}
			}
			//拓展的格力空调的编码发送功能
			if(USART3_RX_BUF[0]==2){
				u16 dataTemp;
				u8 airPower=0,airMode=0,airSpeed=0,airTempreture=0,airUDSweep=0,airLRSweep=0;
				u8 airTemp=USART3_RX_BUF[1];
				u8 airValues1[5]={0,0,0,0,0};
				u8 airValues2[4]={0,0,0,0};
				airTempreture=USART3_RX_BUF[2]&0x0f;

				airPower=(airTemp&0x80)>>7;
				airMode=(airTemp & (7<<4)  )>>4;
				airSpeed=(airTemp & (3<<2) )>>2;
				airUDSweep=(airTemp & 0x02 )>>1;
				airLRSweep=(airTemp & 0x01 )>>0;
				
				airValues1[0]=airMode|(airPower<<3)|(airSpeed<<4)|((airUDSweep|airLRSweep)<<6);
				airValues1[1]=airTempreture;
				airValues1[2]=0xa0;
				airValues1[3]=0x50;
				airValues1[4]=0x02;
				
				airValues2[0]=airUDSweep|(airLRSweep<<4);
				airValues2[1]=0x21;
				airValues2[2]=0x00;
				
				
				dataTemp=5+airMode-1+airTempreture+airLRSweep;
					if(airPower==0){
						if(dataTemp&(1<<3)){
							dataTemp&=0x7;
						}
						else
							dataTemp|=0x8;
					}
				dataTemp&=0x0f;	
				dataTemp<<=4;
				airValues2[3]=dataTemp;
				
				sendGLAirFraSign(airValues1,airValues2);
			}
			//else
				//printf("\r\n%s",USART3_RX_BUF);//其他数据发送回应数据到串口1
		} 	
	}
}

//发射红外码函数
//hetotel:总共有多少组已经学习并保存过的家电遥控码
//keyNum：手机发送来的按键编号
//heNumAdd：发射的红外码是否已经匹配当前家电
void sendFraSign(u8 hetotel,u8 keyNum,u8 heNumAdd){
	static u8 hENum=0;//记录当前发送的是哪个家电编号对应的那组红外码
	u8 sendFraLength=0;//存放当前要发送的红外码位长
	u8 tmepDataGroup[HE_KEY_LENG_MAX]={0};//存放当前要发送的红外码数据数组
	u8 dgleng;//存放当前要发送的红外码数据数组的长度
	//u8 conStart;//原本是一组红外码保留空间中用于保存连发次数的信息（测试万能遥控器时得到的，当感觉没用）
	u16 dataAddr;//要发送的红外码的存储起始地址
	u16 otherAddr;//这组红外码的保留空间起始地址
	u8 i;//临时变量
	//如果没有一组可用红外码则退出
	if(hetotel==0)
		return;
	//根据heNumAdd决定是否家电红外码组序号循环累加，即决定此次发射的红外吗是测试用的还是匹配到hENum代表的那一组红外码
	if(heNumAdd)
		hENum++;
	hENum%=hetotel;//循环累加
	//获得要发送的红外码的存储起始地址
	dataAddr=HE_ADDR_GAP*hENum+HE_KEY_LENG_MAX*keyNum+HE_EAMAIN_MAX;
	//获得这组红外码的保留空间起始地址
	otherAddr=HE_ADDR_GAP*hENum+HE_EAMAIN_MAX+HE_KEY_LENG_MAX*HE_KEY_TOTLE_MAX;
	//得到按键红外码的长度（保存在每个按键存储空间的最后一字节上）
	at24c256ReadRandom(&sendFraLength,1,dataAddr+HE_KEY_LENG_MAX-1);//
	//at24c256ReadRandom(&conStart,1,otherAddr);
	//根据红外码位长得到红外码的字节长度并向上取整
	dgleng=sendFraLength/8;
	if(sendFraLength%8)
		dgleng++;
	//如果字节长度超出范围则视作无效
	if(dgleng<HE_KEY_LENG_MAX)
	{
		at24c256ReadRandom(tmepDataGroup,dgleng,dataAddr);//从EEPROM中读取要发射的红外码数据
		printf("发送红外码的字节长度:%c\t",0x30+dgleng);
		printf("具体值为：");
		infraSendAndRecTest(tmepDataGroup,dgleng);
	}
	infraSendAnyLengData(tmepDataGroup,sendFraLength);//发射红外码
	//闪烁LED提示发射成功
	for(i=0;i<6;i++)
	{
		LED=~LED;
		delay_ms(50);
	}
	LED=0;//还原LED状态
	
	printf("第%c",0x30+hENum);printf("号家电\r\n");
	//printf("第:%c",0x30+keyNum/10);printf("%c",0x30+keyNum%10);printf("号按键\r\n");
	printf("发送按键：");
	printf("%s\r\n",MyHEKeyToCrtl[keyNum].indication);
	printf("\r\n");	
}
void infraSendAndRecTest(u8 *dataGroup,u8 length){
	u8 i;
	printf("Ox");
	for(i=0;i<length;i++){
		if(dataGroup[i]/16<10)
			printf("%c",dataGroup[i]/16+'0');
		else
			printf("%c",dataGroup[i]/16-10+'A');
		if(dataGroup[i]%16<10)
			printf("%c",dataGroup[i]%16+'0');
		else
			printf("%c",dataGroup[i]%16-10+'A');
	}
	printf("\r\n");
}



