#include <stdint.h>
#include "main.h"
#include "user_tim.h"

void USER_TIM3_Init( void ){
	RCC->APBENR1	|=  ( 0x1UL <<  1U );//		Enable TIM3 clock source
	TIM3->SMCR		&= ~( 0x1UL << 16U )
					&  ~( 0x7UL <<  0U );// 	Prescaler is clocked directed by the internal clock
	TIM3->CR1		&= ~( 0x1UL <<  7U ) //		Auto-reload register is not buffered
					&  ~( 0x3UL <<  5U ) //		Selects edge-aligned mode
					&  ~( 0x1UL <<  4U ) //		Counter used as upcounter
					&  ~( 0x1UL <<  1U );//		Update Event (UEV) enabled
}

void USER_TIM3_Delay( uint16_t prescaler, uint16_t maxCount ){
	/* STEP 3. Configure the prescaler and the maximum count */
	TIM3->PSC = prescaler;
	TIM3->ARR = maxCount;
	/* STEP 4. Clear the Timer Update Interrupt Flag */
	TIM3->SR &= ~( 0x1UL << 0U );
	/* STEP 5. Enable the Timer to start counting */
	TIM3->CR1 |= ( 0x1UL << 0U );
	/* STEP 6. Wait for the Overflow */
	while(!(TIM3->SR & ( 0x1UL << 0U )));
	/* STEP 7. Disable the Timer to stop counting */
	TIM3->CR1 &= ~( 0x1UL << 0U );
}
