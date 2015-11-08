#ifndef __ST_FLASH_H
#define __ST_FLASH_H

#include "stm32f10x.h"

/*
配置选项：
#define MIN_PULSE_NUM               50  // 在某个速度下保持的最小脉冲数
#define DRIVERSFRACTION             1   //驱动器细分数  
#define MOTOFRACTION                200 //电机细分数  
#define MOTOMAXSTART                200 //电机最大初速度，初速度超过该值会对电机造成伤害，单位转/min   
#define MOTOMAXSPEED                600 //电机最大转速，单位转/min (最大2400转/min)
#define ACCELE_TIM		    0.5 // 加速总时间(S)Acceleration time 在该时间内加速完绘制出加速曲线
#define ACCELE_PERTIM	            0.01// 加速单位时间（S）(根据单位时间来划分速度梯度) 最大0.025s

//每档倍频值设置（共8档）
#define FRE_MULT_0		    1
#define FRE_MULT_1		    1  
#define FRE_MULT_2		    10
#define FRE_MULT_3		    50
#define FRE_MULT_4		    100
#define FRE_MULT_5		    500
#define FRE_MULT_6		    1000
#define FRE_MULT_7		    5000
*/

#define FLASE_BASE_ADDR                 0

#define MIN_PULSE_NUM      0//(FLASE_BASE_ADDR+0)
  #define MIN_PULSE_NUM0_DEFAULT        10
  #define MIN_PULSE_NUM1_DEFAULT        0
  #define MIN_PULSE_NUM2_DEFAULT        0
  #define MIN_PULSE_NUM3_DEFAULT        0

#define DRIVERSFRACTION         1//(FLASE_BASE_ADDR+4) 
  #define DRIVERSFRACTION0_DEFAULT      1
  #define DRIVERSFRACTION1_DEFAULT      0
  #define DRIVERSFRACTION2_DEFAULT      0
  #define DRIVERSFRACTION3_DEFAULT      0

#define MOTOFRACTION            2//(FLASE_BASE_ADDR+8)
  #define MOTOFRACTION0_DEFAULT         200
  #define MOTOFRACTION1_DEFAULT         0
  #define MOTOFRACTION2_DEFAULT         0
  #define MOTOFRACTION3_DEFAULT         0

#define MOTOMAXSTART       3//(FLASE_BASE_ADDR+12)
  #define MOTOMAXSTART0_DEFAULT         200
  #define MOTOMAXSTART1_DEFAULT         0x00
  #define MOTOMAXSTART2_DEFAULT         0x00
  #define MOTOMAXSTART3_DEFAULT         0x00
 
//600 电机最大转速，单位转/min (最大2400转/min)
#define MOTOMAXSPEED       4//(FLASE_BASE_ADDR+16)
  #define MOTOMAXSPEED0_DEFAULT         0x58
  #define MOTOMAXSPEED1_DEFAULT         0x02
  #define MOTOMAXSPEED2_DEFAULT         0x00
  #define MOTOMAXSPEED3_DEFAULT         0x00

// 0.5加速总时间(S)Acceleration time 在该时间内加速完绘制出加速曲线
#define ACCELE_TIM         5//(FLASE_BASE_ADDR+20)
  #define ACCELE_TIM0_DEFAULT           0x10
  #define ACCELE_TIM1_DEFAULT           0x27
  #define ACCELE_TIM2_DEFAULT           0x00
  #define ACCELE_TIM3_DEFAULT           0x00
// 0.01加速单位时间（S）(根据单位时间来划分速度梯度) 最大0.025s		     
#define ACCELE_PERTIM	    6//(FLASE_BASE_ADDR+24)        
  #define ACCELE_PERTIM0_DEFAULT        0xE8
  #define ACCELE_PERTIM1_DEFAULT        0x03
  #define ACCELE_PERTIM2_DEFAULT        0x00
  #define ACCELE_PERTIM3_DEFAULT        0x00
