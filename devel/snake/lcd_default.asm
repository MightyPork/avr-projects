; Zapojeni (Připojen DMC-50399 v 4-bitovem modu):
;                   +------u------+
;    Vcc -> reset --+ /RST    Vcc +-- napajeni +5V
;                 --+ PD0     PB7 +-- RS (0=instr W, BF+addr R; 1=data W/R)
;                 --+ PD1     PB6 +-- R/W (1=read,0=write)
;                 --+ PA1     PB5 +-- E (clock, active falling edge)
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

;LFUSE: 0xE2 -U lfuse:w:0xE2:m
;HFUSE: 0xDF -U hfuse:w:0xDF:m


;K O N S T A N T Y   +   P R E Z D I V K Y   P O R T U   A   P I N U
.equ LCDPORT = PORTB
.equ LCDPIN = PINB
.equ LCDDDR = DDRB

.equ RS = 7
.equ RW = 6
.equ E = 5

.equ LCD_CLEAR = 0b00000001
.equ LCD_HOME = 0b00000010

.equ LCD_MODE_INC_NOSHIFT = 0b00000110
.equ LCD_MODE_INC_SHIFT = 0b00000111
.equ LCD_MODE_DEC_NOSHIFT = 0b00000100
.equ LCD_MODE_DEC_SHIFT = 0b00000101

.equ LCD_DISPLAY_DISABLED = 0b00001000
.equ LCD_DISPLAY_NOCURSOR = 0b00001100
.equ LCD_DISPLAY_CURSOR = 0b00001110
.equ LCD_DISPLAY_ALTER = 0b00001101
.equ LCD_DISPLAY_CURSOR_ALTER = 0b00001111

.equ LCD_CURSOR_LEFT = 0b00010000
.equ LCD_CURSOR_RIGHT = 0b00010100
.equ LCD_SHIFT_LEFT = 0b00011000
.equ LCD_SHIFT_RIGHT = 0b00011100


;5x7 font, 1-line
.equ LCD_MODE_4BIT_1LINE = 0b00100000
;.equ LCD_MODE_8BIT_1LINE = 0b00110000
;5x7 font, 2-line
.equ LCD_MODE_4BIT_2LINE = 0b00101000
;.equ LCD_MODE_8BIT_2LINE = 0b00111000

.equ ROW1_ADDR = 0x00
.equ ROW2_ADDR = 0x40
.equ ROW3_ADDR = 0x14
.equ ROW4_ADDR = 0x54



;aliases
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
.DB	"HD44780 INTERFACE"		;(nazev programu)
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

; == display init ==
	rcall LCD_INIT

	ldi r17,LCD_MODE_INC_NOSHIFT
	rcall TX_INSTR

	ldi r17,LCD_DISPLAY_NOCURSOR
	rcall TX_INSTR

; == load user-defined characters to CGRAM == (default, array label named MYCHARS, end-mark=0xFE)
	ldi r17,0
	rcall CGRAM_SET_ADDR

	ldi ZH,high(MYCHARS*2)
	ldi ZL,low(MYCHARS*2)
CGRAM_loop:
	lpm r17,Z+
	cpi r17,0xFE
	breq CGRAM_loop_end
	rcall TX_DATA
	rjmp CGRAM_loop
CGRAM_loop_end:


; == pgm body ==

;load text to DDRAM
	ldi r17,ROW1_ADDR
	rcall DDRAM_SET_ADDR

	ldi ZH,high(MYTEXT1*2)
	ldi ZL,low(MYTEXT1*2)
DDRAM_loop:
	lpm r17,Z+
	cpi r17,0xFE
	breq DDRAM_loop_end
	rcall TX_DATA
	rjmp DDRAM_loop
DDRAM_loop_end:

;load text to DDRAM
	ldi r17,ROW2_ADDR
	rcall DDRAM_SET_ADDR

	ldi ZH,high(MYTEXT2*2)
	ldi ZL,low(MYTEXT2*2)
DDRAM2_loop:
	lpm r17,Z+
	cpi r17,0xFE
	breq DDRAM2_loop_end
	rcall TX_DATA
	rjmp DDRAM2_loop
DDRAM2_loop_end:

