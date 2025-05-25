#include <stdint.h>
#include "main.h"
#include "systicklib.h"
#include "exti_func.h"

void USER_EXTI1_Init( void ){
  EXTI->IMR1    |=  (  0x1UL <<  25U );//  Enable interrupt
  EXTI->EMR1    &=  ~(  0x1UL <<  25U );//  Disable event generation
  /* Enable EXTI1 vector */

  NVIC->IPR[6]  &= ~( 0x00UL <<  3U );

  NVIC->ISER    |=  (  0x1UL <<  16U );
  NVIC->ISER    |=  (  0x1UL <<  27U );

}