#define FRE_MULT_0	    7//(FLASE_BASE_ADDR+28) 
  #define FRE_MULT_00_DEFAULT           1
  #define FRE_MULT_01_DEFAULT           0x00
  #define FRE_MULT_02_DEFAULT           0x00
  #define FRE_MULT_03_DEFAULT           0x00
#define FRE_MULT_1	    8//(FLASE_BASE_ADDR+32) 
  #define FRE_MULT_10_DEFAULT           1
  #define FRE_MULT_11_DEFAULT           0x00
  #define FRE_MULT_12_DEFAULT           0x00
  #define FRE_MULT_13_DEFAULT           0x00
#define FRE_MULT_2	    9//(FLASE_BASE_ADDR+36) 
  #define FRE_MULT_20_DEFAULT           10
  #define FRE_MULT_21_DEFAULT           0x00
  #define FRE_MULT_22_DEFAULT           0x00
  #define FRE_MULT_23_DEFAULT           0x00
#define FRE_MULT_3	    10//(FLASE_BASE_ADDR+40) 
  #define FRE_MULT_30_DEFAULT           50
  #define FRE_MULT_31_DEFAULT           0x00
  #define FRE_MULT_32_DEFAULT           0x00
  #define FRE_MULT_33_DEFAULT           0x00
#define FRE_MULT_4	    11//(FLASE_BASE_ADDR+44) 
  #define FRE_MULT_40_DEFAULT           100
  #define FRE_MULT_41_DEFAULT           0x00
  #define FRE_MULT_42_DEFAULT           0x00
  #define FRE_MULT_43_DEFAULT           0x00
#define FRE_MULT_5	    12//(FLASE_BASE_ADDR+48) 
  #define FRE_MULT_50_DEFAULT           0xF4
  #define FRE_MULT_51_DEFAULT           0x01
  #define FRE_MULT_52_DEFAULT           0x00
  #define FRE_MULT_53_DEFAULT           0x00
#define FRE_MULT_6	    13//(FLASE_BASE_ADDR+52) 
  #define FRE_MULT_60_DEFAULT           0xe8
  #define FRE_MULT_61_DEFAULT           0x03
  #define FRE_MULT_62_DEFAULT           0x00
  #define FRE_MULT_63_DEFAULT           0x00
#define FRE_MULT_7	    14//(FLASE_BASE_ADDR+56) 
  #define FRE_MULT_70_DEFAULT           0x88
  #define FRE_MULT_71_DEFAULT           0x13
  #define FRE_MULT_72_DEFAULT           0x00
  #define FRE_MULT_73_DEFAULT           0x00

#define LOCAL_ADDR          15//(FLASE_BASE_ADDR+60) 
  #define LOCAL_ADDR0_DEFAULT           0x00
  #define LOCAL_ADDR1_DEFAULT           0x00
  #define LOCAL_ADDR2_DEFAULT           0x00
  #define LOCAL_ADDR3_DEFAULT           0x00

#define PULSE_EQUIVA                    16//Pulse equivalent
  #define PULSE_EQUIVA0_DEFAULT         0x01
  #define PULSE_EQUIVA1_DEFAULT         0x00
  #define PULSE_EQUIVA2_DEFAULT         0x00
  #define PULSE_EQUIVA3_DEFAULT         0x00
   
#define FLAG_VALUE                      0x76543210
#define WRITE_FLAG_STATUS   17//(FLASE_BASE_ADDR+64)    
  #define WRITE_FLAG0_DEFAULT          0x10
  #define WRITE_FLAG1_DEFAULT          0x32
  #define WRITE_FLAG2_DEFAULT          0x54
  #define WRITE_FLAG3_DEFAULT          0x76

#define FLASH_MSG_LEN            18

typedef union __DWORD_TO_BYTES
{
  uint32_t dword;
  struct
  {
    uint8_t b0;
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
  }bytes;
}DWD_TypeDef;

void FlashMsgInit(void);
void ReFarctory(void);
uint8_t WriteMsgToFlash(uint32_t* buffer,uint8_t buffer_size,uint8_t block_name);
uint8_t ReadMsgFromFlash(uint32_t* buffer,uint8_t buffer_size,uint8_t block_name);

#endif