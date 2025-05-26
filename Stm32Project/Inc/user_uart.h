#ifndef USER_UART_H_
#define USER_UART_H_
#include <stdio.h>


/* USART registers */
typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t CR3;
  volatile uint32_t BRR;
  volatile uint32_t GTPR;
  volatile uint32_t RTOR;
  volatile uint32_t RQR;
  volatile uint32_t ISR;
  volatile uint32_t ICR;
  volatile uint32_t RDR;
  volatile uint32_t TDR;
  volatile uint32_t PRESC;

} USART_TypeDef;

#define USART1_BASE	0x40013800UL//		USART 1 base address


#define USART1	(( USART_TypeDef *)USART1_BASE )

void USER_USART1_Init( void );
int _write(int file, char *ptr, int len); // to change printf function


#endif /* UART_H_ */
