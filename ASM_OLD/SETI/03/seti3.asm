;SETI navigation module of extraterrestrial spaceship BY Ondrej Hruska (c) 2011
;-------------------------------------------------
;Co to dela:
;Po spusteni plynule rozsviti a zhasle LED MAIN, behem toho houka s promennou frekvenci
;Povoli uspani zmenou stavu pinu PB7 




; Zapojeni:
;                   +------u------+
;    Vcc -> reset --+ /RST    Vcc +-- napajeni +5V
;           SW0 G --+ PD0     PB7 +---[wakeswitch]--GND
;           SW1 G --+ PD1     PB6 +-- LED MAIN K
;                 --+ PA1     PB5 +-- LED 5 K
;  Vcc-[speaker]----+ PA0     PB4 +-- LED 4 K
;           SW2 G --+ PD2     PB3 +-- LED 3 K
;           SW3 G --+ PD3     PB2 +-- LED 2 K
;           SW4 G --+ PD4     PB1 +-- LED 1 K
;           SW5 G --+ PD5     PB0 +-- LED 0 K
;             GND --+ GND     PD6 +-- 
;                   +-------------+
;"K" = katoda (ledka zapojena mezi Vcc a timhle pinem, aktivni je 0)
;"G" = ground (spinac mezi tomhle pionem a zemi, aktivni je 0)

.device attiny2313
;běží na 4MHz, ckdiv8=1 (vypnuto)

;LFUSE: 0xE2
;HFUSE: 0xDF



;N A S T A V E N I   V Y S I L A N E   M O R S E O V K Y
.equ DelkaTecky = 4			;delka tecky v desetinach
.equ DelkaCarky = 13			;3x delka tecky = delka carky
.equ MezeraMorspart = 4			;mezera mezi teckami/carkami v pismene
.equ MezeraZnak = 14			;mezera za znakem (pripojena za vnitrni mezeru)
.equ FrekvenceMorse = 43		;62500/tohle Hz



;K O N S T A N T Y   +   P R E Z D I V K Y   P O U R T U   A   P I N U
;PORTA
.equ SYSPORT = PORTA
.equ SYSPIN = PINA
.equ SYSDDR = DDRA

.equ speak = 0


;PORTB
.equ LEDPORT = PORTB
.equ LEDDDR = DDRB
.equ LEDPIN = PINB

.equ led0 = 0
.equ led1 = 1
.equ led2 = 2
.equ led3 = 3
.equ led4 = 4
.equ led5 = 5
.equ ledm = 6
.equ wakepin = 7
.equ AFTER_SLEEP = 0b10101010		;informace ze byl predtim sleep


;PORTD
.equ SWPORT = PORTD
.equ SWPIN = PIND
.equ SWDDR = DDRD

.equ sw0 = 0
.equ sw1 = 1
.equ sw2 = 2
.equ sw3 = 3
.equ sw4 = 4
.equ sw5 = 5



;A L I A S Y   R E G I S T R U
.def cmatch = r19
.def louter = r18
.def linner = r17
.def curchar = r25
.def morspart = r24

;aliasy pointerů
.def 	ZH		= r31
.def 	ZL		= r30
.def 	YH		= r29
.def 	YL		= r28
.def 	XH		= r27
.def 	XL		= r26



; M A K R A
;Reprak (zapnuti a vypnuti pipani 1kHz)
.MACRO BuzzOn
	sbi SYSPORT,speak
	ldi r16,(1<<CS01)|(1<<CS00)	;TC0 zapnut, prescaler na 64 (4000000/64=62500)
	out TCCR0B,r16
.ENDMACRO

.MACRO BuzzOff
	clr r16				;TC0 vypnut (hodiny zastaveny)
	out TCCR0B,r16
	sbi SYSPORT,speak		;vymazat speak (aby netekl proud reprakem)
.ENDMACRO

.MACRO SetTone
	out OCR0A,r16
