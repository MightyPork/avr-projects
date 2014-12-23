.device attiny13
.def	cnt=r19

.org	0x0000				;RESET
	rjmp	RESET			;skok na start po resetu

.org	0x0010
RESET:
	ldi	r16,low(RAMEND)		;nastavi stack pointer
	out	SPL,r16

	cli				;zakazat vsechna preruseni

	ldi	r16,0b00011110		;smer portu
	out	DDRB,r16

	ldi	r16,0			;vypnout PullUp
	out	PORTB,r16

	clr r16
	clr r17
	clr r18
	clr cnt

	sei				;Global Interrupt Enable
	
LOOP:
	in r16,PINB
	sbrc r16,0
	rjmp pinset
	rjmp LOOP
pinset:					;cekej, az bude 100 cyklu po sobe 1 na PB0
	clr cnt
pinset2:
	in r16,PINB
	sbrs r16,0
	rjmp loop
	inc cnt
	cpi cnt,200
	brne pinset2
	
	rcall blik
	
w2:					;cekani pro odruseni zakmitu - 40 cyklu
	inc cnt
	cpi cnt,200
	brne w2

	rjmp LOOP			;skok na loop

blik:
A:
	ldi r16,7			;1
B:
	  nop				;1
	  nop				;1
	  nop				;1
	  nop				;1
	  nop				;1
	  dec r16			;1
	 brne B				;2 (1 +
	nop				;1)
	sbi PINB,1			;2 -> toogle portb1
	nop				;1
	nop				;1
	in r17,pinb			;1
	sbrc r17,0			;1 (2 -> preruseno, nedulezite)
	rjmp A				;2
	cbi PORTB,1			;2
	ret				;4
