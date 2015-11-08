#include "clpd_clk.h"

/*****************************************************************************
*
******************************************************************************/
void Tim4_init(void) 
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  TIM_DeInit(TIM4); 
  
  TIM_TimeBaseStructure.TIM_Period = 4;
  TIM_TimeBaseStructure.TIM_Prescaler = 0; // 10us
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
  //TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;// 每n次更新溢出

  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  
  /* PWM1 Mode configuration: Channel1 */   
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;//TIM_OCMode_PWM1; 
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 使能输出比较状态
  TIM_OCInitStructure.TIM_Pulse = 2;   	      
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;//TIM_OCPolarity_Low;//
  // 选择空闲状态下的非工作状态// 当MOE=0设置TIM1输出比较空闲状态// 默认输出位低电平
  //TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;//TIM_OCIdleState_Set;//
  
  /* PWM4 Mode configuration: Channel3 */   
  TIM_OC3Init(TIM4, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);// 使能TIMx在CCR1上的预装载寄存器
  //TIM_ForcedOC3Config(TIM4, TIM_ForcedAction_Active);
  //TIM_CCxCmd(TIM4, TIM_Channel_3, TIM_CCx_Enable);

  TIM_ARRPreloadConfig(TIM4, ENABLE);//使能TIM1在ARR上的预装载寄存器
  //TIM_ClearFlag(TIM4,TIM_FLAG_Update);
  //TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);	// 定时中断使能
  TIM_Cmd(TIM4, ENABLE);		        // 定时器开始运行
  //TIM_CtrlPWMOutputs(TIM4, ENABLE);             // 这条语句必须要有!!!
}