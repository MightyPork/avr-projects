
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


.equ FrekvenceMorse = 50		;62500/tohle Hz tón 42


;K O N S T A N T Y   +   P R E Z D I V K Y   P O U R T U   A   P I N U
;PORTA
.equ SYSPORT = PORTA
.equ SYSPIN = PINA
.equ SYSDDR = DDRA

.equ speak = 0
.equ inkey = 7


;PORTB
.equ LEDPORT = PORTB
.equ LEDDDR = DDRB
.equ LEDPIN = PINB

.equ ledm = 6



;PORTD
.equ SWPORT = PORTD
.equ SWPIN = PIND
.equ SWDDR = DDRD


;A L I A S Y   R E G I S T R U
.def cmatch = r19
.def louter = r18
.def linner = r17
.def curchar = r25
.def morspart = r24

;aliasy pointerů
.def 	ZH		= r31
.def 	ZL		= r30
;.def 	YH		= r29
;.def 	YL		= r28
;.def 	XH		= r27
;.def 	XL		= r26



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

;.MACRO DefaultTone
;	ldi r16,FrekvenceMorse
;	out OCR0A,r16
;.ENDMACRO



; Z A C A T E K   P R O G R A M U
;vektory preruseni
.org	0x0000				;RESET
	rjmp RESET			;skok na start po resetu

.org	0x000D				;TC0 Compare match A
	sbi SYSPIN,speak		;přepnout speak
	reti				;návrat do programu (0x000E)

.org	0x0013

;nastaveni po resetu
.DB "PIPAC PRO PUNTU NA MORSE"
RESET:
	ldi r16,low(RAMEND)		;nastavi stack pointer
	out SPL,r16
	cli

; Nastaveni portu
	;PORTA = SYSPORT
	ldi r16,0b00000011		;smer portu A - vse na výstup (speak)
	out SYSDDR,r16
	ldi r16,0b00000001		;speak=1 (je mezi Vcc a timhle, takze skrz nej nic netece)
	out SYSPORT,r16

	;PORTB = LEDPORT
	ldi r16,0b01111111		;smer portu B - vystupni (ledky) - použijou se piny 0-6, 7. = pip
	out LEDDDR,r16
	ldi r16,0b11111111		;vse v PORTB na 1 = ledky zhasnuty, pip pullup enabled
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

	ldi r16,50
	SetTone

	;pipani aktivuje makro BuzzOn, vypne makro BuzzOff	
	sei				;Global Interrupt Enable
	ldi r20,0b10000000
loop:
	in r19,LEDPIN
	andi r19,0b10000000
	cp r19,r20
	breq loop			;beze zmeny, opakuj test


	ldi r16,255
l1:	in r18,LEDPIN
	andi r18,0b10000000
	cp r18,r19			;nacti LEDPIN. je porad stajny? (255x)
	brne loop			;ne -> loop
	dec r16				;jo -> dalsi test shodnosti, l1
	brne l1
	
	mov r20,r19			;uloz jako minulou hodnotu pinu
	cpi r20,0b10000000		;je nestisknuto?
	breq beepoff			;vypni pip
	ldi r16,0b10000000
	out LEDPORT,r16			;rozsvit ledky
	BuzzOn				;jinak pipej
	rjmp loop
beepoff:
	BuzzOff
	ldi r16,0b11111111
	out LEDPORT, r16		;zhasni ledky
	out LEDPORT,r16
	rjmp loop
