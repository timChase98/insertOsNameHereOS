/*
 * insertOsNameHereOS.c
 *
 * Created: 9/18/2019 9:06:33 AM
 * Author : tchase
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

#define TICKTIME 1000 // 1000 us = 1ms

#ifdef TICKTIME
	#define CLK_DIV 8
	#define TIMER_TICK_TIME_LOAD_VALUE (TICKTIME * 16 / CLK_DIV) // tick time * 16 CLK per us / clock divider 
#endif


typedef enum {READY, RUNNING, WAITING, DONE} taskState;

typedef struct
{
	
	uint16_t taskID;
	void (*taskFunction)(Task);
	void (*isTaskReady)(void);
	taskState state; 
	uint16_t* stackPointer; 
	uint16_t programCounter;
	void* data; 

} Task;

// function prototypes
void tickTimerSetup();
extern void os(uint16_t* pointerToTaskArray);
//      void function named taskF with void parameter
void createTask(void (*taskF)(void), uint8_t (*taskReady)(void));

void blinkyTaskFunction();
uint8_t blinkyTaskIsReady();


Task taskArray[16] __attribute__((address (0x100)));
uint8_t taskCounter = 0;
uint8_t numberOfTasks = 0; 

volatile uint8_t i = 0; 

int main(void)
{
	DDRE |= 1 << 0;		// heartbeat pin
	DDRB |= 1 << 5;
	
	
	
	tickTimerSetup();
	sei();
	
	createTask(blinkyTaskFunction, blinkyTaskIsReady); 
	
    /* Replace with your application code */
    while (1) 
    {
		i = i + 1;
    }
}

void tickTimerSetup(){
	TCCR4A = 0;
			// ctc mode		// clk / 8
	TCCR4B = (1 << WGM42) | (0b010 << CS40) ;
	OCR4A = TIMER_TICK_TIME_LOAD_VALUE; 
	TIMSK4 = 1 << OCIE4A; 
}

void createTask(void (*taskF)(void), uint8_t (*taskReady)(void)){
	Task t = taskArray[numberOfTasks]; // get memory for task
	t.taskID = numberOfTasks; // tasks start at 1
	t.taskFunction = taskF;
	t.isTaskReady = taskReady;
	t.state = WAITING; 
	t.programCounter = taskF;
	
	taskArray[numberOfTasks] = t;
	
	numberOfTasks++;// increment total number of tasks 
}


void blinkyTaskFunction(){
	PINB |= 1 << 5; 
}

uint8_t blinkyTaskIsReady(){
	return 1; 
	
}

