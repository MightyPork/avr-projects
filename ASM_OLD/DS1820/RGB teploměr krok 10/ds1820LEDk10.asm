.DEVICE ATtiny13
.LIST
.LISTMAC
;běží na 9.6MHz, ckdiv8=0 (zapnuto)
;avrdude příkaz pro FUSEs: -U lfuse:w:0x6a:m -U hfuse:w:0xff:m 
;LFUSE: 0x6A
;HFUSE: 0xFF

;aliasy pointerů
.def 	ZH		= r31
.def 	ZL		= r30
.def 	YH		= r29
.def 	YL		= r28
.def 	XH		= r27
.def 	XL		= r26




.def 	ARG		= r20		;registr pro předání hodnoty rutinám

;počátek paměti SRAM pro uložení přečtené teploty (9 bajtů)
.equ 	SRAM_t_H	= 0x00
.equ 	SRAM_t_L	= 0x00

;komunikační kanál pro ds1820
.equ 	PORTn		= PORTB
.equ 	PINn		= PINB
.equ 	DDRn		= DDRB
.equ 	n		= 0			;pin pro komunikaci s DS1820

; Č A S O V Á N Í
;délka 1 cyklu = T = 1/f
;    tXXXu	= [XXX / (4*T)] - 2
; (ldi, rcall a ret zabere 8 cyklů)
; nejnižší hodnota je 1 ... 12T čekání
; attiny13@9.6MHz+CKDIV8...T=0.83333us
.equ 	t480u		= 142
.equ 	t410u		= 120
.equ 	t70u		= 19
.equ 	t64u		= 17
.equ 	t60u		= 16
.equ 	t54u		= 14
.equ 	t10u		= 1

;registry pro CRC rutinu
.def	rChar		= r19
.def	rCount		= r18		;počet bajtů
.def	rBitCount	= r17
.def	rTemp		= r16
.def	rCRC8		= r21		;výsledek (musí být jiný než ARG)

.def	R		= r4
.def	G		= r5
.def	B		= r6
.def	tL		= r2
.def	tH		= r3

;6us je obvykle moc málo pro volání DELAY
.MACRO t6uDELAY
	ldi r16,2			;pro attiny13@9.6MHz+CKDIV8 je to 2 x 4T = 8T = 8 x 0.83333us = 6us čekání
t6u:	nop				;pozor! některé překladače labely v makrech nepodporují!
	dec r16
	brne t6u
.ENDMACRO

;ovládání sběrnice
.MACRO BUS_L
	cbi PORTn,n	
	sbi DDRn,n			;pin n výstupní, výstup 0
.ENDMACRO

.MACRO BUS_H
	sbi PORTn,n
	cbi DDRn,n			;pin n vstupní, pullup zapnutý (+ doporučeno 4.7k externí pullup)
.ENDMACRO

.MACRO RGB_OFF
	cbi PORTB,1
	cbi PORTB,2
	cbi PORTB,3
.ENDMACRO

.MACRO RGB_ON
	sbi PORTB,1
	sbi PORTB,2
	sbi PORTB,3
.ENDMACRO

.MACRO PWM_CYCLE
	ldi r17,@0			;1
PWM0:	RGB_OFF				; 6
	ldi r16,40			; 1
PWM1:	cp r16,R			; 1
	brne PWM2			; 1/2
	sbi PORTB,1			; 2
PWM2:	cp r16,G			; 1
	brne PWM3			; 1/2
	sbi PORTB,2			; 2
PWM3:	cp r16,B			; 1
	brne PWM4			; 1/2
	sbi PORTB,3			; 2
PWM4:	dec r16				; 1
	brne PWM1			; 1/2 (805)*r17+6
	dec r17				; 1
	brne PWM0			; 1/2
	RGB_OFF				;6
.ENDMACRO





; Z A C A T E K   P R O G R A M U
.org	0x0000				;RESET
	rjmp RESET			;skok na start po resetu


