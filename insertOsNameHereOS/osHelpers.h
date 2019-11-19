/*
 * os.h
 *
 * Created: 11/6/2019 10:24:32 AM
 *  Author: tchase
 */ 

#include <stdlib.h>
#ifndef OS_H_
#define OS_H_

#define TICKTIME 1000 // 1000 us = 1ms

#ifdef TICKTIME
#define CLK_DIV 8
#define TIMER_TICK_TIME_LOAD_VALUE (TICKTIME * 16 / CLK_DIV) // tick time * 16 CLK per us / clock divider
#endif

#define requestSemiphore(S) cli(); getSemi(S); sei();


void tickTimerSetup();
void clrRam();
extern void os();
extern void yield();

// scary voodoo magic
extern void LOADSTATE();

//      void function named taskF with void parameter
void createTask(void (*taskF)(void));
void killTask(uint8_t taskId);
void getSemi(uint8_t s);
void semiphoreSetup();
void incrementTask();
void taskScheduler();





#endif /* OS_H_ */