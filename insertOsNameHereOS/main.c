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


typedef enum __attribute__ ((__packed__)) {READY, RUNNING, WAITING, DONE} taskState;

typedef struct
{

								uint16_t taskID;
								void (*taskFunction)(Task);
								void (*placeHolder)(void);
								uint16_t state;
								uint16_t* stackPointer;
								void (*programCounter)(Task);// stored as function so it mem aligns correctly 
								uint16_t placeHolder2; 
								uint16_t placeHolder3; 

} Task;

// function prototypes
void tickTimerSetup();
void clrRam();
extern void os();
extern void yeild();
//      void function named taskF with void parameter
void createTask(void (*taskF)(void));


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
								DDRE |= 1 << 0; // heartbeat pin
								DDRB |= 1 << 5;

								i = 0;

								

								createTask(blinkyTaskFunction);
								createTask(blinkyTask2Function);

								tickTimerSetup();								
								sei();
								
								taskArray[0].taskFunction();

								/* Replace with your application code */
								while (1)
								{
																i++;
								}
}

void clrRam(){
	for(uint16_t* i = 0x100; i < 0x700; i++){
		*i = 0xAABB;
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
								Task t = taskArray[numberOfTasks]; // get memory for task
								t.taskID = numberOfTasks; // tasks start at 1
								t.taskFunction = taskF;
								t.placeHolder = 0xAABB; 
								t.placeHolder2 = 0xCCDD; 
								t.placeHolder3 = 0xEEFF; 
								t.state = WAITING;
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
