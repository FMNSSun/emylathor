; This tests the CALL instruction
	XOR	R0, R0, R0
	XOR	R1, R1, R1
	LOADADR	R0, _increment
	CALL	R0		; _increment increments R1
	TEST	R1		
	FAILIZ	0x01		; if zero, then the call went wrong
	STOP
.label _increment
	ADD	R1, R1, 1
	JMP	RC
	FAIL	0x02		; should not come here
