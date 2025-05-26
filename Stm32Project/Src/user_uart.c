#include <stdint.h>
#include "main.h"
#include "user_uart.h"


void USER_UART1_Init( void ){ //uart initialization function
  /* STEP 0. Enable the clock peripheral for the USART1 */
	RCC->IOPENR = RCC->IOPENR  | (0x1UL << 0U);
	RCC->APBENR2 = RCC->APBENR2 | (0x1UL << 14U);
  /* STEP 0. Configure the TX pin (PA9) as Alternate Function Push-Pull */
	GPIOA->AFRH = GPIOA->AFRH & ~(0xEUL << 4U);
	GPIOA->AFRH = GPIOA->AFRH | (0x1UL << 4U);
	GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 18U);
	GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL << 9U);
	GPIOA->MODER = GPIOA->MODER & ~(0x1UL << 18U);
	GPIOA->MODER = GPIOA->MODER | (0x2UL << 18U);

  /* STEP 0.1 Configure the Rx pin (PA10) as Alternate Function Push-Pull */
	//MODE 10 OTYPE 0 PUPDR 00 Set as alternate function
	GPIOA->AFRH = GPIOA->AFRH & ~(0xEUL << 8U);
	GPIOA->AFRH = GPIOA->AFRH | (0x1UL << 8U);
	GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 20U);
	GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL << 10U);
	GPIOA->MODER = GPIOA->MODER & ~(0x1UL << 20U);
	GPIOA->MODER = GPIOA->MODER | (0x2UL << 20U);



  /* STEP 1. Program the M bits in USART_CR1 to define the word length (8 bits) */
	USART1->CR1 = USART1->CR1 & ~(0x1UL << 28U);
	USART1->CR1 = USART1->CR1 & ~(0x1UL << 12U);
  /* STEP 2. Select the desired baud rate using the USART_BRR register */
	USART1->BRR = 5000;
  /* STEP 3. Program the number of STOP bits in USART_CR2 (1 stop bit) */
	USART1->CR2 = USART1->CR2 & ~(0x3UL << 12U);

  /* STEP 4. Enable the USART by writting the UE bit in USART_CR1 register */
	USART1->CR1 = USART1->CR1 | (0x1UL << 0U);


  /* STEP 6. Set the TE bit in USART_CR1 to send and idle frame as first transmission */
	USART1->CR1 = USART1->CR1 | (0x1UL << 3U);

	/* STEP 7 Set the RE bit in USART_CR1 to receive */
	USART1->CR1 = USART1->CR1 | (0x1UL << 2U);

	USART1->CR1 = USART1->CR1 | (0x1UL << 5U);


}


//PRINTF FUNCTION EDITED
////////////////////////////////////////////////////////////////////////////////////////
int _write(int file, char *ptr, int len){
	int DataIdx;
	 for(DataIdx=0; DataIdx<len; DataIdx++){
		while(!( USART1->ISR & (0x1UL << 7U)));
		 	 USART1->TDR = *ptr++;
	 }
	return len;
}
///////////////////////////////////////////////////////////////////////////////////////