.ENDMACRO

.MACRO DefaultTone
	ldi r16,FrekvenceMorse
	out OCR0A,r16
.ENDMACRO



; Z A C A T E K   P R O G R A M U
;vektory preruseni
.org	0x0000				;RESET
	rjmp RESET			;skok na start po resetu

.org	0x000B				;pin change
	rjmp PINCHANGE			;probuzeni nebo preruseni cinnosti

.org	0x000D				;TC0 Compare match A
	sbi SYSPIN,speak		;přepnout speak
	reti				;návrat do programu (0x000E)

.org	0x0013

;nastaveni po resetu
.DB	"MORSE PIPAC SETI 2011",0	;(nazev programu)
RESET:
	ldi r16,low(RAMEND)		;nastavi stack pointer
	out SPL,r16
	cli				;zakazat vsechna preruseni
	clr r5				;vymaz navesti ze je po probuzeni (r5=AFTER_SLEEP, ted bude 0)

	ldi r16, (1<<SM0)		;nastavi sleep mode = power-down (nejuspornejsi)
	out MCUCR, r16

; Nastaveni portu
	;PORTA = SYSPORT
	ldi r16,0b00000011		;smer portu A - vse na výstup (shutdown a speak)
	out SYSDDR,r16
	ldi r16,0b00000001		;shutdown=0 (continue), speak=1 (je mezi Vcc a timhle, takze skrz nej nic netece)
	out SYSPORT,r16

	;PORTB = LEDPORT
	ldi r16,0b01111111		;smer portu B - vystupni (ledky) - použijou se piny 0-6, 7. = wakepin
	out LEDDDR,r16
	ldi r16,0b11111111		;vse v PORTB na 1 = ledky zhasnuty, wakepin pullup enabled
	out LEDPORT,r16
	
	;PORTD = SWPORT
	ldi r16,0b00000000		;smer portu D - vse na vstup (tlačítka) - použijou se jen piny 0-5, 6. zbyde
	out SWDDR,r16
	ldi r16,0b01111111		;enable pullup
	out SWPORT,r16

; timer0 - generator f=1kHz (pipak)
	ldi r16,(1<<WGM01)		;port se neovlivnuje citacem, CTC mod
	out TCCR0A,r16
	
	ldi r16,62			;compare match = 62 (vystupni frekvence 1kHz)
	out OCR0A,r16

	ldi r16,(1<<OCIE0A)		;povolit interupt z casovace (jeste nutno zapnout: makro BuzzOn)
	out TIMSK,r16

	;pipani aktivuje makro BuzzOn, vypne makro BuzzOff
	
;konfigurace pinchange interruptu (jeste je treba zapnout PCIE v GIMSK)
	ldi r16,(1<<PCINT7)
	out PCMSK,r16			;povol pinchange interrupt pro PB7 (=wakepin)
	
	
	sei				;Global Interrupt Enable



;plynule rozsviceni ledm + stoupajici ton
	BuzzOn
	ldi cmatch,255			;comparematch (cita se od 255 do 0, tady se ledka vypne)
L1NEXTSTEP:
	dec cmatch			;zvys jas
	breq L1DONE			;je jas=255(full)? pak L1DONE
	mov r16,cmatch
	SetTone
	ldi louter,25			;vnejsi smycka (pocet cyklu se stejnym jasem)
L1OUTER:
		dec louter			;je konec louter? pak novy jas
		breq L1NEXTSTEP
		ldi linner,255			;vnitrni smycka, stara se o regulaci jasu
		cbi LEDPORT,ledm		;rozsviť ledmain
L1INNER:
			dec linner
			breq L1OUTER
			cp linner,cmatch		;je cmatch?
			brne L1Ib
			sbi LEDPORT,ledm		;jo -> zhasni ledmain
L1Ib:
			rjmp L1INNER

