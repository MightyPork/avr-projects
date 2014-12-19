;program cyklicky nacita z EEPROM adresy:0x00 hodnotu (0b00000101) a dava ji do PORTB.

.include	"tn13def.inc"

.cseg
.org	0x0000				;zacatek kodu
	RJMP	RESET			;skok na start po resetu


.org	0x0010				;zacatek vlastniho programu

RESET:	
	LDI	r16,low(RAMEND)	;nastavi stack pointer
	OUT  SPL,r16

	LDI	r16,0b00000111
	OUT	DDRB,r16

LOOP:
	LDI	r17,0
	RCALL EEread
	OUT	PORTB,r16	
	RJMP LOOP

;
;EEread je funkce pro cteni z EEPROM
;do r17 se ulozi adresa,
;zavola se EEread
;a v r16 se objevi hodnota, ktera je na adrese.
EEread:
	SBIC	EECR,1
	RJMP	EEread			;cekani dokud neni EEPROM ready

	OUT	EEARL, r17		;ulozit adresu z r17
	SBI	EECR,0			;nastavenim EERE zacina cteni
	IN	r16,EEDR			;cteni dat z EEDR do r16

	RET


.eseg
.org	0x00
.DB	0b00000101




	

