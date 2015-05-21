; This tests the JMP instruction with relative addresses
	JMP	12
	JMP	16	; + 0  / -16 
	FAIL	0x01	; + 4  / - 12 / + 0
	FAIL	0x02	; + 8  / - 8 / + 4
	JMP	-16	; + 12  / - 4 / + 8
	FAIL	0x03	; / / + 12
	STOP		;  / / + 16
