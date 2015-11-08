#include "st_flashdriver.h"
#include "sys.h" 
#include "string.h"

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/  

/* Private typedef -----------------------------------------------------------*/
#define FAILED  0
#define PASSED  1

/* Private define ------------------------------------------------------------*/
/* Define the STM32F10x FLASH Page Size depending on the used STM32 device */
//#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
//  #define FLASH_PAGE_SIZE    ((uint16_t)0x800) //2k/页
//#else
//  #define FLASH_PAGE_SIZE    ((uint16_t)0x400) //1k/页
//#endif
#define FLASH_PAGE_SIZE    ((uint16_t)0x400) //1k/页
#define BANK1_WRITE_START_ADDR  ((uint32_t)0x0800F000)//((uint32_t)(0x08000000+126*2*0x400L))//
#define BANK1_WRITE_END_ADDR    ((uint32_t)0x0800FC00)//((uint32_t)(0x08000000+127*2*0x400L)) //大小16k 
//#define BANK1_WRITE_START_ADDR  ((uint32_t)0x08010000)//((uint32_t)(0x08000000+126*2*0x400L))//
//#define BANK1_WRITE_END_ADDR    ((uint32_t)0x0801F000)//(

// #define BANK1_WRITE_START_ADDR   ((uint32_t)0x08088000)
// #define BANK1_WRITE_END_ADDR     ((uint32_t)0x0808C000)

uint32_t u32_NbrofPage = 0;
uint32_t u32_FlashAddr = 0;  
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;

static void st_FlashInit(void)
{
  uint32_t EraseCounter = 0;
  uint32_t NbrOfPage = 0;
  
  /* Porgram FLASH Bank1 ********************************************************/       
  /* Unlock the Flash Bank1 Program Erase controller */
  FLASH_Unlock();
//  FLASH_UnlockBank1();
  
  /* Define the number of page to be erased */
  NbrOfPage = (BANK1_WRITE_END_ADDR - BANK1_WRITE_START_ADDR) / FLASH_PAGE_SIZE;
  
  /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
  
  /* Erase the FLASH pages */
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && 
      (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASHStatus = FLASH_ErasePage(BANK1_WRITE_START_ADDR + 
                                  (FLASH_PAGE_SIZE * EraseCounter));
  }
  
  /*start addr 起始地址*/
  u32_FlashAddr = BANK1_WRITE_START_ADDR;
  
  FLASH_Lock();
//  FLASH_LockBank1();
}

uint8_t st_FlashWrite(uint32_t* data_buff , uint8_t buff_lenght)
{
  uint8_t  length = 0;
  uint32_t temp_addr = 0;
  uint8_t  MemoryProgramStatus = PASSED;
  
  /* Porgram FLASH Bank1 ********************************************************/       
  /* Unlock the Flash Bank1 Program Erase controller */
//  FLASH_UnlockBank1();
  FLASH_Unlock();
  
  while((u32_FlashAddr < BANK1_WRITE_END_ADDR) && 
        (FLASHStatus == FLASH_COMPLETE) && 
          (length < buff_lenght))
  {
    FLASHStatus = FLASH_ProgramWord(u32_FlashAddr, data_buff[length]);
    u32_FlashAddr = u32_FlashAddr + 4;
    length ++;
  }
  
  if((length != buff_lenght) || (FLASHStatus != FLASH_COMPLETE))
  {
    MemoryProgramStatus = FAILED;
  }
  
//  FLASH_LockBank1();
  FLASH_Lock();
  
  length = 0;
  
  /* Program Flash Bank1 */
  temp_addr = u32_FlashAddr - (buff_lenght-1)<<2;
  
  while((temp_addr < BANK1_WRITE_END_ADDR) && 
        (MemoryProgramStatus != FAILED) && 
          length < buff_lenght)
  {
    if((*(__IO uint32_t*) temp_addr) != data_buff[buff_lenght])
    {
      MemoryProgramStatus = FAILED;
    }
    temp_addr += 4;
  }
  
  return MemoryProgramStatus;
}

