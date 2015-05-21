; This tests the JMP instruction	
	XOR	R0, R0, R0
	LOADADR	R0, _cont
	JMP	R0
	FAIL	0x01		; we should jump over this
	FAIL	0x02
.label _cont
	STOP
