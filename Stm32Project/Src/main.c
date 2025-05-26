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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>     /* strtod */

#define BUFFER_SIZE 10

char buffer_str[10];
char buffer_vel[10];
//char test_buffer[] = "112.561";
float velocity = 0.0;
int index_k = 0;

uint8_t button_status = 0;
  uint16_t val;

void USART1_IRQHandler( void ) {
	if((USART1->ISR & (0x1UL << 5U))){ // wait until a data is received (ISR register)
		char received =  USART1->RDR;
		if(received == 'V'){
			velocity = atof(buffer_str);
			memset(buffer_str, 0, sizeof(buffer_str));
			index_k = 0;
			}
			else{
				if (index_k < BUFFER_SIZE - 1) {
				            buffer_str[index_k++] = received;
				        }else{
				        	memset(buffer_str, 0, sizeof(buffer_str));
				        				index_k = 0;
				        }
			}
}
}

//void TIM3_IRQHandler( void ) {
//	if(TIM3->SR & (0x1UL<<0U)){
//		val = USER_ADC_Read();
//			  if(GPIOA->IDR & (0x1UL << 7U)){
//				  button_status = 1;
//			  }
//			  else{
//				  button_status = 0;
//			  }
//		LCD_Set_Cursor( 1, 1 );
//		snprintf(buffer_vel, sizeof(buffer_vel), "%f", velocity);
//		LCD_Put_Str( buffer_vel );
//		printf("{adc: %u, button: %u}\n", val, button_status);
////		printf("test %.4f \r\n",velocity);
//		TIM3->SR &= ~(0x1UL << 0U);
//	}
//}
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
  USER_EXTI1_Init( );

  LCD_Clear( );

  /* Repetitive block */
  for(;;){
	  val = USER_ADC_Read();
	  			  if(GPIOA->IDR & (0x1UL << 7U)){
	  				  button_status = 1;
	  			  }
	  			  else{
	  				  button_status = 0;
	  			  }
	  		LCD_Set_Cursor( 1, 1 );
	  		snprintf(buffer_vel, sizeof(buffer_vel), "%f", velocity);
	  		LCD_Put_Str( buffer_vel );
	  		SysTick_Delay( 100 );
	  		printf("{adc: %u, button: %u}\n", val, button_status);

//	  LCD_Set_Cursor( 1, 1 );
//	  LCD_Put_Str( "Vel: " );
//	  LCD_Set_Cursor( 1, 6 );
//	  LCD_Put_Str( "    " );
//	  LCD_Set_Cursor( 1, 6 );
//      LCD_Put_Str( buffer_str );
////	  LCD_Set_Cursor( 2, 1 );
////	  LCD_Put_Str( "Button: " );
////	  LCD_Set_Cursor( 2, 9 );
////	  LCD_Put_Str( " " );
////	  LCD_Set_Cursor( 2, 9 );
////	  LCD_Put_Num( button_status );

//	  SysTick_Delay( 100 );
//	  GPIOA->ODR ^= (0x1UL<< 5U);
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
	  GPIOA->MODER &= ~(0x3UL << 14U);
	  GPIOA->PUPDR &= ~(0x1UL << 14U);
	  GPIOA->PUPDR |= (0x2UL << 14U);


}

