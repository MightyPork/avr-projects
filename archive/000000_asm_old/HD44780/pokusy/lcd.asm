; Zapojeni (Připojen DMC-50399 v 4-bitovem modu):
;                   +------u------+
;    Vcc -> reset --+ /RST    Vcc +-- napajeni +5V
;                 --+ PD0     PB7 +-- RS (0=instr W, BF+addr R; 1=data W/R)
;                 --+ PD1     PB6 +-- R/W (1=read,0=write)
;                 --+ PA1     PB5 +-- E (strobe)
;                 --+ PA0     PB4 +-- 
;                 --+ PD2     PB3 +-- DATA 7
;                 --+ PD3     PB2 +-- DATA 6
;                 --+ PD4     PB1 +-- DATA 5
;                 --+ PD5     PB0 +-- DATA 4
;             GND --+ GND     PD6 +-- 
;                   +-------------+

;DMC-50399: 
; 1 - GND
; 2 - +5V
; 3 - 0V (lcd driver)
; 4 - RS
; 5 - R/W
; 6 - E
; 7 - DATA 0
; 8 - DATA 1
;...
;14 - DATA 7

.device attiny2313
;běží na 4MHz, ckdiv8=1 (vypnuto)
;250x16=1ms=1000us

;LFUSE: 0xE2
;HFUSE: 0xDF


;K O N S T A N T Y   +   P R E Z D I V K Y   P O U R T U   A   P I N U
;PORTB
.equ LCDPORT = PORTB
.equ LCDPIN = PINB
.equ LCDDDR = DDRB

.equ RS = 7
.equ RW = 6
.equ E = 5

.equ LCD_CLEAR = 0b00000001
.equ LCD_HOME = 0b00000010
.equ LCD_MODE_INC_NOSH = 0b00000110
.equ LCD_MODE_INC_SH = 0b00000111
.equ LCD_MODE_DEC_NOSH = 0b00000100
.equ LCD_MODE_DEC_SH = 0b00000101
.equ LCD_DISPLAY_DISABLED = 0b00001000
.equ LCD_DISPLAY_NOCURSOR = 0b00001100
.equ LCD_DISPLAY_CURSOR = 0b00001110
.equ LCD_DISPLAY_BLINK = 0b00001101
.equ LCD_DISPLAY_ALTER = 0b00001111
;4bit,5x7,2line
.equ LCD_MODE_4BIT = 0b00101000
.equ LCD_MODE_8BIT = 0b00111000

.equ ROW1_ADDR = 0x00
.equ ROW2_ADDR = 0x40



;aliasy pointerů
.def 	ZH		= r31
.def 	ZL		= r30
.def 	YH		= r29
.def 	YL		= r28
.def 	XH		= r27
.def 	XL		= r26

; Z A C A T E K   P R O G R A M U
;vektory preruseni
.org	0x0000				;RESET
	rjmp RESET			;skok na start po resetu

.org	0x0013

;nastaveni po resetu
.DB	"DMC-50399 TEST"		;(nazev programu)
RESET:
	ldi r16,low(RAMEND)		;nastavi stack pointer
	out SPL,r16
	cli				;zakazat vsechna preruseni

; Nastaveni portu
	;PORTB = LCDPORT
	ldi r16,0b11111111		;smer portu B
	out LCDDDR,r16
	ldi r16,0b00000000		;vypnout B
	out LCDPORT,r16
	
	sei				;Global Interrupt Enable
	
	rcall LCD_INIT

	ldi r17,LCD_DISPLAY_DISABLED
	rcall TX_INSTR

	ldi r17,ROW1_ADDR
	rcall DDRAM_SET_ADDR

	ldi ZH,high(ROW1*2)
	ldi ZL,low(ROW1*2)
read1:	lpm r17,Z+
	cpi r17,0
	breq text2
	rcall TX_DATA
	rjmp read1

text2:
	ldi r17,ROW2_ADDR
	rcall DDRAM_SET_ADDR

	ldi ZH,high(ROW2*2)
	ldi ZL,low(ROW2*2)
read2:	lpm r17,Z+
	cpi r17,0
	breq loop
	rcall TX_DATA
	rjmp read2

	ldi r17,LCD_DISPLAY_CURSOR
	rcall TX_INSTR
	
loop:	rjmp loop

;r16=počet ms (cca)
delay:
	push r17	;2
	push r18	;2
d1:
	ldi r17,250	;1
d2:
	ldi r18,14	;1
d3:
	dec r18		;1
	nop
	brne d3		;2 (1
	dec r17		;   +1)
	brne d2		;2 (1
	dec r16		;   +1)
	brne d1		;2 (1)
	
	pop r18		;2
	pop r17		;2
	ret


LCD_INIT:
	ldi r16,16
	rcall delay
	ldi r16,0b00000010		;4bit
	out PORTB,r16
	rcall LCD_CLK
	ldi r16,5
	rcall delay
	ldi r17,LCD_MODE_4BIT
	rcall TX_INSTR
	ldi r17,LCD_MODE_INC_NOSH
	rcall TX_INSTR
	ret

;r17
TX_INSTR:
	swap r17
	mov r16,r17
	andi r16,0b00001111
	out LCDPORT,r16
	rcall LCD_CLK
	swap r17
	mov r16,r17
	andi r16,0b00001111
	out LCDPORT,r16
	rcall LCD_CLK	
	ret

;r17
TX_DATA:
	swap r17
	mov r16,r17
	andi r16,0b00001111
	sbr r16,0b10000000
	out LCDPORT,r16
	rcall LCD_CLK
	swap r17
	mov r16,r17
	andi r16,0b00001111
	sbr r16,0b10000000
	out LCDPORT,r16
	rcall LCD_CLK	
	ret

LCD_CLK:
	sbi LCDPORT,E
	nop
	nop
	nop
	cbi LCDPORT,E
	ldi r16,100			;100 funguje na 4MHZ
clkw1:
	dec r16
	brne clkw1
;	ldi r16,1
;	rcall delay
	ret

;7 bitu (1.radek zacina 00,druhej 40)
DDRAM_SET_ADDR:
	clr r16
	sbr r16,0b10000000
	or r17,r16
	rcall TX_INSTR
	ret

;6 bitu (5,4,3 = znak, 2,1,0 = radek - shora)
CGRAM_SET_ADDR:
	clr r16
	sbr r16,0b01000000
	or r17,r16
	rcall TX_INSTR
	ret

ROW1:
.DB	"ATTINY2313 & HD44780",0
ROW2:
.DB	"*  www.ondrovo.com *",0
