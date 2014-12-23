;pokus

.include	"m16def.inc"

.def	POM=r16

.cseg
.org	0x0000				;zacatek kodu
	RJMP	RESET			;skok na start po resetu


.org	0x0010				;zacatek vlastniho programu

RESET:	
	LDI	R16,0x5F			;nastavi stack pointer na 0x045F
	OUT	SPL,R16			;coz je konec SRAM u ATmegy16
	LDI	R16,0x04
	OUT	SPH,R16

	LDI  POM,0xFF			;nastavi vsechny potrebne porty jako vystupni
	OUT  DDRA,POM
	OUT	DDRB,POM
						
LOOP:
	LDI	r16,0b01110111		;display 3 abcef
	LDI	r17,0b00001000
	OUT	PORTA,r16
	OUT	PORTB,r17	
	RCALL	cekani
	
	LDI	r16,0b01110110		;display 2
	LDI	r17,0b00000100
	OUT	PORTA,r16
	OUT	PORTB,r17	
	RCALL	cekani

	LDI	r16,0b00111111		;display 1
	LDI	r17,0b00000010
	OUT	PORTA,r16
	OUT	PORTB,r17	
	RCALL	cekani

	LDI	r16,0b00011110		;display 0	
	LDI	r17,0b00000001
	OUT	PORTA,r16
	OUT	PORTB,r17	
	RCALL	cekani
		
	RJMP	LOOP				;jdi na LOOP

CEKANI:
	PUSH	r18				;ulozit pouzivane registry do STACKU
	PUSH	r17
	PUSH	r16
		
	LDI	r16, 2
L1:	DEC	r16
	LDI	r17, 2
L2:	DEC	r17
	LDI	r18, 2
L3:	DEC	r18
	CPI	r18, 0
	BRNE	L3
	CPI	r17, 0
	BRNE	L2
	CPI	r16, 0
	BRNE	L1
	
	POP	r16				;nacist puvodni obsah registru ze STACKU
	POP	r17
	POP	r18

	RET					;navrat do hlavniho programu

