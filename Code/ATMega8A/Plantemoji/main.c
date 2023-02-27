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
void handleSi1145Failure(unsigned char code);
int ADCsingleREAD(uint8_t adctouse);

#define BAUD 115200
#define BAUDRATE F_CPU/16/BAUD-1
const float V_REF = 5.0; //reference voltage for the ADC

#define FOSC 8000000// Clock Speed
#define MYUBRR FOSC/16/BAUD-1

//Version 1.1.1
int main()
{
	/************************************************************************/
	/* UART and ADC Initialisation										    */
	/************************************************************************/
	initPorts();
	OSCCAL=0x93;
	USART_Init(MYUBRR);
	
	ADCSRA |= (1<<ADEN); //enable ADC
	ADMUX |= ( (0<<REFS1) | (1<<REFS0) ); // set to ref to AVCC & AREF with capacitor
	ADMUX |= 5;
	
	/************************************************************************/
	/* DHT Initialisation                                                   */
	/************************************************************************/
	double airTemperature_dht22[1];
	double airHumidity_dht22[1];
	DHT_Setup();
	
	/************************************************************************/
	/* Si1145 Initialisation                                                */
	/************************************************************************/
	SI1145_WE_init(0x60);
	
	enableHighSignalVisRange(); // Gain divided by 14.5
	enableHighSignalIrRange(); // Gain divided by 14.5
	
	enableMeasurements(PSALSUV_TYPE, AUTO); /* choices: PS_TYPE, ALS_TYPE, PSALS_TYPE, ALSUV_TYPE, PSALSUV_TYPE || FORCE, AUTO, PAUSE */
	
	
	while(1)
	{
		
		/************************************************************************/
		/* DS18B20 Reading                                                      */
		/************************************************************************/
		
		//Start conversion (without ROM matching) 1 << (0) means pin.0, 1 << (5) means pin.5
		ds18b20convert( &PORTC, &DDRC, &PINC, ( 1 << 1 ), NULL );

		//Delay (sensor needs time to perform conversion)
		_delay_ms(1000);

		int digitalTemperature;
		double soilTemperature_ds18b20;
		//Read temperature (without ROM matching)
		ds18b20read( &PORTC, &DDRC, &PINC, ( 1 << 1 ), NULL, &digitalTemperature);
		soilTemperature_ds18b20 = digitalTemperature/16.0;
		_delay_ms(2000);
		
		/************************************************************************/
		/* DHT22 Reading                                                        */
		/************************************************************************/
		
		//Read from sensor
		enum DHT_Status_t dhtStatus = DHT_Read(airTemperature_dht22, airHumidity_dht22);
		
		//Check status
		switch (dhtStatus)
		{
			case (DHT_Ok):
			//Do something
			break;
			case (DHT_Error_Checksum):
			//Do something
			break;
			case (DHT_Error_Timeout):
			//Do something else
			break;
		}
		
		//Sensor needs 1-2s to stabilize its readings
		_delay_ms(2000);
		
		/************************************************************************/
		/* HW-390 Reading                                                       */
		/************************************************************************/
		unsigned int soilHumidityDigital = ADCsingleREAD(0);
		double soilHumidity_HW390 = soilHumidityDigital/10.24;
				
		_delay_ms(2000);
		
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
		
		failureCode = getFailureMode();  // reads the response register
		if((failureCode&128)){  // if bit 7 is set in response register, there is a failure
			handleSi1145Failure(failureCode);
		}
		
		_delay_ms(2000);

		/************************************************************************/
		/* Combine sensor values then send them over serial to Huzzah			*/
		/************************************************************************/
		
		/************************************************************************/
		/*                                                                      */
		/*					Sensor values order:                                */
		/*                                                                      */
		/*					1. Dht22: air temperature => Celsius                */
		/*					2. Dht22: air humidity => Percentage                */
		/*					3. DS18b20: soil temperature => Celsius             */
		/*					4. HW390: soil humidity  => Percentage              */
		/*					5. Si1145: light => Lux                             */
		/*					6. Si1145: infrared => Lux                          */
		/*					7. Si1145: UV => UV Index (1 to 11+)                */
		/*                                                                      */
		/************************************************************************/
		
		char strbuf[400];
		sprintf (strbuf, "%f,%f,%f,%f,%d,%d,%f\r\n",
		airTemperature_dht22[0], 
		airHumidity_dht22[0],
		soilTemperature_ds18b20, 
		soilHumidity_HW390,
		amb_als,
		amb_ir,
		uv);
		usart_pstr(strbuf);

	}
	return (0);
}

int ADCsingleREAD(uint8_t adctouse)
{
	int ADCval;

	ADMUX = adctouse;         // use #1 ADC
	ADMUX |= ( (0<<REFS1) | (1<<REFS0) );    // use AVcc as the reference
	ADMUX &= ~(1 << ADLAR);   // clear for 10 bit resolution
	
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // 128 prescale for 8Mhz
	ADCSRA |= (1 << ADEN);    // Enable the ADC

	ADCSRA |= (1 << ADSC);    // Start the ADC conversion

	while(ADCSRA & (1 << ADSC));      // Thanks T, this line waits for the ADC to finish


	ADCval = ADCL;
	ADCval = (ADCH << 8) + ADCval;    // ADCH is read so ADC can be updated again

	return ADCval;
}

void handleSi1145Failure(unsigned char code){
	//char msg[400];
	switch(code){
		case SI1145_RESP_INVALID_SETTING:
		//sprintf (msg, "Invalid Setting %d \r\n", 0);
		break;
		case SI1145_RESP_PS1_ADC_OVERFLOW:
		//sprintf (msg, "PS ADC Overflow %d \r\n", 0);
		break;
		case SI1145_RESP_ALS_VIS_ADC_OVERFLOW:
		//sprintf (msg, "ALS VIS ADC Overflow %d \r\n", 0);
		break;
		case SI1145_RESP_ALS_IR_ADC_OVERFLOW:
		//sprintf (msg, "ALS IR Overflow %d \r\n", 0);
		break;
		case SI1145_RESP_AUX_ADC_OVERFLOW:
		//sprintf (msg, "AUX ADC Overflow %d \r\n", 0);
		break;
		default:
		//sprintf (msg, "Unknown Failure %d \r\n", 0);
		break;
	}
	//usart_pstr(msg); //disable printing errors for now
	
	clearFailure();
}

void initPorts(){
	DDRB = 0x00; // Set PORT B to input
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


