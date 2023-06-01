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
	//�ж��л�״̬���Ƿ���
	if(deviceKeyValue==DEVICE_STATUS_KEY){
		//���а��������һ��������������������־����ֹһ�ΰ��������Ӧ
		deviceKeyValue=DEVICE_KEY_NULL;
		//���ݲ�ͬ��״̬ѭ���л���������״̬
		if(deviceStatus==DEVICE_WORK_STATUS)
			deviceStatus=DEVICE_OPERATE_STATUS;
		else
			deviceStatus=DEVICE_WORK_STATUS;
		}
	//���ݲ�ͬ�ĳ���״̬���벻ͬ���к���
	if(deviceStatus==DEVICE_WORK_STATUS){
		deviceWorkStatue();//���빤��״̬����
	}
	else{
		deviceOperateStatue();//���������ѧϰ��״̬����
	}
}
//���������ѧϰ��״̬����
void deviceOperateStatue(void){
	//u8 recSendBuf[AT24C256_SEND_LENG_MAX]={0};//
	u8 showTemp[2];
	printf("����״̬��OPERATE_STATUS��\r\n");//��ʾ��ǰ״̬
	
	LED=1;//LEDָʾ��ǰ״̬
	//��ȡ�Ѿ�������ѧ�����豸���������ﵽ���ֵʱ�������ӡ���ѧ���豸һֱ�������һ��
	//�˺�����һ������Ӧ�ô���һ�������ȫ������ֻҪ����λ1ʱ����ֱ�Ӱѱ�����ַ���룬��������쳣����ͬ��
	at24c256ReadRandom(&operateHEtotle,1,0);
	operateHEkeyNum=0;//ÿ�δӵ�0��������ʼѧϰ
	
	
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
	
	//ʵʱ��ʾ��ǰѧϰ�İ�����Ŷ�Ӧ�İ�����˼
	printf("��ǰѧϰ��");printf("%s",showTemp);printf("���豸\r\n");
	printf("ѧϰ������");
	printf("%s\r\n",MyHEKeyToCrtl[operateHEkeyNum].indication);
	
	allowFraRecive();//ֻ����ѧϰ״̬ʱ�������豸���ܺ����ź�
	while(1){
		//���豸����״̬����ʱ����豸״̬
		if(deviceKeyValue==DEVICE_STATUS_KEY){
			deviceKeyValue=DEVICE_KEY_NULL;//�������״̬
			deviceStatus=DEVICE_WORK_STATUS;//�任�豸״̬
		}
		//����豸״̬��־�뵱ǰ��һ�����˳���ǰ״̬ά�ֵ���ѭ��
		if(deviceStatus!=DEVICE_OPERATE_STATUS){
			//���ڱ任״̬ǰ������ܺ����ź�ʱ���е�ȫ�ֱ�Ǳ�����һ�鲻Ӱ���´�ѧϰ�����û��ѧϰ����л�״̬��
			cleanAllFraRecFlag();
			banFraRecive();//�˳�����״̬ʱ��ֹ�豸���ܺ����ź�
			break;
		}
		//���豸���ð�������ʱ���ݰ���ʱ�䳤�̾����Ƿ�������м��䡣
		if(deviceKeyValue==DEVICE_SET){
			u8 time=0;
			deviceKeyValue=DEVICE_KEY_NULL;//�������״̬
			//�ӳ�3ms����Ƿ�һֱ�ڰ������
			while(time<30){
				time++;
				LED=~LED;
				delay_ms(100);
				if(!ReadDEVICE_SETKey())
					break;
			}
			LED=1;
			//�����˼���������м���
			if(time>=30){
				operateHEtotle=0;
				operateHEkeyNum=0;
				at24c256Writedata(&operateHEtotle,1,0);//�������µ��豸����
				
				showTemp[0]=operateHEtotle/10+'0';
				showTemp[1]=operateHEtotle%10+'0';
				OLED_ShowString(88,0,showTemp);
				
				printf("��ɾ�����к���������\r\n");
			}	
		}
		//�����豸���°����ֶ��任ѧϰ�İ������
		if(deviceKeyValue==DEVICE_UP_KEY){
			deviceKeyValue=DEVICE_KEY_NULL;//�������״̬
			operateHEkeyNum < HE_KEY_TOTLE-1 ? operateHEkeyNum++ : operateHEkeyNum;//��������Ŵﵽ���ʱ�������ӣ������ۼ�
			
			OLED_ShowString(36,2,"          ");
			OLED_ShowString(36,2,(u8*)MyHEKeyToCrtl[operateHEkeyNum].indication);
			
			//ʵʱ��ʾ��ǰѧϰ�İ�����Ŷ�Ӧ�İ�����˼
			printf("ѧϰ������");
			printf("%s\r\n",MyHEKeyToCrtl[operateHEkeyNum].indication);
		}
		if(deviceKeyValue==DEVICE_DOWN_KEY){
			deviceKeyValue=DEVICE_KEY_NULL;//�������״̬
			operateHEkeyNum > 0 ? operateHEkeyNum-- : operateHEkeyNum;//��������Ŵﵽ��Сʱ���ټ��٣������ۼ�
			
			OLED_ShowString(36,2,"          ");
			OLED_ShowString(36,2,(u8*)MyHEKeyToCrtl[operateHEkeyNum].indication);
			
			//ʵʱ��ʾ��ǰѧϰ�İ�����Ŷ�Ӧ�İ�����˼
			printf("ѧϰ������");
			printf("%s\r\n",MyHEKeyToCrtl[operateHEkeyNum].indication);
		}
	}
}

