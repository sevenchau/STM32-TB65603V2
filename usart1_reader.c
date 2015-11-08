#include  "usart1_reader.h"
#include "string.h"
#include "queue.h"
#include "config_model.h"
#include "st_flashdriver.h"

#define AD_DOMAIN_EN                0x68    // 数据头
#define DATA_EX                     0x16    // 数据尾

/*
 *      |数据头1|控制域2|地址域4|数据长度域1|数据标示域2|数据（0~255）|校验域1|数据尾1|
*/
// 控制域
typedef struct __CMD_DOMAIN  // 2 byte
{
  uint8_t cmd0;
  uint8_t cmd1;
}CMD_Tpyedef;

// 地址域
typedef struct __ADDR_DOMAIN // 4byte
{
  uint8_t addr0;
  uint8_t addr1;
}ADDR_TypeDef;

// 数据 标示
typedef struct __DATA_MARK  // 2 byte
{
  uint8_t dmark0;
  uint8_t dmark1;
}DMARK_TypeDef;

typedef struct __FRAM_FORMAT
{
  uint8_t header8;
  CMD_Tpyedef cmd16;
  ADDR_TypeDef addr16;
  uint8_t dlen8;
  DMARK_TypeDef dmark16;
}FRAM_TypeDef;

#define Q_USART1_NUM     5
Queue Qusart1;
uint8_t Qusart1Tcb[Q_USART1_NUM][Q_USART1_ELEMENT];

const unsigned char CRC8Table[] =   //CRC校验
{
  0, 94, 188, 226, 97, 63, 221, 131, 194, 156, 126, 32, 163, 253, 31, 65,
  157, 195, 33, 127, 252, 162, 64, 30, 95, 1, 227, 189, 62, 96, 130, 220,
  35, 125, 159, 193, 66, 28, 254, 160, 225, 191, 93, 3, 128, 222, 60, 98,
  190, 224, 2, 92, 223, 129, 99, 61, 124, 34, 192, 158, 29, 67, 161, 255,
  70, 24, 250, 164, 39, 121, 155, 197, 132, 218, 56, 102, 229, 187, 89, 7,
  219, 133, 103, 57, 186, 228, 6, 88, 25, 71, 165, 251, 120, 38, 196, 154,
  101, 59, 217, 135, 4, 90, 184, 230, 167, 249, 27, 69, 198, 152, 122, 36,
  248, 166, 68, 26, 153, 199, 37, 123, 58, 100, 134, 216, 91, 5, 231, 185,
  140, 210, 48, 110, 237, 179, 81, 15, 78, 16, 242, 172, 47, 113, 147, 205,
  17, 79, 173, 243, 112, 46, 204, 146, 211, 141, 111, 49, 178, 236, 14, 80,
  175, 241, 19, 77, 206, 144, 114, 44, 109, 51, 209, 143, 12, 82, 176, 238,
  50, 108, 142, 208, 83, 13, 239, 177, 240, 174, 76, 18, 145, 207, 45, 115,
  202, 148, 118, 40, 171, 245, 23, 73, 8, 86, 180, 234, 105, 55, 213, 139,
  87, 9, 235, 181, 54, 104, 138, 212, 149, 203, 41, 119, 244, 170, 72, 22,
  233, 183, 85, 11, 136, 214, 52, 106, 43, 117, 151, 201, 74, 20, 246, 168,
  116, 42, 200, 150, 21, 75, 169, 247, 182, 232, 10, 84, 215, 137, 107, 53
};
 
unsigned char CRC8(unsigned char * pData, unsigned char len)
{
    unsigned char CRC_BUF = 0;
    for( ; len > 0; len--)
    {
        CRC_BUF = CRC8Table[CRC_BUF ^ *pData];
        pData++;
    }
    return(CRC_BUF);
}

void Usart1ModeInit(void)
{
  USART1_Config();
  USART1_NVIC_Configuration();
  memset(Qusart1Tcb,0,Q_USART1_NUM*Q_USART1_ELEMENT);
  Queue_Init(&Qusart1,Q_USART1_NUM,Q_USART1_ELEMENT);
}

