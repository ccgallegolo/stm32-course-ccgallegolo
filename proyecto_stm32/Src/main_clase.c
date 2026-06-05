/*
 * main_clase.c
 *
 *  Created on: May 29, 2026
 *      Author: cristian
 */



#include <stdint.h>
#include <stm32f4xx.h>


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

   // Función de retardo simple basada en software
    void delay_ms(uint32_t milisegundos) {
        // Este factor (~16000) es aproximado para un reloj interno de 16 MHz.
        // Si tu procesador va más rápido, necesitarás aumentar este valor.
        uint32_t ciclos = milisegundos * 16000;
        for (uint32_t i = 0; i < ciclos; i++) {
            __NOP(); // Instrucción que no hace nada, solo consume 1 ciclo de reloj
        }
    }

int main(void)
{

	   /* Trabajo de clase */

	    /* 1. Activando la señal de reloj para el puerto GPIOC */
	    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

	    /* 2. Configurando el pin C8 como salida */
	    // Primero limpiamos los dos bits correspondientes al pin 8
	    GPIOC->MODER &= ~(GPIO_MODER_MODE8_Msk);
	    // Luego asignamos 0b01 (Salida de propósito general)
	    GPIOC->MODER |= (0b01 << GPIO_MODER_MODE8_Pos);

	    /* 3. Configurando el pin C8 como salida push-pull (0 = Push-Pull) */
	    GPIOC->OTYPER &= ~(GPIO_OTYPER_OT8);

	    /* 4. Configuración de la velocidad como fast (0b10 = Fast speed) */
	    // Primero limpiamos los bits de velocidad para el pin 8
	    GPIOC->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED8_Msk);
	    // Luego asignamos la velocidad rápida
	    GPIOC->OSPEEDR |= (0b10 << GPIO_OSPEEDR_OSPEED8_Pos);

	    /* 5. Encendemos el LED poniendo en alto el pin C8 */
	    GPIOC->ODR |= GPIO_ODR_OD8;

        /* Bucle infinito de parpadeo */
while (1) {
       // Alterna (Togglera) el estado del pin PC8
       // Si estaba encendido lo apaga, si estaba apagado lo enciende
        GPIOC->ODR ^= GPIO_ODR_OD8;

       // PERSONALIZA AQUÍ EL TIEMPO (en milisegundos aproximados)
       delay_ms(2000); // 500 ms encendido, 500 ms apagado (Parpadeo a  1Hz)
        }
    }