//value����ѧ�İ���ֵ����
//valueLength:��ѧ�İ���ֵ����
//valueLength:��ѧ�İ����ķ��ʹ�����һ��û�ã�
void learnKeyValue(u8 *value,u8 valueLength,u8 conStart){
	u16 addr=0;//��ż���������ѧ����ֵ�������ʼ��ַ
	u16 addrOther=0;//��ż���������ѧ������չ���ݵ���ʼ��ַ
	u8 leng=0;//��ż���������ѧ����ֵ����ĳ���
	u8 i;//��ʱ����
	u8 showTemp[2];
	//������ʼ��ַ=��ǰ�豸��ţ������豸����ֵ��*ÿ���豸��ռ��ַ����+�������*ÿ��������ռ��ַ����+�豸��ʼԤ����ַ����
	addr=HE_ADDR_GAP*operateHEtotle+HE_KEY_LENG_MAX*operateHEkeyNum+HE_EAMAIN_MAX;
	//������չ���ݵ���ʼ��ַ=������ʼ��ַ+�����������ֵ*�����������ֵ
	addrOther=HE_ADDR_GAP*operateHEtotle+HE_EAMAIN_MAX+HE_KEY_LENG_MAX*HE_KEY_TOTLE_MAX;
	
	leng=valueLength/8;//�õ�����ĳ���
	//����ȥ������ֹʧ��
	if(valueLength%8)
		leng++;
	
	//ÿ�������洢�ռ�����һλ���浱ǰ�������ȣ�����������󳤶Ȳ���ѧϰ�����ֶ���λ�豸״̬����Ϊ�������˳���ǰ״̬
	if(leng>=HE_KEY_LENG_MAX)
	{
		deviceKeyValue=DEVICE_STATUS_KEY;
		return;
	}
	at24c256Writedata(value,leng,addr);//��EEPROM�д水��ֵ
	//���水��ֵ����
	at24c256Writedata(&valueLength,1,addr+HE_KEY_LENG_MAX-1);
	//���水���ķ��ʹ���
	at24c256Writedata(&conStart,1,addrOther);
	operateHEkeyNum++;//ÿ�α�����ۼӰ������
	//һ������ѧ������Զ����沢�˳����˴��ֶ���λ�豸״̬����Ϊ�������˳���ǰ״̬
	//�˴�����ͨ�������豸���������Ƿ񱣴�ѧϰ�İ���ֵ��ʵ���ϰ�����ÿ�μ����ȷʱ�Ѿ�д��EEPROM�У�
	//������˴��豸���������£�������ⲿ��˵�㲻�ɴ�ŵ�ǰѧϰ����ֵ�Ĵ洢�ռ�,��Чû�б���
	//����ͨ��������һ�����Ĵ洢����ʽ�Ͼ�����һ�鰴��ֵ�Ĵ洢
	if(operateHEkeyNum==HE_KEY_TOTLE){
		deviceKeyValue=DEVICE_STATUS_KEY;
		operateHEtotle<HE_TOTLE_MAX-1?operateHEtotle++:operateHEtotle;//���豸�����ﵽ���ʱ�������ӣ������ۼ�
		at24c256Writedata(&operateHEtotle,1,0);//�������µ��豸����
	}
	printf("ѧϰ�ĺ������ֽڳ���:%c\t",0x30+leng);
	printf("����ֵΪ��");
	infraSendAndRecTest(value,leng);
	
	//ÿ��ѧϰ��ҲҪ����˴ν��ܺ����źŵı�۱���
	cleanAllFraRecFlag();
	
	//��Ļ��ʾѧϰ�ɹ�
	OLED_ShowString(60,4,"		  ");
	OLED_ShowString(60,4,"ok!");
	showTemp[0]=valueLength/10+'0';showTemp[1]=valueLength%10+'0';
	OLED_ShowString(60,6,showTemp);
	//��˸LED��ʾѧϰ�ɹ�
	for(i=0;i<10;i++)
	{
		LED=~LED;
		delay_ms(100);
	}
	LED=1;//��ԭLED״̬
	
	//��Ļ��ʾ������ʾ
	OLED_ShowString(36,2,"          ");
	OLED_ShowString(36,2,(u8*)MyHEKeyToCrtl[operateHEkeyNum].indication);
	OLED_ShowString(60,4,"wait");
	OLED_ShowString(60,6,"00");
	
	//ʵʱ��ʾ��ǰѧϰ�İ�����Ŷ�Ӧ�İ�����˼
	printf("ѧϰ������");
	printf("%s\r\n",MyHEKeyToCrtl[operateHEkeyNum].indication);
}
void sendFraSign(u8 hetotel,u8 keynum,u8 heNumAdd);
//���빤��״̬����
void deviceWorkStatue(void){
	u8 hETotle=0;
	u8 isHENumAdd=1;
	u8 showTemp[2];
	at24c256ReadRandom(&hETotle,1,0);
	printf("����״̬��WORK_STATUS��\r\n");//��ʾ��ǰ״̬
	printf("����:%c",0x30+hETotle);printf("���ҵ����\r\n");
	printf("δƥ��\r\n");
	OLED_Clear();
	
	OLED_ShowString(36,0,"Working");
	
	OLED_ShowString(16,2,"HE_Totle: ");
	showTemp[0]=hETotle/10+'0';
	showTemp[1]=hETotle%10+'0';
	OLED_ShowString(88,2,showTemp);
	
	LED=0;//LEDָʾ��ǰ״̬
	if(HC05_Init())
	{	
		printf("����ģ���޷�������");
		deviceStatus=DEVICE_OPERATE_STATUS;
		return;
	}
	if(HC05_Set_Cmd("AT+ROLE=0"))
		printf("����ģ���쳣��");
	while(1){
		//���豸����״̬����ʱ����豸״̬
		if(deviceKeyValue==DEVICE_STATUS_KEY){
			deviceKeyValue=DEVICE_KEY_NULL;//�������״̬
			deviceStatus=DEVICE_OPERATE_STATUS;//�任�豸״̬
		}
		//����豸״̬��־�뵱ǰ��һ�����˳���ǰ״̬ά�ֵ���ѭ��
		if(deviceStatus!=DEVICE_WORK_STATUS){
			break;
		}
		
		if(deviceKeyValue==DEVICE_SET){
			u8 i;
			deviceKeyValue=DEVICE_KEY_NULL;//
			if(isHENumAdd)	
			{	
				isHENumAdd=0;
				printf("���ƥ��\r\n");
			}
			else{
				isHENumAdd=1;
				printf("ȡ��ƥ��\r\n");
			}
			for(i=0;i<6;i++)
			{
				LED=~LED;
				delay_ms(50);
			}
			LED=0;//��ԭLED״̬
		}
		
		if(USART3_RX_STA&0X8000)//������Ӧ�Ĵ��ڽ��յ�һ������ģ�鴫����������
		{
			u8 temp;
			temp=USART3_RX_STA&0X7FFF;//�õ����ݳ���
			USART3_RX_STA=0;//�������3���ջ��泤�ȣ������´μ�������
			
			USART3_RX_BUF[temp]=0;		//�ӽ�����	
			//����ͷ�����1������͵��ǵ��ӻ�ң�ذ�����Ϣ������뷢�ͺ���
			if(USART3_RX_BUF[0]==1)
			{
				//�õ�����������
				u8 ctrlCode=USART3_RX_BUF[1];
				u8 i;
				//ͨ������������ĵ�������Ӧ�ı��
				for(i=0;i<HE_KEY_TOTLE;i++)
				{
					if(MyHEKeyToCrtl[i].ctrlCode==ctrlCode)
						break;
				}
				if(i<HE_KEY_TOTLE){
					//����Ӧ������ŵĺ����뷢���ȥ
					sendFraSign(hETotle,i,isHENumAdd);
				}
			}
			//��չ�ĸ����յ��ı��뷢�͹���
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
				//printf("\r\n%s",USART3_RX_BUF);//�������ݷ��ͻ�Ӧ���ݵ�����1
		} 	
	}
}

