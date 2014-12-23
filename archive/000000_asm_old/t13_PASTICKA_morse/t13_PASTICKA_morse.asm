;pokus

.include	"tn13def.inc"

.def	CNTt=r24				;CNTt pro tecku
.def CNTc=r25				;CNTc pro carku
.def	POM=r16

.org	0x0000				;zacatek kodu
	RJMP	RESET			;skok na start po resetu


.org	0x0010				;zacatek vlastniho programu

RESET:	
	LDI	POM,low(RAMEND)	;nastavi stack pointer
	OUT  SPL,POM

	LDI  POM,0B00000100		;nastavi vystupni piny
	OUT  DDRB,POM
	;PB0 tecka
	;PB1 carka
	;PB2 vystup
			
LOOP:
	IN	r16,PINB			;dej do r16 stav PINB
	SBRS r16,0			;neni-li bit 0 v r16 1, vymaz CNTt						
	CLR	CNTt				;
	INC	CNTt				;CNTt+1

	IN	r16,PINB			;dej do r16 stav PINB
	SBRS r16,1			;neni-li bit 1 v r16 1, vymaz CNTc
	CLR	CNTc				;
	INC	CNTc				;CNTc+1

	CPI	CNTt,100			;je-li CNTt=100,
	BREQ	TECKA			;jdi na TECKA

	CPI	CNTc,100			;je-li CNTc=100,
	BREQ CARKA

	RJMP	LOOP				;jsi na LOOP

	
KONEC:
	IN	r16,PINB			;r16=PINB
	SBRC r16,0b00000001		;je-li bit 0 v r16 0, praskoc nasledujici
	RJMP KONEC			;jdi na KONEC
	RJMP LOOP				;jdi na LOOP


TECKA:
	LDI	r16,0b00000100		;na PORTB dej 0b00000100
	OUT	PORTB,r16
	RCALL	CEKANI		;udelej 1x cekani
	CLR	r16				;vynuluj r16 a dej ho do PORTB
	OUT	PORTB,r16
	RCALL	CEKANI		;udelej 1x cekani - mezera mezi znaky
	RJMP	LOOP				;navrat na LOOP

CARKA:
	LDI	r16,0b00000100		;na PORTB dej 0b00000100
	OUT	PORTB,r16
	RCALL	CEKANI		;udelej 3x cekani
	RCALL	CEKANI
	RCALL	CEKANI
	CLR	r16				;vymaz r16 a dej ho na PORTB
	OUT	PORTB,r16
	RCALL	CEKANI		;udelej 1x cekani - mezera mezi znaky
	RJMP	LOOP


CEKANI:
	PUSH	r18				;ulozit pouzivane registry do STACKU
	PUSH	r17
	PUSH	r16
		
	LDI	r16, 30			
L1:	DEC	r16				;cekaci smycka vnejsi - zacatek
	LDI	r17, 100
L2:	DEC	r17				;   cekaci smycka stredni - zacatek
	LDI	r18, 130
L3:	DEC	r18				;      cekaci smycka vnitrni - zacatek
	CPI	r18, 0
	BRNE	L3				;      cekaci smycka vnitrni - konec
	CPI	r17, 0			
	BRNE	L2				;   cekaci smycka stredni - konec
	CPI	r16, 0
	BRNE	L1				;cekaci smycka vnejsi - konec
	
	POP	r16				;nacist puvodni obsah registru ze STACKU
	POP	r17
	POP	r18

	RET					;navrat z rutiny do kodu				

