;pracuje jako 5-ti bitovy binarni citac
;vystupy 3 a 4 jsou prohozene kvuli desce
.device attiny13
.def	cnt=r18
.def	cm0=r19
.def	cm1=r20
.def	cm2=r21
.def	chc=r22
.def	dirs=r23

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
	
	ldi cm0,32			;R
	ldi cm1,16			;G
	ldi cm2,0			;B
	clr cnt
	clr chc
	ldi dirs,0b00000000
	
	sei				;zapnout globalne interrupt
	
LOOP:
	rjmp LOOP			;skok na loop

CASOVAC:
	inc cnt
	
	cpi cnt,32
	brne nov
	
	clr cnt
	cbi portb,0
	cbi portb,1
	cbi portb,2
	inc chc
	cpi chc,255
	brne nov
	
	
	
nov:
	cp cnt,cm0
	brne not0
	sbi portb,0

not0:   cp cnt,cm1
	brne not1
	sbi portb,1

not1:   cp cnt,cm2
	brne not2
	sbi portb,2
not2:   
CASEND:
	reti