L1DONE: 
	cbi LEDPORT, ledm		;rozsvit ledku (pro jistotu)



;plynule zhasnuti ledm + klesajici ton
	ldi cmatch,0			;comparematch (cita se od 255 do 0, tady se ledka vypne)
L2NEXTSTEP:
	inc cmatch			;sniz jas
	cpi cmatch,255			;je jas=0? pak L2DONE
	breq L2DONE
	mov r16,cmatch
	SetTone
	ldi louter,20			;vnejsi smycka (pocet cyklu se stejnym jasem)
L2OUTER:
		dec louter			;je konec louter? pak novy jas
		breq L2NEXTSTEP
		ldi linner,255			;vnitrni smycka, stara se o regulaci jasu
		cbi LEDPORT,ledm		;rozsvit ledmain
L2INNER:
			dec linner
			breq L2OUTER
			cp linner,cmatch		;je cmatch?
			brne L2Ib
			sbi LEDPORT,ledm		;jo -> zhasni ledmain
L2Ib:
			rjmp L2INNER

L2DONE: 
	sbi LEDPORT, ledm		;zhasni ledm (pro jistotu)
	ldi r16,1			;prodleva, aby ton neznel tak uriznute
	rcall delay
	BuzzOff				;vypnout zvuk


;ted uz je dost douho po resetu, zepneme moznost uspani wakepinem
	;vymaz zasobnik ext. preruseni
	in r16,EIFR
	andi r16,(1<<PCIF)
	out EIFR,r16
	;povol interrupt změny pinu PB7
	ldi r16,(1<<PCIE)
	out GIMSK,r16			;povol pinchange interrupt (general)



;krouzeni cervenych ledek + pipani
	ldi r18,24			;celkem 24 preklopeni
	ldi r17,0b11111110		;posuvny registr - inicializace
KRUH1:
	sbr r17,0b11000000		;1 na nepouzite bity (ledky zhasnuty)
	out LEDPORT, r17		;vystup
	lsr r17				;rotuj doprava pres carry
	brcs KRUH1b			;neni carry? preskoc
	cbr r17,0b00100000		;dopln mezeru na zacatek (emulace 6bitoveho posuvneho registru)
KRUH1b:
	mov r16,r17			;vypocet tonu pipnuti
	com r16
	lsl r16
	eor r16,r18
	SetTone
	BuzzOn				;pipni
	ldi r16,2
	rcall delay
	BuzzOff
	dec r18
	brne KRUH1			;neni-li to posledni preklopeni, opakuj
K1DONE:
	DefaultTone			;nastav normalni ton = 1kHz (pro morseovku)


;zhasni vse
	ldi r16,0b11111111
	out LEDPORT,r16			;vypni ledky
	ldi r16,4
	rcall delay			;cekej 0.4s

;ukaz co je zapojeno (cervenymi ledkami)
	in r25, SWPIN			;prijmout stav SWPORTu
	andi r25,0b00111111		;ignoruj nepouzite piny
	mov r16,r25
	sbr r16,0b11000000
	out LEDPORT,r16			;ukazat na LEDPORTu
	
;cekej 2 sekundy
	ldi r16,20			;pauza 2s
	rcall delay
	
;spocitani zapnutych jumperu
	;r25 obsahuje stav SWPINu (bity 0-5)
	sbr r25,0b11000000		;zapni i nepouzite bity
	com r25				;neguj (jumpery jsou na zem) -> aktivni budou 1
	clr r24				;priprav citac aktivnich jumperu
	clr r16				;nulovej registr pro ADC (aby procetl jen carry)
COUNTL:
	lsr r25				;(r25>>1) bit 0 do carry
	adc r24,r16			;pricti k citaci r24 carry (r16 je 0)
	cpi r25,0			;je r25 prazdny? ne -> opakuj COUNTL
	brne COUNTL

	;r24 obsahuje pocet zaplych bitu