uint8_t st_FlashRead(uint32_t start_addr , uint32_t* buff_data , 
                            uint32_t buf_len)
{
  uint8_t MemoryReadStatus = PASSED;
  uint8_t temp_buf = 0;
  
  if((start_addr*4+BANK1_WRITE_START_ADDR)%4)
  {
    MemoryReadStatus = FAILED;
  }
  
  while(((BANK1_WRITE_START_ADDR + start_addr*4) < 
         BANK1_WRITE_END_ADDR)&& temp_buf < buf_len)
  {
    buff_data[temp_buf] = *(__IO uint32_t*) (BANK1_WRITE_START_ADDR+start_addr*4);
    start_addr += 1;
    temp_buf ++ ;
  }
  
  return MemoryReadStatus;
}

uint32_t flash_write_buf[FLASH_MSG_LEN];//FLASH_MSG_LEN

void FlashMsgInit(void)
{ 
  DWD_TypeDef tmp_dword;
  
  st_FlashRead(FLASE_BASE_ADDR, flash_write_buf, FLASH_MSG_LEN);
  tmp_dword.bytes.b0 = WRITE_FLAG0_DEFAULT;
  tmp_dword.bytes.b1 = WRITE_FLAG1_DEFAULT;
  tmp_dword.bytes.b2 = WRITE_FLAG2_DEFAULT;
  tmp_dword.bytes.b3 = WRITE_FLAG3_DEFAULT;
  if(tmp_dword.dword != flash_write_buf[FLASH_MSG_LEN-1])
  {
    while(1)
    {
      tmp_dword.bytes.b0 = MIN_PULSE_NUM0_DEFAULT;
      tmp_dword.bytes.b1 = MIN_PULSE_NUM1_DEFAULT;
      tmp_dword.bytes.b2 = MIN_PULSE_NUM2_DEFAULT;
      tmp_dword.bytes.b3 = MIN_PULSE_NUM3_DEFAULT;
      flash_write_buf[0] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = DRIVERSFRACTION0_DEFAULT;
      tmp_dword.bytes.b1 = DRIVERSFRACTION1_DEFAULT;
      tmp_dword.bytes.b2 = DRIVERSFRACTION2_DEFAULT;
      tmp_dword.bytes.b3 = DRIVERSFRACTION3_DEFAULT;      
      flash_write_buf[1] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = MOTOFRACTION0_DEFAULT;
      tmp_dword.bytes.b1 = MOTOFRACTION1_DEFAULT;
      tmp_dword.bytes.b2 = MOTOFRACTION2_DEFAULT;
      tmp_dword.bytes.b3 = MOTOFRACTION3_DEFAULT;        
      flash_write_buf[2] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = MOTOMAXSTART0_DEFAULT;
      tmp_dword.bytes.b1 = MOTOMAXSTART1_DEFAULT;
      tmp_dword.bytes.b2 = MOTOMAXSTART2_DEFAULT;
      tmp_dword.bytes.b3 = MOTOMAXSTART3_DEFAULT;    
      flash_write_buf[3] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = MOTOMAXSPEED0_DEFAULT;
      tmp_dword.bytes.b1 = MOTOMAXSPEED1_DEFAULT;
      tmp_dword.bytes.b2 = MOTOMAXSPEED2_DEFAULT;
      tmp_dword.bytes.b3 = MOTOMAXSPEED3_DEFAULT; 
      flash_write_buf[4] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = ACCELE_TIM0_DEFAULT;
      tmp_dword.bytes.b1 = ACCELE_TIM1_DEFAULT;
      tmp_dword.bytes.b2 = ACCELE_TIM2_DEFAULT;
      tmp_dword.bytes.b3 = ACCELE_TIM3_DEFAULT; 
      flash_write_buf[5] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = ACCELE_PERTIM0_DEFAULT;
      tmp_dword.bytes.b1 = ACCELE_PERTIM1_DEFAULT;
      tmp_dword.bytes.b2 = ACCELE_PERTIM2_DEFAULT;
      tmp_dword.bytes.b3 = ACCELE_PERTIM3_DEFAULT; 
      flash_write_buf[6] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = FRE_MULT_00_DEFAULT;
      tmp_dword.bytes.b1 = FRE_MULT_01_DEFAULT;
      tmp_dword.bytes.b2 = FRE_MULT_02_DEFAULT;
      tmp_dword.bytes.b3 = FRE_MULT_03_DEFAULT;
      flash_write_buf[7] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = FRE_MULT_10_DEFAULT;
      tmp_dword.bytes.b1 = FRE_MULT_11_DEFAULT;
      tmp_dword.bytes.b2 = FRE_MULT_12_DEFAULT;
      tmp_dword.bytes.b3 = FRE_MULT_13_DEFAULT;
      flash_write_buf[8] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = FRE_MULT_20_DEFAULT;
      tmp_dword.bytes.b1 = FRE_MULT_21_DEFAULT;
      tmp_dword.bytes.b2 = FRE_MULT_22_DEFAULT;
      tmp_dword.bytes.b3 = FRE_MULT_23_DEFAULT;
      flash_write_buf[9] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = FRE_MULT_30_DEFAULT;
      tmp_dword.bytes.b1 = FRE_MULT_31_DEFAULT;
      tmp_dword.bytes.b2 = FRE_MULT_32_DEFAULT;
      tmp_dword.bytes.b3 = FRE_MULT_33_DEFAULT;
      flash_write_buf[10] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = FRE_MULT_40_DEFAULT;
      tmp_dword.bytes.b1 = FRE_MULT_41_DEFAULT;
      tmp_dword.bytes.b2 = FRE_MULT_42_DEFAULT;
      tmp_dword.bytes.b3 = FRE_MULT_43_DEFAULT;
      flash_write_buf[11] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = FRE_MULT_50_DEFAULT;
      tmp_dword.bytes.b1 = FRE_MULT_51_DEFAULT;
      tmp_dword.bytes.b2 = FRE_MULT_52_DEFAULT;
      tmp_dword.bytes.b3 = FRE_MULT_53_DEFAULT;
      flash_write_buf[12] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = FRE_MULT_60_DEFAULT;
      tmp_dword.bytes.b1 = FRE_MULT_61_DEFAULT;
      tmp_dword.bytes.b2 = FRE_MULT_62_DEFAULT;
      tmp_dword.bytes.b3 = FRE_MULT_63_DEFAULT;
      flash_write_buf[13] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = FRE_MULT_70_DEFAULT;
      tmp_dword.bytes.b1 = FRE_MULT_71_DEFAULT;
      tmp_dword.bytes.b2 = FRE_MULT_72_DEFAULT;
      tmp_dword.bytes.b3 = FRE_MULT_73_DEFAULT;
      flash_write_buf[14] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = LOCAL_ADDR0_DEFAULT;
      tmp_dword.bytes.b1 = LOCAL_ADDR1_DEFAULT;
      tmp_dword.bytes.b2 = LOCAL_ADDR2_DEFAULT;
      tmp_dword.bytes.b3 = LOCAL_ADDR3_DEFAULT;
      flash_write_buf[15] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = PULSE_EQUIVA0_DEFAULT;
      tmp_dword.bytes.b1 = PULSE_EQUIVA1_DEFAULT;
      tmp_dword.bytes.b2 = PULSE_EQUIVA2_DEFAULT;
      tmp_dword.bytes.b3 = PULSE_EQUIVA3_DEFAULT;
      flash_write_buf[16] = tmp_dword.dword;
      
      tmp_dword.bytes.b0 = WRITE_FLAG0_DEFAULT;
      tmp_dword.bytes.b1 = WRITE_FLAG1_DEFAULT;
      tmp_dword.bytes.b2 = WRITE_FLAG2_DEFAULT;
      tmp_dword.bytes.b3 = WRITE_FLAG3_DEFAULT;
      flash_write_buf[17] = tmp_dword.dword;
      
      st_FlashInit();
      st_FlashWrite(flash_write_buf ,FLASH_MSG_LEN);
      if(FAILED != st_FlashRead(FLASE_BASE_ADDR,flash_write_buf, FLASH_MSG_LEN))
      {
        return ;
      }
      
      delayms(10);
    }
  }
}

