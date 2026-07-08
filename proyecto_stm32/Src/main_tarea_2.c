#include "stm32f411xe.h"



/* Prototipos*/

void delay_ms(uint32_t ms);
void GPIO_init(void);
void apagar_segmentos(void);
void apagar_digitos(void);
void cargar_segmentos(uint8_t numero);
void activar_digito(uint8_t dig);
void actualizar_display(void);
void TIM2_init(void);

/*Variables globales*/

volatile uint16_t contador = 0;
volatile uint8_t digito_actual = 0;
uint8_t display[4];


/*Tabla de números*/

const uint8_t numeros[10][7] =
{

    {0,0,0,0,0,0,1}, //0
    {1,0,0,1,1,1,1}, //1
    {0,0,1,0,0,1,0}, //2
    {0,0,0,0,1,1,0}, //3
    {1,0,0,1,1,0,0}, //4
    {0,1,0,0,1,0,0}, //5
    {0,1,0,0,0,0,0}, //6
    {0,0,0,1,1,1,1}, //7
    {0,0,0,0,0,0,0}, //8
    {0,0,0,0,1,0,0}  //9

};


/* Retardo inicial*/

void delay_ms(uint32_t ms)
{

    for(uint32_t i = 0; i < ms * 16000; i++)
    {
        __NOP();
    }

}


/* Configuración GPIO*/

void GPIO_init(void)
{


    /* Reloj GPIOA GPIOB GPIOC*/

    RCC->AHB1ENR |= (1<<0);
    RCC->AHB1ENR |= (1<<1);
    RCC->AHB1ENR |= (1<<2);


    /* Segmentos*/

    // A PA4

    GPIOA->MODER &= ~(3<<(4*2));
    GPIOA->MODER |= (1<<(4*2));


    // F PB0

    GPIOB->MODER &= ~(3<<(0*2));
    GPIOB->MODER |= (1<<(0*2));


    // D PB1

    GPIOB->MODER &= ~(3<<(1*2));
    GPIOB->MODER |= (1<<(1*2));


    // E PB2

    GPIOB->MODER &= ~(3<<(2*2));
    GPIOB->MODER |= (1<<(2*2));


    // G PB14

    GPIOB->MODER &= ~(3<<(14*2));
    GPIOB->MODER |= (1<<(14*2));


    // C PB15

    GPIOB->MODER &= ~(3<<(15*2));
    GPIOB->MODER |= (1<<(15*2));


    // B PC0

    GPIOC->MODER &= ~(3<<(0*2));
    GPIOC->MODER |= (1<<(0*2));


    /*Dígitos PNP*/

    // DIG1 PA12

    GPIOA->MODER &= ~(3<<(12*2));
    GPIOA->MODER |= (1<<(12*2));


    // DIG3 PA5

    GPIOA->MODER &= ~(3<<(5*2));
    GPIOA->MODER |= (1<<(5*2));


    // DIG2 PC8

    GPIOC->MODER &= ~(3<<(8*2));
    GPIOC->MODER |= (1<<(8*2));


    // DIG4 PB6

    GPIOB->MODER &= ~(3<<(6*2));
    GPIOB->MODER |= (1<<(6*2));


    /*Sensores*/

    // PB12 entrada
    GPIOB->MODER &= ~(3<<(12*2));

    // PA11 entrada
    GPIOA->MODER &= ~(3<<(11*2));

    apagar_segmentos();

    apagar_digitos();

}


/* Apagar segmentos*/

void apagar_segmentos(void)
{

    GPIOA->ODR |= (1<<4);
    GPIOB->ODR |= (1<<0);
    GPIOB->ODR |= (1<<1);
    GPIOB->ODR |= (1<<2);
    GPIOB->ODR |= (1<<14);
    GPIOB->ODR |= (1<<15);
    GPIOC->ODR |= (1<<0);

}


/* Apagar dígitos*/


void apagar_digitos(void)
{

    GPIOA->ODR |= (1<<12);
    GPIOC->ODR |= (1<<8);
    GPIOA->ODR |= (1<<5);
    GPIOB->ODR |= (1<<6);

}


