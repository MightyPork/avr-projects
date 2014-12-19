.DEVICE ATtiny13
.LIST
.LISTMAC
;běží na 9.6MHz, ckdiv8=0 (zapnuto)
;avrdude příkaz pro FUSEs: -U lfuse:w:0x6a:m -U hfuse:w:0xff:m 
;LFUSE: 0x6A
;HFUSE: 0xFF

;aliasy pointerů
.def	ZH	= r31
.def	ZL	= r30
.def	YH	= r29
.def	YL	= r28
.def	XH	= r27
.def	XL	= r26
.def	A2	= r25
.def	A1	= r24


; Z A C A T E K   P R O G R A M U
.org	0x0000				;RESET
	rjmp RESET			;skok na start po resetu


; začátek kódu
.org	0x0010
RESET:	ldi r16,low(RAMEND)		;nastaví stack pointer
	out SPL,r16


;tělo programu


;věčná smyčka
F_LOOP:	rjmp F_LOOP


SHOW:
	;A1,A2 obsahují bajty k zobrazení
	ldi r16,8
NextB:	ror A1
	brcs A1on
	cbi PORTB,PB0
	rjmp A2proc
A1on:	sbi PORTB,PB0
A2proc:	ror A2
	brcs A2on
	cbi PORTB,PB1
	rjmp shift
A1on:	sbi PORTB,PB1
shift:	sbi PORTB,PB2
	nop
	cbi PORTB,PB2
	dec r16
	brne NextB
	sbi PORTB,PB3
	nop
	cbi PORTB,PB3
	ret

CharTbl:
	; GR  GR      GR  GR
	;   GR  GR      GR  GR
.db	0b00000000, 0b00000000




;cteni z EEPROM, v [r17 je ADRESA], v (r16 se objeví DATA)
;EEread:
;	sbic EECR,1
;	rjmp EEread			;cekani dokud neni EEPROM ready
;
;	out EEARL, r17		;ulozit adresu z r17
;	sbi EECR,0			;nastavenim EERE zacina cteni
;	in r16,EEDR			;cteni dat z EEDR do r16
;
;	ret


;zapis do EEPROM, v [R17 je ADRESA], v [R16 jsou DATA]
;EEwrite:
;	sbic EECR,EEPE
;	rjmp EEwrite			;cekani dokud neni EEPROM ready
;
;	cbi EECR,EEPM1			;nastavit programovaci mod
;	cbi EECR,EEPM0
;
;	out EEARL, r17			;dej adresu z r17 do EEARL
;	out EEDR, r16			;dej data z r16 do EEDR
;	sbi EECR,EEMPE			;master program enable
;	sbi EECR,EEPE			;program enable
;
;	ret



