;
; Add a 32 bit unsigned number to a 64 bit unsigned number
; 0xFFFFFFFF + 0x00000003 = 0x0000000100000002
;
	load32	r0,	@0xFFFFFFFF
	load32	r1,	@0x00000000
	load32	r2,	@0x00000003
	add	r0,	r0,	r2
	addic	r1,	r1,	1
	load32	r3,	@0x00000002
	load32	r4,	@0x00000001
	cmp	r3,	r0
	failnz	0x01
	cmp	r4,	r1
	failnz	0x02
	stop
	
	
