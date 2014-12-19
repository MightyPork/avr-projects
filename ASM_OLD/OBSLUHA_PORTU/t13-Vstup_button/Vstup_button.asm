.device attiny13
.def	CNT=r25
.def	LED=r24

.org	0x0000				;zacatek kodu
	RJMP	RESET			;skok na start po resetu

.org	0x0010
RESET:
	LDI	r16,low(RAMEND)	;nastavi stack pointer
	OUT  SPL,r16

	CLI					;zakazat vsechna preruseni

	LDI	LED,0	

	LDI	r16,0b11111110		;PB2 je vystupni, PB0 vstupni
	OUT	DDRB,r16

	LDI	r16,0			;vypnout PullUp
	OUT	PORTB,r16

LOOP:
	IN	r16,PINB			;dej do r16 stav PINB
	ANDI	r16,0b00000001
	CPI	r16,1			;neni-li to 1, jdi na STORNO			
	BRNE	STORNO
	INC	CNT				;CNT+1
	CPI	CNT,50			;neni-li CNT=100, jdi na LOOP
	BRNE	LOOP

	IN	r16,PORTB			;r16=PORTB (vystupni port)
	LDI	r17,0b00000100		;vyxoruj r16 s 0b00000100	
	EOR	r16,R17	
	OUT	PORTB,R16			;a vysledek dej na PORTB 

KONEC:
	IN	r16,PINB			;cekani na vstup nuly do PINB0
	ANDI	r16,0b00000001
	CPI	r16,0
	BRNE	KONEC			;neni-li r16=0, jdi na KONEC
	RJMP LOOP				;jdi na LOOP
				
STORNO:
	LDI	CNT,0			;do CNT dej 0
	RJMP	LOOP				;jdi na LOOP

	

	


			
