#ifndef SYSTICKLIB_H_
#define SYSTICKLIB_H_

/* System Timer registers */
typedef struct
{
	volatile uint32_t CSR;
	volatile uint32_t RVR;
	volatile uint32_t CVR;
	volatile uint32_t CALIB;
} Systick_TypeDef;

/* Nested Vectored Interrupt Controller (NVIC) registers */
typedef struct
{
	volatile uint32_t ISER;
	volatile uint32_t RESERVED0[31];
	volatile uint32_t ICER;
	volatile uint32_t RESERVED1[31];
	volatile uint32_t ISPR;
	volatile uint32_t RESERVED2[31];
	volatile uint32_t ICPR;
	volatile uint32_t RESERVED3[95];
	volatile uint32_t IPR[8];
} NVIC_TypeDef;

#define Systick_BASE	0xE000E010UL//	Systick base address
#define NVIC_BASE			0xE000E100UL//	NVIC base address

#define Systick    	  (( Systick_TypeDef * )Systick_BASE )
#define NVIC    			(( NVIC_TypeDef * )NVIC_BASE )

void USER_SysTick_Init( void );
void SysTick_Delay( uint32_t time );

#endif /* USER_CORE_CM0PLUS_H_ */
