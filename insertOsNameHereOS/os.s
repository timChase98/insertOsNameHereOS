
/*
 * Assembler1.s
 *
 * Created: 9/18/2019 11:18:24 AM
 *  Author: tchase
 */
  #include <avr/io.h>
  .equ STARTOFTASKLIST_L, 0x00; task list memory start at 0x0100
  .equ STARTOFTASKLIST_H, 0x01

  .equ STARTOFTASKSTATE_L 0x00; saved processor states start at 0x0200
  .equ STARTOFTASKSTATE_H 0x20

  ; OS global variables start at 0x0400
  ; Current Task @ 0x0400
  ; Number of Tasks @ 0x0401
  .equ TASKCOUNTERADDR_L, 0x00
  .equ TASKCOUNTERADDR_H, 0x04


 .global os
os:

	sbi _SFR_IO_ADDR(PINB), 5
	ret

.global TIMER4_COMPA_vect

TIMER4_COMPA_vect:
  ; save the status of the processor
  PUSH R0;
         ; R1 is zero and doenst need to be saved
  IN R1, _SFR_IO_ADDR(SREG); save the status register
  PUSH R1
  PUSH R16
  PUSH R17
  PUSH R18
  PUSH ZL; save the Z register
  PUSH ZH

  ; stack grows downward. to get something from from 4 pushes ago add 4 to SP
  IN ZL, _SFR_IO_ADDR(SPL); copy the stack pointer to the Z register
  IN ZH, _SFR_IO_ADDR(SPH);
  ADIW Z, 8; points to SP - 8, pushed PC from before call to interrupt
  LD R0, Z+; get low byte and increment
  LD R1, Z; get high byte
  PUSH R1; save the PC
  PUSH R0
  PUSH ZH; save the stack pointer
  PUSH ZL

  ; get the Task Counter
  LDI ZL, TASKCOUNTERADDR_L; load the address of the task counter into the pointer register
  LDI ZH, TASKCOUNTERADDR_H
  LD R17, Z+; get task counter
  LD R18, Z; get number of tasks
  SBIW Z, 1; point back to task counter


  ; calculate address of current task structure
  LDI R16, 16
  MUL R17, R16; multiply task counter by 16 and put the result in R0

  INC R17; increment task counter
  CP R17, R18
  BRLO TaskNotOverflowed ; brach if lower
  CLR R17; if taskCounter >= numberOfTasks reset to task 0
  TaskNotOverflowed:
  ST Z, R17

  LDI ZL, STARTOFTASKLIST_L + 10; stack pointer address in task list
  LDI ZH, STARTOFTASKLIST_H
  ; task counter to offset
  ADD ZL, R0;
  CLR R0;
  ADC ZH, R0; does a 16bit add correct address now in Z register

  ; save stack pointer and program counter to task structure memeory
  POP R0; low byte of SP
  ST Z+, R0
  POP R0 ; high byte of SP
  ST Z+, R0
  POP R0 ; low byte of PC
  ST Z+, R0
  POP R0 ; high byte of PC
  ST Z+, R0


  ; Restore registers
  POP ZH
  POP ZL
  POP R18
  POP R17
  POP R16
  POP R1; get SREG from stack
  OUT _SFR_IO_ADDR(SREG), R1
  CLR R1; clear R1 to 0
  POP R0
  reti

















  ;