; začátek kódu
.org	0x0010
RESET:	ldi r16,low(RAMEND)		;nastaví stack pointer
	out SPL,r16
	
	ldi r16,0b00011111		;směr portu B - vše na výstup. Směr pinu n se bude měnit za chodu
	out DDRB,r16
	ldi r16,0b00000000		;RGB=0b000, ledka zhasnuta
	out PORTB,r16

;tělo programu
	clr R
	clr G
	clr B
MainLoop:
	rcall GET_T			;teplota do SRAM 0x00-0x08

	cpi ARG, 0
	brne MainLoop

	;výpočet barvy
	ldi ZH,SRAM_t_H			;počátek řetězce přijatých bajtů do Z
	ldi ZL,SRAM_t_L
	ld tL,Z+			;načíst teplotu z paměti
	ld tH,Z
	ldi r16,40
	mov R,r16			;defaultní barva - bílá, 40 40 40
	mov G,r16
	mov B,r16
	ldi r16,0
	cpse tH,r16			;pokud tH != 0x00, skoč na mínus
	rjmp MINUS
;PLUS nebo NULA
	ldi r16,21			;tL >= 10.5? --> c10_20
	cp tL,r16
	brge c10_20
	;0-blue,10-white
	clr R
	clr G
	mov r16,tL
	lsl r16
	mov R,r16
	mov G,r16
	rjmp C_DONE
	
c10_20:	;10-white,20-yellow
	ldi r16,41			;tL >= 20.5? --> c20_30
	cp tL,r16
	brge c20_30
	
	mov r16,tL
	subi r16,20
	lsl r16
	sub B,r16
	rjmp C_DONE

c20_30:	;20-yellow,30-red
	ldi r16,61			;tL >= 30.5? --> c30_40
	cp tL,r16
	brge c30_40
	clr B
	mov r16,tL
	subi r16,40
	lsl r16
	sub G, r16
	rjmp C_DONE

c30_40:	;30-red, 40-purple
	ldi r16,81			;tL >= 40.5? --> c40_50
	cp tL,r16
	brge c40_50
	clr B
	clr G
	mov r16,tL
	subi r16,60
	lsl r16
	add B, r16
	rjmp C_DONE

c40_50:	;40-purple, 50-"UV"
	ldi r16,101			;tL >= 50.5? --> c50_hi
	cp tL,r16
	brge c50_hi
	clr G
	mov r16,tL
	subi r16,80
	sub R, r16
	rjmp C_DONE

c50_hi:
	clr G
	ldi r16,20
	mov R,r16
	rjmp C_DONE

MINUS:
	neg tL
	ldi r16,21			;-tL >= 10.5? --> cm10_20
	cp tL,r16
	brge cm10_20
	mov r16,tL
	lsl r16
	clr R
	clr G
	add G,r16
	rjmp C_DONE

cm10_20:
	ldi r16,41			;-tL >= 20? --> cm20_lo
	cp tL,r16
	brge cm20_lo
	clr R
	mov r16,tL
	subi r16,20
	lsl r16
	sub B,r16
	rjmp C_DONE

cm20_lo:
	clr R			;-tL >= 20? --> cm40_lo
	clr B
	rjmp C_DONE
	
C_DONE:

;	mov r16,R
;	ldi r17,11
;	rcall EEwrite
;	mov r16,G
;	ldi r17,12
;	rcall EEwrite
;	mov r16,B
;	ldi r17,13
;	rcall EEwrite

;	ldi ZH, SRAM_t_H
;	ldi ZL, SRAM_t_L
;NextEE:	ld r16,Z
;	mov r17, ZL
;	rcall EEwrite
;	inc ZL
;	cpi ZL,2
;	brlo NextEE

	PWM_CYCLE 255

	rjmp MainLoop





