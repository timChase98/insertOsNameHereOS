/*
 * insertOsNameHereOS.c
 *
 * Created: 9/18/2019 9:06:33 AM
 * Author : tchase
 */ 

#include <avr/io.h>
#include "avr/interrupt.h"

#define TICKTIME 1000 // 1000 us = 1ms

#ifdef TICKTIME
	#define CLK_DIV 8
	#define TIMER_TICK_TIME_LOAD_VALUE (TICKTIME * 16 / CLK_DIV) // tick time * 16 CLK per us / clock divider 
#endif

struct Task
{
	int taskID;
	int priority;
	void* taskFunction(void); 
	};

// function prototypes
void tickTimerSetup();

int main(void)
{
	DDRB |= 1 << 5;
	
	tickTimerSetup();
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
    }
}

void tickTimerSetup(){
	TCCR4A = 0;
			// ctc mode		// clk / 8
	TCCR4B = (1 << WGM42) | (0b010 << CS40) ;
	OCR4A = TIMER_TICK_TIME_LOAD_VALUE; 
	TIMSK4 = 1 << OCIE4A; 
}


ISR(TIMER4_COMPA_vect){
	// heartbeat 
	PINB |= 1 << 5;
	
	
}