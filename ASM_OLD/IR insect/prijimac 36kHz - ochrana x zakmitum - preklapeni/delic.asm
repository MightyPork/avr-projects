;tohle ceka az bude na PB0 dlouho jednicka a pak invertuje PB1 a ceka na nulu na PB0
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

	ldi	r16,0			
	out	PORTB,r16

	clr r16
	clr r17
	clr r18
	clr cnt

	sei				;Global Interrupt Enable
	
LOOP:					;cekam na 0
	in r16,PINB
	sbrs r16,0
	rjmp pinset
	rjmp LOOP
pinset:					;cekej, az bude hodne cyklu po sobe 0 na PB0
	clr cnt
pinset2:
	 clr r20
pinset2b:
	  clr r21
pinset2c:
	      in r16,PINB
	      sbrc r16,0
	        rjmp loop
	   inc r21
	   cpi r21,2
	   brne pinset2c
	  inc r20
	  cpi r20,255
	  brne pinset2b
	 inc cnt
	 cpi cnt,255
	 brne pinset2
	
	sbi PINB,1			;invertuj to co je na PB1
	      
odrus0:					;cekam na 1
	in r16,PINB
	sbrc r16,0
	rjmp w2
	rjmp odrus0
	
w2:					;cekani pro odruseni zakmitu
	inc cnt
	cpi cnt,255
	brne w2

	rjmp LOOP			;skok na loop
