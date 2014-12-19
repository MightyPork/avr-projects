.device attiny2313
;běží na 8MHz, ckdiv8=1 (vypnuto)

; A L I A S Y
.def numL	= r19
.def numH	= r20
.def dispL	= r21
.def dispH	= r22
.def wNow	= r23
.def wOld	= r24
.def InitLoopN	= r25

; M A K R A
.MACRO TimIntEnable
	ldi r16,(1<<OCIE1A)
	out TIMSK,r16			;povolit interrupt pri Compare Match A
.ENDMACRO

.MACRO TimIntDisable
	ldi r16,0
	out TIMSK,r16			;zakazat interrupt pri Compare Match A
.ENDMACRO

; Z A C A T E K   P R O G R A M U
.org	0x0000				;RESET
	rjmp	RESET			;skok na start po resetu

.org	0x0004				;TC1 Compare match A
	rjmp	TIMER

.org	0x0010
; NASTAVENI PO RESETU
RESET:
	ldi	r16,low(RAMEND)		;nastavi stack pointer
	out	SPL,r16

	cli				;zakazat vsechna preruseni

; Nastaveni portu
	ldi	r16,255			;smer portu B - vystupni: segmenty (=katody)
	out	DDRB,r16
	ser	r16			;r16=255 - vse na 1, segmenty zhasnuty
	out	PORTB,r16
	
	ldi	r16,0b00000011		;smer portu D - PB0 a PB1 jsou vystupni - spinaji anody displeje. Zbytek: dratky.
	out	DDRD,r16
	ldi	r16,0b11111100		;enable pullup, 0 na anody = zhasnout
					;dratky pripojeny na GND, s pullupy
	out	PORTD,r16

; 16bitovy citac - generator f=1Hz
	ldi r16,0			;port se neovlivnuje citacem
	out TCCR1A,r16
	ldi r16,(1<<WGM12)|(1<<CS12)	;TC1 prescaler na 256 (8000000/256=31250), CTC mod
	out TCCR1B,r16
	
	ldi r16,0b01111010		;Compare Match 1 A high
	ldi r17,0b00010010		;Compare Match 1 A low (celkem 31250, overflow f=1Hz)
	out OCR1AH,r16			;set high
	out OCR1AL,r17			;set low
	
	TimIntDisable			;vypni casovac (makro)

; Nastaveni promennych
	;cas 99
	ldi numL,9
	ldi numH,9
	
	;vychozi rozlozeni dratu
	in r16,PORTD
	andi r16, 0b11111100		;ignorovat anody
	mov wOld, r16			;nastaveni stareho rozlozeni dratu
	mov wNow, r16			;nastaveni soucasneho rozlozeni dratu
	
	sei				;Global Interrupt Enable
	
	; INIT LOOP
	ldi InitLoopN, 0		;cekej n intervalu
	ldi r18,60			;delka intervalu v sekundach
	mov r17,r18			;delka prvniho intervalu jako obecny interval
	;soucasny interval: r17, defaultni interval: r18.
	TimIntEnable			;zapni casovac (makro)
InitLoop:
	;kontroluje se InitLoopN. snizovani zajistuje casovac.
	cpi InitLoopN,0
	brne InitLoop

; HLAVNI PROGRAM
	TimIntDisable			;vypni casovac (makro) - skoncila  uvodni smycka, zacina odpocitavani
	;nastaveni
	clr r16				;vynuluj registry ovlivnene uvodnim cekanim
	clr r17
	clr r18

	TimIntEnable			;zacina hlavni program s casovacem.

	ser r16				;255 na PORTB, vse zhasnuto
	out PORTB, r16
loop:
	rjmp loop

; r17 konvertuj do 7segmentovyho kodu pro displej se spol. anodou DECGAFB ---> r16
dec7seg:
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


; TEST DRATU (nutno volat rucne)
WireTest:
	in r16,PORTD
	andi r16, 0b11111100		;odfiltrovat anody
	mov wNow, r16			;nastaveni soucasneho rozlozeni dratu
	;test dratku
	cp wOld,wNow			;porovnej wOld a wNow
	brge WireTestReturn		;wOld >= wNow: nebyl prestrizen dratek, nebo byl nejaky spojen

	  ;dratek byl prestrizen
	  mov wOld, wNow		;nastaveni 
	  ;ODECTENI CASU
	  cpi numH,3			;jsou aspon 3 desitky?
	  brlo WireTestLowTens		;nejsou-li ani 3 desitky, vynuluj vse a vybuchni
	    subi numH,3			;jinak odecti 3 desitky
	    ;nedoslo k vynulovani?
	    brne WireTestReturn		;desitky > 0: odejdi bez vybuchu
	      cpi numL,0			;desitky = 0, kolik je jednotek?
	      breq WireTestBum		;je-li 0 jednotek, vybuchni
	        ;0 desitek, n > 0 jednotek
		rjmp WireTestReturn	;0 desitek ale n > 0 jednotek, odejdi bez vybuchu
WireTestLowTens:
	clr numL			;vymazat jednotky
	clr numH			;vymazat desitky
WireTestBum:
	rcall BUM
WireTestReturn:  
	ret

; B U M
BUM:
	;nekonecna smycka, blikaji nuly
	TimIntDisable			;vypni casovac (makro)
smycka:
	rjmp smycka


; PRERUSENI OD CASOVACE f=1Hz
	rjmp loop			; aby se TIMER nevykonal mimo preruseni, kdyby procesor zabloudil
TIMER:  ; casovac ma frekvenci 1Hz
	cpi InitLoopN,0			;je InitLoopN = 0?
	breq TIMERnormal		;Je-li InitLoopN = 0, pokracuj normalnim prerusenim
	  ; probiha uvodni cekani
	  ; test intervaloveho citace
	  cpi r17,0			;je r17 = 0?
	  breq TIMERsnizitILN		;je-li r17 = 0, snizit InitLoopN: jdi na TIMERsnizitILN
	    dec r17			;jinak o 1 sniz r17 (intervalovy citac)
	    ;out PORTB,r17		;DEBUG test r17
	    rjmp TIMERreturn		;a odejdi
TIMERsnizitILN:
	dec InitLoopN			;snizit InitLoopN
	mov r17,r18			;nastaveni delky dalsiho intervalu
	rjmp TIMERreturn		;odejdi
TIMERnormal:
	; STANDARTNI PRERUSENI (1s)
	sbi PINB,4 ;toogle PB4 (tecka na displeji)
TIMERreturn:
	reti