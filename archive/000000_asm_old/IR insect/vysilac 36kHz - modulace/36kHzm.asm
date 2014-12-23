.device attiny13
.def	cnt=r19
.equ	clks=200			;100 kloknuti (36kHz) na 1 bit (200 preklopeni)

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
pinset:					;cekej, az bude 200 cyklu po sobe 1 na PB0
	clr cnt
pinset2:
	in r16,PINB
	sbrs r16,0
	rjmp loop
	inc cnt
	cpi cnt,200
	brne pinset2
	
	rcall blik
	rcall nblik
	rcall nblik
	rcall blik
	rcall blik
	rcall blik
	rcall blik
	rcall nblik
	rcall nblik
	rcall blik

	rcall nblik
	rcall nblik
	rcall nblik
	rcall nblik
	rcall nblik
	rcall nblik
	rcall nblik
	rcall nblik
	rcall nblik
	
pinset0:				;cekej, az bude 200 cyklu po sobe 0 na PB0
	;clr cnt
pinset3:
	;in r16,PINB
	;sbrc r16,0
	;rjmp pinset0
	;inc cnt
	;cpi cnt,200
	;brne pinset3

	rjmp LOOP			;skok na loop

blik:
	ldi r17,clks
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
	dec r17				;1
	breq endblik			;1 (2 -> preruseno, nedulezite)
	rjmp A				;2
endblik:
	cbi PORTB,1			;2
	ret				;4

nblik:
	ldi r17,clks
nA:
	ldi r16,7			;1

nB:
	  nop				;1
	  nop				;1
	  nop				;1
	  nop				;1
	  nop				;1
	  dec r16			;1
	 brne nB			;2 (1 +
	nop				;1)
	nop				;1
	nop				;1
	nop				;1
	nop				;1
	dec r17				;1
	breq endnblik			;1 (2 -> preruseno, nedulezite)
	rjmp nA				;2
endnblik:
	cbi PORTB,1			;2
	ret				;4


