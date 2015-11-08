#include "stepper_motor.h"
#include "st_flashdriver.h"

extern uint32_t NOR_FlashWriteBuf[FLASH_MSG_LEN];

//#define MIN_PULSE_NUM               50 // 在某个速度下保持的最小脉冲数
//加减速法则，2分之一法则或3分之一法则  
//#define SPEEDPRINCIPLE              1//3  
//电机最大转速，单位转/min  
//#define MOTOMAXSPEED                1500      
//电机每转对应的传动距离,单位：mm  
//#define MOTOROUNDLENGTH             4  
//电机最大初速度，初速度超过该值会对电机造成伤害，单位转/min  
//#define MOTOMAXSTART                200  
//驱动器细分数  
//#define DRIVERSFRACTION             1  
//电机细分数  
//#define MOTOFRACTION                200  
//根据初速度，末速度，以及时间，计算加速路程  
#define ACCELERATESPACE(V0,Vt,t)    (((V0) + (Vt)) * (t) / 2)  
//根据加速路程，初速度，以及时间，计算末速度  
#define LASTVELOCITY(S,V0,t)        (2 * (S) / (t) - (V0))  
//根据电机转速（转/min），计算电机步速度（step/s）  
#define STEPSPEED(RV)               ((RV) * NOR_FlashWriteBuf[2] * NOR_FlashWriteBuf[1] / 60)//((RV) * MOTOFRACTION * DRIVERSFRACTION / 60)  
//根据长度计算电机所需走的步数，S的单位为mm  
#define TOTALSTEP(S)                S * NOR_FlashWriteBuf[2] * NOR_FlashWriteBuf[1] / NOR_FlashWriteBuf[16] //S * MOTOFRACTION * DRIVERSFRACTION / MOTOROUNDLENGTH   

int sped_tab[MAX_NUM_X];      // 速度列表
int cur_per_dis[MAX_NUM_X];   // 当前单位时间消耗的脉冲数
int cur_total_dis[MAX_NUM_X]; // 当前总共消耗的脉冲数
int cur_keep_cnt[MAX_NUM_X];  // 当前速度下保持次数，正常为0
extern T1_TypeDef T1ParaCfg[MAX_NUM_X];

strSpeed                            stepSped;

//函数名称： CalculateSpeedTab  
//函数功能：计算加减速表  
//参数参数：Speed 速度结构体 
//返回输出：速度结构体  
static Speed_t CalculateSpeedTab(Speed_t Speed)
{  
  int i;                          
  double aa;                    //加加速  
  double per_s;
  int DeltaV;                   //速度变化量  
  int tmpValue = 0;

  // 计算加加速度
  aa = (double)((Speed->Vt - Speed->V0) / 2)      //加速度变化拐点时的速度值                           a|  /|\ 面积为DeltaV              
    * 2                                         //知道直角三角形面积，逆推三角型高度时将面积*2        |  / | \面积为DeltaV  
      / (Speed->time / 2)                         //除以底边                                            | /  |  \面积为DeltaV              
        / (Speed->time / 2);                        //再除以底边，得到斜率，即加加速                      |/___|___\__time     
  
  /*开始速度计算 得到速度列表 */
  for(i = 0; i < ((Speed->time / 2) + 1); i++)  
  {  
    DeltaV = (int)((aa * i * i) / 2);                              //V = V0 + a * t / 2;  a = aa * t;  
    *(Speed->speed_tab + i) = Speed->V0 + DeltaV;                   //当前点的速度 
    *(Speed->speed_tab + Speed->time - i) = Speed->Vt - DeltaV;     //对称点的速度
  }
  
  Speed->pulse_keep_min = NOR_FlashWriteBuf[0];//MIN_PULSE_NUM;
  Speed->v_gradient_num = 0;
  
  /*得到单位时间内 脉冲列表*/
  for(i = 0; i < (Speed->time + 1); i++) 
  { 
    if(0 == i)
    {
      per_s = (double)((ACCELERATESPACE(Speed->V0,Speed->V0,Speed->t_unit)));
      if((int)per_s)
      {
        *(Speed->unit_cnt+Speed->v_gradient_num) = (int)per_s;//(int)TOTALSTEP(per_s);
        Speed->v_gradient_num ++;
      }
    }
    else
    {
      per_s = (double)((ACCELERATESPACE(*(Speed->speed_tab + i-1),*(Speed->speed_tab + i),Speed->t_unit)));
      if((int)per_s)
      {
        *(Speed->unit_cnt+Speed->v_gradient_num) = (int)per_s;//(int)TOTALSTEP(per_s);
        if(*(Speed->unit_cnt+Speed->v_gradient_num-1) != *(Speed->unit_cnt+Speed->v_gradient_num))
        {
          Speed->v_gradient_num ++;
        }
      }
    }
  }
  
  for(i=0;i<Speed->v_gradient_num;i++)
  {
    if(*(Speed->unit_cnt+i) < Speed->pulse_keep_min)
    {
      if(Speed->pulse_keep_min%(*(Speed->unit_cnt+i)))//是否能整除
      {
        *(Speed->plus_keep_cnt+i) = Speed->pulse_keep_min/(*(Speed->unit_cnt+i))+1;
      }
      else
      {
        *(Speed->plus_keep_cnt+i) = Speed->pulse_keep_min/(*(Speed->unit_cnt+i));
      }
    }
    else
    {
      *(Speed->plus_keep_cnt+i) = 0;
    }
    
    if(*(Speed->plus_keep_cnt+i))
    {
      tmpValue += (*(Speed->plus_keep_cnt+i))*(*(Speed->unit_cnt+i));
    }
    else
    {
      tmpValue += *(Speed->unit_cnt+i);
    }
    
    *(Speed->total_cnt+i) = tmpValue;
  }
  
  return Speed;
}

//函数名称：TestSpeed  
//函数功能：速度表计算函数  
//函数参数：V0      初速度，单位：转/min  
//          Vt      末速度，单位：转/min  
//          total_time     加速总时间
//          unit_time      单位时间 
//函数返回：无
void TestSpeed(int V0, int Vt,double total_time,double unit_time)  
{  
  Speed_t Speed = &stepSped;  
  Speed->V0 = STEPSPEED(V0);//STEPSPEED(V0);    // 起速/ 
  Speed->Vt = STEPSPEED(Vt);//STEPSPEED(Vt);    // 末速 / 
//  Speed->S = S;//TOTALSTEP(S);       // 路程  
//  Speed->t = time;                   // 加速时间设为0.2秒
  if(total_time/unit_time > MAX_NUM_X-1)
  {
    Speed->time = MAX_NUM_X-1;
  }
  else
  {
    Speed->time = (int)(total_time/unit_time);                 // 加速次数 
  }
  
  Speed->t_unit = unit_time;              // 单位时间
  Speed->pcur_tcb = 0;               // 当前电机所处的位置
  
  Speed->unit_cnt = cur_per_dis;   // 消耗脉冲数量
  Speed->total_cnt = cur_total_dis;   // 当前总距离
  Speed->speed_tab = sped_tab;         // 根据加速次数申请表格内存，加1，以防用的时候超界  
  Speed->pt1_cfg_tcb = T1ParaCfg;      // 每个点的时间配置
  Speed->plus_keep_cnt  = cur_keep_cnt;
  CalculateSpeedTab(Speed);          // 开始计算
}