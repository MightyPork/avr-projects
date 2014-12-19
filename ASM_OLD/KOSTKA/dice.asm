;pracuje jako 5-ti bitovy binarni citac
;vystupy 3 a 4 jsou prohozene kvuli desce
.device attiny13
.def	i=r20				;prvni citac, cita jen pro zpomaleni
.def	j=r21				;druhy citac, cita az do TOP, pak se oba vynulujou
.def	top=r22				;maximalni hodnota pro citac j -> vynulovani a dalsi faze
.def	faze=r23

.org	0x0000				;RESET
	rjmp	RESET			;skok na start po resetu
.org	0x0006				;TC0 overflow A
	rjmp	CASOVAC 


.org	0x0010
RESET:
	ldi	r16,low(RAMEND)		;nastavi stack pointer
	out	SPL,r16

	cli				;zakazat vsechna preruseni

	ldi	r16,0b11111110		;smer pinu
	out	DDRB,r16

	ldi	r16,0			;PullUpy
	out	PORTB,r16

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
	ldi faze,0

	sei				;Global Interrupt Enable
	
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
	      inc faze			;PODMINENY VYBER VYSTUPNIHO SYMBOLU
	      cpi faze,6
	      brne VYBER
	        ldi faze,0
VYBER:	      cpi faze,0
	      breq F0
	      cpi faze,1
	      breq F1
	      cpi faze,2
	      breq F2
	      cpi faze,3
	      breq F3
	      cpi faze,4
	      breq F4
	      cpi faze,5
	      breq F5

F0:	      ldi r16,0b00000010
	      out PORTB,r16
	      rjmp CASEND
F1:	      ldi r16,0b00000100
	      out PORTB,r16
	      rjmp CASEND
F2:	      ldi r16,0b00001010
	      out PORTB,r16
	      rjmp CASEND
F3:	      ldi r16,0b00001100
	      out PORTB,r16
	      rjmp CASEND
F4:	      ldi r16,0b00001110
	      out PORTB,r16
	      rjmp CASEND
F5:	      ldi r16,0b00011100
	      out PORTB,r16
CASEND:
	reti

