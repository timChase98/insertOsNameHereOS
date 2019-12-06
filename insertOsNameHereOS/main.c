/*
 _                     _    ____   _____                            _                    ____   _____
(_)                   | |  / __ \ / ____|                          | |                  / __ \ / ____|
 _ _ __  ___  ___ _ __| |_| |  | | (___  _ __   __ _ _ __ ___   ___| |__   ___ _ __ ___| |  | | (___
| | '_ \/ __|/ _ \ '__| __| |  | |\___ \| '_ \ / _` | '_ ` _ \ / _ \ '_ \ / _ \ '__/ _ \ |  | |\___ \
| | | | \__ \  __/ |  | |_| |__| |____) | | | | (_| | | | | | |  __/ | | |  __/ | |  __/ |__| |____) |
|_|_| |_|___/\___|_|   \__|\____/|_____/|_| |_|\__,_|_| |_| |_|\___|_| |_|\___|_|  \___|\____/|_____/

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "task.h"
#include "semiphore.h"

#define TICKTIME 100 // 1000 us = 1ms

#ifdef TICKTIME
#define CLK_DIV 8
#define TIMER_TICK_TIME_LOAD_VALUE (TICKTIME * 16 / CLK_DIV) // tick time * 16 CLK per us / clock divider
#endif

#define requestSemiphore(S) cli(); getSemi(S); sei();


// function prototypes
void tickTimerSetup();
void clrRam();
extern void os();
extern void yield();

// scary voodoo magic
extern void LOADSTATE();

//      void function named taskF with void parameter

void createTask(void (*taskF)(void), uint16_t priority);
void initStackForTask(int taskId);
void killTask(uint8_t taskId);
void getSemi(uint8_t s);
void semiphoreSetup();

void incrementTask();

void taskScheduler();
void blinkyTaskFunction();
void blinkyTask2Function();
void semiphoreTestFunction();

void digitA();
void digitB();
void digitC();
void digitD();
void LEDWhite();
void Speaker();

volatile Task taskArray[16] __attribute__((address (0x100)));// at start of RAM
uint8_t taskCounter __attribute__((address (0x400))); // at start of OS VAR space
uint8_t numberOfTasks __attribute__((address (0x401)));

volatile uint8_t i __attribute__((address (0x402)));

volatile uint8_t highestTaskNotServiced __attribute__((address (0x403)));
volatile uint8_t highestpriorityNotServiced __attribute__((address (0x404)));
volatile uint8_t highestPriority __attribute__((address (0x405))); 

volatile Task* pTask  __attribute__((address (0x406)));

volatile uint64_t semis __attribute__((address (0x408)));

int main(void)
{
	semiphoreSetup();

	clrRam();
	taskCounter = 0;
	numberOfTasks = 0;
	semis = 0;
	DDRC |= (1 << 0) | (1 << 3); // heartbeat pin
	DDRB |= (1 << 5) | (0b1111);
	DDRD = 0xFF;
	DDRE |= 1<<3;

	i = 0;

	

	createTask(taskScheduler, 0);
	createTask(blinkyTaskFunction, 16);
	createTask(blinkyTaskFunction, 14);
	createTask(blinkyTask2Function, 15);
	createTask(blinkyTask2Function, 13);
// 	createTask(semiphoreTestFunction);
// 	
// 	createTask(digitA);
// 	createTask(digitB);
// 	createTask(digitC);
// 	createTask(digitD);
// 	createTask(LEDWhite);
	//createTask(Speaker);
	
	tickTimerSetup();
	LOADSTATE();
	

	/* Replace with your application code */
	while (1)
	{
		i++;
	}
}

void semiphoreSetup(){
	PCICR = 1 << PCIE0 | 1 << PCIE3;
	PCMSK0 = 1 << PCINT0;
	PCMSK3 = 1 << PCINT24;

}

