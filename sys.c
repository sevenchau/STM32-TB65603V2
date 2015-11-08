#include "sys.h" 
#include "stdio.h"
//******************************************************************************
// SysTick设置初始化
//******************************************************************************
void SysTick_Config1(void)
{
#if 1
#define SystemFreq		(FCLK*1000000.0)    // 单位为Hz
#define TB_SysTick		(TIME_TB*1000)		// 单位为uS,与示波器实测一致
  
  static INT32U ticks;
  
  ticks=(INT32U)((TB_SysTick/1000000.0)*SystemFreq);
  SysTick_Config(ticks);
#endif	
}

//******************************************************************************
// 时钟设置初始化
//******************************************************************************
void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;
  /*
  RCC_AdjustHSICalibrationValue 调整内部高速晶振（HSI）校准值
  RCC_ITConfig 使能或者失能指定的RCC中断
  RCC_ClearFlag 清除RCC的复位标志位
  RCC_GetITStatus 检查指定的RCC中断发生与否
  RCC_ClearITPendingBit 清除RCC的中断待处理位
  */
  /* RCC system reset(for debug purpose) */
  // 时钟系统复位
  RCC_DeInit();
  
  // 使能外部的8M晶振
  // 设置外部高速晶振（HSE）
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);
  
  // 使能或者失能内部高速晶振（HSI）
  RCC_HSICmd(DISABLE);
  
  // 等待HSE起振
  // 该函数将等待直到HSE就绪，或者在超时的情况下退出
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  
  if(HSEStartUpStatus == SUCCESS)
  {
    // 设置AHB时钟（HCLK）
    RCC_HCLKConfig(RCC_HCLK_Div_);	// 36 MHz
    
    // 设置低速AHB时钟（PCLK1）
    RCC_PCLK1Config(RCC_PCLK1_Div_);	// 2.25 MHz
    
    // 设置高速AHB时钟（PCLK2）
    RCC_PCLK2Config(RCC_PCLK2_Div_);	// 2.25 MHz
    
    /* ADCCLK = PCLK2/8 */
    // 设置ADC时钟（ADCCLK）
    RCC_ADCCLKConfig(RCC_ADC_DIV_);	// 0.281Mhz
    
    // 设置USB时钟（USBCLK）
    // USB时钟 = PLL时钟除以1.5
    //RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
    
    // 设置外部低速晶振（LSE）
    RCC_LSEConfig(RCC_LSE_OFF);
    
    // 使能或者失能内部低速晶振（LSI）
    // LSE晶振OFF
    RCC_LSICmd(DISABLE);
    
    // 设置RTC时钟（RTCCLK）
    // 选择HSE时钟频率除以128作为RTC时钟
    //RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);
    
    // 使能或者失能RTC时钟
    // RTC时钟的新状态
    RCC_RTCCLKCmd(DISABLE);
    
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    // 设置PLL时钟源及倍频系数
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_); 
    
    /* Enable PLL */
    // 使能或者失能PLL
    RCC_PLLCmd(ENABLE);
    
    /* Wait till PLL is ready */
    // 检查指定的RCC标志位设置与否
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }
    
    /* Select PLL as system clock source */
    // 设置系统时钟（SYSCLK）
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    
    /* Wait till PLL is used as system clock source */
    // 返回用作系统时钟的时钟源
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  
//  // 使能或者失能AHB外设时钟
//  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1
//                        |RCC_AHBPeriph_DMA2
//                          |RCC_AHBPeriph_SRAM
//                            |RCC_AHBPeriph_FLITF
//                              |RCC_AHBPeriph_CRC
//                                |RCC_AHBPeriph_FSMC
//                                  |RCC_AHBPeriph_SDIO,DISABLE);
//  // 使能或者失能APB1外设时钟
//  RCC_APB1PeriphClockCmd(RCC_APB1Periph_ALL,DISABLE);
//  
//  // 强制或者释放高速APB（APB2）外设复位
//  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ALL,ENABLE);
//  // 退出复位状态
//  RCC_APB2PeriphResetCmd(RCC_APB2Periph_ALL,DISABLE);
//  
//  // 强制或者释放低速APB（APB1）外设复位
//  RCC_APB1PeriphResetCmd(RCC_APB1Periph_ALL,ENABLE);
//  
//  // 强制或者释放后备域复位
//  RCC_BackupResetCmd(ENABLE);
//  
//  // 使能或者失能时钟安全系统
//  RCC_ClockSecuritySystemCmd(DISABLE);
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);
}
//******************************************************************************
// NVIC设置
//******************************************************************************
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;
  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel=TIM1_CC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //NVIC_InitStructure.NVIC_IRQChannel=DMA1_Channel5_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  //NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  //NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  //NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel= EXTI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  NVIC_InitStructure.NVIC_IRQChannel= EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

//******************************************************************************
// GPIO设置
//******************************************************************************
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB
                         |RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
  
  //time1 set----------------------------------------------------------------
//  GPIO_Write(GPIOA,0xffff);
  /* GPIOA Configuration: Channel 3 as alternate function push-pull PA10/USART1_RX/TIM1_CH3*/
//  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(GPIOA, &GPIO_InitStructure);
//  GPIO_SetBits(GPIOA,GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11);
  /*PA11 / USART1_CTS/CANRX / USBDM/TIM1_CH4*/
  
//  GPIO_Write(GPIOB,0xffff);	// 11111101-11111111 
//  /* GPIOB Configuration: Channel 3N as alternate function push-pull PB15/SPI2_MOSI/TIM1_CH3N*/
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
//  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
//  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //LED-------------------------------------------------------------------------
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIO_SetBits(GPIOA,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_5);
  
  //倍频设置 1 10 100档位 优先级从高至低----------------------------
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOC,GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15);
  
  /*
  *     x-y-z-a Axis channel 轴向选择 xyza轴(信号捕捉) 优先级从高至低
  *     电机在开始运行前将捕捉这些IO口上的信号,判别运行的是哪一轴。
  *     为了确保捕捉稳定性，需要多捕捉几次。
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  //x-y-z-a Axis channel 轴向选择 xyza轴(信号输出给电机控制器) 电机的使能---------
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOB, GPIO_Pin_14);    // 电机的 使能 管脚  
  
  /*
  *     PA7 脉冲方向（电机正反转） —— 该IO用于捕捉来着CPLD上的正反转信号
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

 /*
  *     输入脉冲信号捕捉
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);
  EXTI_InitStructure.EXTI_Line = EXTI_Line2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 

 /*
  *     急停键
  */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure); 
}

void ModeInquiryInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
  
  //配置模式 —— 普通模式 切换 仅开机时 读取并确认
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//GPIO_Mode_IPD;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t GetModeInquiry(void)
{
  return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
}

//******************************************************************************
// pcb上的指示灯
//******************************************************************************
void led_toggle(uint16_t led_pin_x)//翻转
{
  GPIOA->ODR ^= led_pin_x;
}
void led_off(uint16_t led_pin_x)// 关灯 BSRR GPIO_SetBits
{
  GPIOA->BSRR = led_pin_x;
}
void led_on(uint16_t led_pin_x)// 开灯 BRR 
{
  GPIOA->BRR = led_pin_x;
}

//******************************************************************************
// 延时程序，单位为*1ms
//******************************************************************************
void delayms(INT16U cnt)
{
  //#define  	CONST_1MS  7333  	// 72MhZ
  //#define  	CONST_1MS 3588  	// 32MhZ
#define  	CONST_1MS (105*FCLK) 
  
  INT16U i;
  
  __no_operation();
  while(cnt--)
    for (i=0; i<CONST_1MS; i++);
}
