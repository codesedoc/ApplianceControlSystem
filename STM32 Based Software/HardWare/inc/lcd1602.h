#ifndef __LCD1602_H
#define __LCD1602_H

#include "sys.h"

#define LCD1602_GPIOR_RCC RCC_APB2Periph_GPIOB


#define LCD1602_RS_GPIO GPIOB
#define LCD1602_RS_PIN GPIO_Pin_12
#define LCD1602_RS PBout(12)

#define LCD1602_RW_GPIO GPIOB
#define LCD1602_RW_PIN GPIO_Pin_13
#define LCD1602_RW PBout(13)

#define LCD1602_E_GPIO GPIOB
#define LCD1602_E_PIN GPIO_Pin_14
#define LCD1602_E PBout(14)

#define LCD1602_D0_GPIO GPIOB
#define LCD1602_D1_GPIO GPIOB
#define LCD1602_D2_GPIO GPIOB
#define LCD1602_D3_GPIO GPIOB
#define LCD1602_D4_GPIO GPIOB
#define LCD1602_D5_GPIO GPIOB
#define LCD1602_D6_GPIO GPIOB
#define LCD1602_D7_GPIO GPIOB

#define LCD1602_D0_PIN GPIO_Pin_1
#define LCD1602_D1_PIN GPIO_Pin_2
#define LCD1602_D2_PIN GPIO_Pin_3
#define LCD1602_D3_PIN GPIO_Pin_4
#define LCD1602_D4_PIN GPIO_Pin_5
#define LCD1602_D5_PIN GPIO_Pin_6
#define LCD1602_D6_PIN GPIO_Pin_7
#define LCD1602_D7_PIN GPIO_Pin_8
void Lcd1602_Init(void);
void Lcd1602Test(void);
#endif
