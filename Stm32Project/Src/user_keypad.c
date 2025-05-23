#include <stdint.h>
#include "main.h"
#include "user_keypad.h"
#include "user_tim.h"
void USER_Keypad_Init( void );
uint8_t USER_Key( void );

void USER_Keypad_Init( void ){
  //Active clock for PB GPIOS
  RCC->IOPENR = RCC->IOPENR  | (0x1UL << 0U); // active PINA
  RCC->IOPENR = RCC->IOPENR  | (0x1UL << 1U); // active PINB
  // for outputs MODER 01 OTYPE 0 PUPDR 00
  //Setup B0 OUTPUT
  GPIOB->MODER = GPIOB->MODER & ~(0x2UL << 0U);
	GPIOB->MODER = GPIOB->MODER | (0x1UL << 0U);
	GPIOB->OTYPER = GPIOB->OTYPER & ~(0x1UL << 0U);
	GPIOB->PUPDR = GPIOB->PUPDR & ~(0x3UL << 0U);
	// Reset the PIN to be turn off by default
	GPIOB->ODR = GPIOB->ODR & ~(0x1UL << 0U);

  //Setup B1 OUTPUT
  GPIOB->MODER = GPIOB->MODER & ~(0x2UL << 2U);
	GPIOB->MODER = GPIOB->MODER | (0x1UL << 2U);
	GPIOB->OTYPER = GPIOB->OTYPER & ~(0x1UL << 1U);
	GPIOB->PUPDR = GPIOB->PUPDR & ~(0x3UL << 2U);
	// Reset the PIN to be turn off by default
	GPIOB->ODR = GPIOB->ODR & ~(0x1UL << 1U);

  //Setup B2 OUTPUT
  GPIOB->MODER = GPIOB->MODER & ~(0x2UL << 4U);
	GPIOB->MODER = GPIOB->MODER | (0x1UL << 4U);
	GPIOB->OTYPER = GPIOB->OTYPER & ~(0x1UL << 2U);
	GPIOB->PUPDR = GPIOB->PUPDR & ~(0x3UL << 4U);
	// Reset the PIN to be turn off by default
	GPIOB->ODR = GPIOB->ODR & ~(0x1UL << 2U);

  //Setup B3 OUTPUT
  GPIOB->MODER = GPIOB->MODER & ~(0x2UL << 6U);
	GPIOB->MODER = GPIOB->MODER | (0x1UL << 6U);
	GPIOB->OTYPER = GPIOB->OTYPER & ~(0x1UL << 3U);
	GPIOB->PUPDR = GPIOB->PUPDR & ~(0x3UL << 6U);
	// Reset the PIN to be turn off by default
	GPIOB->ODR = GPIOB->ODR & ~(0x1UL << 3U);

  //for inputs Moder 00 Pupdr 01
  //Setup B4 INPUT
  GPIOB->MODER = GPIOB->MODER & ~(0x3UL << 8U);
	GPIOB->PUPDR = GPIOB->PUPDR & ~(0x1UL << 8U);
  GPIOB->PUPDR = GPIOB->PUPDR | (0x2UL << 8U);

	//Setup B5 INPUT
  GPIOB->MODER = GPIOB->MODER & ~(0x3UL << 10U);
	GPIOB->PUPDR = GPIOB->PUPDR & ~(0x1UL << 10U);
  GPIOB->PUPDR = GPIOB->PUPDR | (0x2UL << 10U);

	//Setup B6 INPUT
  GPIOB->MODER = GPIOB->MODER & ~(0x3UL << 12U);
	GPIOB->PUPDR = GPIOB->PUPDR & ~(0x1UL << 12U);
  GPIOB->PUPDR = GPIOB->PUPDR | (0x2UL << 12U);

	//Setup B7 INPUT
  GPIOB->MODER = GPIOB->MODER & ~(0x3UL << 14U);
	GPIOB->PUPDR = GPIOB->PUPDR & ~(0x1UL << 14U);
  GPIOB->PUPDR = GPIOB->PUPDR | (0x2UL << 14U);



}

uint8_t USER_Key( void ){
	for(int i = 0U; i < 4U; i++){ // Iterate in rows activating high on pins
		GPIOB->ODR = GPIOB->ODR | (0x1UL << i); // Activvate actual row
		if(i == 3U && (GPIOB->IDR & (0x1UL << 5u))){ // check special case 0
			GPIOB->ODR = GPIOB->ODR & ~(0x1UL << i); //desactivate actual row
			return 0x0; //return 0x0
		}
		else if( i == 3U && (GPIOB->IDR & (0x1UL << 7U))){ //special case D hex 13
			GPIOB->ODR = GPIOB->ODR & ~(0x1UL << i); // desactivate actual row
			return 13U;//Send an unsigned 13 value
		}
		else if(i != 3U){ // Other cases of the rows
			for(int j = 4U; j < 8U; j++){ // iterate in columns
			if((GPIOB->IDR & (0x1UL << j))){// check every colimns
				GPIOB->ODR = GPIOB->ODR & ~(0x1UL << i); //desactivate actual row
				USER_TIM3_Delay(36,64864);   //optional delayy for debounce
				if(j == 7U){
				return (uint8_t)(10U + i); // return special cases
				}
				return (uint8_t)(i*3U + (j-3U)); //return num of pad
			}
		}
		}
		GPIOB->ODR = GPIOB->ODR & ~(0x1UL << i); // desactivate actual row before changing.
	}

  return 0xFF; // return 0x0
}

