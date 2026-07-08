#include <stdint.h>
#include <stm32f4xx.h>


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif



int main(void)
{
/*HABILITA LA SEÑAL DE RELOJ*/
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
/*HABILITA EL PIN 13 COMO SALIDA*/
GPIOA->MODER &= ~(3 << 26);
GPIOA->MODER |=  (1 << 26);
/*ASEGURA QUE EL PIN 13 ESTE EN PUSH PULL*/
GPIOA->OTYPER &= ~(1 << 5);
/* CAMBIAR LA VELOCIDAD A MUY ALTA 11*/
GPIOA->OSPEEDR &= ~(3 << (13 * 2));
GPIOA->OSPEEDR |=  (3 << (13 * 2));
/*HABILITAR UNA RESISTANCIA PULL UP 01*/
GPIOA->PUPDR &= ~(3 << (13 * 2));
GPIOA->PUPDR |=  (1 << (13 * 2));


while (1) {

}
}


