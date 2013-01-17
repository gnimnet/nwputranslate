#ifndef _OPCODE_H
#define _OPCODE_H

#include <stdio.h>
#include <stdlib.h>

//here define the instructions (frist 4 bit)
#define	ADD	0
#define	AND	1
#define	IN	2
#define	OUT	3
#define	MOV	4
#define	SL	5
#define	LDI	6
#define	INC	7
#define	SUB	8
#define	OR	9
#define	RR	10
#define	SR	11
#define	LDA	12
#define	STA	13
#define	JMP	14
#define	BZC	15

//here define the OP mode num (Part in OP called "M")
#define OPMODE1	0
#define OPMODE2	1
#define OPMODE3	2
#define OPMODE4	3

#endif /* OPcode.h */