;direct write to X,Y - example
	ldi r16,3	;Y, zacina 0 a roste smerem dolu
	ldi r17,5	;X, zacina nulou a roste smerem doprava
	rcall LCD_CURSOR_XY
	ldi r17,"%"
	rcall TX_DATA
	ldi r17,"%"
	rcall TX_DATA
	ldi r17,"%"
	rcall TX_DATA
;infinite loop
loop:	rjmp loop


MYTEXT1:
.DB	0,0,0," POKUSNY TEXT ",0,0,0,0xFE
MYTEXT2:
.DB	"Opravdu pekny text!",0xFE



; == USER-DEFINED CHARS ==
MYCHARS:
; 5x8, first 3 bits are not used
;end of mychars
.DB	0xe,0x1f,0x15,0x1f,0x1f,0x1f,0x15	;smajlik

;konec
.DB	0xFE





;r16=Y
;r17=X
LCD_CURSOR_XY:
	cpi r16,0
	brne test1
fail:	ldi r16,ROW1_ADDR
	rjmp addrdone
test1:
	cpi r16,1
	brne test2
	ldi r16,ROW2_ADDR
	rjmp addrdone
test2:
	cpi r16,2
	brne test3
	ldi r16,ROW3_ADDR
	rjmp addrdone
test3:
	cpi r16,3
	brne fail
	ldi r16,ROW4_ADDR
addrdone:
	add r17,r16
	rcall DDRAM_SET_ADDR
	ret

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
	ldi r17,LCD_MODE_4BIT_2LINE		;set 4-bit mode
	rcall TX_INSTR
	ret

;r17
TX_INSTR:
	swap r17			;send high nibble
	mov r16,r17
	andi r16,0b00001111
	out LCDPORT,r16
	rcall LCD_CLK

	swap r17			;send low nibble
	mov r16,r17
	andi r16,0b00001111
	out LCDPORT,r16
	rcall LCD_CLK

	ret

;r17
TX_DATA:
	swap r17			;send high nibble
	mov r16,r17
	andi r16,0b00001111
	sbr r16,(1<<RS)
	out LCDPORT,r16
	rcall LCD_CLK

	swap r17			;send low nibble
	mov r16,r17
	andi r16,0b00001111
	sbr r16,(1<<RS)
	out LCDPORT,r16
	rcall LCD_CLK

	ret

;r17 disabled to reduce code size
;RX_DATA:
;	;input
;	ldi r16,0b11110000		;LCDPORT dirrection (RS RW E n.c.) output, (D7 D6 D5 D4) input
;	out LCDDDR,r16
;	ldi r16,0b00001111		;pullups to data pins enabled
;	out LCDPORT,r16
;
;	clr r17
;	ldi r16,(1<<RW)|(1<<RS)
;	out LCDPORT,r16			;set LCD to read mode, for data
;
;	rcall LCD_CLK			;receive high nibble
;	in r16,LCDPIN
;	andi r16,0b00001111
;	or r17,r16
;	swap r17			;store high nibble
;
;	rcall LCD_CLK			;receive low nibble
;	in r16,LCDPIN
;	andi r16,0b00001111
;	or r17,r16			;store low nibble
;	
;	;output
;	ldi r16,0b11111111		;LCDPORT as output (RS RW E n.c. D7 D6 D5 D4)
;	out LCDDDR,r16
;	ldi r16,0b00000000		;LCDPORT off
;	out LCDPORT,r16
;	
;	;r17=received value (D7 D6 D5 D4 D3 D2 D1 D0)
;	ret

LCD_CLK:
	sbi LCDPORT,E			;EXECUTE on
	nop
	nop
	nop
	cbi LCDPORT,E			;EXECUTE off
	ldi r16,150			;pause: 100 for 4MHZ
clkw:
	dec r16
	brne clkw
	ret

;r17
;7 bitu (1.radek zacina 00,druhej 40)
DDRAM_SET_ADDR:
	clr r16
	sbr r16,0b10000000
	or r17,r16
	rcall TX_INSTR
	ret

;r17
;6 bitu (5,4,3 = znak, 2,1,0 = radek - shora)
CGRAM_SET_ADDR:
	clr r16
	sbr r16,0b01000000
	or r17,r16
	rcall TX_INSTR
	ret