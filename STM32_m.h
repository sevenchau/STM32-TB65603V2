/********************************************************************/

#define uchar		u8
#define uint		u16
#define ulong		u32

#define INT8U   	u8
#define INT8S  	 	s8
#define INT16U  	u16
#define INT16S  	s16
#define INT32U  	u32
#define INT32S  	s32
#define FP32    	float
#define FP64   	 	double

/********************************************************************/
											/*以下的bit0取值位BIT0-15*/
#define clrbit(reg,bit)   reg &= ~(bit)	/*清寄存器的某1比特位*/
#define bitclr(reg,bit)   reg &= ~(bit)	/*清寄存器的某1比特位*/

#define setbit(reg,bit)   reg |=  (bit)	/*设置寄存器的某1比特位*/
#define bitset(reg,bit)   reg |=  (bit)	/*设置寄存器的某1比特位*/
#define cplbit(reg,bit)   reg ^=  (bit)	/*对寄存器的某1比特位取反*/
#define bitcpl(reg,bit)   reg ^=  (bit)	/*对寄存器的某1比特位取反*/

#define  testbit(reg,bit) (reg&(bit))
#define  bittest(reg,bit) (reg&(bit))

#define regcpl(reg)		 reg = ~reg
#define regclr(reg)		 reg = 0

#define bit 	u8


//*************************************************************************
//*************************************************************************
//*************************************************************************

