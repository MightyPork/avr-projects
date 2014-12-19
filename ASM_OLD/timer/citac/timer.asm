;pracuje jako 5-ti bitovy binarni citac
;vystupy 3 a 4 jsou prohozene kvuli desce
.device attiny13
.def	ze=r19
.def	pr=r20
.def	sc=r21
.def	te=r22
.def	kv=r23
.def	qi=r24
.def	divf=r25

.org	0x0000				;RESET
	RJMP	RESET			;skok na start po resetu
.org	0x0006				;TC0 overflow
	RJMP	CASOVAC 
.org 	0x0007
	rjmp	casovac

.org	0x0010
RESET:
	LDI	r16,low(RAMEND)		;nastavi stack pointer
	OUT	SPL,r16

	CLI				;zakazat vsechna preruseni

	LDI	r16,0b00011111		;výstupní = 1
	OUT	DDRB,r16

	LDI	r16,0			;vypnout PullUp
	OUT	PORTB,r16

	ldi r16,0b00000011		;TC0 prescaler na 64
	out TCCR0B,r16			;4800000/64=75000Hz
	
	ldi r16,0b00000010		;TC0 je v rezimu CTC
	out TCCR0A,r16
	
	ldi r16,75			;Compare Match 0 A je 75
	out OCR0A,r16			;75000/75=1000Hz interrupt
	
	in r16, TIMSK0
	ORI r16, 0b00000100
	out timsk0,r16

	clr r16
	clr r17
	clr r18
	clr pr
	clr sc
	clr te
	clr kv
	ldi divf,2
	SEI				;Global Interrupt Enable
	
LOOP:
	rjmp LOOP			;skok na loop

CASOVAC:
	inc ze
	cpi ze,100
	BRNE CASEND
	clr ze
	INC pr
	CPi pr,10
	BRNE CASEND
	  clr pr
	  INC sc

	  in r16,PORTB
	  SBRC r16,0
	    cbi portb,0
	  SBRS r16,0
	    sbi portb,0

	  CP sc,divf
	  BRNE CASEND
	    clr pr
	    clr sc
	    inc te

	    in r16,PORTB
	    SBRC r16,1
	      cbi portb,1
	    SBRS r16,1
	      sbi portb,1
	    
	    CP te,divf
	    BRNE CASEND
	      clr pr
	      clr sc
	      clr te
	      inc kv
	      
	      in r16,PORTB
	      SBRC r16,2
	        cbi portb,2
	      SBRS r16,2
	        sbi portb,2
	        
	      CP kv,divf
	      BRNE CASEND
		clr pr
		clr sc
		clr te
		clr kv
		inc qi

		in r16,PORTB
	        SBRC r16,4
	          cbi portb,4
	        SBRS r16,4
	          sbi portb,4
		CP qi,divf
		BRNE CASEND
		  clr pr
		  clr sc
		  clr te
		  clr kv
		  clr qi

		  in r16,PORTB
		  SBRC r16,3
		    cbi portb,3
		  SBRS r16,3
		    sbi portb,3
	      

CASEND:
	RETI

