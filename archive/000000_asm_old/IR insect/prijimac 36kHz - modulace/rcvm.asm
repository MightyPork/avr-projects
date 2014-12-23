.device attiny13
.def	cnt=r22
.def	i=r18
.def	j=r19
.def	pom=r16
.def	dat=r20
.equ	clks=200			;100 kloknuti (36kHz) na 1 bit (200 preklopeni)
.equ	clksh=100

.org	0x0000				;RESET
	rjmp	RESET			;skok na start po resetu

.org	0x0010
RESET:
	ldi	r16,low(RAMEND)		;nastavi stack pointer
	out	SPL,r16

	cli				;zakazeme preruseni

	ldi	r16,0b00011110		;smer portu
	out	DDRB,r16

	clr pom
	clr i
	clr j
	clr cnt

	sei				;povolime preruseni
	
NIC:	clr i
	clr dat
W0:	   in pom,PINB			;1 [10+56 =66 cyklu *clksh celkani]
	   sbrc pom,0			;2 /
	   rjmp NIC			;(1)
	   rcall sclk			;3
	   inc i			;1
	   cpi i,clksh			;1
	  brne W0			;2 10 kloku + 56 kloku z sclk = 66 kloku
	  ;bylo 100 kloku 0
	  
	  
	  ldi j,8			;1 -> bude se cist osm bitu
BREAD:	  dec j				;1
	  breq ENDB			;1
	  clr i				;1
	  
W1:	   nop				;1 -> cekaci smycka 1klok
	   nop				;1
	   nop				;1
	   rcall sclk			;3
	   inc i			;1
	   cpi i,clks			;1
	  brne W1			;2
	  
	  in pom,PINB			;1	zanedbatelne zpozdeni - netreba zohlednovat
	  sbrs pom,0			;2 (1)  precte se PINB0 a invertovany se zapise do bufferu
	  sbr dat,0			;(+1)
	  lsl dat			;1
	  rjmp BREAD			;2
	  
ENDB:
	;skoncil byte, ted bude stopbit - netreba cist
	cpi dat,0b00111100
	breq toogle
	clr dat
	clr cnt
	clr i
	clr j
	rjmp NIC

toogle:	sbi PORTB,1
	;sbi pinb,1
	rjmp NIC

sclk:					;presne nastavena doba jednoho kloku - 200 kloku == 1bit
	ldi r16,6			;1
cB:
	  nop				; [48
	  nop				;  -
	  nop				;  -
	  nop				;  -
	  nop				;  -
	  dec r16			;  -
	  brne cB			;  -
	nop				; ]
	nop				;3
	nop				;
	nop				;
	ret				;4 celkem: 56