COUNTDONE:
	cpi r24,6			;je 6 jumperu?
	brne TEST5
	ldi ZH,high(TEXT6*2)
	ldi ZL,low(TEXT6*2)
	rjmp TESTDONE
TEST5:
	cpi r24,5			;je 5 jumperu?
	brne TEST4
	ldi ZH,high(TEXT5*2)
	ldi ZL,low(TEXT5*2)
	rjmp TESTDONE
TEST4:
	cpi r24,4			;jsou 4 jumpery?
	brne TEST3
	ldi ZH,high(TEXT4*2)
	ldi ZL,low(TEXT4*2)
	rjmp TESTDONE
TEST3:
	cpi r24,3			;jsou 3 jumpery?
	brne TEST2
	ldi ZH,high(TEXT3*2)
	ldi ZL,low(TEXT3*2)
	rjmp TESTDONE
TEST2:
	cpi r24,2			;jsou 2 jumpery?
	brne TEST1
	ldi ZH,high(TEXT2*2)
	ldi ZL,low(TEXT2*2)
	rjmp TESTDONE
TEST1:
	cpi r24,1			;je 1 jumper?
	brne TEST0
	ldi ZH,high(TEXT1*2)
	ldi ZL,low(TEXT1*2)
	rjmp TESTDONE
TEST0:
	ldi ZH,high(TEXT0*2)		;je 0 jumperu.
	ldi ZL,low(TEXT0*2)
TESTDONE:
	
;Z obsahuje pointer na zacatek vysilaneho textu.
;Dalsi znak textu (nebo prvni)
NextCharTxt:
	lpm curchar,Z+			;do curchar nacti soucasny znak. Z je pointer na dalsi
	cpi curchar,0
	breq EndCom			;curchar = 0? (konec komunikace) - jdi na konec
	
	push ZH				;uschovej Z pointer (bude pouzivan jako pointer tabulky)
	push ZL

;hledani znaku v tabulce
	ldi ZH,high(MORSE*2)		;zacatek MORSE tabulky do Z
	ldi ZL,low(MORSE*2)
NextCharTbl:
	lpm morspart,Z+			;precti znak z tabulky

	cpi morspart,0			;je to 0? (= konec tabulky)
	breq CharDone			;pak jdi na CharDone

	cp morspart,curchar		;je to hledany znak?
	breq CharFound			;pak jdi na CharFound
	adiw ZL,7			;jinak jdi na nasledujici znak v tabulce (1 z postincrementu + 7 dalsich bytu)
	rjmp NextCharTbl

;znak nalezen v tabulce
CharFound:
	lpm morspart,Z+			;nacti MorsPart
	cpi morspart,0			;je to 0? (= konec znaku)
	breq CharDone			;pak jdi na CharDone
	
	cpi morspart,1			;je to tecka? (=1) - zustan tady
	brne CARKA			;je to carka? (=3) - jdi na CARKA
;TECKA
	cbi LEDPORT, ledm		;rozsvit ledm
	BuzzOn				;zacni pipnuti
	ldi r16,DelkaTecky		;cekej podle DelkaTecky
	rcall delay
	BuzzOff				;konec pipnuti
	sbi LEDPORT, ledm		;zhasni ledm
	rjmp MorsPartDone		;konec morspartu -> MorsPartDone

;CARKA
CARKA:
	cbi LEDPORT, ledm		;rozsvit ledm
	BuzzOn				;zacni pipnuti
	ldi r16,DelkaCarky		;cekej podle DelkaTecky
	rcall delay
	BuzzOff				;konec pipnuti
	sbi LEDPORT, ledm		;zhasni ledm

;tecka/carka dokoncena
MorsPartDone:				;konec morspartu (=tecka/carka)
	ldi r16,MezeraMorspart		;udelej mezeru
	rcall delay
	rjmp CharFound			;jdi na dalsi cast morspartu

