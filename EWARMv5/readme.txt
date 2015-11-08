/**
  @page ewarmv5 EWARMv5 Project Template
  
  @verbatim
 ******************** (C) COPYRIGHT 2009 STMicroelectronics ********************
 * @file      readme.txt
 * @author    MCD Application Team
 * @version   V3.0.0
 * @date      04/06/2009
 * @brief     This sub-directory contains all the user-modifiable files 
 *            needed to create a new project linked with the STM32F10x  
 *            Standard Peripheral Library and working with IAR Embedded 
 *            Workbench for ARM (EWARM) software toolchain (version 5.30 
 *            and later).
 ********************************************************************************
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
 * AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
 * CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
 * INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *******************************************************************************
 @endverbatim
 
 @par Directory contents

 - project .ewd/.eww/.ewp: A pre-configured project file with the provided library 
                           structure that produces an executable image with IAR 
                           Embedded Workbench.
                
 - stm32f10x_flash.icf : This file is the IAR Linker configuration file used to 
                         place program code (readonly) in internal FLASH and data
                         (readwrite, Stack and Heap)in internal SRAM. 
                         You can customize this file to your need.                    
                      
 - stm32f10x_flash_extsram.icf: This file is the IAR Linker configuration file 
                                used to place program code (readonly) in internal 
                                FLASH and data (readwrite, Stack and Heap)in 
                                external SRAM. 
                                You can customize this file to your need.  

 - stm32f10x_nor.icf:  This file is the IAR Linker configuration file used to 
                       place program code (readonly) in external NOR FLASH and data
                       (readwrite, Stack and Heap)in internal SRAM. 
                       You can customize this file to your need.                                            

 - stm32f10x_ram.icf:  This file is the IAR Linker configuration file used to 
                       place program code (readonly) and data (readwrite, Stack 
                       and Heap)in internal SRAM. 
                       You can customize this file to your need. .                      
                             
 @par How to use it ?

 - Open the Project.eww workspace.
 - In the workspace toolbar select the project config:
     - STM3210B-EVAL: to configure the project for STM32 Medium-density devices
     - STM3210E-EVAL: to configure the project for STM32 High-density devices
 - Rebuild all files: Project->Rebuild all
 - Load project image: Project->Debug
 - Run program: Debug->Go(F5)

@note
 - Low-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 16 and 32 Kbytes.
 - Medium-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 32 and 128 Kbytes.
 - High-density devices are STM32F101xx and STM32F103xx microcontrollers where
   the Flash memory density ranges between 256 and 512 Kbytes.    

 * <h3><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h3>
 */