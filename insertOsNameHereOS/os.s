
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

.global yeild
yeild:
  JMP yeild ; Spin.  This should do something more usefull later


.global TIMER4_COMPA_vect

TIMER4_COMPA_vect:
  ; save the status of registers modified by the OS
    PUSH ZH
    PUSH ZL
    PUSH R18
    PUSH R17
    PUSH R16
    ; R1 is zero and doenst need to be saved
    IN R1, _SFR_IO_ADDR(SREG); save the status register
    PUSH R1
    PUSH R0;

  ; get the Task Counter into R17 and number of tasks into R18
    LDI ZL, TASKCOUNTERADDR_L; load the address of the task counter into the pointer register
    LDI ZH, TASKCOUNTERADDR_H
    LD R17, Z+; get task counter
    LD R18, Z; get number of tasks

  ; save the state of the processor into the saved state RAM
    LDI R16, 32
    MUL R17, R16; multiply task counter by 32 and put result in R0, R1
    LDI ZL, STARTOFSAVEDSTATE_L
    LDI ZH, STARTOFSAVEDSTATE_H
    ADD ZL, R0; 16bit add
    ADC ZH, R1

    POP R0; saved R0 from jmp
    ST Z+ R0
    POP R0; saved SREG from JMP
    ST Z+, R0
    ST Z+, R2
    ST Z+, R3
    ST Z+, R4
    ST Z+, R5
    ST Z+, R6
    ST Z+, R7
    ST Z+, R8
    ST Z+, R9
    ST Z+, R10
    ST Z+, R11
    ST Z+, R12
    ST Z+, R13
    ST Z+, R14
    ST Z+, R15

    POP R0; saved R16
    ST Z+, R0
    POP R0; saved R17
    ST Z+, R0
    POP R0; saved R18
    ST Z+, R0

    ST Z+, R19
    ST Z+, R20
    ST Z+, R21
    ST Z+, R22
    ST Z+, R23
    ST Z+, R24
    ST Z+, R25
    ST Z+, R26
    ST Z+, R27

    POP R0; saved ZL
    ST Z+, R0
    POP R0; saved ZH
    ST Z+, R0

  ; get the stack pointer and program counter
    ; program counter is on the stack from jump to interrupt
    IN ZL, _SFR_IO_ADDR(SPL); copy the stack pointer to the Z register
    IN ZH, _SFR_IO_ADDR(SPH);
    PUSH ZH; save the stack pointer
    PUSH ZL


  ; calculate address of current task structure
    LDI R16, 16
    MUL R17, R16; multiply task counter by 16 and put the result in R0
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

  INC R17; increment task counter
  CP R17, R18
  BRLO TaskNotOverflowed ; brach if lower
  CLR R17; if taskCounter >= numberOfTasks reset to task 0
  TaskNotOverflowed:
  LDI ZL, TASKCOUNTERADDR_L
  LDI ZH, TASKCOUNTERADDR_H
  ST Z, R17

  ; Load saved state
    ; get saved PC and SP
      LDI R16, 16
      MUL R17, R16; multiply task counter by 16 and put the result in R0
      LDI ZL, STARTOFTASKLIST_L + 12; program counter address in task list
      LDI ZH, STARTOFTASKLIST_H
      ; task counter to offset
      ADD ZL, R0;
      CLR R0;
      ADC ZH, R0; does a 16bit add correct address now in Z register
      LD R0, Z+; PCL
      PUSH R0
      LD R0, Z; PCH
      PUSH R0

      ; subtract 4 to get the stack pointer
      SBIW Z, 4
      LD R0, Z+; SPL
      PUSH R0
      LD R0, Z; SPH
      PUSH R0

    ; calculate start of saved state for current task
      LDI R16, 32
      MUL R17, R16; multiply task counter by 32 and put result in R0, R1
      LDI ZL, STARTOFSAVEDSTATE_L
      LDI ZH, STARTOFSAVEDSTATE_H
      ADD ZL, R0; 16bit add
      ADC ZH, R1

    ; restore register
      LD RO, Z+
      LD R1, Z+; saved status register
      OUT _SFR_IO_ADDR(SREG), R1
      LD, R2, Z+
      LD, R3, Z+
      LD, R4, Z+
      LD, R5, Z+
      LD, R6, Z+
      LD, R7, Z+
      LD, R8, Z+
      LD, R9, Z+
      LD, R10, Z+
      LD, R11, Z+
      LD, R12, Z+
      LD, R13, Z+
      LD, R14, Z+
      LD, R15, Z+
      LD, R16, Z+
      LD, R17, Z+
      LD, R18, Z+
      LD, R19, Z+
      LD, R20, Z+
      LD, R21, Z+
      LD, R22, Z+
      LD, R23, Z+
      LD, R24, Z+
      LD, R25, Z+
      LD, R26, Z+
      LD, R27, Z+
      LD, R28, Z+
      LD, R29, Z+
      LD R1, Z+ ; ZL
      PUSH R1;
      LD ZH, Z,; ZH
      MOV ZL, R1

    ; restore stack pointer 
      POP R1; SPH
      OUT _SFR_IO_ADDR(SPH), R1
      POP R1; SPL
      OUT _SFR_IO_ADDR(SPL), R1
      CLR R1

    ; PC shoudld be on stack

  reti

















  ;
