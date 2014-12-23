;pokus

.include	"m16def.inc"

.def	POM=r16
.def	A=r17
.def	B=r18
.def	C=r19
.def	D=r20

.cseg
.org	0x0000				;zacatek kodu
	RJMP	RESET			;skok na start po resetu


.org	0x0010				;zacatek vlastniho programu

RESET:	
	LDI	R16,0x5F			;nastavi stack pointer na 0x045F
	OUT	SPL,R16			;coz je konec SRAM u ATmegy16
	LDI	R16,0x04
	OUT	SPH,R16

	LDI  POM,0xFF			;nastavi vsechny porty jako vystupni
	OUT  DDRA,POM
	OUT	DDRB,POM
	OUT	DDRC,POM
	OUT	DDRD,POM

	LDI	A,0b01110000		;hodnoty portu A az D
	LDI	B,0b10001000
	LDI	C,0b10001000
	LDI	D,0b00000111
	
				
LOOP:
	CLC
	ROL	A				;preroluj A registr 		
	BRCC	ACC				;pokud je C=0, skoc na ACC (A Carry Clear)
	SBR	A,0b00000001		;nastav prvni bit v A na 1
ACC:	OUT	PORTA,A			;output A do PORTA

	CLC
	ROL	B				;preroluj B registr - totez co u A reg.		
	BRCC	BCC				
	SBR	B,0b00000001		
BCC:	OUT	PORTB,B			

	CLC
	ROL	C				;preroluj C registr - totez co u A reg.		
	BRCC	CCC				
	SBR	C,0b00000001		
CCC:	OUT	PORTC,C

	CLC
	ROL	D				;preroluj D registr - totez co u A reg.		
	BRCC	DCC				
	SBR	D,0b00000001		
DCC:	OUT	PORTD,D		
	RCALL	CEKANI
	RJMP	LOOP				;jdi na LOOP

CEKANI:
	PUSH	r18				;ulozit pouzivane registry do STACKU
	PUSH	r17
	PUSH	r16
		
	LDI	r16, 40
L1:	DEC	r16
	LDI	r17, 40
L2:	DEC	r17
	LDI	r18, 40
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


