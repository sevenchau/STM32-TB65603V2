/**
  ******************************************************************************
  * @file    main.c
  * @author  
  * @version V1.0.0
  * @date    07/13/2014
  * @brief 
  --- led
  *LED1---------PB5
  *LED2---------PB6
  *LED3---------PB7
  *LED4---------PB8
  --- time1
  *TIM1 CH3  ------PA10
  *TIM1 CH3N ------ PB15 
  --- dma addr
  *TIM1_CCR3_Address    0x40012C3C
  --- External Interrupt
  *
  ******************************************************************************
**/

#include "stm32f10x.h"
#include "sys.h"
#include "stepper_motor.h"
#include "config_model.h"
#include  "usart1_reader.h" 
#include "st_flashdriver.h"
#include "clpd_clk.h"

#include <string.h>

#define STEP0      0
#define STEP1      1

#define RCC_APB2Periph_WATCHDOG     RCC_APB2Periph_GPIOB
#define GPIOWATCHDOG                GPIOB
#define WATCHDOGPIN                 GPIO_Pin_15

//#define DEBUG

uint32_t NOR_FlashWriteBuf[FLASH_MSG_LEN];

INT8U StepxStatus = STEP0;

INT16U rand_vlaue;
extern volatile INT32U PulsesNumber;
extern uint8_t Tim1OverFlag;

void ConfigProcess(void);
void SoftReset(void) ;

/*
SP706 是 Exar（原 Sipex）公司推出的低功耗、高可靠、低价格的 MCU复位监控芯片。
以下是其关键特性： 
z  分为 4 个子型号：SP706P、SP706R、SP706S、SP706T 
z  复位输出：P为高电平有效，R/S/T 为低电平有效 
z  精密的低电压监控：P/R为 2.63V、S为 2.93V、T为 3.08V 
z  复位脉冲宽度：200ms（额定值） 
z  独立的看门狗定时器：1.6 秒超时（额定值） 
z  去抖 TTL/CMOS 手动复位输入（/MR 管脚） 
*/
void WDG_SP706_GPIO_Configration(void)
{
#if DEBUG
  GPIO_InitTypeDef  gpio_init;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_WATCHDOG, ENABLE);
  
  //GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE); 
  gpio_init.GPIO_Pin   = WATCHDOGPIN;
  gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
  gpio_init.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOWATCHDOG, &gpio_init);	
#endif
}

void WDG_SP706_Enable(void)
{
#if DEBUG
  GPIO_InitTypeDef  gpio_init;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_WATCHDOG, ENABLE);
  
  gpio_init.GPIO_Pin   = WATCHDOGPIN;
  gpio_init.GPIO_Speed = GPIO_Speed_10MHz;
  gpio_init.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOWATCHDOG, &gpio_init);
#endif
}

void WDG_SP706_Feed(void)
{
#if DEBUG  
  static u8 reg_val = 0;
  reg_val = GPIO_ReadOutputDataBit(GPIOWATCHDOG , WATCHDOGPIN);
//  reg_val = ~reg_val;
  if(1 == reg_val)
  {
    GPIO_ResetBits(GPIOWATCHDOG, WATCHDOGPIN);
    delayms(10);	
    return;
  }
  GPIO_SetBits(GPIOWATCHDOG, WATCHDOGPIN);   
  delayms(10);	
#endif
}

void Wdt_Init(void)
{
#if DEBUG
  // Enable write access to IWDG_PR and IWDG_RLR registers 
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //IWDG->KR = 0x5555
  // IWDG counter clock: 40KHz(LSI) / 64 = 0.625 KHz 
  IWDG_SetPrescaler(IWDG_Prescaler_16);   
  // Set counter reload value to 1250 
  IWDG_SetReload(0xfff);           //IWDG->RLR =0xFFF
  //Red IWDG counter
  IWDG_ReloadCounter();   //IWDG->KR = 0xAAAA
  // Enable IWDG (the LSI oscillator will be enabled by hardware) 
  IWDG_Enable();   //IWDG->KR = 0xCCCC
#endif
}

