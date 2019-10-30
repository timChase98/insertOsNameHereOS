/*
* insertOsNameHereOS.c
*
* Created: 9/18/2019 9:06:33 AM
* Author : tchase
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "task.h"

#define TICKTIME 1000 // 1000 us = 1ms

#ifdef TICKTIME
#define CLK_DIV 8
#define TIMER_TICK_TIME_LOAD_VALUE (TICKTIME * 16 / CLK_DIV) // tick time * 16 CLK per us / clock divider
#endif


typedef enum __attribute__ ((__packed__)) {READY, RUNNING, WAITING, DONE} taskState;



// function prototypes
void tickTimerSetup();
void clrRam();
extern void os();
extern void yield();

// scary voodoo magic
extern void LOADSTATE();

//      void function named taskF with void parameter
void createTask(void (*taskF)(void));
void killTask(uint8_t taskId);

void incrementTask();

void taskScheduler();
void blinkyTaskFunction();
void blinkyTask2Function();


volatile Task taskArray[16] __attribute__((address (0x100)));// at start of RAM
uint8_t taskCounter __attribute__((address (0x400))); // at start of OS VAR space
uint8_t numberOfTasks __attribute__((address (0x401)));

volatile uint8_t i __attribute__((address (0x402)));

int main(void)
{
	clrRam();
	taskCounter = 0;
	numberOfTasks = 0;
	DDRC |= 1 << 0; // heartbeat pin
	DDRB |= 1 << 5;

	i = 0;

	

	createTask(taskScheduler);
	createTask(blinkyTaskFunction);
	createTask(blinkyTask2Function);

	tickTimerSetup();
	LOADSTATE();
	

	/* Replace with your application code */
	while (1)
	{
		i++;
	}
}

void clrRam(){
	// hella sketch
	for(uint16_t* i = 0x100; i < 0x700; i++){
		*i = 0x0000;
	}
}

void tickTimerSetup(){
	TCCR4A = 0;
	// ctc mode		// clk / 8
	TCCR4B = (1 << WGM42) | (0b010 << CS40);
	OCR4A = TIMER_TICK_TIME_LOAD_VALUE;
	TIMSK4 = 1 << OCIE4A;
	TCNT4 = 0;
}

void createTask(void (*taskF)(void)){
	uint8_t firstOpenIndex = 0; 
	Task t;
	
	// find the first open slot for a task 
	while(1){
		t = taskArray[firstOpenIndex];
		if (t.taskFunction == 0) break;
		firstOpenIndex++; 
		
	}
	
	t.taskID = numberOfTasks; // tasks start at 1
	t.taskFunction = taskF;
	t.placeHolder = (uint16_t *) 0xAABB;
	t.placeHolder2 = (uint16_t *) 0xCCDD;
	t.placeHolder3 = (uint16_t *) 0xEEFF;
	t.state = READY;
	t.programCounter = taskF;
	t.stackPointer = 0x043F + 64 * numberOfTasks - 34;
	
	// sketchy hard coded pointer stuff
	uint16_t defNotAPtrtoTaskF = taskF; // compilers suck
	uint8_t* ptrToStackIntial = 0x043F + 64 * numberOfTasks -1;
	*ptrToStackIntial = defNotAPtrtoTaskF >> 8;
	ptrToStackIntial += 1;
	*ptrToStackIntial = defNotAPtrtoTaskF & 0xFF;
	

	taskArray[numberOfTasks] = t;

	numberOfTasks++;// increment total number of tasks
}

void killTask(uint8_t taskId){
	// verify its a real task and not the os
	if(taskId >= numberOfTasks || taskId == 0) return;
	taskArray[taskId].taskFunction = NULL;
	
		
}

void incrementTask(){
	while(1){
		taskCounter = (taskCounter + 1) % numberOfTasks;
		// if has a task function 
		if(taskArray[taskCounter].taskFunction && taskArray[taskCounter].state == READY) break;
	}
}

// do all the task scheduling
void taskScheduler(){
	while (1)
	{
		cli();
		if(numberOfTasks == 3){
			killTask(2);
			createTask(blinkyTaskFunction);
			createTask(blinkyTask2Function);
		}
	
		sei();
	}
}

void blinkyTaskFunction(){
	while (1) {
		PORTB |= 1 << 5;
	}
}
void blinkyTask2Function(){
	while (1) {
		PORTB &= ~(1 << 5);
	}
}