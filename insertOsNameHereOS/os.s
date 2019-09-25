
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
	pop R18;  get PC from stack 
	pop R19
	sbi		 _SFR_IO_ADDR(PINE),0				;toggle PE.0
	


    reti										;and done
