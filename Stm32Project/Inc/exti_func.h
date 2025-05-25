#ifndef EXTI_FUNC_H_
#define EXTI_FUNC_H_

/* Extendend Interrupt and Event Controller (EXTI) registers */
typedef struct
{
	volatile uint32_t RTSR1;
	volatile uint32_t FTSR1;
	volatile uint32_t SWIER1;
	volatile uint32_t RPR1;
  volatile uint32_t FPR1;
  volatile uint32_t RESERVED0[19];
	volatile uint32_t EXTICR1;
	volatile uint32_t EXTICR2;
	volatile uint32_t EXTICR3;
  volatile uint32_t EXTICR4;
	volatile uint32_t RESERVED1[4];
	volatile uint32_t IMR1;
	volatile uint32_t EMR1;
} EXTI_TypeDef;

#define EXTI_BASE 0x40021800UL//	EXTI base address
#define EXTI    	(( EXTI_TypeDef * )EXTI_BASE )

void USER_EXTI1_Init( void );

#endif /* USER_EXTI_H_ */
