#ifndef USER_TIM_H_
#define USER_TIM_H_

void USER_TIM3_PWM_Init( void );
void update_cycle(uint8_t duty, uint8_t pin);
uint16_t USER_Duty_Cycle( uint8_t duty );
void USER_TIM14_Init(void);
void USER_TIM14_Delay(uint16_t ms);

#endif /* USER_TIM_H_ */
