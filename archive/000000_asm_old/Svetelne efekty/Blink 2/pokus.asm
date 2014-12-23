;pokus

.include	"tn13def.inc"

.def	POM=r16

.org	0x0000				;zacatek kodu
	RJMP	RESET			;skok na start po resetu


.org	0x0010				;zacatek vlastniho programu

RESET:	
	LDI	POM,low(RAMEND)		;nastavi stack pointer
	OUT  SPL,POM

	LDI  POM,0B00000011		;nastavi vystupni piny
	OUT  DDRB,POM
			
LOOP:
	SBI	PORTB,0			;PB0=0, PB1=1
	CBI	PORTB,1
	RCALL CEKANI			;CEKACI SUB
	CBI	PORTB,0			;PB0=1, PB1=0
	SBI	PORTB,1
	RCALL CEKANI			;CEKACI SUB
	RJMP	LOOP				;znovu na navesti LOOP

CEKANI:
	PUSH	r18				;ulozit pouzivane registry do STACKU
	PUSH	r17
	PUSH	r16
	
	LDI	r16, 100
L1:	DEC	r16
	LDI	r17, 100
L2:	DEC	r17
	LDI	r18, 125
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

	
