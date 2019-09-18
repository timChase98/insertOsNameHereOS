
/*
 * Assembler1.s
 *
 * Created: 9/18/2019 11:18:24 AM
 *  Author: tchase
 */ 
  #include <avr/io.h>

 .global os
os:
	sbi PINB, 5
	ret