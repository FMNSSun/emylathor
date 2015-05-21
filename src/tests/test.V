; Tests the TEST instruction
	; Positive, non-zero
	LOAD32	R0, @16
	TEST	R0
	FAILIZ	0x01
	FAILIS	0x02

	; Positive, zero
	LOAD32 	R0, @0
	TEST	R0
	FAILNZ	0x03
	FAILIS	0x04

	; Negative, non-zero
	LOAD32	R0, @-2
	TEST	R0
	FAILIZ	0x05
	FAILNS	0x06

	STOP
