; This testst the XOR instruction
	; normal xor
	LOAD32	R0, @1337
	LOAD32	R1, @6666
	LOAD32	R2, @7987
	XOR	R3, R0, R1
	CMP	R3, R2
	FAILNZ	0x01

	; xor with immediate
	LOAD32	R2, @1343
	XOR	R3, R0, 6
	CMP	R3, R2
	FAILNZ	0x02

	STOP
