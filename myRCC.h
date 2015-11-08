
//------------------------------------------------------------------------------
#if	(RCC_PLLMul==2)             // ok
	#define RCC_PLLMul_  RCC_PLLMul_2
#elif (RCC_PLLMul==4)
	#define RCC_PLLMul_  RCC_PLLMul_4
#elif (RCC_PLLMul==8)
	#define RCC_PLLMul_  RCC_PLLMul_8
#elif (RCC_PLLMul==9)
	#define RCC_PLLMul_  RCC_PLLMul_9
#elif (RCC_PLLMul==16)
	#define RCC_PLLMul_  RCC_PLLMul_16
#else
//#error	"error"
#endif

#define SYSCLK  (float)(FREQ_HSE*RCC_PLLMul)

//------------------------------------------------------------------------------
#if	(RCC_AHB_Div==1)    		 // ok
	#define RCC_HCLK_Div_ RCC_SYSCLK_Div1
#elif (RCC_AHB_Div==2)
	#define RCC_HCLK_Div_ RCC_SYSCLK_Div2
#elif (RCC_AHB_Div==4)
	#define RCC_HCLK_Div_ RCC_SYSCLK_Div4
#elif (RCC_AHB_Div==8)
	#define RCC_HCLK_Div_ RCC_SYSCLK_Div8
#elif (RCC_AHB_Div==16)
	#define RCC_HCLK_Div_ RCC_SYSCLK_Div16
#elif (RCC_AHB_Div==64)
	#define RCC_HCLK_Div_ RCC_SYSCLK_Div64
#elif (RCC_AHB_Div==128)
	#define RCC_HCLK_Div_ RCC_SYSCLK_Div128
#elif (RCC_AHB_Div==256)
	#define RCC_HCLK_Div_ RCC_SYSCLK_Div256
#elif (RCC_AHB_Div==512)
	#define RCC_HCLK_Div_ RCC_SYSCLK_Div512
#else
//	error
#endif

#define HCLK (float)(SYSCLK/RCC_AHB_Div)
#define FCLK (float)(HCLK)
//------------------------------------------------------------------------------

#if (RCC_APB1_Div==1)			 // ok
	#define RCC_PCLK1_Div_ RCC_HCLK_Div1
#elif (RCC_APB1_Div==2)
	#define RCC_PCLK1_Div_ RCC_HCLK_Div2
#elif (RCC_APB1_Div==4)
	#define RCC_PCLK1_Div_ RCC_HCLK_Div4
#elif (RCC_APB1_Div==8)
	#define RCC_PCLK1_Div_ RCC_HCLK_Div8
#elif (RCC_APB1_Div==16)
	#define RCC_PCLK1_Div_ RCC_HCLK_Div16
#else
//	error
#endif

#define PCLK1 (float)(HCLK/RCC_APB1_Div)


//------------------------------------------------------------------------------

#if (RCC_APB2_Div==1)			 // ok
	#define RCC_PCLK2_Div_ RCC_HCLK_Div1
#elif (RCC_APB2_Div==2)
	#define RCC_PCLK2_Div_ RCC_HCLK_Div2
#elif (RCC_APB2_Div==4)
	#define RCC_PCLK2_Div_ RCC_HCLK_Div4
#elif (RCC_APB2_Div==8)
	#define RCC_PCLK2_Div_ RCC_HCLK_Div8
#elif (RCC_APB2_Div==16)
	#define RCC_PCLK2_Div_ RCC_HCLK_Div16
#else
//	error
#endif

#define PCLK2 (float)(HCLK/RCC_APB2_Div)

//------------------------------------------------------------------------------

#if (RCC_ADC_DIV==2)
	#define RCC_ADC_DIV_ RCC_PCLK2_Div2
#elif (RCC_ADC_DIV==4)
	#define RCC_ADC_DIV_ RCC_PCLK2_Div4
#elif (RCC_ADC_DIV==6)
	#define RCC_ADC_DIV_ RCC_PCLK2_Div6
#elif (RCC_ADC_DIV==8)
	#define RCC_ADC_DIV_ RCC_PCLK2_Div8
#else
//	error
#endif

//------------------------------------------------------------------------------

#if (RCC_APB1_Div==1)
	#define FACTOR_TIM234567	1
#else
	#define FACTOR_TIM234567	2
#endif
#define TIM234567CLK 	(PCLK1*FACTOR_TIM234567) 
#define TIM2CLK 		TIM234567CLK
#define TIM3CLK 		TIM234567CLK
#define TIM4CLK 		TIM234567CLK
#define TIM5CLK 		TIM234567CLK
#define TIM6CLK 		TIM234567CLK
#define TIM7CLK 		TIM234567CLK

//------------------------------------------------------------------------------
#if (RCC_APB2_Div==1)
#define TIM1CLK 	(PCLK2)
#define TIM8CLK 	(PCLK2)
#else
#define TIM1CLK 	(PCLK2*2)   
#define TIM8CLK 	(PCLK2*2)   
#endif

//------------------------------------------------------------------------------
#define ADCCLK 		((float)PCLK2/RCC_ADC_DIV)  

#define ADCCLKx10 	(10*FREQ_HSE*RCC_PLLMul/(RCC_AHB_Div*RCC_APB2_Div*RCC_ADC_DIV))  // 仅仅用于下面的判断
#if (ADCCLKx10<6)//	error     		// adc的时钟最小为0.6MHz
#elif (ADCCLKx10>140)//	error     		// adc的时钟最大为14MHz
#else
					// 正常的adc频率范围
#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

