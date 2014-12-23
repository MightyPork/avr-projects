.device attiny13
.def	cnt=r17
.def	P0=r20
.def	chc=r18
.def	faze=r19

;### NASTAVENI ###
.equ	CChDelay=8			;DELAY pro zmenu barev - cim min tim rychlejsi je zmena
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
	clr cnt
	clr chc
	clr faze			;faze: 0 = nahoru 1=dolu
	
	sei				;zapnout globalne interrupt
	
LOOP:
	rjmp LOOP			;skok na loop

CASOVAC:
	inc cnt				;cnt=cnt+1
	
	cpi cnt,128			;cnt==128? ANO->pokracovat, NE->skok na NOV
	brne nov

	clr cnt				;CNT je 128 -> vymazat CNT
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
	cpi P0,85
	brne nov
	inc faze

FazeNot0:
	;FAZE je 1
	dec P0
	cpi P0,0
	brne nov
	clr faze

;sprava PWM kanalu - podminene zapnuti LED
nov:					;NOV label
	cp cnt,P0			;P0==CNT? ANO->zapni P0, NE->skok na NOT0
	brne endcounter
	sbi portb,0
	sbi portb,1
	sbi portb,2
	sbi portb,3
	sbi portb,4
endcounter:   				;ENDCOUNTER label - konec preruseni
	reti				;navrat z obsluhy preruseni

