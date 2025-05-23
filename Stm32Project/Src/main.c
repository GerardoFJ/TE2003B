/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    This code is for testing timers
   ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/*
 * Gerardo Fregoso Jimenez
 *
 */

/* **************** START *********************** */
/* Libraries, Definitions and Global Declarations */
#include <stdint.h>
#include "main.h"
#include "user_tim.h"
#include "user_uart.h"
#include "lcd.h"
#include "systicklib.h"
#include "adclib.h"

/* Superloop structure */
int main(void)
{
	/* Declarations and Initializations */
  USER_RCC_Init();
//  USER_TIM3_Init();
  USER_SysTick_Init( );
  USER_UART1_Init();
  USER_GPIO_Init();
  LCD_Init( );
  USER_ADC_Init( );
  uint8_t button_status = 0;

  uint16_t val = 0;
  LCD_Clear( );
  /* Repetitive block */
  for(;;){
	  LCD_Clear( );
	  val = USER_ADC_Read();
	  button_status = USER_UART1_Receive_8bit(); // checar el valor que llega
	  if(GPIOA->IDR & (0x1UL << 9U)){
		  button_status = 1;
	  }
	  else{
		  button_status = 0;
	  }

	  LCD_Set_Cursor( 1, 1 );
	  LCD_Put_Str( "ADC: " );
	  LCD_Put_Num( val );
	  LCD_Set_Cursor( 2, 1 );
	  LCD_Put_Str( "Button: " );
	  LCD_Put_Num( button_status );
//	  LCD_BarGraphic( 0, 64 );

	  SysTick_Delay( 100 );
	  GPIOA->ODR ^= (0x1UL<< 5U);

  }
}

void USER_RCC_Init( void ){
	//set gpio output
	RCC->IOPENR = RCC->IOPENR  | (0x1UL << 0U);
	/* System Clock (SYSCLK) configuration for 48 MHz */
	FLASH->ACR	&= ~( 0x6UL <<  0U );// 2 HCLK cycles latency, if SYSCLK >=24MHz <=48MHz
	FLASH->ACR	|=  ( 0x1UL <<  0U );// 2 HCLK cycles latency, if SYSCLK >=24MHz <=48MHz
	while(( FLASH->ACR & ( 0x7UL <<  0U )) != 0x001UL );// wait until LATENCY[2:0]=001
	RCC->CR		&= ~( 0x7UL << 11U );// select HSISYS division factor by 1
	while(!( RCC->CR & ( 0x1UL << 10U )));// wait until HSISYS is stable and ready
	RCC->CFGR	&= ~( 0x7UL <<  0U );// select HSISYS as the SYSCLK clock source
	RCC->CFGR	&= ~( 0x1UL << 11U );// select HCLK division factor by 1
}

void USER_GPIO_Init(void){
  //SET 4 BIT LEDS AS OUTPUT
  //PINA5 AS OUTPUT
	  GPIOA->BSRR   =	0x1UL << 21U; // Reset PA5 low to turn off LED
	  GPIOA->PUPDR  = GPIOA->PUPDR  & ~( 0x3UL << 10U ); // Clear pull-up/pull-down bits for PA5
	  GPIOA->OTYPER = GPIOA->OTYPER & ~( 0x1UL <<  5U ); // Clear output type bit for PA5
	  GPIOA->MODER  = GPIOA->MODER  & ~( 0x2UL << 10U ); // Set PA5 as output
	  GPIOA->MODER  = GPIOA->MODER  |  ( 0x1UL << 10U ); // Set PA5 as output
	  //PINA9 AS INPUT PULL DOWN
	  GPIOA->MODER &= ~(0x3UL << 18U);
	  GPIOA->PUPDR &= ~(0x1UL << 18U);
	  GPIOA->PUPDR |= (0x2UL << 18U);


}

