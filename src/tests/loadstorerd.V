; Test for the instructions LOADRD and STORERD

	; 32 bit destination
	LOAD32	R0,	0xFFEEAABB
	LOAD32	R1,	0x00000004 ;displacement
	LOAD32	R2,	@8192 ; Adress
	STORERD	R2,	R0,	R1
	XOR	R4,	R4,	R4
	LOADRD	R4,	R2,	R1
	CMP	R4,	R0
	FAILNZ	0x01
	
	
	; 16 bit destination
	LOAD32	R0,	0x0000AABB
	LOAD32	R1,	0x0000000C ;displacement
	LOAD32	R2,	@8192 ; Adress
	STORERD	R2,	R0L,	R1
	XOR	R4,	R4,	R4
	LOADRD	R4L,	R2,	R1
	CMP	R4,	R0
	FAILNZ	0x02
	
	; 8 bit destination
	LOAD32	R4,	0x000000BB
	LOAD32	R1,	@8192 ;displacement
	LOAD32	R2,	@0 ; Adress
	STORERD	R2,	R0LL,	R1
	XOR	R0,	R0,	R0
	LOADRD	R0LL,	R2,	R1
	CMP	R0,	R4
	FAILNZ	0x02
	
	
	
	STOP