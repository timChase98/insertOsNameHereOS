/*
 * task.h
 *
 * Created: 10/30/2019 10:19:12 AM
 *  Author: tchase
 */ 

#include <stdlib.h>
#ifndef TASK_H_

#define TASK_H_
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




#endif /* TASK_H_ */