.device attiny2313
;běží na 8MHz, ckdiv8=1 (vypnuto)

; A L I A S Y
.def numL		= r20
.def numH		= r21
.def dispL		= r22
.def dispH		= r23
.def wOld		= r24
.def InitLoopN		= r25

.equ CUvodniIntervaly   = 60	;pocet intervalu cekani
.equ CInitLoopMax	= 180	;delka 1 intervalu cekani (s)
.equ CUnitSecs		= 60	;delka 1 jednotky (s) - zacina se s 99 jednotkami

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
	rjmp RESET			;skok na start po resetu

.org	0x0004				;TC1 Compare match A
	rjmp TIMER

.org	0x0010
; NASTAVENI PO RESETU
.DB	"Ondrej Hruska (C) 2010"
RESET:
	ldi	r16,low(RAMEND)		;nastavi stack pointer
	out	SPL,r16

	cli				;zakazat vsechna preruseni

; Nastaveni portu
	ldi r16,0b11111111		;smer portu B - vystupni: segmenty (=katody)
	out DDRB,r16
	ldi r16,0b11111111		;vse v PORTB na 1, segmenty zhasnuty
	out PORTB,r16
	
	ldi r16,0b00000011		;smer portu D - PD0 a PD1 jsou vystupni - spinaji anody displeje. Zbytek: dratky. PD2 je spravny drat
	out DDRD,r16
	ldi r16,0b01111100		;enable pullup, 0 na anody = zhasnout. dratky pripojeny na GND, s pullupy
	out PORTD,r16

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
	sei				;Global Interrupt Enable
	
	; Navesti, ze se povedl start
	;           DEChGAFB
	ldi dispH,0b00011000
	ldi dispL,0b10010111
	ldi r16,100
nav:
	ldi r17,255
nav2:
	 rcall multiplex
	 dec r17
	 brne nav2
	dec r16
	brne nav

	ldi r16,0b00000000		;disable pullupy a 0 na anody. Musime setrit energii!
	out PORTD,r16
	clr r16
	clr r17
	
	; INIT LOOP
	ldi InitLoopN, CUvodniIntervaly	;cekej n intervalu
	ldi r19,CInitLoopMax		;delka prvniho intervalu jako obecny interval
	;soucasny interval: r19, defaultni interval: CInitLoopMax
	TimIntEnable			;zapni casovac (makro)
InitLoop:
	;kontroluje se InitLoopN. snizovani zajistuje casovac.
	cpi InitLoopN,0			;zbyva 0 intervalu?
	brne InitLoop			;pokud ne, testuj znova

; P R I P R A V Y   N A   H L A V N I   P R O G R A M
	cli				;vypni preruseni - skoncila  uvodni smycka, zacina odpocitavani
	;vycisteni registru
	clr r16				;vynuluj registry
	clr r17
	clr r18
	clr r19
	
	;pullupy PORTD
	ldi r16,0b01111100		;enable pullup
	out PORTD,r16

	;cas 99
	ldi numL,9
	ldi numH,9

	mov r17,numL			;zpracovat numL
	rcall dec7seg			;najdi 7seg. reprezentaci jednotek a dej ji do r16
	mov dispL,r16			;presunout vysledek do vystupu displeje
	
	mov r17,numH			;zpracovat numH
	rcall dec7seg			;najdi 7seg. reprezentaci jednotek a dej ji do r16
	mov dispH,r16			;presunout vysledek do vystupu displeje

	;vychozi rozlozeni dratu
	in wOld,PIND			;nastaveni stareho rozlozeni dratu
	andi wOld,0b01111000		;ignorovat anody a spravny drat
	;nastaveni vychoziho rozlozeni dratu k prestrihnuti
	mov r16, wOld			;uloz wOld -> r16
	;v r16 jsou pouze rozpojene draty: 0b00010000
	in r17, PORTD			;do r17 stav portu D
	mov r18,r16
	com r18
	and r17,r18			;v r17 vynuluj bity patrici k rozpojenym dratum (ochrana X zkratu)
	out PORTD,r17			;dej na vystup
	
	in r17, DDRD			;do r17 DDRD
	or r17, r16			;k vystupnim pinum pridej prave rozpojeny drat
	out DDRD,r17
	
	ldi r19,CUnitSecs		;nastav citac intervalu pro hlavni odpocet
	
	ldi r16,0b11111111		;255 na PORTB, vse zhasnuto
	out PORTB, r16

	sei				;zacina hlavni program s casovacem, bude se pouzivat preruseni

