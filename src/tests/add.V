; Tests the ADD instruction

	; Test result of positive + positive
	LOAD32	R0, @1024
	LOAD32	R1, @6
	LOAD32	R2, @1030
	ADD	R0, R0, R1
	CMP	R0, R2
	FAILNZ 0x01

	; Test result of positive + negative
	LOAD32	R0, @1024
	LOAD32	R1, @-6
	LOAD32	R2, @1018
	ADD	R0, R0, R1
	CMP	R0, R2
	FAILNZ 0x02

	; Test result of negative + negative
	LOAD32	R0, @-256
	LOAD32	R1, @-256
	LOAD32	R2, @-512
	ADD	R0, R0, R1
	CMP	R0, R2
	FAILNZ 0x03

	; Test immediate add
	LOAD32	R0, @1024
	LOAD32	R2, @1030
	ADD	R4, R0, 6
	CMP	R4, R2
	FAILNZ 0x04

	; Todo: Test carry and overflow flags

	STOP
