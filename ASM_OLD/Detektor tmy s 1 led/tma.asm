;pracuje jako 5-ti bitovy binarni citac
;vystupy 3 a 4 jsou prohozene kvuli desce
.device attiny13
.def	i=r20				;prvni citac, cita jen pro zpomaleni
.def	j=r21				;druhy citac, cita az do TOP, pak se oba vynulujou
.def	cnt=r22				;maximalni hodnota pro citac j -> vynulovani a dalsi faze

.org	0x0000				;RESET
	rjmp	RESET			;skok na start po resetu
.org	0x0006				;TC0 overflow A
	rjmp	CASOVAC 


.org	0x0010
RESET:
	ldi	r16,low(RAMEND)		;nastavi stack pointer
	out	SPL,r16

	cli				;zakazat vsechna preruseni

	ldi	r16,0b11111111		;smer pinu
	out	DDRB,r16

	ldi	r16,0b00000000		;PullUpy
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
	
	in r16, MCUCR
	ori r16, (1<<PUD)
	out MCUCR, r16			;pull-ups disabled
	
	clr i				;nastavime pocatecni hodnoty
	clr j

	sei				;Global Interrupt Enable
	
LOOP:
	rjmp LOOP

CASOVAC:
	inc i
	cpi i,100			;je i=100? ne->ceka se na dalsi interrupt
	  brne CASEND
	    clr i
	    inc j
	    cpi j,10
	    brne CASEND			;je j=10 (delay=1s)? ne -> ceka se dal
		clr cnt
		clr i
		clr j
		
	      	ldi r16,0b00000011		;outputy
		out DDRB,r16

		ldi r16,0b00000010		;obracene pripojeni led
		out PORTB,r16
		
		ldi r16,0b00000001		;input na PB1
		out DDRB,r16
		
cekani:		in r16,PINB			;kdyz je pinb1 zapnuty dele nez 150 cyklu tak je svetlo
		sbrs r16,1
		  rjmp tma
		inc cnt
		cpi cnt, 250
		brne cekani
		
		ldi r16,0b00000011		;svetlo -> zhasiname
		out DDRB,r16
		ldi r16,0b00000000		;led nesviti
		out PORTB,r16
		rjmp casend

tma:		ldi r16,0b00000011		;tma -> sekundu bude svitit
		out DDRB,r16
		ldi r16,0b00000001		;led sviti
		out PORTB,r16
		
     
CASEND:
	reti

