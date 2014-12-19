.device attiny13
.def	cnt=r17
.def	P0=r20
.def	P1=r21
.def	P2=r22
.def	P3=r23
.def	P4=r24
.def	chc=r18
.def	faze=r19

;### NASTAVENI ###
.equ	CChDelay=6			;DELAY pro zmenu barev - cim min tim rychlejsi je zmena
.equ	ColDepth=128			;Barevna hloubka - urcuje na kolik kroku se rozdeli prepinani 2 barev
.equ	TIMEROV=2			;porovnavaci hodnota pro timer - konstanta

.org	0x0000				;RESET
	rjmp	RESET			;skok na start po resetu
.org	0x0006				;TC0 overflow
	rjmp	CASOVAC 
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

	ldi r16,0b00000001		;TC0 prescaler na 1 - velice plynule, bez blikani
	out TCCR0B,r16			;4800000Hz
	
	ldi r16,0b00000010		;TC0 je v rezimu CTC
	out TCCR0A,r16
	
	ldi r16,TIMEROV			;overflow 32 clks - upravuje rychlost prepinani (konstanta)
	out OCR0A,r16			;
	
	in r16, TIMSK0			;interrupt enable
	ori r16, 0b00000100
	out timsk0,r16
	
	ldi P0,0			;0
	ldi P1,ColDepth			;1
	ldi P2,ColDepth			;2
	ldi P3,ColDepth			;3
	ldi P4,ColDepth			;4
	clr cnt
	clr chc
	clr faze			;faze: 0 = 0->1, 1=1->2, 2=2->3 atd
	
	sei				;zapnout globalne interrupt
	
LOOP:
	rjmp LOOP			;skok na loop

CASOVAC:
	inc cnt				;cnt=cnt+1
	
	cpi cnt,ColDepth		;cnt==ColorDepth? ANO->pokracovat, NE->skok na NOV
	brne nov

	clr cnt				;CNT je ColorDepth -> vymazat CNT
					;vynulovat LED kanaly:
	cbi portb,0			; (0)
	cbi portb,1			; (1)
	cbi portb,2			; (2)
	cbi portb,3			; (3)
	cbi portb,4			; (4)

	inc chc				;Zvysime citac pro zmenu barvy CHC o 1
	cpi chc,CChDelay		;CHC==delay? ANO->pokracovat, NE->skok na NOV
	brne nov			

	clr chc				;MENI SE BARVY

	cpi faze,0
	brne FazeNot0
;FAZE je 0
	inc P0
	dec P1
	cpi P1,0
	brne nov
	inc faze

FazeNot0:
	cpi faze,1
	brne FazeNot1
	;FAZE je 1
	inc P1
	dec P2
	cpi P2,0
	brne nov
	inc faze

FazeNot1:
	cpi faze,2
	brne FazeNot2
	;FAZE je 2
	inc P2
	dec P3
	cpi P3,0
	brne nov
	inc faze
FazeNot2:
	cpi faze,3
	brne FazeNot3
	;FAZE je 3
	inc P3
	dec P4
	cpi P4,0
	brne nov
	inc faze
FazeNot3:
	;FAZE je 4
	inc P4
	dec P0
	cpi P0,0
	brne nov
	clr faze

;sprava PWM kanalu - podminene zapnuti LED
nov:					;NOV label
	cp cnt,P0			;P0==CNT? ANO->zapni P0, NE->skok na NOT0
	brne not0
	sbi portb,0
not0:   				;NOTR label
	cp cnt,P1			;P1==CNT? ANO->zapni P1, NE->skok na NOT1
	brne not1
	sbi portb,1
not1:   				;NOTG label
	cp cnt,P2			;P2==CNT? ANO->zapni P2, NE->skok na NOT2
	brne not2			
	sbi portb,2
not2:   				;NOTG label
	cp cnt,P3			;P3==CNT? ANO->zapni P3, NE->skok na NOT3
	brne not3			
	sbi portb,4
not3:   				;NOTG label
	cp cnt,P4			;P4==CNT? ANO->zapni P4, NE->skok na NOT4
	brne endcounter			
	sbi portb,3
endcounter:   				;ENDCOUNTER label - konec preruseni
	reti				;navrat z obsluhy preruseni

