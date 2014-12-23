;pracuje jako 5-ti bitovy binarni citac
;vystupy 3 a 4 jsou prohozene kvuli desce
.device attiny13
.def	i=r20				;prvni citac, cita jen pro zpomaleni
.def	j=r21				;druhy citac, cita az do TOP, pak se oba vynulujou
.def	top=r22				;maximalni hodnota pro citac j -> vynulovani a dalsi faze

.org	0x0000				;RESET
	rjmp	RESET			;skok na start po resetu
.org	0x0006				;TC0 overflow A
	rjmp	CASOVAC 


.org	0x0010
RESET:
	ldi	r16,low(RAMEND)		;nastavi stack pointer
	out	SPL,r16

	cli				;zakazat vsechna preruseni

	LDI	r16,0b11111110		;smer pinu
	OUT	DDRB,r16

	LDI	r16,0			;PullUpy
	OUT	PORTB,r16

	ldi r16,(1<<CS01)|(1<<CS00)	;TC0 prescaler na 64
	out TCCR0B,r16			;4800000/64=75000Hz
	
	ldi r16,(1<<WGM01)		;TC0 je v rezimu CTC
	out TCCR0A,r16
	
	ldi r16,75			;Compare Match 0 A je 75
	out OCR0A,r16			;75000/75=1000Hz interrupt
	
	in r16, TIMSK0			;povolit interrupt od preteceni
	ori r16, (1<<OCIE0A)
	out timsk0,r16
	
	clr i				;nastavime pocatecni hodnoty
	clr j
	ldi top,2

	SEI				;Global Interrupt Enable
	
LOOP:
	in r16,PINB			;kontrola jestli je PINB0 zapnutej
	sbrs r16, 0
	  rjmp LOOP			;NE -> jdi na loop
	ldi top,1			;ANO -> nastavit top na 1 a vymazat citace
	clr i
	clr j
CEKANI:
	ldi top,2			;cekame na uvolneni, zatim se prepina s top=2
	in r16,PINB			;kontrola jestli je PINB0 vypnutej
	sbrc r16, 0
	  rjmp CEKANI
	rjmp LOOP			;skok na loop

CASOVAC:
	cpi top,40
	breq CASEND
	  inc i
	  cpi i,10
	  brne CASEND
	    clr i
	    inc j
	    cp j,top
	    brne CASEND
	      inc top
	      clr i
	      clr j
	      in r16,PORTB
	      sbrs r16,1
		sbi PORTB,1
	      sbrc r16,1
		cbi PORTB,1
CASEND:
	RETI

