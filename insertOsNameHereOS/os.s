
/*
 * Assembler1.s
 *
 * Created: 9/18/2019 11:18:24 AM
 *  Author: tchase
 */
  #include <avr/io.h>
  .equ STARTOFTASKLIST_L, 0x00; task list memory start at 0x0100
  .equ STARTOFTASKLIST_H, 0x01

  .equ STARTOFTASKSTATE_L, 0x00; saved processor states start at 0x0200
  .equ STARTOFTASKSTATE_H, 0x02

  ; OS global variables start at 0x0400
  ; Current Task @ 0x0400
  ; Number of Tasks @ 0x0401
  .equ TASKCOUNTERADDR_L, 0x00
  .equ TASKCOUNTERADDR_H, 0x04

 .global LoadSTATE

 .global os
os:

	sbi _SFR_IO_ADDR(PINB), 5
	ret

.global yeild
yeild:
  JMP yeild ; Spin.  This should do something more usefull later


.global TIMER4_COMPA_vect

TIMER4_COMPA_vect:
  ; save the registers
	IN R1, _SFR_IO_ADDR(SREG); R1 is zero and doesnt need to be saved 
	PUSH R31
	PUSH R30
	PUSH R29
	PUSH R28
	PUSH R27
	PUSH R26
	PUSH R25
	PUSH R24
	PUSH R23
	PUSH R22
	PUSH R21
	PUSH R20
	PUSH R19
	PUSH R18
	PUSH R17
	PUSH R16
	PUSH R15
	PUSH R14
	PUSH R13
	PUSH R12
	PUSH R11
	PUSH R10
	PUSH R9
	PUSH R8
	PUSH R7
	PUSH R6
	PUSH R5
	PUSH R4
	PUSH R3
	PUSH R2
	PUSH R1
	PUSH R0

  ; get the Task Counter into R17 and number of tasks into R18
    LDI ZL, TASKCOUNTERADDR_L; load the address of the task counter into the pointer register
    LDI ZH, TASKCOUNTERADDR_H
    LD R17, Z+; get task counter
    LD R18, Z; get number of tasks

  ; get the stack pointer
    IN ZL, _SFR_IO_ADDR(SPL); copy the stack pointer to the Z register
    IN ZH, _SFR_IO_ADDR(SPH);
    PUSH ZH; save the stack pointer
    PUSH ZL


  ; calculate address of current task structure
    LDI R16, 16
    MUL R17, R16; multiply task counter by 16 and put the result in R0
    LDI ZL, STARTOFTASKLIST_L + 8; stack pointer address in task list
    LDI ZH, STARTOFTASKLIST_H
    ; task counter to offset
    ADD ZL, R0;
    ADC ZH, R1; does a 16bit add correct address now in Z register

  ; save stack pointer and program counter to task structure memeory
    POP R0; low byte of SP
	POP R1; high byte of SP 
    ST Z+, R0
    ST Z+, R1

   INC R17; increment task counter
  CP R17, R18
  BRLO TaskNotOverflowed ; brach if lower
  CLR R17; if taskCounter >= numberOfTasks reset to task 0
  TaskNotOverflowed:
  LDI ZL, TASKCOUNTERADDR_L
  LDI ZH, TASKCOUNTERADDR_H
  ST Z, R17

  LOADSTATE:
  ; Load saved state
    ; get saved SP
      LDI R16, 16
      MUL R17, R16; multiply task counter by 16 and put the result in R0
      LDI ZL, STARTOFTASKLIST_L + 8; program counter address in task list
      LDI ZH, STARTOFTASKLIST_H
      ; task counter to offset
      ADD ZL, R0;
      ADC ZH, R1; does a 16bit add correct address now in Z register
      LD R0, Z+; SPL
      PUSH R0
      LD R0, Z; SPH
      PUSH R0


    ; restore stack pointer 
      POP R1; SPH
      POP R0; SPL
      OUT _SFR_IO_ADDR(SPH), R1
      OUT _SFR_IO_ADDR(SPL), R0
      

	POP R0
	POP R1
	OUT _SFR_IO_ADDR(SREG), R1; put the Status register back 
	POP R2
	POP R3
	POP R4
	POP R5
	POP R6
	POP R7
	POP R8
	POP R9
	POP R10
	POP R11
	POP R12
	POP R13
	POP R14
	POP R15
	POP R16
	POP R17
	POP R18
	POP R19
	POP R20
	POP R21
	POP R22
	POP R23
	POP R24
	POP R25
	POP R26
	POP R27
	POP R28
	POP R29
	POP R30
	POP R31
	CLR R1
    ; PC shoudld be on stack

  reti

















  ;