;znak dokoncen
CharDone:
	ldi r16,MezeraZnak		;mezera za znakem
	rcall delay

	pop ZL				;obnov pointer pro text
	pop ZH

	rjmp NextCharTxt		;jdi na dalsi znak textu

EndCom:
	ldi r16,10
	rcall delay


	BuzzOn
;plynule zhasnuti ledm + klesajici ton
	ldi cmatch,0			;comparematch (cita se od 255 do 0, tady se ledka vypne)
	in r24,LEDPORT			;uloz stav ledportu
	cbr r24,0b01000000		;zapni ledm
	sbr r24,0b10000000		;zapni pullup
	ldi r23,0b11111111		;priprav stav pro vypnute ledky
L3NEXTSTEP:
	inc cmatch			;sniz jas
	cpi cmatch,255			;je jas=255? pak L3DONE
	breq L3DONE
	mov r16,cmatch
	lsr r16
	SetTone
	ldi louter,25			;vnejsi smycka (pocet cyklu se stejnym jasem)
L3OUTER:
		dec louter			;je konec louter? pak novy jas
		breq L3NEXTSTEP
		ldi linner,255			;vnitrni smycka, stara se o regulaci jasu
		;cbi LEDPORT,ledm		;rozsvit ledmain
		out LEDPORT,r24			;zapni ledky
L3INNER:
			dec linner
			breq L3OUTER
			cp linner,cmatch		;je cmatch?
			brne L3Ib
			;sbi LEDPORT,ledm		;jo -> zhasni ledmain
			out LEDPORT,r23			;zhasni ledky
L3Ib:
			rjmp L3INNER

L3DONE: 
	sbi LEDPORT, ledm		;zhasni ledm (pro jistotu)
	ldi r16,2			;prodleva, aby ton neznel tak uriznute
	rcall delay
	BuzzOff				;vypnout zvuk





USNI:
;SLEEP (a po probuzeni skok na RESET)
	clr r16
	out SWPORT,r16			;vypni pullupy jumperu
	sbi SYSPORT, speak		;vypni speaker (pro jistotu)
	ser r16
	out LEDPORT,r16			;zhasni ledky, PB7 (ext. interrupt) pullup enabled
	

	;nastavit navesti, ze jde o probuzeni po spanku
	ldi r16,AFTER_SLEEP
	mov r5,r16
	;probudi se pinchange interruptem na PB7
	in r16, MCUCR
	ori r16, (1<<SE)
	out MCUCR, r16			;sleep enable && sleep

	sleep

	in r16, MCUCR
	andi r16, ~(1<<SE)
	out MCUCR, r16			;sleep disable

	;reset
	rjmp RESET

;R U T I N Y

delay:
;ldi r16,#desetin
	push r17
	push r18
	;255*225*7
d0:
		ldi r17,255
d1:
			ldi r18,227
d2:
			nop
			nop
			nop
			dec r18
			brne d2
		dec r17
		brne d1
	dec r16
	brne d0

	pop r18
	pop r17
	ret


PINCHANGE:
	mov r16,r5
	cpi r16,AFTER_SLEEP
	brne NENIPOSLEEPU

;je po SLEEPu -> reset
	ldi r16,2			;cekej 0.2s
	rcall delay
	clr r5				;vymazat navesti ze byl SLEEP
	clr r16
	out GIMSK,r16			;zakaz pinchange interrupt (general)
	sei
	rjmp RESET			;pri resetu se vynuluje SP a sei uz bylo, takze se reti delat nemusi. bude reset

;preruseni normalniho behu programu tlacitkem -> usnout
NENIPOSLEEPU:
	ser r16				;zhasne ledky
	out LEDPORT,r16
	BuzzOff				;pro jistotu vypne speaker
	ldi r16,20			;ceka 2 sekundy (aby se tlacitko urcite stihlo uvolnit -> jinak se udela dalsi "pinchange" a hned se to probudi)
	rcall delay
	sei				;povol preruseni (vypnuto pri interruptu)
	rjmp USNI			;usnout (probudi se zmenou PB7)



