; Test for INT
	
	; Register ISR
	XOR	r0,	r0,	r0
	LOADADR	r1,	_isr_myisr
	STOREID	r0,	r1,	132	; 33 * 4 = 132
	
	INT 33
	FAIL	0x01
	
	.label _isr_myisr
	STOP
	IRET

	
