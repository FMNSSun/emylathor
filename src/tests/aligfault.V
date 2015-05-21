; This tests if an alignment fault when writing to a non-aligned address

	;ISR SETUP
	LOAD32	R0, @32
	LOADADR	R1, _pt_isr
	STOREID	R0, R1, 0	

	;Try access
	load32 r0, 0xAFFFFFF1
	loadid r0, r0, 0
	
	FAIL 0x01

.label _pt_isr
	STOP
	FAIL 0x02
