
/*
 * Assembler1.s
 *
 * Created: 9/18/2019 11:18:24 AM
 *  Author: tchase
 */
  #include <avr/io.h>

  .equ STARTOFTASKLIST, 0x0100


 .global os
os:

	sbi _SFR_IO_ADDR(PINB), 5
	ret

.global TIMER4_COMPA_vect

TIMER4_COMPA_vect:
  push ZL; save value of Z register
  push ZH
  inc SP; Increment Stack pointer twice to get value from before pushing Z
  inc SP
  pop ZL; get PC from before interrupt vector call
  pop ZH
  SUBI SP, 2; decrement SP by two

	sbi		 _SFR_IO_ADDR(PINE),0				;toggle PE.0
  reti									           	;and done
