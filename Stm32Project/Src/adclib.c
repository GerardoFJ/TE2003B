#include <stdint.h>
#include "main.h"
#include "systicklib.h"
#include "adclib.h"

void USER_ADC_Init(void) {
    // Habilitar reloj del ADC y del puerto GPIOA
    RCC->IOPENR |= (1 << 0);     // GPIOAEN
    RCC->APBENR2 |= (1 << 20);   // ADCEN

    // PA0 en modo analógico
    GPIOA->MODER |= (0x3 << (0*2));   // Modo analógico
    GPIOA->PUPDR &= ~(0x3 << (0*2));  // Sin pull-up/pull-down

    // Configurar CKMODE para reloj síncrono dividido entre 2
    ADC->CFGR2 &= ~(0x3 << 30);        // Borrar CKMODE
    //ADC1->CFGR2 |=  (0x1 << 30);        // CKMODE = 01: PCLK/2

    ADC->CCR &= ~(0xE << 18);
    ADC->CCR|=  (0x1 << 18);

    // Configurar resolución, alineación, modo de conversión
    ADC->CFGR1 &= ~(0x1 << 13); // Single conversion mode
    ADC->CFGR1 &= ~(0x1 << 5);  // Right alignment
    ADC->CFGR1 &= ~(0x3 << 3);  // 12-bit resolution

    // Tiempo de muestreo
    ADC->SMPR |= ~(0x7 << 0);   // Sampling time = shortest

    ADC->ISR &= ~( 0x1UL << 13U );
    ADC->CFGR1 &= ~( 0x1UL << 21U ) & ~( 0x1UL << 2U );

    // Seleccionar canal 0 (PA0)
    ADC->CHSELR |= (1 << 0);

    while( !(ADC->ISR & (0x1UL << 13U)));

    // Habilitar regulador interno

    ADC->CR |= (1 << 28);       // ADVREGEN
    SysTick_Delay(1);            // Delay > 10 us

    // Calibración
    while (!USER_ADC_Calibration());

    // Habilitar ADC
    ADC->CR |= (1 << 0);         // ADEN
    for (uint32_t i = 0; i < 1000 && !(ADC->ISR & (1 << 0)); i++) SysTick_Delay(1); // Wait up to 1ms
    if (!(ADC->ISR & (1 << 0))) return;  // Fail if ADRDY not set
}

uint8_t USER_ADC_Calibration(void) {
    ADC->CR |= (1 << 31);                   // ADCAL
    while (ADC->CR & (1 << 31));            // Esperar fin de calibración

    // (Opcional) Ajustar factor de calibración
    if (ADC->CALFACT > 0x7F) {
        ADC->CALFACT = 0x7F;
    }
    return 1;
}

uint16_t USER_ADC_Read(void) {
    ADC->CR |= (1 << 2);               // ADSTART
    while (!(ADC->ISR & (1 << 2)));    // Esperar EOC
    if (ADC->ISR & (1 << 4)) {         // Check for overrun error
        ADC->ISR |= (1 << 4);          // Clear overrun flag
    }
    return (uint16_t)(ADC->DR);        // Leer valor convertido
}
