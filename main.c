/*
 * Prelab6_UART.c
 
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "UARTlib/UART.h"

int main(void)
{
    cli();

    // 1. Puerto B como salida (Para prender los LEDs en pines D8, D9, D10, D11, D12 y D13)
    DDRB |= (1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3)|(1<<DDB4)|(1<<DDB5);
    
    // 2. Puerto D como salida (Para prender los LEDs en pines D6 y D7)
    DDRD |= (1<<DDD6)|(1<<DDD7);
    
    // Apagar todos los LEDs inicialmente al arrancar
    PORTB &= ~0x3F; 
    PORTD &= ~0xC0; 

    initUART();

    sei();

    // Parte 1: Enviar un caracter (la 'A') al arrancar hacia la hiperterminal
    writeChar('A');

    while(1)
    {
        // El ciclo principal no hace nada, todo se maneja al instante en la interrupción (ISR)
    }
}

// Parte 2 y final: Interrupción que recibe el caracter y lo muestra de forma inteligente
ISR(USART_RX_vect)
{
    char rx = UDR0;
    
    // 1. Eco: muestra en la terminal el caracter exacto que recibimos
    writeChar(rx); 
    
    // 2. Control de los LEDs
    // Ignoramos los enter (New Line \n o Carriage Return \r) para no apagar
    // ni arruinar lo que muestran los LEDs del caracter que sí queríamos ver.
    if(rx != '\n' && rx != '\r') {
        
        // Enviamos los bits 0 al 5 del caracter hacia el Puerto B (Pines D8 a D13)
        PORTB = (PORTB & 0xC0) | (rx & 0x3F);   
        
        // Enviamos los bits 6 y 7 del caracter hacia el Puerto D (Pines D6 y D7)
        PORTD = (PORTD & 0x3F) | (rx & 0xC0);   
    }
}
