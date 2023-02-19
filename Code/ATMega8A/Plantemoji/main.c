#include <avr/io.h>
#include <util/delay.h>
#include <stddef.h>
#include "ds18b20/ds18b20.h"
#include "dht22/DHT.h"
#include "si11452/SI1145_WE.h"
//#include "i2c/i2c.h"

//static const char *I2C_BUS="/dev/i2c-1";
//static const int I2C_ADDR = 0x00;

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
	
	DHT_Setup();
	

	while(1)
	{
		
		/************************************************************************/
		/* DS18B20 Reading                                                      */
		/************************************************************************/	
	
		//Start conversion (without ROM matching) 1 << (0) means pin.0, 1 << (5) means pin.5
		ds18b20convert( &PORTC, &DDRC, &PINC, ( 1 << 1 ), NULL );

		//Delay (sensor needs time to perform conversion)
		_delay_ms( 1000 );

		//Read temperature (without ROM matching)
		ds18b20read( &PORTC, &DDRC, &PINC, ( 1 << 1 ), NULL, &temperature_ds18b20 );

		char strbuf3[400];
		sprintf (strbuf3, "temp-ds18: %d \r\n", temperature_ds18b20);
		usart_pstr(strbuf3);
		
		/************************************************************************/
		/* DHT22 Reading                                                        */
		/************************************************************************/
		
		//Read from sensor
		enum DHT_Status_t dhtStatus = DHT_Read(temperature_dht22, humidity_dht22);
		
		char strbuf4[400];
		char strbuf5[400];
		char strbuf6[400];
				
		//Check status
		switch (dhtStatus)
		{
			case (DHT_Ok):
				sprintf (strbuf4, "temp-dht22: %f \r\n", temperature_dht22[0]);
				usart_pstr(strbuf4);
				
				sprintf (strbuf5, "humidity-dht22: %f \r\n", humidity_dht22[0]);
				usart_pstr(strbuf5);
		
			break;
			case (DHT_Error_Checksum):
				sprintf (strbuf6, "dht22-error: %d \r\n", 0);
				usart_pstr(strbuf6);
			break;
			case (DHT_Error_Timeout):
				//Do something else
			break;
		}
		
		//Sensor needs 1-2s to stabilize its readings
		_delay_ms(1000);
		
		/************************************************************************/
		/* Si1145 Reading                                                       */
		/************************************************************************/
		 
		SI1145_WE_init();

		/* in case you want to change the I2C Address */
		//mySI1145.setI2CAddress(0x59);
		
		enableHighSignalVisRange(); // Gain divided by 14.5
		enableHighSignalIrRange(); // Gain divided by 14.5
		
		/* choices: PS_TYPE, ALS_TYPE, PSALS_TYPE, ALSUV_TYPE, PSALSUV_TYPE || FORCE, AUTO, PAUSE */
		enableMeasurements(PSALSUV_TYPE, AUTO);
	
		//unsigned char failureCode = 0;
		unsigned int amb_als = 0;
		unsigned int amb_ir = 0;
		/* uncomment if you want to perform PS measurements */
		//unsigned int proximity = 0;
		float uv = 0.0;
		
		amb_als = getAlsVisData();
		amb_ir = getAlsIrData();

		/* uncomment if you want to perform PS measurements */
		// proximity = mySI1145.getPsData();

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
		
		_delay_ms(2000);
	}
	return (0);
}



void initPorts(){
	DDRB = 0x00; // Set PORT B to input
	
	DDRC = DDRC & 0b11101100; // Set PORT C to input
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


