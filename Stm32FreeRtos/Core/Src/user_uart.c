#include "main.h"
#include "user_uart.h"



void USER_UART1_Init(void) {
    // Activar reloj de GPIOA y USART1
    RCC->IOPENR  |= (0x1UL << 0U);
    RCC->APBENR2 |= (0x1UL << 14U);

    /* STEP 0. Configure the TX pin (PA9) as Alternate Function Push-Pull */
    GPIOA->AFR[1] &= ~((0xF << 4) | (0xF << 8));
    GPIOA->AFR[1] |=  (0x1 << 4) | (0x1 << 8);
    GPIOA->PUPDR &= ~((0x3 << 18) | (0x3 << 20));
    GPIOA->OTYPER &= ~((1 << 9) | (1 << 10));
    GPIOA->MODER &= ~((0x3 << 18) | (0x3 << 20));
    GPIOA->MODER |=  (0x2 << 18) | (0x2 << 20);

    // Configuración USART1: 8 bits, 1 stop bit, baudrate
    USART1->CR1 &= ~((1 << 28) | (1 << 12));
    USART1->CR2 &= ~(0x3 << 12);
    USART1->BRR  = 5000;  // Para 9600 baudios @ 48 MHz

    // Habilitar USART, transmisión y recepción
    USART1->CR1 |= (1 << 0) | (1 << 2) | (1 << 3) | (1 << 5);

    NVIC->ISER[0] = (1UL << 27); // interrupcion
}


void USER_UART2_Init( void ){
  RCC->IOPENR   |=  ( 0x1UL <<  0U ); // GPIOA clock enabled
  RCC->APBENR1  |=  ( 0x1UL << 17U ); // USART2 clock enabled
  GPIOA->AFR[0]   &= ~( 0xEUL <<  8U );
  GPIOA->AFR[0]   |=  ( 0x1UL <<  8U ); // Select the AF1 for the PA2
  GPIOA->PUPDR  &= ~( 0x3UL <<  4U ); // Clear pull-up/pull-down bits for PA2
  GPIOA->OTYPER &= ~( 0x1UL <<  2U ); // Clear output type bit for PA2
  GPIOA->MODER  &= ~( 0x1UL <<  4U );
  GPIOA->MODER  |=  ( 0x2UL <<  4U ); // Set PA2 as AF
  GPIOA->AFR[0]   &= ~( 0xEUL << 12U );
  GPIOA->AFR[0]   |=  ( 0x1UL << 12U ); // Select the AF1 for the PA3
  GPIOA->PUPDR  &= ~( 0x3UL <<  6U ); // Clear pull-up/pull-down bits for PA3
  GPIOA->OTYPER &= ~( 0x1UL <<  3U ); // Clear output type bit for PA3
  GPIOA->MODER  &= ~( 0x1UL <<  6U );
  GPIOA->MODER  |=  ( 0x2UL <<  6U ); // Set PA3 as AF
  USART2->CR1   &= ~( 0x1UL << 28U ); // 8-bit word length
  USART2->CR1   &= ~( 0x1UL << 12U ); // 8-bit word length
  USART2->BRR   =   ( 48000000 / 115200 ); // Desired baud rate
  USART2->CR2   &= ~( 0x3UL << 12U ); // 1 stop bit
  USART2->CR1   |=  ( 0x1UL <<  0U ); // USART is enabled
  USART2->CR1   |=  ( 0x1UL <<  3U ); // Transmitter is enabled
  USART2->CR1   |=  ( 0x1UL <<  2U ); // Receiver is enabled
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

static void USER_UART1_Send_8bit( uint8_t Data ){
	while(!( USART1->ISR & ( 0x1UL <<  7U)));//	wait until next data can be written
	USART1->TDR = Data;// Data to send
}

void USER_UART1_Transmit( uint8_t *pData, uint16_t size ){
	for( int i = 0; i < size; i++ ){
		USER_UART1_Send_8bit( *pData++ );
	}
}
