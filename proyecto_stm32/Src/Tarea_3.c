#include "stm32f411xe.h"
#include <stdint.h>

// Prototipos de funciones
void delay(uint32_t tiempo);
void ADC1_Init(void);
uint16_t ADC1_Read(void);

int main(void)
{
    /************************************************
     * 1. HABILITAR RELOJES (GPIOA, GPIOB, TIM3 y ADC1)
     ************************************************/
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    // Ya no necesitamos GPIOCEN porque nos movimos a la fila A
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;  // Reloj para el ADC1

    /************************************************
     * 2. CONFIGURAR PA0 (CLK) Y PA1 (DT) DEL ENCODER
     ************************************************/
    GPIOA->MODER &= ~((3 << (0 * 2)) | (3 << (1 * 2))); // Modo Entrada
    GPIOA->PUPDR &= ~((3 << (0 * 2)) | (3 << (1 * 2)));
    GPIOA->PUPDR |=  ((1 << (0 * 2)) | (1 << (1 * 2))); // Activar Pull-ups

    /************************************************
     * 3. CONFIGURAR PA4 COMO ENTRADA ANALÓGICA (NUEVO PIN POTENCIÓMETRO)
     ************************************************/
    GPIOA->MODER |= (3 << (4 * 2));  // Modo Analógico (11) para PA4
    GPIOA->PUPDR &= ~(3 << (4 * 2)); // Sin Pull-up/Pull-down interno

    /************************************************
     * 4. CONFIGURAR PINES DE SALIDA PWM (PA7 y PB1)
     ************************************************/
    // PA7 (TIM3_CH2 - Verde controlado por Potenciómetro)
    GPIOA->MODER &= ~(3 << (7 * 2));
    GPIOA->MODER |=  (2 << (7 * 2)); // AF Mode
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT7);
    GPIOA->OSPEEDR |= (3 << (7 * 2));
    GPIOA->AFR[0] &= ~(0xF << (7 * 4));
    GPIOA->AFR[0] |=  (2 << (7 * 4)); // AF2 (TIM3)

    // PB1 (TIM3_CH4 - Rojo controlado por Encoder)
    GPIOB->MODER &= ~(3 << (1 * 2));
    GPIOB->MODER |=  (2 << (1 * 2)); // AF Mode
    GPIOB->OTYPER &= ~(GPIO_OTYPER_OT1);
    GPIOB->OSPEEDR |= (3 << (1 * 2));
    GPIOB->AFR[0] &= ~(0xF << (1 * 4));
    GPIOB->AFR[0] |=  (2 << (1 * 4)); // AF2 (TIM3)

    /************************************************
     * 5. CONFIGURACIÓN DEL TIMER 3 (PWM @ 1kHz)
     ************************************************/
    TIM3->PSC = 84 - 1;
    TIM3->ARR = 1000 - 1;

    // Configurar Canal 2 (PA7) en PWM Modo 1
    TIM3->CCMR1 &= ~(7 << 12);
    TIM3->CCMR1 |=  (6 << 12);
    TIM3->CCMR1 |=  TIM_CCMR1_OC2PE;

    // Configurar Canal 4 (PB1) en PWM Modo 1
    TIM3->CCMR2 &= ~(7 << 12);
    TIM3->CCMR2 |=  (6 << 12);
    TIM3->CCMR2 |=  TIM_CCMR2_OC4PE;

    // Habilitar las salidas físicas de los Canales 2 y 4
    TIM3->CCER |= (TIM_CCER_CC2E | TIM_CCER_CC4E);

    // Inicializar ciclos de trabajo
    int16_t brillo_rojo = 0;
    TIM3->CCR4 = brillo_rojo; // Rojo inicial en 0
    TIM3->CCR2 = 0;           // Verde inicial en 0

    // Arrancar el Timer 3
    TIM3->CR1 |= TIM_CR1_ARPE;
    TIM3->EGR |= TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN;

    /************************************************
     * 6. INICIALIZAR EL PERIFÉRICO ADC1
     ************************************************/
    ADC1_Init();

    /************************************************
     * VARIABLES DE CONTROL PARA EL ENCODER
     ************************************************/
    uint8_t clk_actual;
    uint8_t clk_anterior;
    clk_anterior = (GPIOA->IDR & GPIO_IDR_IDR_0) ? 1 : 0;

    while(1)
    {
        /************************************************
         * CONTROL DEL ROJO (PB1) - ENCODER
         ************************************************/
        clk_actual = (GPIOA->IDR & GPIO_IDR_IDR_0) ? 1 : 0;

        if (clk_anterior == 1 && clk_actual == 0) // Detecta flanco de bajada
        {
            uint8_t dt_estado = (GPIOA->IDR & GPIO_IDR_IDR_1) ? 1 : 0;

            if (dt_estado != clk_actual)
            {
                brillo_rojo += 33; // Sentido horario aumenta intensidad
                if (brillo_rojo > 999) brillo_rojo = 999;
            }
            else
            {
                brillo_rojo -= 33; // Sentido antihorario disminuye intensidad
                if (brillo_rojo < 0) brillo_rojo = 0;
            }
            TIM3->CCR4 = (uint16_t)brillo_rojo;
        }
        clk_anterior = clk_actual;

        /************************************************
         * CONTROL DEL VERDE (PA7) - POTENCIÓMETRO (ADC)
         ************************************************/
        uint16_t adc_val = ADC1_Read(); // Leer valor actual de 0 a 4095

        // Escalar de forma segura (0-4095) al rango del PWM (0-999)
        uint16_t brillo_verde = (uint32_t)(adc_val * 999) / 4095;

        TIM3->CCR2 = brillo_verde;

        // Pequeño retardo de estabilidad general (Debouncing + Muestreo)
        delay(1000);
    }
}

/************************************************
 * FUNCIONES COMPLEMENTARIAS
 ************************************************/

void ADC1_Init(void)
{
    // Configurar la velocidad del reloj del ADC (División por 6 para máxima estabilidad)
    ADC->CCR &= ~(3 << 16);
    ADC->CCR |=  (2 << 16);

    // CAMBIADO: El pin PA4 está asociado al Canal 4 del ADC1
    ADC1->SQR3 = 4;       // Definir canal 4 como la 1ra conversión de la secuencia

    // Configurar el tiempo de muestreo (Sampling Time) para el Canal 4.
    // Ponemos el canal 4 a 84 ciclos (bits 12:14 en 100) en SMPR2.
    ADC1->SMPR2 &= ~(7 << 12);
    ADC1->SMPR2 |=  (4 << 12);

    ADC1->CR2 |= ADC_CR2_ADON; // Encender el módulo ADC1

    // Retardo para que la circuitería analógica se estabilice
    delay(10000);
}

uint16_t ADC1_Read(void)
{
    // Iniciar la conversión por software
    ADC1->CR2 |= ADC_CR2_SWSTART;

    // Esperar a que la conversión finalice (Bit EOC - End of Conversion)
    uint32_t timeout = 20000;
    while (!(ADC1->SR & ADC_SR_EOC) && timeout--)
    {
        __NOP();
    }

    // Retornar los 12 bits medidos. Al leer 'DR' se limpia automáticamente el bit EOC.
    return (uint16_t)(ADC1->DR);
}

void delay(uint32_t tiempo)
{
    while(tiempo--)
    {
        __NOP();
    }
}
