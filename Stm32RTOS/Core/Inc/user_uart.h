#ifndef USER_UART_H_
#define USER_UART_H_

void USER_UART2_Init( void );
void USER_UART1_Init( void );
void USER_UART2_Transmit( uint8_t *pData, uint16_t size );
int _write(int file, char *ptr, int len); // to change printf function


#endif /* USER_UART_H_ */