void ReFarctory(void)
{
  DWD_TypeDef tmp_dword;
  
  tmp_dword.bytes.b0 = MIN_PULSE_NUM0_DEFAULT;
  tmp_dword.bytes.b1 = MIN_PULSE_NUM1_DEFAULT;
  tmp_dword.bytes.b2 = MIN_PULSE_NUM2_DEFAULT;
  tmp_dword.bytes.b3 = MIN_PULSE_NUM3_DEFAULT;
  flash_write_buf[0] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = DRIVERSFRACTION0_DEFAULT;
  tmp_dword.bytes.b1 = DRIVERSFRACTION1_DEFAULT;
  tmp_dword.bytes.b2 = DRIVERSFRACTION2_DEFAULT;
  tmp_dword.bytes.b3 = DRIVERSFRACTION3_DEFAULT;      
  flash_write_buf[1] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = MOTOFRACTION0_DEFAULT;
  tmp_dword.bytes.b1 = MOTOFRACTION1_DEFAULT;
  tmp_dword.bytes.b2 = MOTOFRACTION2_DEFAULT;
  tmp_dword.bytes.b3 = MOTOFRACTION3_DEFAULT;        
  flash_write_buf[2] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = MOTOMAXSTART0_DEFAULT;
  tmp_dword.bytes.b1 = MOTOMAXSTART1_DEFAULT;
  tmp_dword.bytes.b2 = MOTOMAXSTART2_DEFAULT;
  tmp_dword.bytes.b3 = MOTOMAXSTART3_DEFAULT;    
  flash_write_buf[3] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = MOTOMAXSPEED0_DEFAULT;
  tmp_dword.bytes.b1 = MOTOMAXSPEED1_DEFAULT;
  tmp_dword.bytes.b2 = MOTOMAXSPEED2_DEFAULT;
  tmp_dword.bytes.b3 = MOTOMAXSPEED3_DEFAULT; 
  flash_write_buf[4] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = ACCELE_TIM0_DEFAULT;
  tmp_dword.bytes.b1 = ACCELE_TIM1_DEFAULT;
  tmp_dword.bytes.b2 = ACCELE_TIM2_DEFAULT;
  tmp_dword.bytes.b3 = ACCELE_TIM3_DEFAULT; 
  flash_write_buf[5] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = ACCELE_PERTIM0_DEFAULT;
  tmp_dword.bytes.b1 = ACCELE_PERTIM1_DEFAULT;
  tmp_dword.bytes.b2 = ACCELE_PERTIM2_DEFAULT;
  tmp_dword.bytes.b3 = ACCELE_PERTIM3_DEFAULT; 
  flash_write_buf[6] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = FRE_MULT_00_DEFAULT;
  tmp_dword.bytes.b1 = FRE_MULT_01_DEFAULT;
  tmp_dword.bytes.b2 = FRE_MULT_02_DEFAULT;
  tmp_dword.bytes.b3 = FRE_MULT_03_DEFAULT;
  flash_write_buf[7] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = FRE_MULT_10_DEFAULT;
  tmp_dword.bytes.b1 = FRE_MULT_11_DEFAULT;
  tmp_dword.bytes.b2 = FRE_MULT_12_DEFAULT;
  tmp_dword.bytes.b3 = FRE_MULT_13_DEFAULT;
  flash_write_buf[8] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = FRE_MULT_20_DEFAULT;
  tmp_dword.bytes.b1 = FRE_MULT_21_DEFAULT;
  tmp_dword.bytes.b2 = FRE_MULT_22_DEFAULT;
  tmp_dword.bytes.b3 = FRE_MULT_23_DEFAULT;
  flash_write_buf[9] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = FRE_MULT_30_DEFAULT;
  tmp_dword.bytes.b1 = FRE_MULT_31_DEFAULT;
  tmp_dword.bytes.b2 = FRE_MULT_32_DEFAULT;
  tmp_dword.bytes.b3 = FRE_MULT_33_DEFAULT;
  flash_write_buf[10] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = FRE_MULT_40_DEFAULT;
  tmp_dword.bytes.b1 = FRE_MULT_41_DEFAULT;
  tmp_dword.bytes.b2 = FRE_MULT_42_DEFAULT;
  tmp_dword.bytes.b3 = FRE_MULT_43_DEFAULT;
  flash_write_buf[11] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = FRE_MULT_50_DEFAULT;
  tmp_dword.bytes.b1 = FRE_MULT_51_DEFAULT;
  tmp_dword.bytes.b2 = FRE_MULT_52_DEFAULT;
  tmp_dword.bytes.b3 = FRE_MULT_53_DEFAULT;
  flash_write_buf[12] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = FRE_MULT_60_DEFAULT;
  tmp_dword.bytes.b1 = FRE_MULT_61_DEFAULT;
  tmp_dword.bytes.b2 = FRE_MULT_62_DEFAULT;
  tmp_dword.bytes.b3 = FRE_MULT_63_DEFAULT;
  flash_write_buf[13] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = FRE_MULT_70_DEFAULT;
  tmp_dword.bytes.b1 = FRE_MULT_71_DEFAULT;
  tmp_dword.bytes.b2 = FRE_MULT_72_DEFAULT;
  tmp_dword.bytes.b3 = FRE_MULT_73_DEFAULT;
  flash_write_buf[14] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = LOCAL_ADDR0_DEFAULT;
  tmp_dword.bytes.b1 = LOCAL_ADDR1_DEFAULT;
  tmp_dword.bytes.b2 = LOCAL_ADDR2_DEFAULT;
  tmp_dword.bytes.b3 = LOCAL_ADDR3_DEFAULT;
  flash_write_buf[15] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = PULSE_EQUIVA0_DEFAULT;
  tmp_dword.bytes.b1 = PULSE_EQUIVA1_DEFAULT;
  tmp_dword.bytes.b2 = PULSE_EQUIVA2_DEFAULT;
  tmp_dword.bytes.b3 = PULSE_EQUIVA3_DEFAULT;
  flash_write_buf[16] = tmp_dword.dword;
  
  tmp_dword.bytes.b0 = WRITE_FLAG0_DEFAULT;
  tmp_dword.bytes.b1 = WRITE_FLAG1_DEFAULT;
  tmp_dword.bytes.b2 = WRITE_FLAG2_DEFAULT;
  tmp_dword.bytes.b3 = WRITE_FLAG3_DEFAULT;
  flash_write_buf[17] = tmp_dword.dword;
  
  while(1)
  {
    st_FlashInit();
    st_FlashWrite(flash_write_buf ,FLASH_MSG_LEN);
    if(st_FlashRead(FLASE_BASE_ADDR,flash_write_buf, FLASH_MSG_LEN))
    {
      return ;
    }
    
    delayms(10);
  }
}

