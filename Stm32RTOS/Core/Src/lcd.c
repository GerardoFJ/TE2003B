#include <stdint.h>
#include "main.h"
#include "lcd.h"
#include "user_tim.h"

//Caracter definido por usuario para cargar en la memoria CGRAM del LCD
const int8_t UserFont[8][8] =
{
		{ 0x11, 0x0A, 0x04, 0x1B, 0x11, 0x11, 0x11, 0x0E },
		{ 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10 },
		{ 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18 },
		{ 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C },
		{ 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E },
		{ 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

//Funcion que inicializa el LCD a 4 bits
void LCD_Init(void){
	int8_t const *p;
/**
  * Habilitamos las señales de reloj para los puertos I/O
	*/
	RCC->IOPENR		|=	( 0x1UL <<  1U );

/**
  * Configuración de los pines de control (RS, RW, EN) como GP output PP
	*/
	GPIOB->PUPDR  &= ~( 0x3UL << 18U );
	GPIOB->OTYPER &= ~( 0x1UL <<  9U );
	GPIOB->MODER  &= ~( 0x2UL << 18U );
	GPIOB->MODER  |=  ( 0x1UL << 18U );

	GPIOB->PUPDR  &= ~( 0x3UL << 20U );
	GPIOB->OTYPER &= ~( 0x1UL << 10U );
	GPIOB->MODER  &= ~( 0x2UL << 20U );
	GPIOB->MODER  |=  ( 0x1UL << 20U );

	GPIOB->PUPDR  &= ~( 0x3UL << 22U );
	GPIOB->OTYPER &= ~( 0x1UL << 11U );
	GPIOB->MODER  &= ~( 0x2UL << 22U );
	GPIOB->MODER  |=  ( 0x1UL << 22U );

/**
  * Configuración de los pines de datos (D4-D7) como GP output PP
	*/
	GPIOB->PUPDR  &= ~( 0x3UL << 24U );
	GPIOB->OTYPER &= ~( 0x1UL << 12U );
	GPIOB->MODER  &= ~( 0x2UL << 24U );
	GPIOB->MODER  |=  ( 0x1UL << 24U );

	GPIOB->PUPDR  &= ~( 0x3UL << 26U );
	GPIOB->OTYPER &= ~( 0x1UL << 13U );
	GPIOB->MODER  &= ~( 0x2UL << 26U );
	GPIOB->MODER  |=  ( 0x1UL << 26U );

	GPIOB->PUPDR  &= ~( 0x3UL << 28U );
	GPIOB->OTYPER &= ~( 0x1UL << 14U );
	GPIOB->MODER  &= ~( 0x2UL << 28U );
	GPIOB->MODER  |=  ( 0x1UL << 28U );

	GPIOB->PUPDR  &= ~( 0x3UL << 30U );
	GPIOB->OTYPER &= ~( 0x1UL << 15U );
	GPIOB->MODER  &= ~( 0x2UL << 30U );
	GPIOB->MODER  |=  ( 0x1UL << 30U );

/**
  * Inicialización del LCD
  * https://web.alfredstate.edu/faculty/weimandn/lcd/lcd_initialization/lcd_initialization_index.html
  * Power ON
  */
	GPIOB->BSRR	 =	 LCD_RS_PIN_LOW;
	GPIOB->BSRR	 =	 LCD_RW_PIN_LOW;
	GPIOB->BSRR	 =	 LCD_EN_PIN_LOW;
	GPIOB->BSRR	 =	 LCD_D4_PIN_LOW;
	GPIOB->BSRR	 =	 LCD_D5_PIN_LOW;
	GPIOB->BSRR	 =	 LCD_D6_PIN_LOW;
	GPIOB->BSRR	 =	 LCD_D7_PIN_LOW;
	USER_TIM14_Delay(50);

	/* Special case of 'Function Set'	*/
	GPIOB->BSRR	 =	 LCD_D4_PIN_HIGH;
	GPIOB->BSRR	 =	 LCD_D5_PIN_HIGH;
	GPIOB->BSRR	 =	 LCD_D6_PIN_LOW;
	GPIOB->BSRR	 =	 LCD_D7_PIN_LOW;
	LCD_Pulse_EN( );
	USER_TIM14_Delay(50);

	/* Special case of 'Function Set' */
	GPIOB->BSRR	 =	 LCD_D4_PIN_HIGH;
	GPIOB->BSRR	 =	 LCD_D5_PIN_HIGH;
	GPIOB->BSRR	 =	 LCD_D6_PIN_LOW;
	GPIOB->BSRR	 =	 LCD_D7_PIN_LOW;
	LCD_Pulse_EN( );
	USER_TIM14_Delay(50);

	/* Special case of 'Function Set' */
	GPIOB->BSRR	 =	 LCD_D4_PIN_HIGH;
	GPIOB->BSRR	 =	 LCD_D5_PIN_HIGH;
	GPIOB->BSRR	 =	 LCD_D6_PIN_LOW;
	GPIOB->BSRR	 =	 LCD_D7_PIN_LOW;
	LCD_Pulse_EN( );
	while( LCD_Busy( ) );

	/* Initial 'Function Set' to change 4-bit mode	*/
	GPIOB->BSRR	 =	 LCD_D4_PIN_LOW;
	GPIOB->BSRR	 =	 LCD_D5_PIN_HIGH;
	GPIOB->BSRR	 =	 LCD_D6_PIN_LOW;
	GPIOB->BSRR	 =	 LCD_D7_PIN_LOW;
	LCD_Pulse_EN( );
	while( LCD_Busy( ) );
	/* 'Function Set' (I=1, N and F as required)	*/
	LCD_Write_Cmd( 0x28U );
	/* 'Display ON/OFF Control' (D=1, C=0, B=0)	*/
	LCD_Write_Cmd( 0x0CU );
	/* 'Clear Display'	*/
	LCD_Write_Cmd( 0x01U );//
	/* 'Entry Mode Set' (I/D and S as required)	*/
	LCD_Write_Cmd( 0x06U );
	/* Initialization Ends	*/
//	LCD_Write_Cmd( 0x0FU );

	//Cargamos el caracter definido por el usuario en la CGRAM
	LCD_Write_Cmd( 0x40 );
	p = &UserFont[0][0];

	for( int i = 0; i < sizeof( UserFont ); i++, p++ )
		LCD_Put_Char( *p );

	/*	Set DDRAM address in address			*/
	LCD_Write_Cmd( 0x80 );//
}

//Funcion que genera un strobe en el LCD
void LCD_Out_Data4(uint8_t val){
	if( ( val & 0x01U ) == 0x01U )
		GPIOB->BSRR	=	LCD_D4_PIN_HIGH;
	else
		GPIOB->BSRR	=	LCD_D4_PIN_LOW;

	if( ( val & 0x02U ) == 0x02U )
		GPIOB->BSRR	=	LCD_D5_PIN_HIGH;
	else
		GPIOB->BSRR	=	LCD_D5_PIN_LOW;

	if( ( val & 0x04U ) == 0x04U )
		GPIOB->BSRR	=	LCD_D6_PIN_HIGH;
	else
		GPIOB->BSRR	=	LCD_D6_PIN_LOW;

	if( ( val & 0x08U ) == 0x08U )
		GPIOB->BSRR	=	LCD_D7_PIN_HIGH;
	else
		GPIOB->BSRR	=	LCD_D7_PIN_LOW;
}

//Funcion que escribe 1 byte de datos en el LCD
void LCD_Write_Byte(uint8_t val){
	LCD_Out_Data4( ( val >> 4 ) & 0x0FU );
	LCD_Pulse_EN( );
	LCD_Out_Data4( val & 0x0FU );
	LCD_Pulse_EN( );
	while( LCD_Busy( ) );
}

//Funcion que escribe un comando en el LCD
void LCD_Write_Cmd(uint8_t val){
	GPIOB->BSRR	=	LCD_RS_PIN_LOW;
	LCD_Write_Byte( val );
}

//Escribe un caracter ASCII en el LCD
void LCD_Put_Char(uint8_t c){
	GPIOB->BSRR	=	LCD_RS_PIN_HIGH;
	LCD_Write_Byte( c );
}

//Funcion que establece el cursor en una posicion de la pantalla del LCD
//Minimum values for line and column must be 1
void LCD_Set_Cursor(uint8_t line, uint8_t column){
	uint8_t address;
	column--;
	line--;
	address = ( line * 0x40U ) + column;
	address = 0x80U + ( address & 0x7FU );
	LCD_Write_Cmd( address );
}

//Funcion que envia una cadena de caracteres ASCII al LCD
void LCD_Put_Str(char * str){
	for( int16_t i = 0; i < 16 && str[ i ] != 0; i++ )
		LCD_Put_Char( str[ i ] );
}

//Funcion que envia un caracter numerico al LCD
//El número debe ser entero y de 5 dígitos máximo
void LCD_Put_Num(int16_t num){
	int16_t p;
	int16_t f = 0;
	int8_t ch[ 5 ];

	for( int16_t i = 0; i < 5; i++ ){
		p = 1;
		for( int16_t j = 4 - i; j > 0; j-- )
			p = p * 10;
		ch[ i ] = ( num / p );
		if( num >= p && !f )
			f = 1;
		num = num - ch[ i ] * p;
		ch[ i ] = ch[ i ] + 48;
		if( f )
			LCD_Put_Char( ch[ i ] );
	}
}

//Funcion que provoca tiempos de espera en el LCD
char LCD_Busy(void){
/**
  * Configuracion de D7 as input floating
  */
	GPIOB->PUPDR &= ~( 0x3UL << 30U );
  GPIOB->MODER &= ~( 0x3UL << 30U );
	GPIOB->BSRR	  =	 LCD_RS_PIN_LOW;
	GPIOB->BSRR	  =	 LCD_RW_PIN_HIGH;
	GPIOB->BSRR	  =	 LCD_EN_PIN_HIGH;
	USER_TIM14_Delay(1);
	if(( GPIOB->IDR	& LCD_D7_PIN_HIGH )) {
		GPIOB->BSRR	=  LCD_EN_PIN_LOW;
		GPIOB->BSRR	=	 LCD_RW_PIN_LOW;
/**
  * Configuracion de D7 as output push-pull
  */
		GPIOB->PUPDR  &= ~( 0x3UL << 30U );
  	GPIOB->OTYPER &= ~( 0x1UL << 15U );
  	GPIOB->MODER  &= ~( 0x2UL << 30U );
  	GPIOB->MODER  |=  ( 0x1UL << 30U );
		return 1;
	} else {
		GPIOB->BSRR	=  LCD_EN_PIN_LOW;
		GPIOB->BSRR	=	 LCD_RW_PIN_LOW;
/**
  * Configuracion de D7 as output push-pull
  */
		GPIOB->PUPDR  &= ~( 0x3UL << 30U );
  	GPIOB->OTYPER &= ~( 0x1UL << 15U );
  	GPIOB->MODER  &= ~( 0x2UL << 30U );
  	GPIOB->MODER  |=  ( 0x1UL << 30U );
		return 0;
	}
}

//Funcion que genera un pulso en el pin EN del LCD
void LCD_Pulse_EN(void){
	GPIOB->BSRR	=	LCD_EN_PIN_LOW;//
	USER_TIM14_Delay(1);
	GPIOB->BSRR	=	LCD_EN_PIN_HIGH;
	USER_TIM14_Delay(1);
	GPIOB->BSRR	=	LCD_EN_PIN_LOW;
	USER_TIM14_Delay(1);
}

/*
 * Funcion que muestra un caracter grafico en el LCD
 * en 'value' el valor de su posicion en CGRAM y
 * en 'size' especificamos su tamaño
 */
void LCD_BarGraphic(int16_t value, int16_t size){
	value = value * size / 20;//
	for( int16_t i = 0; i < size; i++ ){
		if( value > 5 ){
			LCD_Put_Char( 0x05U );
			value -= 5;
		} else {
			LCD_Put_Char( value );
			break;
		}
	}
}

/*
 * Funcion que muestra un caracter grafico en el LCD especificando
 * la posicion pos_x horizontal de inicio y la posicion pos_y vertical de la pantalla LCD
 */
void LCD_BarGraphicXY(int16_t pos_x, int16_t pos_y, int16_t value){
	LCD_Set_Cursor( pos_x, pos_y );
	for( int16_t i = 0; i < 16; i++ ){
		if( value > 5 ){
			LCD_Put_Char( 0x05U );
			value -= 5;
		} else {
			LCD_Put_Char( value );
			while( i++ < 16 )
				LCD_Put_Char( 0 );
		}
	}
}