loop:	
	rcall WireTest			;test dratku

	sbic PIND,2			;pokud je spravny drat PD2 = 0, preskoc
	rjmp CHCIP			;skoc na CHCIP
	;spravny drat je neprerusen
	
	rcall multiplex

	rjmp loop			;hlavni smycka se opakuje

multiplex:
	push r16
	push r17
;DISPLEJ JEDNOTEK
	ser r16				;katody na +: vypnout vsechny segmenty
	out PORTB,r16
	cbi PORTD,0
	sbi PORTD,1			;PORTD = 01 - zapnout anodu pro jednotky
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
	cbi PORTD,1			
	sbi PORTD,0			;PORTD 10 - zapnout anodu pro desitky
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
	cbi PORTD,0
	cbi PORTD,1
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

; TEST DRATU (nutno volat rucne)
WireTest:
	push r16
	push r17
	push r18
	in r16,PIND
	andi r16, 0b01111000		;odfiltrovat anody a spravny drat
	;test dratku
	cp wOld,r16			;porovnej r16 a wOld
	brsh WireTestReturn		;wOld >= r16: nebyl prestrizen dratek, nebo byl nejaky spojen
	  ;dratek byl prestrizen
	  ;ODECTENI 3 desitek
	  cpi numH,3			;jsou aspon 3 desitky?
	  brlo WireTestMaloCasu		;nejsou-li ani 3 desitky, vynuluj vse a vybuchni
	    subi numH,3			;jinak odecti 3 desitky

	    ;nedoslo k vynulovani?
	    brne WireTestReturn		;desitky > 0: odejdi bez vybuchu
	      cpi numL,0		;desitky = 0. kolik je jednotek?
	      breq WireTestBum		;je-li 0 jednotek, vybuchni
	        ;0 desitek, n > 0 jednotek
		rjmp WireTestReturn	;0 desitek ale n > 0 jednotek, odejdi bez vybuchu
WireTestMaloCasu:
	clr numL			;vymazat jednotky
	clr numH			;vymazat desitky
WireTestBum:
	rcall BUM			;vybuchnout
WireTestReturn:
	mov wOld, r16			;uloz soucasny stav dratu do wOld
	;v r16 jsou pouze rozpojene draty: 0b00010000
	in r17, PORTD			;do r17 stav portu D
	mov r18,r16
	com r18
	and r17,r18			;v r17 vynuluj bity patrici k rozpojenym dratum (ochrana X zkratu)
	out PORTD,r17			;dej na vystup
	
	in r17, DDRD			;do r17 DDRD
	or r17, r16			;k vystupnim pinum prodej prave rozpojeny drat
	out DDRD,r17
	
	;obnovit displej
	mov r17,numL			;zpracovat numL

	rcall dec7seg			;najdi 7seg. reprezentaci jednotek a dej ji do r16
	sbrc dispL,4			;neni-li H nastaven na displeji, preskoc
	sbr r16,0b00010000		;nastaveni H v novem stavu displeje
	mov dispL,r16			;presunout vysledek do vystupu displeje
	
	mov r17,numH			;zpracovat numL
	rcall dec7seg			;najdi 7seg. reprezentaci jednotek a dej ji do r16
	sbrc dispH,4			;neni-li H nastaven na displeji, preskoc
	sbr r16,0b00010000		;nastaveni H v novem stavu displeje
	mov dispH,r16			;presunout vysledek do vystupu displeje

	pop r18
	pop r17
	pop r16
	ret				;navrat

; B U M
BUM:
	;vybuch - blikajici nuly
	cli				;vypni preruseni

	ldi r17,0			;0 jednotek
	rcall dec7seg			;najdi 7seg. reprezentaci jednotek a dej ji do r16
	sbr r16,0b00010000		;1 na katodu H - zhasnout tecku
	mov dispL,r16			;presunout vysledek do vystupu displeje
	
	
	ldi r17,0			;0 desitek
	rcall dec7seg			;najdi 7seg. reprezentaci jednotek a dej ji do r16
	sbr r16,0b00010000		;1 na katodu H - zhasnout tecku
	mov dispH,r16			;presunout vysledek do vystupu displeje

BUMsmycka: ;nekonecna smycka, blikaji nuly
	ldi r16, 255
BUMsmyckaMX:
	rcall multiplex
	dec r16
	brne BUMsmyckaMX

	ldi r16, 255
BUMsmyckaPAUZA:
	 ldi r17, 255
BUMsmyckaPAUZAb:
	 dec r17
	 brne BUMsmyckaPAUZAb
	dec r16
	brne BUMsmyckaPAUZA

	rjmp BUMsmycka

; C H C I P
CHCIP:
	;nekdo prestrihl spravny drat
	cli
	;ldi dispL,0b00000000
	;ldi dispH,0b00000000
	ldi r16,180 ; citac delky cekani