//喂狗 
void Kick_Dog(void)
{
#if DEBUG
  //Reload IWDG counter 
  IWDG_ReloadCounter();   //IWDG->KR = 0xAAAA   
//  WDG_SP706_Enable();
  WDG_SP706_Feed();
#endif
} 

static void buzzerOn(void)
{
  GPIO_SetBits(GPIOA, GPIO_Pin_12);
}

static void buzzerOff(void)
{
  GPIO_ResetBits(GPIOA, GPIO_Pin_12);
}

//******************************************************************************
// 主程序
//******************************************************************************
void main(void)
{
  RCC_Configuration();    
  SysTick_Config1();
  Tim4_init();
  ModeInquiryInit();
  Wdt_Init();
  WDG_SP706_GPIO_Configration();
  WDG_SP706_Enable();
  //TIM_BDTRStructInit(&TIM1_BDTRInitStruct)
  delayms(100);			// 延时，等待电压稳定
  Kick_Dog();
  
  if(GetModeInquiry())
  {
    delayms(5);
    if(GetModeInquiry())
    {
      delayms(5);
      if(GetModeInquiry())
      {
        delayms(5);
        if(GetModeInquiry())
        {
          buzzerOn();
          delayms(20);
          Kick_Dog();
          
          buzzerOff();
          delayms(100);
          Kick_Dog();
          
          buzzerOn();
          delayms(50);
          Kick_Dog();
          
          buzzerOff();
          delayms(50);
          Kick_Dog();
          
          delayms(100);
          Kick_Dog();
          
          buzzerOn();
          delayms(20);
          Kick_Dog();
          
          delayms(100);
          Kick_Dog();
          
          delayms(100);
          Kick_Dog();

          buzzerOff();
          delayms(20);
          Kick_Dog();
          
          ConfigProcess();// 进入配置模式
        }
      }
    }
  }

  GPIO_Configuration();
  buzzerOff();
  GPIO_ResetBits(GPIOA,GPIO_Pin_3);
  delayms(30);
  GPIO_ResetBits(GPIOA,GPIO_Pin_4);
  delayms(30);
  GPIO_ResetBits(GPIOA,GPIO_Pin_5);
  delayms(30);
  GPIO_ResetBits(GPIOA,GPIO_Pin_6);
  delayms(30);
  GPIO_SetBits(GPIOA,GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_5);
  FlashMsgInit();
  ReadMsgFromFlash(NOR_FlashWriteBuf,FLASH_MSG_LEN,FLASE_BASE_ADDR);
  if(FLAG_VALUE != NOR_FlashWriteBuf[FLASH_MSG_LEN-1])
  {
    buzzerOn();
    delayms(10);
    Kick_Dog();
    buzzerOff();
    delayms(300);
    SoftReset();
  }
  //启动速度Vmin  最大速度Vmax     加速时间  和  刷新周期  作为参数就可以完成设置 
  TestSpeed(NOR_FlashWriteBuf[3], 
            NOR_FlashWriteBuf[4], 
            (double)(NOR_FlashWriteBuf[5]/10000.0), 
            (double)(NOR_FlashWriteBuf[6]/10000.0)); 
  //(int V0, int Vt, int S, dobule time,int cnt)
  PerStepTimConfigInit(); // 每个速度阶梯TIME1的设置
  Tim1_init();
  NVIC_Configuration();
  while(1)
  {
//    for(uint8_t i=0;i<200;i++)
//    {
//      EXTI_GenerateSWInterrupt(EXTI_Line0);
//      delayms(1);
//    }
    Kick_Dog();
    delayms(100);
  }
}
uint32_t bcd_value;
uint32_t FlashWriteBuf[FLASH_MSG_LEN];
void ConfigProcess(void)
{
  
  uint32_t tmp_value;
  uint16_t cmd16 = 0;
  uint16_t dcmd16 = 0;
  uint8_t  pdata[30];
  uint8_t  tx_data[30];
  uint8_t  plen,tx_len;
  
  Usart1ModeInit();
  Usart1RxIntEnable();

  FlashMsgInit();
  ReadMsgFromFlash(FlashWriteBuf,FLASH_MSG_LEN,FLASE_BASE_ADDR);
  
  while(1)
  {
    Kick_Dog();
    if(!Usart1EventDelqueue((const uint8_t*)&FlashWriteBuf[LOCAL_ADDR],(uint8_t*)&cmd16,pdata,&plen))
    {
      switch(cmd16)
      {
      case CMD_REBOOT:
        cmd16 = REP_REBOOT;
        ReFarctory();
        Usart1SendMsgDataPackage((const uint8_t*)&cmd16,(const uint8_t*)&FlashWriteBuf[LOCAL_ADDR],
                                 pdata,plen,tx_data,&tx_len);
        Usart1Tx(tx_data,tx_len);  
        delayms(100);
        SoftReset();
        break;
      case CMD_READMSG://REP_READMSG:
        dcmd16 = *((uint16_t*)pdata);
        switch(dcmd16)
        {
        case DCMD_MIN_PULSE:// 在某个速度下保持的最小脉冲数
          *(pdata+5) = (uint8_t)FlashWriteBuf[0];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[0]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[0]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[0]>>24);
          plen = 6;
          break;
        case DCMD_DRIVER:// 驱动器细分数  
          *(pdata+5) = (uint8_t)FlashWriteBuf[1];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[1]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[1]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[1]>>24);
          plen = 6;
          break;
        case DCMD_MOTOFRACTION:// 电机细分数  
          *(pdata+5) = (uint8_t)FlashWriteBuf[2];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[2]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[2]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[2]>>24);
          plen = 6;
          break;
        case DCMD_MOTOMAXSTART:// 电机最大初速度，初速度超过该值会对电机造成伤害，单位转/min 
          *(pdata+5) = (uint8_t)FlashWriteBuf[3];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[3]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[3]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[3]>>24);
          plen = 6;
          break;
        case DCMD_MOTOMAXSPEED:// 电机最大转速，单位转/min (最大2400转/min)
          *(pdata+5) = (uint8_t)FlashWriteBuf[4];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[4]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[4]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[4]>>24);
          plen = 6;
          break;
        case DCMD_ACCELE_TIM:// 加速总时间(MS)Acceleration time 在该时间内加速完绘制出加速曲线
          *(pdata+5) = (uint8_t)FlashWriteBuf[5];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[5]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[5]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[5]>>24);
          plen = 6;
          break;
        case DCMD_ACCELE_PERTIM	:// 加速单位时间（MS）(根据单位时间来划分速度梯度) 最大25MS
          *(pdata+5) = (uint8_t)FlashWriteBuf[6];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[6]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[6]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[6]>>24);
          plen = 6;
          break;
        case DCMD_FRE_MULT_0:// 每档倍频值设置（共8档） 档1
          *(pdata+5) = (uint8_t)FlashWriteBuf[7];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[7]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[7]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[7]>>24);
          plen = 6;
          break;
        case DCMD_FRE_MULT_1:// 每档倍频值设置（共8档） 档2
          *(pdata+5) = (uint8_t)FlashWriteBuf[8];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[8]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[8]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[8]>>24);
          plen = 6;
          break;
        case DCMD_FRE_MULT_2:// 每档倍频值设置（共8档） 档3
          *(pdata+5) = (uint8_t)FlashWriteBuf[9];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[9]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[9]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[9]>>24);
          plen = 6;
          break;
        case DCMD_FRE_MULT_3:// 每档倍频值设置（共8档） 档4
          *(pdata+5) = (uint8_t)FlashWriteBuf[10];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[10]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[10]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[10]>>24);
          plen = 6;
          break;
        case DCMD_FRE_MULT_4:// 每档倍频值设置（共8档） 档5
          *(pdata+5) = (uint8_t)FlashWriteBuf[11];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[11]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[11]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[11]>>24);
          plen = 6;
          break;
        case DCMD_FRE_MULT_5:// 每档倍频值设置（共8档） 档6
          *(pdata+5) = (uint8_t)FlashWriteBuf[12];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[12]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[12]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[12]>>24);
          plen = 6;
          break;
        case DCMD_FRE_MULT_6:// 每档倍频值设置（共8档） 档7
          *(pdata+5) = (uint8_t)FlashWriteBuf[13];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[13]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[13]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[13]>>24);
          plen = 6;
          break;
        case DCMD_FRE_MULT_7:// 每档倍频值设置（共8档） 档8
          *(pdata+5) = (uint8_t)FlashWriteBuf[14];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[14]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[14]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[14]>>24);
          plen = 6;
          break;
        case DCMD_LOCAL_ADDR:// 本机地址配置
          *(pdata+5) = (uint8_t)FlashWriteBuf[15];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[15]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[15]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[15]>>24);
          plen = 6;
          break;
        case DCMDPULSE_EQUIVA: // 配置脉冲当量
          *(pdata+5) = (uint8_t)FlashWriteBuf[16];
          *(pdata+3) = (uint8_t)(FlashWriteBuf[16]>>8);
          *(pdata+4) = (uint8_t)(FlashWriteBuf[16]>>16);
          *(pdata+2) = (uint8_t)(FlashWriteBuf[16]>>24);
          plen = 6;
          break;
        case DCDM_VERSION:   // 版本信息
          pdata[2] = 0x00;
          pdata[3] = 0x01;
          pdata[4] = 0x00;
          pdata[5] = 0x01;
          plen = 6;
        default:
          break;
        }
        cmd16 = REP_READMSG;
        memset(tx_data,0,sizeof(tx_data));
        tmp_value = 0;
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]>>24);
        tmp_value <<= 8;
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]>>16);
        tmp_value <<= 8;
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]>>8);
        tmp_value <<= 8;
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]);
        tmp_value <<= 8;
        Usart1SendMsgDataPackage((const uint8_t*)&cmd16,(const uint8_t*)&tmp_value,
                                 pdata,plen,tx_data,&tx_len);
        Usart1Tx(tx_data,tx_len);
        buzzerOn();
        delayms(20);
        buzzerOff();
        delayms(20);
        break;
      case CMD_WRITEMSG://REP_WRITEMSG:
        dcmd16 = *((uint16_t*)pdata);
        tmp_value = 0;
        tmp_value |= (uint32_t)((*(pdata+2))<<24);
        tmp_value |= (uint32_t)((*(pdata+3))<<16);
        tmp_value |= (uint32_t)((*(pdata+4))<<8);
        tmp_value |= (uint32_t)(*(pdata+5));
        bcd_value = (tmp_value&0x0000000f)+10*((tmp_value>>4)&0x0000000f)
          +100*((tmp_value>>8)&0x0000000f)+1000*((tmp_value>>12)&0x0000000f)
           + 10000*((tmp_value>>16)&0x0000000f)+100000*((tmp_value>>20)&0x0000000f)
            +  1000000*((tmp_value>>24)&0x0000000f)+10000000*((tmp_value>>28)&0x0000000f);
        switch(dcmd16)
        {
        case DCMD_MIN_PULSE:// 在某个速度下保持的最小脉冲数
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[0],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_DRIVER:// 驱动器细分数  
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[1],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_MOTOFRACTION:// 电机细分数  
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[2],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_MOTOMAXSTART:// 电机最大初速度，初速度超过该值会对电机造成伤害，单位转/min 
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[3],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_MOTOMAXSPEED:// 电机最大转速，单位转/min (最大2400转/min)
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[4],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_ACCELE_TIM:// 加速总时间(MS)Acceleration time 在该时间内加速完绘制出加速曲线
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[5],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_ACCELE_PERTIM	:// 加速单位时间（MS）(根据单位时间来划分速度梯度) 最大25MS
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[6],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_FRE_MULT_0:// 每档倍频值设置（共8档） 档1
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[7],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_FRE_MULT_1:// 每档倍频值设置（共8档） 档2
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[8],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_FRE_MULT_2:// 每档倍频值设置（共8档） 档3
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[9],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_FRE_MULT_3:// 每档倍频值设置（共8档） 档4
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[10],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_FRE_MULT_4:// 每档倍频值设置（共8档） 档5
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[11],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_FRE_MULT_5:// 每档倍频值设置（共8档） 档6
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[12],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_FRE_MULT_6:// 每档倍频值设置（共8档） 档7
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[13],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_FRE_MULT_7:// 每档倍频值设置（共8档） 档8
          if(0 == bcd_value)goto loop1;
          memcpy((uint8_t*)&FlashWriteBuf[14],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMD_LOCAL_ADDR:// 本机地址配置
          if((0 == bcd_value)||(bcd_value) > 65535) continue;
          memcpy((uint8_t*)&FlashWriteBuf[15],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        case DCMDPULSE_EQUIVA: // 配置脉冲当量
          if(0 == bcd_value) continue;
          memcpy((uint8_t*)&FlashWriteBuf[16],(uint8_t*)&bcd_value,4);
          plen = 6;
          break;
        default:
          break;
        }
        cmd16 = REP_WRITEMSG;
        memset(tx_data,0,sizeof(tx_data));
        tmp_value = 0;
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]>>24);
        tmp_value <<= 8;
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]>>16);
        tmp_value <<= 8;
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]>>8);
        tmp_value <<= 8;
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]);
        tmp_value <<= 8;
        Usart1SendMsgDataPackage((const uint8_t*)&cmd16,(const uint8_t*)&tmp_value,
                                 pdata,plen,tx_data,&tx_len);
        Usart1Tx(tx_data,tx_len); 
        buzzerOn();
        delayms(20);
        buzzerOff();
        delayms(20);
        break;
      case CMD_WRITE_OK://REP_WRITE_OK:
        WriteMsgToFlash(FlashWriteBuf,FLASH_MSG_LEN,FLASE_BASE_ADDR);
        cmd16 = REP_WRITE_OK;
        tmp_value = 0;
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]>>24);
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]>>16);
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]>>8);
        tmp_value |= (uint8_t)(FlashWriteBuf[LOCAL_ADDR]);
        Usart1SendMsgDataPackage((const uint8_t*)&cmd16,(const uint8_t*)&tmp_value,
                                 pdata,plen,tx_data,&tx_len);
        Usart1Tx(tx_data,tx_len);
        buzzerOn();
        delayms(20);
        buzzerOff();
        delayms(20);
        Kick_Dog();
        delayms(100);
        SoftReset();
        break;
      default:
        continue;
        break;
      }   
      
    }
  loop1:
    cmd16 = 0;
    plen = 0;
    memset(pdata,0,sizeof(pdata));
    delayms(5);
  }
}

//******************************************************************************
// 软件复位
//******************************************************************************
void SoftReset(void) 
{  
  __set_FAULTMASK(1);      // 关闭所有中端
  NVIC_SystemReset();      // 复位
}

//******************************************************************************
// 进入睡眠模式
//******************************************************************************
void myPWR_EnterSleepMode(void)
{
  //PWR->CR |= CR_CWUF_Set;	/* Clear Wake-up flag */
  
  /* Set SLEEPDEEP bit of Cortex System Control Register */
  //*(__IO uint32_t *) SCB_SysCtrl |= SysCtrl_SLEEPDEEP_Set;
  /* This option is used to ensure that store operations are completed */
  
  __WFI(); 		/* Request Wait For Interrupt */
}


//******************************************************************************
#ifdef  USE_FULL_ASSERT
/**
* @brief  Reports the name of the source file and the source line number
*   where the assert_param error has occurred.
* @param file: pointer to the source file name
* @param line: assert_param error line source number
* @retval : None
*/
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
  ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  /* Infinite loop */
  while (1)
  {
  }
}
#endif