uint8_t WriteMsgToFlash(uint32_t* buffer,uint8_t buffer_size,uint8_t block_name)
{
  if(buffer_size > FLASH_MSG_LEN) return 0;
  
  if(st_FlashRead(FLASE_BASE_ADDR, flash_write_buf, FLASH_MSG_LEN))
  {
    for(uint8_t i=0;i<buffer_size;i++)
    {
      flash_write_buf[block_name+i] = buffer[i];
    }
  }
  st_FlashInit();
  st_FlashWrite(flash_write_buf , FLASH_MSG_LEN);
  st_FlashRead(FLASE_BASE_ADDR,flash_write_buf  , FLASH_MSG_LEN);

  return 1;
}

uint8_t ReadMsgFromFlash(uint32_t* buffer,uint8_t buffer_size,uint8_t block_name)
{
  
  for(uint8_t i=0;i<10;i++)
  {
    delayms(3);
    if(flash_write_buf[FLASH_MSG_LEN-1] != FLAG_VALUE)
    {
      FlashMsgInit();
      for(uint8_t i=0;i<buffer_size;i++)
      {
        buffer[i] = flash_write_buf[block_name+i];
      }
      
      return 1;
    }
    else
    {
      st_FlashRead(FLASE_BASE_ADDR,flash_write_buf  , FLASH_MSG_LEN);
      for(uint8_t i=0;i<buffer_size;i++)
      {
        buffer[i] = flash_write_buf[block_name+i];
      }
      return 1;
    }    
  }

  ReFarctory();
  for(uint8_t i=0;i<buffer_size;i++)
  {
    buffer[i] = flash_write_buf[block_name+i];
  }
  
  return 1;   
}
