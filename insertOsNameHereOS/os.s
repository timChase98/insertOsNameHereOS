
/*
 * Assembler1.s
 *
 * Created: 9/18/2019 11:18:24 AM
 *  Author: tchase
 */
  #include <avr/io.h>
  .include "m328pbdef.inc"
  .equ STARTOFTASKLIST, 0x0100


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
  PUSH ZL; save the Z register
  PUSH ZH

  ; stack grows downward. to get something from from 4 pushes ago add 4 to SP
  MOVW Z, SP; copy the stack pointer to the Z register
  ADIW Z, 4; points to PC - 4, pushed PC from before call to interrupt
  LD R0, Z+; get low byte and increment
  LD R1, Z; get high byte

  ; Restore registers
  POP ZH
  POP ZL
  CLR R1; clear R1 to zero
  POP R1; get SREG from stack 
  OUT _SFR_IO_ADDR(SREG), R1
  CLR R1
  POP R0
  reti

















  ;
