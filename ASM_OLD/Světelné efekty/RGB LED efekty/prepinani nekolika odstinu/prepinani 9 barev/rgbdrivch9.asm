;pracuje jako 5-ti bitovy binarni citac
;vystupy 3 a 4 jsou prohozene kvuli desce
.device attiny13
.def	cnt=r18
.def	RED=r19
.def	GREEN=r20
.def	BLUE=r21
.def	chc=r22
.def	faze=r23

.org	0x0000				;RESET
	RJMP	RESET			;skok na start po resetu
.org	0x0006				;TC0 overflow
	RJMP	CASOVAC 
.org 	0x0007
	rjmp	casovac

.org	0x0010
RESET:
	ldi	r16,low(RAMEND)		;nastavi stack pointer
	out	SPL,r16

	cli				;zakazat vsechna preruseni

	ldi	r16,0b00011111		;výstupní = 1
	out	DDRB,r16

	ldi	r16,0			;vypnout PullUp
	out	PORTB,r16

	ldi r16,0b00000011		;TC0 prescaler na 64
	out TCCR0B,r16			;4800000/32
	
	ldi r16,0b00000010		;TC0 je v rezimu CTC
	out TCCR0A,r16
	
	ldi r16,20			;overflow 22
	out OCR0A,r16			;
	
	in r16, TIMSK0			;interrupt enable
	ORI r16, 0b00000100
	out timsk0,r16
	
	ldi RED,0			;R
	ldi GREEN,16			;G
	ldi BLUE,32			;B
	clr cnt
	clr chc
	clr faze
	
	sei				;zapnout globalne interrupt
	
LOOP:
	rjmp LOOP			;skok na loop

CASOVAC:
	inc cnt
	
	cpi cnt,32
	breq ov
	rjmp nov
ov:
	clr cnt
	cbi portb,0
	cbi portb,1
	cbi portb,2

	inc chc
	cpi chc,150
	brne nov
	inc faze
	cpi faze,9
	brne fazenov
	clr faze
fazenov:
	cpi faze,0
	breq f0
	cpi faze,1
	breq f1
	cpi faze,2
	breq f2
	cpi faze,3
	breq f3
	cpi faze,4
	breq f4
	cpi faze,5
	breq f5
	cpi faze,6
	breq f6
	cpi faze,7
	breq f7
	cpi faze,8
	breq f8

f0:
	ldi RED,0
	ldi GREEN,32
	ldi BLUE,32
	rjmp nov
f1:
	ldi RED,0
	ldi GREEN,16
	ldi BLUE,32
	rjmp nov
f2:
	ldi RED,16
	ldi GREEN,0
	ldi BLUE,32
	rjmp nov
f3:
	ldi RED,32
	ldi GREEN,0
	ldi BLUE,32
	rjmp nov
f4:
	ldi RED,32
	ldi GREEN,0
	ldi BLUE,16
	rjmp nov
f5:
	ldi RED,32
	ldi GREEN,16
	ldi BLUE,0
	rjmp nov

f6:
	ldi RED,32
	ldi GREEN,32
	ldi BLUE,0
	rjmp nov
f7:
	ldi RED,16
	ldi GREEN,32
	ldi BLUE,0
	rjmp nov
f8:
	ldi RED,0
	ldi GREEN,32
	ldi BLUE,16
	rjmp nov

nov:
	cp cnt,RED
	brne notr
	sbi portb,0

notr:   cp cnt,GREEN
	brne notg
	sbi portb,1

notg:   cp cnt,BLUE
	brne notb
	sbi portb,2
notb:   
	reti

