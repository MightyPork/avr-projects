;EEPROM_nahoda
;pokud je stisknuto tlacitko PB0, 
;hrozne rychle se z EEPROM ctou hodnoty na adresach 0-3
;a davaji se na PORTB
;Vypada to pak, jako by se pri drzeni tlacitka rozsvitily obe ledky PB1 a PB2,
;ale po pusteni tlacitka na nich zustane 'nahodna' kombinace 1 a 0.

.include	"tn13def.inc"

.def	CNT=r20

.cseg
.org	0x0000				;zacatek kodu
	RJMP	RESET			;skok na start po resetu


.org	0x0010				;zacatek vlastniho programu

RESET:	
	LDI	r16,low(RAMEND)	;nastavi stack pointer
	OUT  SPL,r16

	LDI	r16,0b00000110		;nastavi I/O nohy
	OUT	DDRB,r16

LOOP:
	RCALL	cekani		;zavolej cekani pro zpomaleni celeho programu
	IN	r16,PINB			;Porovnej stav PB0 s 1 a pokud neni rovno, jdi na LOOP
	ANDI	r16,0b00000001
	CPI	r16,0b00000001
	BRNE	LOOP

	MOV	r17,CNT			;do r17 (adresovy registr) dej CNT
	RCALL EEread			;zavolej cteni z EEPROM - adresa je r17, do r16 se ulozi data.
	OUT	PORTB,r16			;posli data z r16 ven

	INC	CNT				;inkrementuj CNT
	CPI	CNT,4			;pokud je mensi nez 4, jdi na loop
	BRLT LOOP
	CLR	CNT				;jinak vynuluj CNT
	RJMP LOOP				;a jdi na LOOP

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
;zapis do EEPROM, v R17 je ADRESA, v R16 jsou DATA
EEwrite:
	SBIC EECR,EEPE
	RJMP EEwrite			;cekani dokud neni EEPROM ready

	LDI	r16, 0			;nastavit programovaci mod
	OUT EECR, r16

	OUT EEARL, r17			;dej adresu z r17 do EEARL
	OUT EEDR, r16			;dej data z r16 do EEDR
	SBI EECR,EEMPE			;master program enable
	SBI EECR,EEPE			;program enable

	RET

;cekaci cyklus
CEKANI:
	PUSH	r18				;ulozit pouzivane registry do STACKU
	PUSH	r17
	PUSH	r16
		
	LDI	r16, 3			
L1:	DEC	r16				;cekaci smycka vnejsi - zacatek
	LDI	r17, 3
L2:	DEC	r17				;   cekaci smycka stredni - zacatek
	LDI	r18, 3
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

	RET					;navrat do hlavniho programu

.eseg					;zacatek ESEGu
.org	0x00					;adresa 0
.DB	0b00000000			;tady se definujou byty postupne podle adres.
.DB	0b00000010
.DB	0b00000100
.DB	0b00000110




	

