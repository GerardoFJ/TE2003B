#include <stdint.h>
#include "main.h"
#include "systicklib.h"
#include "exti_func.h"

void USER_EXTI1_Init( void ){
  /* Configure PA1 as input pull up */
	GPIOA->PUPDR   = GPIOA->PUPDR  & ~( 0x2UL <<  2U );
  GPIOA->PUPDR   = GPIOA->PUPDR  |  ( 0x1UL <<  2U );
	GPIOA->MODER   = GPIOA->MODER  & ~( 0x3UL <<  2U );

  /* Configure EXTI1 for Port A */
  EXTI->EXTICR1 &= ~( 0xFFUL <<  8U );

  /* Configure EXTI1 for falling edge detection */
  EXTI->RTSR1   &= ~(  0x1UL <<  1U );//  Disable rising trigger event
  EXTI->FTSR1   |=  (  0x1UL <<  1U );//  Enable falling trigger event

  /* Unmask EXTI1 line */
  EXTI->EMR1    &= ~(  0x1UL <<  1U );//  Disable event generation
  EXTI->IMR1    |=  (  0x1UL <<  1U );//  Enable interrupt

  /* Define EXTI1 Priority */
  NVIC->IPR[1]  &= ~( 0x00UL <<  4U );//  High priority

  /* Enable EXTI1 vector */
  NVIC->ISER    |=  (  0x1UL <<  5U );
}
