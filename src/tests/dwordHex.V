
	; Test for the unescaped and escaped 32 bit hex immediate values
	LOAD32	R0,	0x11111111
	LOAD32	R1,	@0x11111111
	CMP	R0,	R1
	FAILNZ	0x01
	
	STOP