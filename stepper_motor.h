#ifndef __STEPPER_MOTOR_h
#define __STEPPER_MOTOR_h

#include "stm32f10x.h"
#include "sys.h" 

#define SPD_INIT                0       //Initialization 初始化
#define SPD_STOP                1       //Stop           停止
//#define SPD_KEEP                2       //               保持当前状态
#define SPD_ACCELERATE          2       //Accelerate     加速
#define SPD_KEEP_ACCELERATE     3
#define SPD_SLOW_DOWN           4       //Slow down      减速
#define SPD_KEEP_DOWN           5
#define SPD_UNIFORM             6       //Uniform        匀速
#define SPD_KEEP_UNIFORM        7       
#define SPD_REMNANT             8       //               残余脉冲处理
#define SPD_KEEP_REMNANT        9

#define KEEP_STATUS             0x01
#define NOKEEP_STATUS           0x00

#define  MAX_NUM_X       255

#define AXIS_CHOICE(x)     PWM1_ON##x()
#define AXIS_UNCHOICE(x)   PWM1_OF##x()

#define PWM1_ON1()                 TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Enable)
#define PWM1_OF1()                 TIM_CCxCmd(TIM1, TIM_Channel_1, TIM_CCx_Disable)   
#define PWM1_ON2()                 TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Enable)
#define PWM1_OF2()                 TIM_CCxCmd(TIM1, TIM_Channel_2, TIM_CCx_Disable)
#define PWM1_ON3()                 TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Enable)
#define PWM1_OF3()                 TIM_CCxCmd(TIM1, TIM_Channel_3, TIM_CCx_Disable)   
#define PWM1_ON4()                 TIM_CCxCmd(TIM1, TIM_Channel_4, TIM_CCx_Enable)
#define PWM1_OF4()                 TIM_CCxCmd(TIM1, TIM_Channel_4, TIM_CCx_Disable)

#define PWMALL_OFF() TIM_CCxCmd(TIM1, TIM_Channel_1|TIM_Channel_2|TIM_Channel_3|TIM_Channel_4, TIM_CCx_Disable)  
#define PWMALL_ON()  TIM_CCxCmd(TIM1, TIM_Channel_1|TIM_Channel_2|TIM_Channel_3|TIM_Channel_4, TIM_CCx_Enable)

typedef struct __TIME1_ST
{
  uint16_t time_period;
  uint16_t tim_prescaler;
  uint16_t tim_overflow_cnt;  //溢出次数，每溢出一次计tim_overflow_cnt一次
  uint8_t  tim_repetition_cnt;//每计tim_repetition_cnt次溢出一次(溢出剩余值)
}T1_TypeDef;

struct SPEED  
{  
  int         V0;                 //初速度，单位：step/s  
  int         Vt;                 //末速度，单位：step/s  
  
  int         pulse_keep_min;     // 脉冲保持最小值
  int         v_gradient_num;     // 速度梯度数
  int         time;               //加速次数计算值    单位：次 
  double      t_unit;             //单位时间 s
   
  int         pcur_tcb;           //当前位置

  int         *speed_tab;         //加速速度表，速度单位：step/s  
  int         *unit_cnt;          //单位时间内行走距离消耗脉冲数量
  int         *total_cnt;         //当前走过的总路程
  int         *plus_keep_cnt; 

  T1_TypeDef  * pt1_cfg_tcb;       //每个点的时间配置
};  
typedef struct SPEED * Speed_t;  
typedef struct SPEED   strSpeed;  
extern strSpeed stepSped;

void Tim1_init(void);
uint8_t Tim1_Configuration(uint8_t speed_status);// tim1 参数配置

void PerStepTimConfigInit(void);// 每个速度阶梯TIME1的设置
void MotorRunningDisable(void);/*定时器输出使能*/
void MotorRunningEnable(void); /*定时器输出失能*/

void IntFrequencyMAxisCapture(void);/*根据倍频系数来,捕捉中断中调用*/
INT32U GetCapturePulsesValue(void);// 获取当前捕捉到的脉冲数
void AxisChoiseSwitchDisable(void);// 轴向选择并失能当前轴

INT16U GetCurMinPulsesNum(void);// 获取启动速度时最小消耗的脉冲数
INT16U GetCurTotalPulsesNum(void);//获取当前总路程需要消耗脉的冲数
INT8U IntSpeedCalculation(void);//加减速计算 
void TestSpeed(int V0, int Vt,double total_time,double unit_time) ;//通过这个函数将产生各类表数据

void StopTimmer(void);

#endif