ISR(PCINT0_vect){
	if(PINB & 1){
		semis |= 1 << SEMI_PORTB_0;
		return;
	}
	semis= 0;
}
ISR(PCINT3_vect){
	if((PINE & 0x01) == 0){
		semis |= 1 << SEMI_PORTE_0;
		;
		return;
	}
	semis= 0;
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


void createTask(void (*taskF)(void), uint16_t priority){
	uint8_t firstOpenIndex = 0;
	Task t;
	
	// find the first open slot for a task
	while(1){
		t = taskArray[firstOpenIndex];
		if (t.taskFunction == 0) break;
		firstOpenIndex++;
		
	}
	
	t.taskID = firstOpenIndex; // tasks start at 1
	t.taskFunction = taskF;
	t.servicedThisRound = 0;
	t.waitingFor = (uint16_t *) NONE;
	t.priority = (uint16_t *) priority;
	t.state = RUNNING;
	t.programCounter = taskF;
	t.stackPointer = 0x043F + 64 * firstOpenIndex - 34;
	taskArray[numberOfTasks] = t;	
	
	highestPriority = (priority > highestPriority) ? priority : highestPriority; 

	initStackForTask(firstOpenIndex);

	numberOfTasks++;// increment total number of tasks
}

void initStackForTask(int taskId){
	// sketchy hard coded pointer stuff
	taskArray[taskId].stackPointer = 0x043F + 64 * taskId - 34;
	uint16_t defNotAPtrtoTaskF = taskArray[taskId].taskFunction; // compilers suck
	uint8_t* ptrToStackIntial = 0x043F + 64 * taskId - 1;
	*ptrToStackIntial = defNotAPtrtoTaskF >> 8;
	ptrToStackIntial += 1;
	*ptrToStackIntial = defNotAPtrtoTaskF & 0xFF;
}

void killTask(uint8_t taskId){
	// verify its a real task and not the os
	if(taskId >= numberOfTasks || taskId == 0) return;
	taskArray[taskId].taskFunction = NULL;
	
	
}
/*
void incrementTask(){

	while(1){
		taskCounter = (taskCounter + 1) % numberOfTasks;
		// if has a task function and is ready
		if(taskArray[taskCounter].taskFunction && taskArray[taskCounter].state == RUNNING){
			return;
		}
	}
}
*/

void incrementTask(){
	// find the highest priority unserviced task 
	highestTaskNotServiced = 0;
	highestpriorityNotServiced = 0;
	// if everything has been serviced run the task scheduler 
	taskCounter = 0; 
	
	// loop through all the tasks 
	// if everything has been serviced 
	for (i = 1; i < 16; i++){
		pTask = &taskArray[i];
		if(!(pTask->servicedThisRound) && pTask->priority > highestpriorityNotServiced){
			taskCounter = i; 
			highestpriorityNotServiced = pTask->priority;
		}
	}

	taskArray[taskCounter].servicedThisRound = 1; 

}

void getSemi(uint8_t s){
	taskArray[taskCounter].state = WAITING;
	taskArray[taskCounter].waitingFor = s;
}

void yieldReady(){
	taskArray[taskCounter].state = READY; 
	while(1); // wait
}

// do all the task scheduling
void taskScheduler(){
	while (1)
	{
		cli();
		for (i = 0; i < 16; i++)
		{
			pTask = &taskArray[i];
			pTask->servicedThisRound = 0; 
			if(pTask->state == WAITING){
			// if its waiting check if what its for is done 
				if ((semis >> pTask->waitingFor) & 0x01)
				{
				pTask->waitingFor = NONE;
					pTask->state = READY;
				}
			}else if (pTask->state == READY){
			// if its ready make it running 
				initStackForTask(i);
				pTask->state = RUNNING; 
			}
		}
		
		sei();
	}
}

void blinkyTaskFunction(){
		PORTB |= 1 << 5;
		yieldReady(); 

}
void blinkyTask2Function(){
	while (1) {
		PORTB &= ~(1 << 5);
	}
}

void digitA(){
	while (1) {
		PORTE &= ~(1 << 3);	// turn off speaker
		
		PORTC &= ~(1 << 3); // turn off LED
		PORTD = 0b01110111; // portd controls segments
		
		PORTB &= 0xF0;
		PORTB |= 1 << 0; // portb controls digit
	}
}

void digitB(){
	while (1) {
		PORTC &= ~(1 << 3); // turn off LED
		PORTD = 0b01111100; // portd controls segments
		
		PORTB &= 0xF0;
		PORTB |= 1 << 1; // portb controls digit
	}
}

void digitC(){
	while (1) {
		PORTC &= ~(1 << 3); // turn off LED
		PORTD = 0b01011000; // portd controls segments
		
		PORTB &= 0xF0;
		PORTB |= 1 << 2; // portb controls digit
	}
}

void digitD(){
	while (1) {
		PORTC &= ~(1 << 3); // turn off LED
		PORTD = 0b01011110; // portd controls segments
		
		PORTB &= 0xF0;
		PORTB |= 1 << 3; // portb controls digit
	}
}

void Speaker(){
	while (1) {
		for(uint16_t t = 0; t < 0xFFFF; t++){
			PORTE |= 1 << 3;
			PIND |= 1 << 2;
		}
		requestSemiphore(SEMI_PORTE_0);
	}
}

void LEDWhite(){
	while(1){
		PORTC |= (1 << 3); // enable anode
		PORTD &= ~(1 << 6); // turn on red
		PORTB &= ~(1 << 1); // turn on grn
		PORTB &= ~(1 << 3); // turn on blu
		requestSemiphore(SEMI_PORTE_0);
	}
}

void semiphoreTestFunction(){
	while(1){
		PINC |= 1 << 0;
		requestSemiphore(SEMI_PORTB_0);
	}
}
























//