/* Cargar segmentos*/

void cargar_segmentos(uint8_t numero)
{

    // A
    if(numeros[numero][0])
        GPIOA->ODR |= (1<<4);
    else
        GPIOA->ODR &= ~(1<<4);

    // B
    if(numeros[numero][1])
        GPIOC->ODR |= (1<<0);
    else
        GPIOC->ODR &= ~(1<<0);

    // C
    if(numeros[numero][2])
        GPIOB->ODR |= (1<<15);
    else
        GPIOB->ODR &= ~(1<<15);

    // D
    if(numeros[numero][3])
        GPIOB->ODR |= (1<<1);
    else
        GPIOB->ODR &= ~(1<<1);

    // E
    if(numeros[numero][4])
        GPIOB->ODR |= (1<<2);
    else
        GPIOB->ODR &= ~(1<<2);

    // F
    if(numeros[numero][5])
        GPIOB->ODR |= (1<<0);
    else
        GPIOB->ODR &= ~(1<<0);

    // G
    if(numeros[numero][6])
        GPIOB->ODR |= (1<<14);
    else
        GPIOB->ODR &= ~(1<<14);

}


/* Activar un dígito*/

void activar_digito(uint8_t dig)
{

    apagar_digitos();


    switch(dig)
    {

        case 0:

            GPIOA->ODR &= ~(1<<12);

        break;


        case 1:

            GPIOC->ODR &= ~(1<<8);

        break;


        case 2:

            GPIOA->ODR &= ~(1<<5);

        break;


        case 3:

            GPIOB->ODR &= ~(1<<6);

        break;

    }

}


/* Separar contador en 4 dígitos*/

void actualizar_display(void)
{

    display[0] = contador / 1000;
    display[1] = (contador / 100) % 10;
    display[2] = (contador / 10) % 10;
    display[3] = contador % 10;

}


/* Configuración TIM2*/

void TIM2_init(void)
{

    /*Activar reloj TIM2*/
    RCC->APB1ENR |= (1<<0);

    /* Detener timer*/
    TIM2->CR1 = 0;

    /* 16 MHz / 16000 = 1 kHz*/
    TIM2->PSC = 15999;

    /*interrupción cada 1 ms*/
    TIM2->ARR = 1;

    /* Actualizar registro*/
    TIM2->EGR |= (1<<0);

    /* Habilitar interrupción update*/
    TIM2->DIER |= (1<<0);

    /*Habilitar IRQ*/
    NVIC_EnableIRQ(TIM2_IRQn);

    /*Encender timer*/
    TIM2->CR1 |= (1<<0);

}


/*Interrupción TIM2*/

void TIM2_IRQHandler(void)
{

    if(TIM2->SR & 1)
    {

        /*limpiar bandera*/
        TIM2->SR &= ~(1);

        /*evitar sombras*/
        apagar_digitos();

        cargar_segmentos(display[digito_actual]);

        activar_digito(digito_actual);

        digito_actual++;

        if(digito_actual >= 4)
            digito_actual = 0;

    }

}


int main(void)
{

    GPIO_init();
    TIM2_init();

    delay_ms(300);

    actualizar_display();

    uint8_t suma_anterior =
        (GPIOB->IDR >> 12) & 1;

    uint8_t resta_anterior =
        (GPIOA->IDR >> 11) & 1;

    while(1)
    {
        uint8_t suma =
            (GPIOB->IDR >> 12) & 1;

        uint8_t resta =
            (GPIOA->IDR >> 11) & 1;


        /*Incrementar*/

        if((suma == 1) && (suma_anterior == 0))
        {

            if(contador == 9999)
            {
                contador = 0;
            }
            else
            {
                contador++;
            }

            actualizar_display();

            delay_ms(50);

        }

        /*Decrementar*/

        if((resta == 1) && (resta_anterior == 0))
        {

            if(contador == 0)
            {
                contador = 9999;
            }
            else
            {
                contador--;
            }

            actualizar_display();

            delay_ms(50);

        }

        suma_anterior = suma;

        resta_anterior = resta;

    }

}
