#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include "ds18b20/ds18b20.h"
#include "dht22/DHT.h"
#include "si1145/SI1145_WE.h"

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
	USART_Init(MYUBRR);
	
	ADCSRA |= (1<<ADEN); //enable ADC
	ADMUX |= ( (0<<REFS1) | (1<<REFS0) ); // set to ref to AVCC & AREF with capacitor
	ADMUX |= 5;

	//Variables
	int temperature_ds18b20;
	double temperature_dht22[1];
	double humidity_dht22[1];
	
	char strbuf0[400];
	sprintf (strbuf0, "hw0! %d \r\n", 0);
	usart_pstr(strbuf0);
	
	//DHT_Setup();
	SI1145_WE_init(0x60);
	
	enableHighSignalVisRange(); // Gain divided by 14.5
	enableHighSignalIrRange(); // Gain divided by 14.5
	
	/* choices: PS_TYPE, ALS_TYPE, PSALS_TYPE, ALSUV_TYPE, PSALSUV_TYPE || FORCE, AUTO, PAUSE */
	enableMeasurements(PSALSUV_TYPE, AUTO);
	
	while(1)
	{
		
		char strbuf1[400];
		sprintf (strbuf1, "hi! %d \r\n", 0);
		usart_pstr(strbuf1);
		
		
		/************************************************************************/
		/* Si1145 Reading                                                       */
		/************************************************************************/
		
		unsigned char failureCode = 0;
		unsigned int amb_als = 0;
		unsigned int amb_ir = 0;
		float uv = 0.0;
		
		amb_als = getAlsVisData();
		amb_ir = getAlsIrData();
		uv = getUvIndex();
		
		char strbuf7[400];
		sprintf (strbuf7, "light: %d \r\n", amb_als);
		usart_pstr(strbuf7);
		
		char strbuf8[400];
		sprintf (strbuf8, "infra: %d \r\n", amb_ir);
		usart_pstr(strbuf8);
		
		char strbuf9[400];
		sprintf (strbuf9, "uv: %f \r\n", uv);
		usart_pstr(strbuf9);
		
		failureCode = getFailureMode();  // reads the response register
		if((failureCode&128)){  // if bit 7 is set in response register, there is a failure
			handleFailure(failureCode);
		}
		
		_delay_ms(1000);
	}
	return (0);
}

void handleFailure(unsigned char code){
	char msg[400];
	switch(code){
		case SI1145_RESP_INVALID_SETTING:
		sprintf (msg, "Invalid Setting %d \r\n", 0);
		break;
		case SI1145_RESP_PS1_ADC_OVERFLOW:
		sprintf (msg, "PS ADC Overflow %d \r\n", 0);
		break;
		case SI1145_RESP_ALS_VIS_ADC_OVERFLOW:
		sprintf (msg, "ALS VIS ADC Overflow %d \r\n", 0);
		break;
		case SI1145_RESP_ALS_IR_ADC_OVERFLOW:
		sprintf (msg, "ALS IR Overflow %d \r\n", 0);
		break;
		case SI1145_RESP_AUX_ADC_OVERFLOW:
		sprintf (msg, "AUX ADC Overflow %d \r\n", 0);
		break;
		default:
		sprintf (msg, "Unknown Failure %d \r\n", 0);
		break;
	}
	//usart_pstr(msg);
	
	clearFailure();
}

void initPorts(){
	DDRB = 0x00; // Set PORT B to input
	
	//DDRC = DDRC & 0b11101100; // Set PORT C to input
	//PORTC = PORTC | 0b00010000; //enable pull-ups
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


