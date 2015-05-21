; This tests the CALL instruction with relative addresses
	XOR	R0, R0, R0
	XOR	R1, R1, R1
	LOADI	R0LL, 2		; set R0 = 2d
	CALL	12		; _increment increments R1
	CMP	R1, R0		; R1 should be 2 now		
	FAILNZ	0x01		; if not, then the call went wrong
	STOP
	
.label _increment
	MOV	RB,	RC
	ADD	R1, R1, 1
	TEST	R1
	FAILIZ	0x02		; if zero, then add failed
	JMP	8		; skip next two instructions
	ADD	R1, R1, 1
	JMP	RC
	CALL	-12		; relative call with negative offset
	JMP	RB
	FAIL	0x03		; should not come here
