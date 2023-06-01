#ifndef __OLED_H
#define __OLED_H			  	 
#include "sys.h"

#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

//OLEDģʽ����
//0:4�ߴ���ģʽ
//1:����8080ģʽ
#define OLED_MODE 0

#define OLED_GPIO_RCC RCC_APB2Periph_GPIOB

#define OLED_CS_GPIO GPIOB//Ƭѡ
#define OLED_CS_PIN GPIO_Pin_1

#define OLED_RST_GPIO GPIOB//��λ
#define OLED_RST_PIN GPIO_Pin_12

#define OLED_DC_GPIO  GPIOB//����/�������
#define OLED_DC_PIN GPIO_Pin_2

#define OLED_SCL_GPIO  GPIOB//ʱ�� D0��SCLK)
#define OLED_SCL_PIN GPIO_Pin_5

#define OLED_SDIN_GPIO  GPIOB//D1��MOSI�� ����
#define OLED_SDIN_PIN GPIO_Pin_13


#define OLED_CS_Clr()  PBout(1)=0
#define OLED_CS_Set()  PBout(1)=1

#define OLED_RST_Clr() PBout(12)=0
#define OLED_RST_Set() PBout(12)=1

#define OLED_DC_Clr() PBout(2)=0
#define OLED_DC_Set() PBout(2)=1

#define OLED_SCLK_Clr() PBout(5)=0
#define OLED_SCLK_Set() PBout(5)=1

#define OLED_SDIN_Clr() PBout(13)=0
#define OLED_SDIN_Set() PBout(13)=1


#define SIZE 16
#define XLevelL		0x02
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF 
#define X_WIDTH 	128
#define Y_WIDTH 	64	    						  
//-----------------OLED�˿ڶ���----------------  					   
//OLED�����ú���
void OLED_WR_Byte(u8 dat,u8 cmd);	    
void OLED_Display_On(void);
void OLED_Display_Off(void);	   							   		    
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size2);
void OLED_ShowString(u8 x,u8 y, u8 *p);	 
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
#endif  
	 



