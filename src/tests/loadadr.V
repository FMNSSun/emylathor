; Test for the LOADADR pseudo instruction

	.offset @10
	.label _test
	LOADADR	R0,	_test
	LOAD32	R1,	@10
	CMP	R0,	R1
	FAILNZ	0x01
	STOP
