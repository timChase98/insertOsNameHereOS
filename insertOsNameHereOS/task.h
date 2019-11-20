/*
 * task.h
 *
 * Created: 10/30/2019 10:19:12 AM
 *  Author: tchase
 */ 

#include <stdlib.h>
#ifndef TASK_H_

#define TASK_H_

typedef enum __attribute__ ((__packed__)) {READY, RUNNING, WAITING, DONE} taskState;

typedef struct
{

	uint16_t taskID;
	void (*taskFunction)(Task);
	uint16_t roundsWithoutService;
	uint16_t state;
	uint16_t* stackPointer;
	void (*programCounter)(Task);// stored as function so it mem aligns correctly
	uint16_t waitingFor;
	uint16_t priority;

} Task;




#endif /* TASK_H_ */