;G E T   T E M P E R A T U R E
;Přečte 9 bajtů z teplotního čidla na pinu n (PINn, PORTn, DDRn) a uloží je do SRAM na adresu 0x00 až 0x08.
;status je uložen do ARG (ARG == 0 -> CRC se shoduje, OK; ARG != 0 -> CRC se neshoduje, chyba přenosu)
;
;SRAM po vykonání procedury GET_T:
;	0x00	temperature low byte - unit: 0.5°C
;	0x01 	temperature high byte (sign, 0x00 = positive, 0xff = negative)
;	0x02	"user byte 1"
;	0x03	"user byte 2"
;	0x04	reserved, 0xFF
;	0x05	reserved, 0xFF
;	0x06	count remain (default 0x0C)
;	0x07	count per °C (fixed to 0x10)
;	0x08	CRC
;celá čtecí rutina trvá obvykle asi 550ms, max je asi 750ms. Během rutiny "W4C" může být prováděno něco jiného s pevnou délkou 700ms.
GET_T:
	push r17			;ulož používané registry do STACKu

	rcall TxRESET			;RESET
	ldi ARG,0xCC			;SKIP_ROM
	rcall TxBYTE
	ldi ARG,0x44			;CONVERT_T
	rcall TxBYTE
	rcall W4C			;čekej na konverzi teploty
	rcall TxRESET			;RESET
	ldi ARG,0xCC			;SKIP_ROM
	rcall TxBYTE
	ldi ARG,0xBE			;READ_SCRATCHPAD
	rcall TxBYTE

	;přijmout 9 bajtů, uložit na 0x00 až 0x09 v SRAM
	ldi ZH, SRAM_t_H
	ldi ZL, SRAM_t_L
NEXT_B:	rcall RxBYTE			;přijmi 1 bajt -> ARG
	
N_B3:	st Z+,ARG			;do SRAM na adresu Z ulož výsledek. Z++
	cpi ZL,9			;je Z == 9?
	brlo NEXT_B			;je-li menší, čti další bajt
	
	;vypočítat CRC, pokud se shoduje s přečtenou hodnotou, r16=1, jinak r16=0
	ldi XH,SRAM_t_H
	ldi XL,SRAM_t_L
	ldi rCount,9
	rcall CRC8
	mov ARG,rCRC8
	
	pop r17
	ret				;návrat, ARG==0 .. ok, ARG!=0 .. error


;D E L A Y
; r16 = d = poček čtyřkloků zmenšený o 2 (dva čtyřkloly se vykonají při nastavení r16 pomocí LDI (1T), RCALL (3T) a RET (4T))
DELAY:	nop
	dec r16
	brne DELAY
	nop
	ret ; po návratu je r16 = 0



;R E S E T   P U L S E
TxRESET:
	BUS_L				;stáhni sběrnici

	PWM_CYCLE 1
	;ldi r16,t480u
	;rcall DELAY			;480us čekání

	BUS_H				;uvolni sběrnici

	ldi r16,t70u
	rcall DELAY			;70us čekání

	sbic PINn,n
	rjmp TxRESET			;není-li presence pulse, zopakuj TxRESET

	;ldi r16,t410u
	;rcall DELAY			;410T čekání
	PWM_CYCLE 1

	ret				;návrat


;T R A N S M I T   B Y T E
; odešlou se data z ARG
TxBYTE: ldi r17,8			;čítač bytů = 8
;nový bit
TxNB:	lsr ARG				;nový bit; rotovat ARG: ARG.0 -> C
	brcs Tx1			;jednička - skok na Tx1, jinak pokračuj

					;-- Odeslání 0 --
	BUS_L				;stáhni sběrnici

	ldi r16,t60u
	rcall DELAY			;60us čekání

	BUS_H				;uvolni sběrnici

	ldi r16,t10u
	rcall DELAY			;10us čekání

	rjmp TxLOOP			;skok na konec rutiny

					;-- Odeslání 1 --
Tx1:	BUS_L				;stáhni sběrnici

	t6uDELAY			;6us čekání

	BUS_H				;uvolni sběrnici

	ldi r16,t64u
	rcall DELAY			;64us čekání

TxLOOP:	dec r17				;odečti 1 od čítače bitů
	brne TxNB			;zbývají bity? ano -> TxNB
	ret				;návrat


;R E C E I V E   B Y T E
; do ARG se uloží přečtený byte
RxBYTE:	ldi r17,8			;čítač bitů
	clr ARG				;vymaž ARG
