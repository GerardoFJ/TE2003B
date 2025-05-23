#ifndef ADCLIB_H_
#define ADCLIB_H_

/* Analog-to-Digital Converter registers */
typedef struct
{
	volatile uint32_t ISR;
	volatile uint32_t IER;
	volatile uint32_t CR;
	volatile uint32_t CFGR1;
	volatile uint32_t CFGR2;
	volatile uint32_t SMPR;
	volatile uint32_t RESERVED0[2];
	volatile uint32_t AWD1TR;
	volatile uint32_t AWD2TR;
	volatile uint32_t CHSELR;
	volatile uint32_t AWD3TR;
	volatile uint32_t RESERVED1[4];
	volatile uint32_t DR;
	volatile uint32_t RESERVED2[23];
	volatile uint32_t AWD2CR;
	volatile uint32_t AWD3CR;
	volatile uint32_t RESERVED3[3];
	volatile uint32_t CALFACT;
	volatile uint32_t RESERVED4[148];
	volatile uint32_t CCR;
} ADC_TypeDef;

#define ADC_BASE	0x40012400UL//	ADC base address
#define ADC    	(( ADC_TypeDef * )ADC_BASE )

static uint8_t USER_ADC_Calibration( void );
void USER_ADC_Init( void );
uint16_t USER_ADC_Read( void );

#endif /* USER_ADC_H_ */
