; Tests the SDIV instruction

	; Tests using some basic values
	; we don't expect any overflows here
	LOAD32	R0, @8192
	LOAD32	R1, @-2
	LOAD32	R2, @-4096
	SDIV	R3, R0, R1
	FAILIO	0x01
	CMP	R3, R2
	FAILNZ	0x02

	LOAD32	R0, @-8192
	LOAD32	R1, @-2
	LOAD32	R2, @4096
	SDIV	R3, R0, R1
	FAILIO	0x03
	CMP	R3, R2
	FAILNZ	0x04

	LOAD32	R0, @-8192
	LOAD32	R1, @2
	LOAD32	R2, @-4096
	SDIV	R3, R0, R1
	FAILIO	0x05
	CMP	R3, R2
	FAILNZ	0x06

	LOAD32	R0, @-10
	LOAD32	R1, @3
	LOAD32	R2, @-3
	SDIV	R3, R0, R1
	FAILIO 	0x07
	CMP	R3, R2
	FAILNZ	0x08

	; tests overflow behaviour
	; this time we expect overflow ofcourse because -0x80 / -1 is 0x80 which is too
	; big for two's complement
	LOAD32	R0, @-2147483648
	LOAD32  R1, @-1
	SDIV	R2, R0, R1
	FAILNO	0x09	; must be an overflow
	TEST	R2
	FAILNZ	0x0A	; overflow yields zero

	; immedate stuff
	LOAD32	R0, @120
	SDIV	R2, R0, 60
	FAILIO	0x0B
	LOAD32	R1, @2
	CMP	R1, R2
	FAILNZ	0x0C
	
	LOAD32	R0, @-120
	SDIV	R2, R0, 60
	FAILIO	0x0D
	LOAD32	R1, @-2
	CMP	R1, R2
	FAILNZ	0x0E

	STOP
