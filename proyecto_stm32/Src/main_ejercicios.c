#include <stdint.h>
#include <stm32f4xx.h>


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif



int main(void)
{
	/* Ejercicio 2.1*/
	uint8_t value = 0b10110101;
	uint8_t mask = 0b00001111;
	uint8_t result = value & mask;

	/*Ejercicio 2.2*/
	uint8_t value = 0b10100000;
	uint8_t mask = 0b00000101;
	uint8_t result = value | mask;

	/*Ejercicio 2.3*/
	uint8_t a = 0b00001111;
	uint8_t result1 = ~a;
	uint8_t b = 0b10100101;
	uint8_t result2 = ~b;

	uint8_t value = 0b11111111;
	value &= ~(0b00000011);

	/*Ejercicio 2.4*/
	uint8_t value = 0b10110011;
	uint8_t mask = 0b00001111;
	uint8_t result1 = value ^ mask;
	uint8_t result2 = result1 ^ mask;

	/*Ejercicio 2.5*/
	uint8_t simulated_register = 0x00;
	uint8_t simulated_register |= 0b00011000;
	uint8_t simulated_register &= ~(0b00001000);
	uint8_t simulated_register ^= 0b00010000;
	uint8_t simulated_register ^= 0b00010000;

	/*Ejercicio 2.6*/
	RCC->AHB1ENR |= RCC_ABH1ENR_GPIOAEN; /*Encender la señal de reloj*/

	/*Ejercicio 2.7*/
	GPIOA->MODER &= ~(3<<10); /*Limpiar el pin 5*/
	GPIOA->MODER |= (1<<10); /*Configurar el pin 5*/
	GPIOA->ODR |= (1<<5); /*Encender el led*/

	/*Ejercicio 2.8*/
	/* Maquina de estado en papel*/

	/* Ejercicio 2.9*/




while (1) {

}
}


