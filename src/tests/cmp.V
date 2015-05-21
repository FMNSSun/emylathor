; This tests the CMP instruction
	; Equal
	LOAD32	R0, @189337
	LOAD32	R1, @189337
	CMP	R0, R1
	FAILNZ	0x01
	FAILIS	0x02
	FAILIC	0x03

	; Not equal, smaller (unsigned)
	LOAD32	R0, @189336
	CMP	R0, R1
	FAILIZ	0x04
	FAILNC	0x05

	; Not equal, bigger (unsigned)
	LOAD32	R0, @189338
	CMP	R0, R1
	FAILIZ	0x06
	FAILIC	0x07

	; Not equal, signed
	LOAD32	R1, @-1
	LOAD32	R0, @1
	CMP	R0, R1
	FAILIZ	0x08
	FAILIS	0x09		; 1 > -1 (signed)
	FAILNC	0x0A		; 1 < -1 (unsigned)

	; Not equel, signed
	LOAD32	R1, @1
	LOAD32	R0, @-1
	CMP	R0, R1
	FAILIZ	0x0B
	FAILNS	0x0C		; -1 < 1 (signed)
	FAILIC	0x0D		; -1 > 1 (unsigned)

	STOP