;nový bit
RxNB:	rcall RxBIT

	ror ARG				;rotuj ARG C[=PINB.n] -> ARG.7, ARG.0 -> C
	dec r17				;odečti 1 od čítače bitů
	brne RxNB			;zbývají bity? ano -> RxNB
	ret				;návrat


;R E C E I V E   S I N G L E   B I T
RxBIT:  BUS_L				;stáhni sběrnici

	t6uDELAY			;6us čekání

	BUS_H				;uvolni sběrnici

	ldi r16,t10u
	rcall DELAY			;10us čekání

	sbic PINn,n			;PINB.n -> C
	sec
	sbis PINn,n
	clc

	ldi r16,t54u
	rcall DELAY			;54us čekání
	ret				;návrat


;Č E K Á N Í   N A   D O K O N Č E N Í   K O N V E R Z E
;čekání na dokončení konverze teploty (nelze použít v parazitním režimu, v tom je třeba čekat 700ms.)
W4C:	BUS_H
;až 700ms dlouhá čekací smyčka
W4C0:	ldi r18,5
W4C1:	PWM_CYCLE 200
	dec r18
	brne W4C1
	
	rcall RxBIT			;přijmi bit do C.
	brcc W4C0			;C=0 -> čekej dál

	ret				;návrat


;V Ý P O Č E T   C R C
;XH,XL,rCount --> rCRC8
CRC8:	push	XH			;save XH
	push	XL			;save XL
	push	rChar			;save rChar
	push	rCount			;save rCount
	push	rBitCount		;save rBitCount
	push	rTemp			;save rTemp

	clr	rCRC8			;start with a zero CRC-8

	;begin loop to do each byte in the string
CRC8BYT:
	ld	rChar,X+		;fetch next string byte and bump pointer
	ldi	rBitCount,8		;load the bit-counter for this byte

	;begin loop to do each bit in the byte
CRC8BIT:
	mov	rTemp,rChar		;get a temporary copy of current data
	eor	rTemp,rCRC8		;XOR the data byte with the current CRC
	lsr	rCRC8			;position to the new CRC
	lsr	rChar			;position to next bit of this byte	
	lsr	rTemp			;get low bit of old result into c-bit
	brcc	CRC8NA			;br if low bit was clear (no adjustment)
	ldi	rTemp,$8C		;magical value needed for CRC-8s
	eor	rCRC8,rTemp		;fold in the magic CRC8 value
CRC8NA:
	dec	rBitCount		;count the previous bit done
	brne	CRC8BIT			;br if not done all bits in this byte
	;end loop to do each bit in the byte

	dec	rCount			;count this byte done
	brne	CRC8BYT			;br if not done all bytes in the string
	;end loop to do each byte in the string

        pop	rTemp			;restore rTemp
        pop	rBitCount		;restore rBitCount
	pop	rCount			;restore rCount
	pop	rChar			;restore rChar
	pop	XL			;restore XL
	pop	XH			;restore XH
	ret				;return to caller
;
;END OF SUBROUTINE CRC8








;cteni z EEPROM, v [r17 je ADRESA], v (r16 se objeví DATA)
;EEread:
;	sbic EECR,1
;	rjmp EEread			;cekani dokud neni EEPROM ready
;
;	out EEARL, r17		;ulozit adresu z r17
;	sbi EECR,0			;nastavenim EERE zacina cteni
;	in r16,EEDR			;cteni dat z EEDR do r16
;
;	ret


;zapis do EEPROM, v [R17 je ADRESA], v [R16 jsou DATA]
EEwrite:
	sbic EECR,EEPE
	rjmp EEwrite			;cekani dokud neni EEPROM ready

	cbi EECR,EEPM1			;nastavit programovaci mod
	cbi EECR,EEPM0

	out EEARL, r17			;dej adresu z r17 do EEARL
	out EEDR, r16			;dej data z r16 do EEDR
	sbi EECR,EEMPE			;master program enable
	sbi EECR,EEPE			;program enable

	ret



