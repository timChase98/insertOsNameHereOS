/*
 * os.c
 *
 * Created: 11/6/2019 10:26:01 AM
 *  Author: tchase
 */ 
#include "osHelpers.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

void semiphoreSetup(){
	
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
	t.waitingFor = (uint16_t *) NONE;
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
		// if has a task function and is ready
		if(taskArray[taskCounter].taskFunction && taskArray[taskCounter].state == READY) break;
	}
}

void getSemi(uint8_t s){
	taskArray[taskCounter].state = WAITING;
	taskArray[taskCounter].waitingFor = s;

}


































//