//��������뺯��
//hetotel:�ܹ��ж������Ѿ�ѧϰ��������ļҵ�ң����
//keyNum���ֻ��������İ������
//heNumAdd������ĺ������Ƿ��Ѿ�ƥ�䵱ǰ�ҵ�
void sendFraSign(u8 hetotel,u8 keyNum,u8 heNumAdd){
	static u8 hENum=0;//��¼��ǰ���͵����ĸ��ҵ��Ŷ�Ӧ�����������
	u8 sendFraLength=0;//��ŵ�ǰҪ���͵ĺ�����λ��
	u8 tmepDataGroup[HE_KEY_LENG_MAX]={0};//��ŵ�ǰҪ���͵ĺ�������������
	u8 dgleng;//��ŵ�ǰҪ���͵ĺ�������������ĳ���
	//u8 conStart;//ԭ����һ������뱣���ռ������ڱ���������������Ϣ����������ң����ʱ�õ��ģ����о�û�ã�
	u16 dataAddr;//Ҫ���͵ĺ�����Ĵ洢��ʼ��ַ
	u16 otherAddr;//���������ı����ռ���ʼ��ַ
	u8 i;//��ʱ����
	//���û��һ����ú��������˳�
	if(hetotel==0)
		return;
	//����heNumAdd�����Ƿ�ҵ�����������ѭ���ۼӣ��������˴η���ĺ������ǲ����õĻ���ƥ�䵽hENum�������һ�������
	if(heNumAdd)
		hENum++;
	hENum%=hetotel;//ѭ���ۼ�
	//���Ҫ���͵ĺ�����Ĵ洢��ʼ��ַ
	dataAddr=HE_ADDR_GAP*hENum+HE_KEY_LENG_MAX*keyNum+HE_EAMAIN_MAX;
	//������������ı����ռ���ʼ��ַ
	otherAddr=HE_ADDR_GAP*hENum+HE_EAMAIN_MAX+HE_KEY_LENG_MAX*HE_KEY_TOTLE_MAX;
	//�õ�����������ĳ��ȣ�������ÿ�������洢�ռ�����һ�ֽ��ϣ�
	at24c256ReadRandom(&sendFraLength,1,dataAddr+HE_KEY_LENG_MAX-1);//
	//at24c256ReadRandom(&conStart,1,otherAddr);
	//���ݺ�����λ���õ���������ֽڳ��Ȳ�����ȡ��
	dgleng=sendFraLength/8;
	if(sendFraLength%8)
		dgleng++;
	//����ֽڳ��ȳ�����Χ��������Ч
	if(dgleng<HE_KEY_LENG_MAX)
	{
		at24c256ReadRandom(tmepDataGroup,dgleng,dataAddr);//��EEPROM�ж�ȡҪ����ĺ���������
		printf("���ͺ�������ֽڳ���:%c\t",0x30+dgleng);
		printf("����ֵΪ��");
		infraSendAndRecTest(tmepDataGroup,dgleng);
	}
	infraSendAnyLengData(tmepDataGroup,sendFraLength);//���������
	//��˸LED��ʾ����ɹ�
	for(i=0;i<6;i++)
	{
		LED=~LED;
		delay_ms(50);
	}
	LED=0;//��ԭLED״̬
	
	printf("��%c",0x30+hENum);printf("�żҵ�\r\n");
	//printf("��:%c",0x30+keyNum/10);printf("%c",0x30+keyNum%10);printf("�Ű���\r\n");
	printf("���Ͱ�����");
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