CHCIPsmycka:
	 mov r17,r16
	 com r17
CHCIPsmyckaMX:
	 rcall multiplex
	 dec r17
	 brne CHCIPsmyckaMX

	mov r17,r16 ; nastaveni smycky cekani
	inc r16
	cpi r16,255
	brsh CHCIPsleep ;uz je to moc dlouhy, usnout

CHCIPsmyckaPAUZA: ;r17*200
	 mov r18,r16
CHCIPsmyckaPAUZAb:
	  dec r18
	  brne CHCIPsmyckaPAUZAb
	 dec r17
	 brne CHCIPsmyckaPAUZA
	rjmp CHCIPsmycka

CHCIPsleep:
	cli
	ldi r16,(1<<SE)|(1<<SM0)
	out MCUCR,r16
	sleep

; PRERUSENI OD CASOVACE f=1Hz
TIMER:  ; casovac ma frekvenci 1Hz
	push r16
	push r17
	push r18
	; r19 je citac intervalu
	cpi InitLoopN,0			;je InitLoopN = 0?
	breq TIMERnormal		;Je-li InitLoopN = 0, pokracuj normalnim prerusenim
	  ; probiha uvodni cekani
	  ; test intervaloveho citace (pokud je InitLoopN > 0, pouziva k tomu se r19)
	  cpi r19,0			;je r19 = 0?
	  breq TIMERsnizitILN		;je-li r19 = 0, snizit InitLoopN: jdi na TIMERsnizitILN
	    dec r19			;jinak o 1 sniz r19 (intervalovy citac)
	    ;out PORTB,r19		;DEBUG test r19
	    rjmp TIMERreturn		;a odejdi
TIMERsnizitILN:
	dec InitLoopN			;snizit InitLoopN
	ldi r19,CInitLoopMax		;nastaveni delky dalsiho intervalu
	rjmp TIMERreturn		;odejdi

TIMERnormal:
	;invertuj tecku (Low)
	ldi r16,0b00010000
	eor dispL,r16
	cbr dispH,0b00010000
	sbrc dispL,4			;do H zkopiruj tecku z L (tim se zajisti, ze budou vzdy stejne)
	sbr dispH,0b00010000

	; r19 slouzi jako citac sekund
	dec r19				;sniz citac intervalu o 1
	brne TIMERreturn		;neni-li 0, odejdi
	ldi r19,CUnitSecs		;nastaveni dalsiho intervalu
	; ODECTENI JEDNOTKY
	cpi numL,0			;je jednotek vic nez 0?
	brne TIMERnOdectiJednotku	;jednotky > 0, jdi na odecteni jednotky
	;0 jednotek
	cpi numH, 0			;0 jednotek. je desitek vic nez 0?
	brne TIMERnOdectiPresDesitku	;je-li desitek vic nez 0, jdi na odecteni desitky
	;0 jednotek i desitek
	rcall BUM

TIMERnOdectiPresDesitku:
	;0 jednotek, n > 0 desitek
	dec numH			;odecti desitku
	ldi numL,9			;dej jednotky=9
	rjmp TIMERnOdecteniHotovo	;preskoc na TIMERnOdecteniHotovo
	
TIMERnOdectiJednotku:
	dec numL			;odecti jednotku

TIMERnOdecteniHotovo:
	cpi numH,0			;test desitek
	brne TIMERnHotovoNevybuch	; neni nula -> ok
	cpi numL,0			;test jednotek
	brne TIMERnHotovoNevybuch	; neni nula -> ok
	;je nula
	rcall BUM			;je nula: vybuchni
	
TIMERnHotovoNevybuch:
	;uprav displej
	mov r17,numL			;zpracovat numL
	rcall dec7seg			;najdi 7seg. reprezentaci jednotek a dej ji do r16
	sbrc dispL,4			;neni-li H nastaven na displeji, preskoc
	sbr r16,0b00010000		;nastaveni H v novem stavu displeje
	mov dispL,r16			;presunout vysledek do vystupu displeje
	
	mov r17,numH			;zpracovat numH
	rcall dec7seg			;najdi 7seg. reprezentaci jednotek a dej ji do r16
	sbrc dispH,4			;neni-li H nastaven na displeji, preskoc
	sbr r16,0b00010000		;nastaveni H v novem stavu displeje
	mov dispH,r16			;presunout vysledek do vystupu displeje
	;je obnoven stav displeje, konec preruseni

TIMERreturn:
	pop r18				;navratit puvodni hodnoty registru
	pop r17
	pop r16
	reti