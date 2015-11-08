#ifndef __USART1_READER_H
#define __USART1_READER_H

#include "stm32f10x.h"

#define Q_USART1_ELEMENT    30//element

#define EX_AD_DOMAIN_EN                0x68    // 数据头
#define EX_DATA_EX                     0x16    // 数据尾

/*
 * 指令：CMDL CMDH  0x0000复位  0x0020读数据  0x0021 写数据  0x0022 写操作完成 
 * 答复：CMDL CMDH  0x8000复位  0x8020读数据  0x8021 写数据  0x8022 写操作完成 
 * HEADER    CMDL CMDH    ADDRL ADDRH    DATA_LEN    DATAx    CRC8    END
 *  68                                                                16
*/
//配置选项：
#define CMD_REBOOT                   0x0000  // 恢复出厂设置
#define REP_REBOOT                   0x0080
#define CMD_READMSG                  0x2000
#define REP_READMSG                  0x2080
#define CMD_WRITEMSG                 0x2100
#define REP_WRITEMSG                 0x2180
#define CMD_WRITE_OK                 0x2200
#define REP_WRITE_OK                 0x2280

#define DCMD_MIN_PULSE               0x0001  // 在某个速度下保持的最小脉冲数
#define DCMD_DRIVER                  0x0002  // 驱动器细分数  
#define DCMD_MOTOFRACTION            0x0003  // 电机细分数  
#define DCMD_MOTOMAXSTART            0x0004  // 电机最大初速度，初速度超过该值会对电机造成伤害，单位转/min   
#define DCMD_MOTOMAXSPEED            0x0005  // 电机最大转速，单位转/min (最大2400转/min)
#define DCMD_ACCELE_TIM		     0x0006  // 加速总时间(MS)Acceleration time 在该时间内加速完绘制出加速曲线
#define DCMD_ACCELE_PERTIM	     0x0007  // 加速单位时间（MS）(根据单位时间来划分速度梯度) 最大25MS
#define DCMD_FRE_MULT_0              0x0008  // 每档倍频值设置（共8档） 档1
#define DCMD_FRE_MULT_1		     0x0108  // 每档倍频值设置（共8档） 档2
#define DCMD_FRE_MULT_2		     0x0208  // 每档倍频值设置（共8档） 档3
#define DCMD_FRE_MULT_3		     0x0308  // 每档倍频值设置（共8档） 档4
#define DCMD_FRE_MULT_4		     0x0408  // 每档倍频值设置（共8档） 档5
#define DCMD_FRE_MULT_5		     0x0508  // 每档倍频值设置（共8档） 档6
#define DCMD_FRE_MULT_6		     0x0608  // 每档倍频值设置（共8档） 档7
#define DCMD_FRE_MULT_7		     0x0708  // 每档倍频值设置（共8档） 档8
#define DCMD_LOCAL_ADDR              0x0009  // 本机地址配置
#define DCMDPULSE_EQUIVA             0x000A  // 脉冲当量设置
#define DCDM_VERSION                 0x0109  // 版本信息

void Usart1ModeInit(void);
void Usart1RxIntEnable(void);
void Usart1RxIntDisable(void);
void Usart1Tx(const u8* pbuf_msg , u8 size);

uint8_t Usart1EventEnqueue(uint8_t* pdata);// 数据接收 入队列
uint8_t Usart1EventDelqueue(const uint8_t* addr,uint8_t* pcmd,
                            uint8_t* prdata,uint8_t* prlen);// 数据 出队列 消耗
void Usart1SendMsgDataPackage(const uint8_t* cmd,const uint8_t* addr_domain,
                              const uint8_t* pdata,uint8_t len,
                              uint8_t* prdata,uint8_t* rlen);       // 数据打包

#endif