#ifndef __SYS_H
#define __SYS_H

#include "stm32f10x.h"
#include "dzy.h"
#include "stm32_m.h"
#include "myRCC.h"  
#include "stdio.h"

#define LED_1   GPIO_Pin_3
#define LED_2   GPIO_Pin_4
#define LED_3   GPIO_Pin_5
#define LED_4   GPIO_Pin_6

void SysTick_Config1(void);// SysTick设置初始化
void RCC_Configuration(void);// 时钟设置初始化
void NVIC_Configuration(void);// NVIC设置
void GPIO_Configuration(void);// GPIO设置

void ModeInquiryInit(void);
uint8_t GetModeInquiry(void);

void led_toggle(uint16_t led_pin_x);// 指示灯 取反
void led_on(uint16_t led_pin_x);// 开灯
void led_off(uint16_t led_pin_x);// 关灯

void func_time_tb(void);// 基本定时程序,周期为 TIME_TB
void delayms(INT16U cnt);// 延时程序，单位为*1ms

#endif