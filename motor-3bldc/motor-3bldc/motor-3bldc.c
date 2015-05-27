/*
* motor_3bldc.c
*
* Created: 15.01.01 16:05:43
*  Author: Administrator
*/

#define F_CPU 24000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>




//int a = 600;
char dir = 0;
char go = 0;
//char numd[6] = {6, 5, 5, 3, 3, 6};//0 = work; 7 = off
//char numc[6] = {32, 32, 8, 8, 16, 16};//1 = work; 0 = off
char numb[6] = {1, 2, 2, 4, 4, 1};
char numd[6] = {12, 12, 24, 24, 20, 20};
char curr = 0;
char count = 0;
int divider = 6;
int update = 0;

int main(void)
{
	TCCR1B |= (1 << WGM12) | (1 << CS10); // Configure timer 1 for CTC mode
	//TCCR1A |= (1 << COM1A0); // Enable timer 1 Compare Output channel A in toggle mode
	TIMSK1 |= (1 << OCIE1A); // Enable CTC interrupt
	OCR1A = 65535;
	
	//TCCR1A |= (1 << COM1A1);
	// set none-inverting mode
	//TCCR1A |= (1 << WGM11);
	//TCCR1B |= (1 << WGM12)|(1 << WGM13);
	
	
	//ICR1 = 1024;
	//OCR1A   = 128; // Set CTC compare value to 1Hz at 1MHz AVR clock, with a prescaler of 64
	//TCCR1B |= ((1 << CS10) | (1 << CS11)); // Start timer at Fcpu/64
	
	//TCCR1B |= (1 << CS10);
	// START the timer with no prescaler
	TCCR0A = _BV(COM0A1) | (1<<COM0B1) | _BV(WGM00);
	TCCR0B |=  (1<<CS00);
	OCR0A = 100; //current control; low val = low current [0-255]
	OCR0B = 255; //turn motor off or on
	
		//TCCR0A |= (1<<COM0A1) | (1<<COM0A0) | (1<<WGM00);
	
	
	DDRD = 0b11111100;//PORTD5/6 FOR PWM; init oscillator
	DDRD = 0b01111100; //PORTD5/6 FOR PWM; start oscillator
	//DDRC = 56;
	DDRB = 0b111;
	
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz

	ADMUX = (1 << REFS0); // Set ADC reference to AVCC
	//ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading

	// No MUX values needed to be changed to use ADC0

	//ADCSRA |= (1 << ADATE);  // Set ADC to Free-Running Mode
	ADCSRA |= (1 << ADEN);  // Enable ADC
	ADCSRA |= (1 << ADSC);  // Start A2D Conversions
	
	
	
	//TCCR0B |= (1 << CS02); // Start timer at Fcpu/1024
	//TCCR0A |= (1 << WGM01); // Configure timer 0 for CTC mode
	//TIMSK0 |= (1 << OCIE0A); // Enable CTC interrupt
	//OCR0A = 255;
	
	sei(); //  Enable global interrupts
	
	

	for(;;)  // Loop Forever
	{
		//cli();
		//sei();
		//_delay_ms(100);

	}
}
int g = 0;

double freq = 30;

ISR(TIMER1_COMPA_vect)
{
	PORTD = numd[curr];
	PORTB = numb[curr];
	++g;
	g = g%10;
	if(!g){
		PORTD = numd[curr];
		PORTB = numb[curr];

		++curr;
		curr = curr%6;
		
		int period = 1000.0/freq; //in milliseconds
		freq += 0.03*period;

		OCR1A = 16000000L/((int)freq*10);
		
		if(freq > 96*6){
			freq = 96*6; //960RPM=16FPSW
			OCR0A = 70;
		}
		
		

		
		
		
		//ADCSRA |= (1 << ADSC);
		//while (ADCSRA & (1 << ADSC));
		
		//OCR0A = ADC/4;
		

		//ADMUX++;

		
		//ADCSRA |= (1 << ADSC);
		//while (ADCSRA & (1 << ADSC));
		
		//OCR1A = ADC;
		

		//ADMUX--;
	}

}