; T A B U L K A  Z N A K U
MORSE:
	.DB	"A",1,3,0,0,0,0,0	;A
	.DB	"B",3,1,1,1,0,0,0	;B
	.DB	"C",3,1,3,1,0,0,0	;C
	.DB	"D",3,1,1,0,0,0,0	;D
	.DB	"E",1,0,0,0,0,0,0	;E
	.DB	"F",1,1,3,1,0,0,0	;F
	.DB	"G",3,3,1,0,0,0,0	;G
	.DB	"H",1,1,1,1,0,0,0	;H
	.DB	"I",1,1,0,0,0,0,0	;I
	.DB	"J",1,3,3,3,0,0,0	;J
	.DB	"K",3,1,3,0,0,0,0	;K
	.DB	"L",1,3,1,1,0,0,0	;L
	.DB	"M",3,3,0,0,0,0,0	;M
	.DB	"N",3,1,0,0,0,0,0	;N
	.DB	"O",3,3,3,0,0,0,0	;O
	.DB	"P",1,3,3,1,0,0,0	;P
	.DB	"Q",3,3,1,3,0,0,0	;Q
	.DB	"R",1,3,1,0,0,0,0	;R
	.DB	"S",1,1,1,0,0,0,0	;S
	.DB	"T",3,0,0,0,0,0,0	;T
	.DB	"U",1,1,3,0,0,0,0	;U
	.DB	"V",1,1,1,3,0,0,0	;V
	.DB	"W",1,3,3,0,0,0,0	;W
	.DB	"X",3,1,1,3,0,0,0	;X
	.DB	"Y",3,1,3,3,0,0,0	;Y
	.DB	"Z",3,3,1,1,0,0,0	;Z
	.DB	"0",3,3,3,3,3,0,0	;0
	.DB	"1",1,3,3,3,3,0,0	;1
	.DB	"2",1,1,3,3,3,0,0	;2
	.DB	"3",1,1,1,3,3,0,0	;3
	.DB	"4",1,1,1,1,3,0,0	;4
	.DB	"5",1,1,1,1,1,0,0	;5
	.DB	"6",3,1,1,1,1,0,0	;6
	.DB	"7",3,3,1,1,1,0,0	;7
	.DB	"8",3,3,3,1,1,0,0	;8
	.DB	"9",3,3,3,3,1,0,0	;9
	.DB	".",1,3,1,3,1,3,0	;.
	.DB	",",3,3,1,1,3,3,0	;,
	.DB	" ",0,0,0,0,0,0,0	;space
	.DB	  0,0,0,0,0,0,0,0	;nic=konec tabulky

;T E X T Y   K   O D V Y S I L A N I
;         
;        0       1       2        3       4        5       6       7       8        minut9
TEXT0:
.db	"TOTO JE NAVIGATOR  CHYBI ANTENA  HLEDEJ TADY  N 50S 12.345  E014S 12.345",0
TEXT1:
.db	"POTREBUJI ZESILOVAC  LEZI TADY  N 50S 12.345  E014S 12.345",0
TEXT2:
.db	"SLABY SIGNAL  DALSI ANTENA JE ZDE  N 50S 12.345  E 14S 12.345",0
TEXT3:
.db 	"USMERNOVAC VYKONU DOPADL NA  N 50S 12.345  E014S 12.345",0
TEXT4:
.db 	"POLOHA DALSI ANTENY JE  N 50S 12.345  E014S 12.345",0
TEXT5:
.db	"PRESNOST ZVYSI SUPERSENZOR  N 50S 12.345  E014S 12.345",0 
TEXT6:
.db	"VRAK MODULU ZAMEREN NA  N 50S 12.345  E014S 12.345  SLIZ JE NESKODNY",0

