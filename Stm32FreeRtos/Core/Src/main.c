/* **************** START *********************** */
/* Libraries, Definitions and Global Declarations */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "user_uart.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "user_tim.h"
#include "exti_func.h"
#include "adclib.h"
#include "lcd.h"

#define configUSE_PREEMPTION  1

#define BUFFER_SIZE 8

char buffer_str[8];
char buffer_vel[8];
char buffer_rpm[8];
char buffer_gear[8];
// char test_buffer[] = "112.561";
int velocity = 0;
int index_k = 0;

uint8_t button_status = 0;
uint16_t val;


void USER_RCC_Init( void );
void System_init(void);
void USER_GPIO_Init(void);
void USART1_IRQHandler(void);

TaskHandle_t Task1Handle;
TaskHandle_t Task2Handle;
TaskHandle_t Task3Handle;
TaskHandle_t Task4Handle;

void StartTask1( void *pvParameters );
void StartTask2( void *pvParameters );
void StartTask3( void *pvParameters );
void StartTask4( void *pvParameters );
/* Superloop structure */
int main(void)
{
	/* Declarations and Initializations */
	HAL_Init();
	System_init();

	/* Create a task with a priority of 0 (idle), 1 (belowNormal), 2 (Normal), 3 (High), 4 (VeryHigh) */
	xTaskCreate(StartTask1, "Task1", 128, NULL, 1, &Task1Handle);
	xTaskCreate(StartTask2, "Task2", 128, NULL, 3, &Task2Handle);
	xTaskCreate(StartTask3, "Task3", 128, NULL, 2, &Task3Handle);
	xTaskCreate(StartTask4, "Task4", 128, NULL, 4, &Task4Handle);

	vTaskStartScheduler();


	/* Repetitive block */
	for(;;){

	}

}

// Task1 function
void StartTask1(void *pvParameters) {

  /* Infinite loop */
  for(;;) {

	  val = USER_ADC_Read();
	  if (GPIOA->IDR & (0x1UL << 7U))
				{
					button_status = 1;
				}
				else
				{
					button_status = 0;
				}
	  vTaskDelay(6); //1
  }
}

void StartTask2(void *pvParameters) {
 //vTaskDelay(100);
	vTaskDelay(2);
  /* Infinite loop */
  for(;;) {

		LCD_Set_Cursor(1, 1);
		LCD_Put_Str("Vel:       G:  ");
		LCD_Set_Cursor(1, 5);
		LCD_Put_Str(buffer_vel);
		LCD_Set_Cursor(1, 14);
		LCD_Put_Str(buffer_gear);
		LCD_Set_Cursor(2, 1);
		LCD_Put_Str("RPM:       ");
		LCD_Set_Cursor(2, 5);
		LCD_Put_Str(buffer_rpm);
		vTaskDelay(6); //3
  }
}

void StartTask3(void *pvParameters) {

	vTaskDelay(1);
  /* Infinite loop */
  for(;;) {
	  printf("{adc: %u, button: %u}\n", val, button_status);
	  vTaskDelay(6); //1
  }
}

void StartTask4(void *pvParameters) {
  /* Infinite loop */
 vTaskDelay(5);
  for(;;) {
	  //printf("Task4, enter \r\n");
	  if(velocity > 50 ){
	  		velocity = 50;
	  	}
	  		update_cycle(velocity,1);
	  		update_cycle(velocity,2);
	  		update_cycle(velocity,3);
	  		update_cycle(velocity,4);
	  vTaskDelay(5); //1
  }
}

void System_init(void){
	USER_RCC_Init();
	USER_UART1_Init();
	USER_UART2_Init();
	USER_GPIO_Init();
	USER_TIM14_Init();
	USER_TIM3_PWM_Init( );
	USER_ADC_Init();
    LCD_Init();
	LCD_Clear();
}


void USART1_IRQHandler(void) {
	if ((USART1->ISR & (0x1UL << 5U)))
		{ // wait until a data is received (ISR register)
			char received = USART1->RDR;
			if (received == 'V')
			{
				velocity = atoi(buffer_str);
				memcpy(buffer_vel, buffer_str, sizeof(buffer_str));
				memset(buffer_str, 0, sizeof(buffer_str));
				index_k = 0;
			}
			else if (received == 'S')
					{
						memcpy(buffer_rpm, buffer_str, sizeof(buffer_str));
						memset(buffer_str, 0, sizeof(buffer_str));
						index_k = 0;
					}
			else if (received == 'E')
							{
								memcpy(buffer_gear, buffer_str, sizeof(buffer_str));
								memset(buffer_str, 0, sizeof(buffer_str));
								index_k = 0;
							}
			else
			{
				if (index_k < BUFFER_SIZE - 1)
				{
					buffer_str[index_k++] = received;
				}
				else
				{
					memset(buffer_str, 0, sizeof(buffer_str));
					index_k = 0;
				}
			}
		}
}

void USER_GPIO_Init(void)
{
	// SET 4 BIT LEDS AS OUTPUT
	// PINA5 AS OUTPUT
//	GPIOA->BSRR = 0x1UL << 21U;						// Reset PA5 low to turn off LED
//	GPIOA->PUPDR = GPIOA->PUPDR & ~(0x3UL << 10U);	// Clear pull-up/pull-down bits for PA5
//	GPIOA->OTYPER = GPIOA->OTYPER & ~(0x1UL << 5U); // Clear output type bit for PA5
//	GPIOA->MODER = GPIOA->MODER & ~(0x2UL << 10U);	// Set PA5 as output
//	GPIOA->MODER = GPIOA->MODER | (0x1UL << 10U);	// Set PA5 as output
	// PINA9 AS INPUT PULL DOWN
	GPIOA->MODER &= ~(0x3UL << 14U);
	GPIOA->PUPDR &= ~(0x1UL << 14U);
	GPIOA->PUPDR |= (0x2UL << 14U);
}

void USER_RCC_Init( void ){
	/* System Clock (SYSCLK) configuration for 48 MHz */
	FLASH->ACR	&= ~( 0x6UL <<  0U );// 2 HCLK cycles latency, if SYSCLK >=24MHz <=48MHz
	FLASH->ACR	|=  ( 0x1UL <<  0U );// 2 HCLK cycles latency, if SYSCLK >=24MHz <=48MHz
	while(( FLASH->ACR & ( 0x7UL <<  0U )) != 0x001UL );// wait until LATENCY[2:0]=001
	RCC->CR		&= ~( 0x7UL << 11U );// select HSISYS division factor by 1
	while(!( RCC->CR & ( 0x1UL << 10U )));// wait until HSISYS is stable and ready
	RCC->CFGR	&= ~( 0x7UL <<  0U );// select HSISYS as the SYSCLK clock source
	RCC->CFGR	&= ~( 0x1UL << 11U );// select HCLK division factor by 1
	SystemCoreClock = 48000000;
}
