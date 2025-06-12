#include <stdint.h>
#include "main.h"
#include "adclib.h"
#include "user_tim.h"

void USER_ADC_Init(void) {
    // Habilitar reloj del ADC y del puerto GPIOA
    RCC->IOPENR |= (1 << 0);     // GPIOAEN
    RCC->APBENR2 |= (1 << 20);   // ADCEN

    // PA0 en modo analógico
    GPIOA->MODER |= (0x3 << (0*2));   // Modo analógico
    GPIOA->PUPDR &= ~(0x3 << (0*2));  // Sin pull-up/pull-down

    // Configurar CKMODE para reloj síncrono dividido entre 2
    ADC1->CFGR2 &= ~(0x3 << 30);        // Borrar CKMODE
    //ADC1->CFGR2 |=  (0x1 << 30);        // CKMODE = 01: PCLK/2

    ADC->CCR &= ~(0xE << 18);
    ADC->CCR|=  (0x1 << 18);

    // Configurar resolución, alineación, modo de conversión
    ADC1->CFGR1 &= ~(0x1 << 13); // Single conversion mode
    ADC1->CFGR1 &= ~(0x1 << 5);  // Right alignment
    ADC1->CFGR1 &= ~(0x3 << 3);  // 12-bit resolution

    // Tiempo de muestreo
    ADC1->SMPR |= ~(0x7 << 0);   // Sampling time = shortest

    ADC1->ISR &= ~( 0x1UL << 13U );
    ADC1->CFGR1 &= ~( 0x1UL << 21U ) & ~( 0x1UL << 2U );

    // Seleccionar canal 0 (PA0)
    ADC1->CHSELR |= (1 << 0);

    while( !(ADC1->ISR & (0x1UL << 13U)));

    // Habilitar regulador interno

    ADC1->CR |= (1 << 28);       // ADVREGEN
    USER_TIM14_Delay(1);

    // Calibración
    while (!USER_ADC_Calibration());

    // Habilitar ADC
    ADC1->CR |= (1 << 0);         // ADEN
    for (uint32_t i = 0; i < 1000 && !(ADC1->ISR & (1 << 0)); i++) USER_TIM14_Delay(1); // Wait up to 1ms
    if (!(ADC1->ISR & (1 << 0))) return;  // Fail if ADRDY not set
}

uint8_t USER_ADC_Calibration(void) {
    ADC1->CR |= (1 << 31);                   // ADCAL
    while (ADC1->CR & (1 << 31));            // Esperar fin de calibración

    // (Opcional) Ajustar factor de calibración
    if (ADC1->CALFACT > 0x7F) {
        ADC1->CALFACT = 0x7F;
    }
    return 1;
}

uint16_t USER_ADC_Read(void) {
    ADC1->CR |= (1 << 2);               // ADSTART
    while (!(ADC1->ISR & (1 << 2)));    // Esperar EOC
    if (ADC1->ISR & (1 << 4)) {         // Check for overrun error
        ADC1->ISR |= (1 << 4);          // Clear overrun flag
    }
    return (uint16_t)(ADC1->DR);        // Leer valor convertido
}
