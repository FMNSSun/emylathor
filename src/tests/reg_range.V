; tests register ranges for the instructions storema, loadma

	LOAD32	R1,	@0x03848
	LOAD32 	R2,	@0x02343
	
	STOREMA	SP,	R1:R2
	LOADMA	SP,	R3:R4
	
	CMP	R2,	R4
	FAILNZ	0x01
	
	CMP	R1,	R3
	FAILNZ	0x02
	
	STOP