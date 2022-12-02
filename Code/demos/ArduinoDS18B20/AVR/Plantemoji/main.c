#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include "ds18b20/ds18b20.h"

//Macros
#define bit_is_low(sfr,bit)(!(_SFR_BYTE(sfr) & _BV(bit)))
#define bit_is_high(sfr,bit)(_SFR_BYTE(sfr) & _BV(bit))
#define PORT_ON(port,pin) port |= (1<<pin)
#define PORT_OFF(port,pin)  port &= ~(1<<pin)

//Head of methods
void USART_Init( unsigned int ubrr);
void USART_Transmit( unsigned char data );
void usart_putchar( char data );
void usart_pstr (char *s);
void initPorts();

#define BAUD 115200
#define BAUDRATE F_CPU/16/BAUD-1

#define FOSC 8000000// Clock Speed
#define MYUBRR FOSC/16/BAUD-1

int main()
{
	initPorts();
	OSCCAL=0x93;
	USART_Init(MYUBRR);	//RTC_Init();
	
	ADCSRA |= (1<<ADEN); //enable ADC
	ADMUX |= ( (0<<REFS1) | (1<<REFS0) ); // set to ref to AVCC & AREF with capacitor
	ADMUX |= 5;

	int temp;
	
	while(1)
	{
		char strbuf2[400];
		sprintf (strbuf2, "hi! \r\n");
		usart_pstr(strbuf2);
		_delay_ms(1000);

		//Start conversion (without ROM matching)
		ds18b20convert( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL );

		//Delay (sensor needs time to perform conversion)
		_delay_ms( 1000 );

		//Read temperature (without ROM matching)
		ds18b20read( &PORTB, &DDRB, &PINB, ( 1 << 0 ), NULL, &temp );

		char strbuf3[400];
		sprintf (strbuf3, "temp: %d \r\n", temp);
		usart_pstr(strbuf3);
		
		//Somehow use data stored in `temp` variable

	}
	return (0);
}



void initPorts(){
	DDRB = 0x00; // Set PORT B to input
	
	DDRC = DDRC & 0b11101111; // Set PORT D to input
	PORTC = PORTC | 0b00010000; //enable pull-ups
}


void USART_Init( unsigned int ubrr)
{
	/* Set baud rate */
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	 // Enable the receiver interrupt
	 UCSRB |= (1 << RXCIE);                  
	/* Set frame format: 8data, 2stop bit */
	//UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
}
void USART_Transmit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
	UDR = data;
}


void usart_putchar(char data) {
	// Wait for empty transmit buffer
	while ( !(UCSRA & (_BV(UDRE))) );
	// Start transmission
	UDR = data;
}

void usart_pstr(char *s) {
	// loop through entire string
	while (*s) {
		usart_putchar(*s);
		s++;
	}
}


