;Kostka pro attiny13
;sestupna hrana impulzu na PB0 spousti kostku, vzestupna hrana spousti zpomalovani
;kostka se zastavi na nahodnem cisle.
;segmenty:

; PB2     PB3
; PB4 PB1 PB4
; PB3     PB2

; diody na spolecne noze maji kazda svuj odpor. muze byt pole odporu

.device attiny13
.def	i=r20				;prvni citac, cita jen pro zpomaleni
.def	j=r21				;druhy citac, cita az do TOP, pak se oba vynulujou
.def	top=r22				;maximalni hodnota pro citac j -> vynulovani a dalsi faze
.def	faze=r23			;faze kostky. jsou zde cisla 0-5
.def	plus=r24			;prirustek v dobe cekani na timer overflow - vzdy se inkrementuje
.def	fx=r25

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

	ldi	r16,1			;PullUpy
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
	ldi fx,5
	ldi faze,0
	ldi plus,1

	sei				;Global Interrupt Enable
	
LOOP:
	inc fx
	cpi fx,6
	brne fxok
	  ldi fx, 0
fxok:
	in r16,PINB			;kontrola jestli je PINB0 vypnutej (stisknuto)
	andi r16, 1
	sbrc r16, 0
	  rjmp LOOP			;NE -> jdi na loop
	mov faze, fx
	ldi top,1			;ANO -> nastavit top na 1 a vymazat citace
	ldi plus,1
	clr i
	clr j
CEKANI:
	ldi top,2			;cekame na uvolneni, zatim se prepina s top=2
	ldi plus,1

	inc fx
	cpi fx,6
	brne fxok2
	  ldi fx, 0
fxok2:
	in r16,PINB			;kontrola jestli je PINB0 zapnutej (pusteny tlacitko)
	andi r16, 1
	sbrs r16, 0
	  rjmp CEKANI
	mov faze, fx
	ldi top,1
	ldi plus,1
	clr i
	clr j
	rjmp LOOP			;skok na loop

CASOVAC:
	cpi top,50			;porovnam top s 50, kdyz vetsi nebo rovno
	brge CASEND			;tak se dal nepocita - preskok na konec
	  inc i				;i++
	  cpi i,8			;je i == 8 ?
	  brne CASEND			; ne -> konec, citam dal
	    clr i			;i=0
	    inc j			;j++
	    cp j,top			;je j == top?
	    brne CASEND			; ne -> konec, citam dal
	      add top,plus		;k top prictu plus
	      inc plus			;a plus zvysim o 1
	      clr i			;vymazu pomocne
	      clr j
	      inc faze			;dalsi faze kostky
	      cpi faze,6		;je faze 6?
	      brne VYBER		; neni -> skok na VYBER
	        ldi faze,0		; je -> nuluju fazi
VYBER:	      cpi faze,0		;podminene se vybere, co se ma zobrazit
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

F0:	      ldi r16,0b00000011
	      out PORTB,r16
	      rjmp CASEND
F1:	      ldi r16,0b00000101
	      out PORTB,r16
	      rjmp CASEND
F2:	      ldi r16,0b00001011
	      out PORTB,r16
	      rjmp CASEND
F3:	      ldi r16,0b00001101
	      out PORTB,r16
	      rjmp CASEND
F4:	      ldi r16,0b00001111
	      out PORTB,r16
	      rjmp CASEND
F5:	      ldi r16,0b00011101
	      out PORTB,r16
CASEND:					;navrat z preruseni
	reti

