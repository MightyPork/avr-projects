.device attiny2313
;běží na 8MHz, ckdiv8=1 (vypnuto)

; A L I A S Y
.def numL		= r20
.def numH		= r21
.def dispL		= r22
.def dispH		= r23
.def wOld		= r24
.def InitLoopN		= r25

.equ CUvodniIntervaly   = 1	;pocet intervalu cekani
.equ CInitLoopMax	= 10	;delka 1 intervalu cekani (s)
.equ CUnitSecs		= 1	;delka 1 jednotky (s) - zacina se s 99 jednotkami

; Z A C A T E K   P R O G R A M U
.org	0x0000				;RESET
	rjmp	RESET			;skok na start po resetu

.org	0x0010
; NASTAVENI PO RESETU
RESET:
	ldi	r16,low(RAMEND)		;nastavi stack pointer
	out	SPL,r16

	cli				;zakazat vsechna preruseni

; Nastaveni portu
	ldi	r16,0b11111111		;smer portu B - vystupni: segmenty (=katody)
	out	DDRB,r16
	ldi	r16,0b11111111		;vse v PORTB na 1, segmenty zhasnuty
	out	PORTB,r16
	
	ldi	r16,0b00000011		;smer portu D - PB0 a PB1 jsou vystupni - spinaji anody displeje. Zbytek: dratky. PB2 je spravny drat
	out	DDRD,r16
	ldi	r16,0b11111100		;enable pullup, 0 na anody = zhasnout. dratky pripojeny na GND, s pullupy
	out	PORTD,r16

	sei				;Global Interrupt Enable
	
; P R I P R A V Y   N A   H L A V N I   P R O G R A M
	cli				;vypni preruseni - skoncila  uvodni smycka, zacina odpocitavani
	;vycisteni registru
	clr r16				;vynuluj registry
	clr r17
	clr r18
	clr r19

	;cas 99
	ldi numL,3
	ldi numH,7

	;vychozi rozlozeni dratu
	in wOld,PORTD			;nastaveni stareho rozlozeni dratu
	andi wOld,0b01111000		;ignorovat anody a spravny drat
	
	mov r17,numL			;zpracovat numL
	rcall dec7seg			;najdi 7seg. reprezentaci jednotek a dej ji do r16
	mov dispL,r16			;presunout vysledek do vystupu displeje

	
	mov r17,numH			;zpracovat numH
	rcall dec7seg			;najdi 7seg. reprezentaci jednotek a dej ji do r16
	mov dispH,r16			;presunout vysledek do vystupu displeje
	
	;dispL a dispH maji nastaveny obsah
	
	ldi r19,CUnitSecs		;nastav citac intervalu pro hlavni odpocet
	
	ldi r16,0b111111		;255 na PORTB, vse zhasnuto
	out PORTB, r16

	sei				;zacina hlavni program s casovacem, bude se pouzivat preruseni

loop:	
	rcall multiplex

	rjmp loop			;hlavni smycka se opakuje

multiplex:
	push r16
	push r17
;DISPLEJ JEDNOTEK
	ser r16				;katody na +: vypnout vsechny segmenty
	out PORTB,r16
	
	cbi PORTD,1
	sbi PORTD,0			;PORTD = 01 - zapnout anodu pro jednotky
	out PORTB,dispL			;PORTB = dispL (displej jednotek)
	ldi r17,100			;pocet smycek
dL:					;smycka
	nop
	nop
	nop
	dec r17
	brne dL
	
;DISPLEJ DESITEK
	ser r16				;katody na +: vypnout vsechny segmenty
	out PORTB,r16
	cbi PORTD,0			
	sbi PORTD,1			;PORTD 10 - zapnout anodu pro desitky
	out PORTB,dispH			;PORTB = dispH (displej desitek)
	ldi r17,100			;pocet smycek
dH:
	nop
	nop
	nop
	dec r17
	brne dH
	
	ser r16				;katody na +: vypnout vsechny segmenty
	out PORTB,r16

	pop r17
	pop r16
	ret

; r17 konvertuj do 7segmentovyho kodu pro displej se spol. anodou DECGAFB ---> r16
dec7seg: ;dec7seg(r17) -> r16
	clr r16				;do r16 budeme ukladat vystup, v r17 je vstupni cislo 0-9

	cpi r17,9			;je r17 = 9?
	brne dec7segN8			;neni -> pokracuj s 8
	;          DEChGAFB
	ldi r16, 0b01000000		;katody DCGAFB na 0; H = 0, ale bude prepsano pri vystupu
	rjmp dec7segReturn
dec7segN8:
	cpi r17,8			;je r17 = 8?
	brne dec7segN7			;neni -> pokracuj s 7
	;          DEChGAFB
	ldi r16, 0b00000000		;katody DECGAFB na 0; H = 0, ale bude prepsano pri vystupu
	rjmp dec7segReturn
dec7segN7:
	cpi r17,7			;je r17 = 7?
	brne dec7segN6			;neni -> pokracuj s 6
	;          DEChGAFB
	ldi r16, 0b11001010		;katody CAB na 0; H = 0, ale bude prepsano pri vystupu
	rjmp dec7segReturn
dec7segN6:
	cpi r17,6			;je r17 = 6?
	brne dec7segN5			;neni -> pokracuj s 5
	;          DEChGAFB
	ldi r16, 0b00000001		;katody DECGAF na 0; H = 0, ale bude prepsano pri vystupu
	rjmp dec7segReturn
dec7segN5:
	cpi r17,5			;je r17 = 5?
	brne dec7segN4			;neni -> pokracuj s 4
	;          DEChGAFB
	ldi r16, 0b01000001		;katody DCGAF na 0; H = 0, ale bude prepsano pri vystupu
	rjmp dec7segReturn
dec7segN4:
	cpi r17,4			;je r17 = 4?
	brne dec7segN3			;neni -> pokracuj s 3
	;          DEChGAFB
	ldi r16, 0b11000100		;katody CGFB na 0; H = 0, ale bude prepsano pri vystupu
	rjmp dec7segReturn
dec7segN3:
	cpi r17,3			;je r17 = 3?
	brne dec7segN2			;neni -> pokracuj s 2
	;          DEChGAFB
	ldi r16, 0b01000010		;katody DCGAB na 0; H = 0, ale bude prepsano pri vystupu
	rjmp dec7segReturn
dec7segN2:
	cpi r17,2			;je r17 = 2?
	brne dec7segN1			;neni -> pokracuj s 1
	;          DEChGAFB
	ldi r16, 0b00100010		;katody DEGAB na 0; H = 0, ale bude prepsano pri vystupu
	rjmp dec7segReturn
dec7segN1:
	cpi r17,1			;je r17 = 1?
	brne dec7segN0			;neni -> pokracuj s 0
	;          DEChGAFB
	ldi r16, 0b11001110		;katody CB na 0; H = 0, ale bude prepsano pri vystupu
	rjmp dec7segReturn
dec7segN0:
	;          DEChGAFB
	ldi r16, 0b00001000		;katody DECAFB na 0; H = 0, ale bude prepsano pri vystupu
dec7segReturn:
	ret