.device attiny13
.def	a=r16
.def	b=r17
.def	i=r18
.def	j=r19
.def	k=r20
.def	secs=r21
.DEF	mins=r22

;běží na 4.8MHz

.MACRO cvrk1
	sbi portb,3
.ENDMACRO

.MACRO cvrk0
	cbi portb,3
.ENDMACRO

.MACRO osc1
	in a, TIMSK0
	ORI a, 0b00000100
	out timsk0,a
.ENDMACRO

.MACRO osc0
	clr a
	out timsk0,a
.ENDMACRO

.org	0x0000				;RESET
	rjmp	RESET			;skok na start po resetu

.org	0x0006				;TC0 overflow
	RJMP	TIMER

.org	0x0010
RESET:
	ldi	a,low(RAMEND)		;nastavi stack pointer
	out	SPL,a

	cli				;zakazat vsechna preruseni

	ldi	a,0b00001000		;smer portu
	out	DDRB,a

	ldi	a,0			;vymazat port
	out	PORTB,a

	clr a
	clr b
	clr i
	clr j
	clr k
	clr secs
	clr mins

	ldi r16,0b00000011		;TC0 prescaler na 64
	out TCCR0B,r16			;4800000/64=75000Hz
	
	ldi r16,0b00000010		;TC0 je v rezimu CTC
	out TCCR0A,r16
	
	ldi r16,75			;Compare Match 0 A je 75
	out OCR0A,r16			;75000/75=1000Hz interrupt
	
	osc0				;zakazeme interrupt
	
	in a,MCUCR			;zapne se vnitrni pullup
	ori a,0b01000000
	out MCUCR,a

	sei				;Global Interrupt Enable
	
	in a,PINB
	andi a,0b00000111
	
	;volba programu podle nastaveni prepinacu
	cpi a,0
	breq prog0
	cpi a,1
	breq prog1
	cpi a,2
	breq prog2
	cpi a,3
	breq prog3
	cpi a,4
	breq prog4
	cpi a,5
	breq prog5
	cpi a,6
	breq prog6
	rjmp prog7


prog0:					;5 min, (3 cvrky / 1 min pauza) x5, sleep
	ldi mins,5	;pauza úvod
	rcall minuty
	ldi k,5		;počet opakování
pd00:
	rcall scvrk
	rcall scvrk
	rcall scvrk
	ldi mins,1	;minuty pauza
	rcall minuty
	dec k
	brne pd00
	
	rjmp usni


prog1:					;2 min, (2 cvrk / 2 min pauza) x5, sleep
	ldi mins,2	;pauza úvod
	rcall minuty

	ldi k,6		;počet opakování
pd01:
	rcall scvrk
	rcall scvrk
	ldi mins,2	;minuty pauza
	rcall minuty
	dec k
	brne pd01
	
	rjmp usni


prog2:					;40 min, (2 cvrky / 20 sec pauza) x13, sleep
	ldi mins,40	;pauza úvod
	rcall minuty

	ldi k,13	;počet opakování
pd02:
	rcall scvrk
	rcall scvrk
	ldi secs,20	;sekundy pauza
	rcall sekundy
	dec k
	brne pd02
	
	rjmp usni
	
prog3:					;5 min, (2 cvrk / 5 min pauza) x8, sleep
	ldi mins,5	;pauza úvod
	rcall minuty

	ldi k,9		;počet opakování
pd03:
	rcall scvrk
	rcall scvrk
	rcall scvrk
	rcall scvrk
	ldi mins,5	;minuty pauza
	rcall minuty
	dec k
	brne pd03
	
	rjmp usni
	
prog4:					;10 min, (4 cvrk / 10 min pauza) x5, sleep
	ldi mins,10	;pauza úvod
	rcall minuty

	ldi k,5		;počet opakování
pd04:
	rcall scvrk
	rcall scvrk
	rcall scvrk
	rcall scvrk
	ldi mins,10	;minuty pauza
	rcall minuty
	dec k
	brne pd04
	
	rjmp usni
	
prog5:					;20 min, (4 cvrk / 10 min pauza) x1, sleep
	ldi mins,20	;pauza úvod
	rcall minuty

	rcall scvrk
	rcall scvrk
	rcall scvrk
	rcall scvrk

	rjmp usni
	
prog6: 					;0, (2 cvrk / 10s) x inf
	rcall scvrk
	rcall scvrk
	ldi secs,10	;minuty pauza
	rcall sekundy
	rjmp prog6
	
prog7: ;trvale
	cvrk1
infloop:
	rjmp infloop

usni:
	in a,MCUCR
	ori a,0b00110000
	andi a,0b01110111
	out MCUCR,a
	sleep
	rjmp usni			;kdyby se probudil tak zase usne
  
TIMER:					;pracuje se s prerusenim 1000x za sekundu
	inc i
	cpi i,100
	brne endcas
	  clr i
	  inc j
endcas:
	reti

scvrk:
	clr i
	clr j
	cvrk1
	osc1
del01:	cpi j,20			;konstanta pro dobu jednoho cvrku v desetinach sekundy
	brne del01
	osc0
	cvrk0
	ret

sekundy:
	clr i
	clr j
	osc1
del02:	cpi j,10			;konstanta pro dobu 1 sekundy
	brne del02
	dec secs
	breq endsecs
	clr i
	clr j
	rjmp del02
endsecs:
	osc0
	ret

minuty:
	ldi secs,60			;minuta ma 60 sekund -> secs=60
	rcall sekundy			;a vykoname cekani secs sekund
	dec mins
	breq endmins
	rjmp minuty
endmins:
	ret
	

