; Tests the SEX instruction

	; Test result of 8 to 32 bit sign extension with negative number
	LOAD32	R3, @0xFFFFFFFF ;expected value
	XOR	R0, R0, R0
	LOADI	R0LL, 0xFF
	MOV	R2, FLGS
	SEX	R1, R0LL
	CMP	R2, FLGS ;flags should remain unchanged
	FAILNZ	0x01
	CMP	R3, R1
	FAILNZ	0x02

	; Test result of 8 to 32 bit sign extension with positive number
	LOAD32 	R3, @0x0000007F ;expected value
	LOADI	R0LL, 0x7F
	MOV	R2, FLGS
	SEX	R1, R0LL
	CMP	R2, FLGS
	FAILNZ	0x03
	CMP	R3, R1
	FAILNZ	0x04

	; Test result of 16 to 32 bit sign extension with negative number
	LOAD32	R3, @0xFFFFFFFF	;expected value
	LOAD32	R0, @0x0000FFFF	;TODO: replace if possible with LOADI 16bit
	MOV	R2, FLGS
	SEX	R1, R0L
	CMP	R2, FLGS
	FAILNZ	0x05
	CMP	R3, R1
	FAILNZ	0x06

	; Test result of 16 to 32 bit sign extension with positive number
	LOAD32	R3, @0x00007FFF
	LOAD32	R0, @0xFFFF7FFF ;TODO: replace if possible with LOADI 16bit
	MOV	R2, FLGS
	SEX	R1, R0L
	CMP	R2, FLGS
	FAILNZ	0x07
	CMP	R3, R1
	FAILNZ	0x08
	
	STOP