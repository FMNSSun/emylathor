; isr setup
	LOAD32	R0, @4
	LOADADR	R1, _pt_isr
	STOREID	R0, R1, 0		
	
	; switch to unprivileged mode
	MOVFC	R0,	CR1
	LOAD32	R1,	@2
	NOT	R1,	R1
	AND	R0,	R0,	R1
	MOVTC	CR1,	R0

	XCHGC	R0,	MR1

	FAIL	0x01
.label _pt_isr
	STOP
	FAIL	0x02
