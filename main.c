/*
 * Laboratorio6_UART_ADC.c
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h> 
#include "UARTlib/UART.h"

// Variables compartidas entre la interrupción y el ciclo principal
volatile uint8_t dato_recibido = 0;
volatile char caracter_rx = 0;

// Función rápida para imprimir el menú y no ensuciar el código
void printMenu(void)
{
    writeString("\r\n----- MENU -----\r\n");
    writeString("1. Leer potenciometro (A0)\r\n");
    writeString("2. Enviar ascii\r\n");
    writeString("Seleccione una opcion: ");
}

// Función para inicializar el Convertidor Análogo-Digital (ADC)
void initADC(void)
{
    // REFS0=1: Referencia de voltaje en AVCC. 
    // ADLAR=1: Ajuste a la izquierda ADCH.
    ADMUX = (1<<REFS0) | (1<<ADLAR);
    
    // ADEN=1: Enciende ADC. 
    // ADPS2..0=111: Prescaler en 128 
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

int main(void)
{
    cli();

    // Configuración de Puertos (LEDs)
    DDRB |= (1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3)|(1<<DDB4)|(1<<DDB5);
    DDRD |= (1<<DDD6)|(1<<DDD7);
    PORTB &= ~0x3F; 
    PORTD &= ~0xC0; 

    // Inicializaciones
    initUART();
    initADC();

    sei();

    writeString("\r\n¡Bienvenido al sistema!\r\n");
    printMenu();

    uint8_t estado = 0; // 0 = Esperando opcion del Menú, 1 = Esperando la letra/número ASCII

    while(1)
    {
        // Revisa la "bandera" de que nos teclearon algo
        if (dato_recibido == 1) 
        {
            dato_recibido = 0; // Baja la bandera 
            char letra = caracter_rx; 

            if (estado == 0) // espera a menu principal
            {
                if (letra == '1') 
                {
                    writeString("1\r\n"); 
                    
                    // Iniciamos la conversión del potenciómetro
                    ADCSRA |= (1<<ADSC);
                    while(ADCSRA & (1<<ADSC)); // Espera milisegundos a que termine de leer
                    uint8_t valorADC = ADCH;   // Guarda su valor de 8 bits 
                    
                    // Muestra el voltaje en los LEDs
                    PORTB = (PORTB & 0xC0) | (valorADC & 0x3F);   
                    PORTD = (PORTD & 0x3F) | (valorADC & 0xC0);
                    
                    // Muestra en la terminal su valor en decimal 
                    writeString(">> Lectura del potenciometro: ");
                    char buffer[5];
                    itoa(valorADC, buffer, 10); // Convierte el numero binario a texto decimal 
                    writeString(buffer);
                    writeString("\r\n");
                    
                    printMenu(); // regresa automáticamente al menu
                } 
                else if (letra == '2') 
                {
                    writeString("2\r\n");
                    writeString(">> Escriba cualquier tecla para mostrarla en LEDs: ");
                    estado = 1; // Activa el estado de enviar ASCII para la próxima tecla
                }
            }
            else if (estado == 1) // espera a la tecla ascii
            {
                // Ignora los enter invisibles de configuración 
                if (letra != '\n' && letra != '\r') 
                {
                    writeChar(letra); // Eco de la letra
                    writeString("\r\n");
                    
                    // Muestra en LEDs 
                    PORTB = (PORTB & 0xC0) | (letra & 0x3F);   
                    PORTD = (PORTD & 0x3F) | (letra & 0xC0);
                    
                    estado = 0; // Desactiva el estado ASCII, regresa a menu
                    printMenu();
                }
            }
        }
    }
}

// Nuestra interrupción ahora solo recibe caracteres velozmente y avisa
ISR(USART_RX_vect)
{
    caracter_rx = UDR0;
    dato_recibido = 1; // 
}