void Usart1RxIntEnable(void)
{
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

void Usart1RxIntDisable(void)
{
  USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
}

void Usart1Tx(const u8* pbuf_msg , u8 size)
{
  u8 i;
  for(i=0;i<size;i++)
  {
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    USART_SendData(USART1, (uint8_t)(pbuf_msg[i]));
  }
}

// 数据接收 入队列
uint8_t Usart1EventEnqueue(uint8_t* pdata)
{
  unsigned short ptigger;
  
  __set_PRIMASK(1);
  if(!enQueue(&Qusart1, &ptigger))
  {
    memcpy(&Qusart1Tcb[ptigger],pdata,Q_USART1_ELEMENT);
    __set_PRIMASK(0);
    return 0;
  }
  __set_PRIMASK(0);
  return 1;
}

uint8_t TMP;
uint8_t TMPBUF[20];
// 数据 出队列 消耗
uint8_t Usart1EventDelqueue(const uint8_t* addr,uint8_t* pcmd,uint8_t* prdata,uint8_t* prlen)
{
  unsigned short ptigger;
  uint8_t tmp_tcb[Q_USART1_ELEMENT];
  FRAM_TypeDef* fram_data;

  while(1)
  {
    memset(tmp_tcb,0,Q_USART1_ELEMENT);
    __set_PRIMASK(1);
    if(deQueue(&Qusart1, &ptigger))
    {
      __set_PRIMASK(0);
      return 1;
    }
    memcpy(&tmp_tcb,&Qusart1Tcb[ptigger],Q_USART1_ELEMENT);
    memset(Qusart1Tcb[ptigger],0,Q_USART1_ELEMENT);
    __set_PRIMASK(0);
    fram_data = (FRAM_TypeDef*)tmp_tcb;
    
    // 判断数据头
    if(AD_DOMAIN_EN != fram_data->header8)
    {
      memset(tmp_tcb,0,Q_USART1_ELEMENT);
      continue;
    }
    // 判断数据尾
    if(DATA_EX != tmp_tcb[7+fram_data->dlen8])
    {
      memset(tmp_tcb,0,Q_USART1_ELEMENT);
      continue;
    }
    // 判断CRC校验
    if(tmp_tcb[6+fram_data->dlen8] != CRC8((unsigned char*)tmp_tcb+1, 5+fram_data->dlen8))
    {
      memset(tmp_tcb,0,Q_USART1_ELEMENT);
      continue;
    }
    
    // 地址判断
    if((0x0000 != *((uint16_t*)&(fram_data->addr16)))&&((*((uint16_t*)addr)) != (*((uint16_t*)&fram_data->addr16))))
    {
      memset(tmp_tcb,0,Q_USART1_ELEMENT);
      continue;
    }

    memcpy(pcmd,&fram_data->cmd16,sizeof(CMD_Tpyedef));
    *prlen = fram_data->dlen8;
    
    memcpy(TMPBUF,(&fram_data->dlen8)+1,fram_data->dlen8);
    memcpy(prdata,(&fram_data->dlen8)+1,fram_data->dlen8);
    
    return 0;
  }
}

// 数据打包
void Usart1SendMsgDataPackage(const uint8_t* cmd,const uint8_t* addr_domain,
                              const uint8_t* pdata,uint8_t len,
                              uint8_t* prdata,uint8_t* rlen)
{
  uint8_t data_len = 0;
  
  // 数据头
  *(prdata+0)  = AD_DOMAIN_EN;
  data_len += 1;
  // 控制域
  memcpy(prdata+data_len,cmd,sizeof(CMD_Tpyedef));
  data_len += sizeof(CMD_Tpyedef);
  // 地址域
  memcpy(prdata+data_len,addr_domain,sizeof(ADDR_TypeDef));
  data_len += sizeof(ADDR_TypeDef);
  // 数据长度
  *(prdata+data_len) = len;
  data_len += 1;
  // 数据
  memcpy(prdata+data_len,pdata,len);
  data_len += len;
  // CRC校验
  *(prdata+data_len) = CRC8((unsigned char*)prdata+1, data_len-1);
  data_len += 1;
  // 数据尾
  *(prdata+data_len) = DATA_EX;
  data_len += 1;
  *rlen = data_len;
}