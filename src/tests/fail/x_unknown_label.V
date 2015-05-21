; Test if unknown labels produce an error

	.label _label1

	
	LOADADR	R0,	_label2
	
	FAIL	0x01