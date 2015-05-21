	load32	r0,	@6000
	load32	r1,	@305419896	; 0x12345678
	storeid	r0,	r1,	0
	xor	r2,	r2,	r2
	loadid	r2ll,	r0,	0
	xor	r3,	r3,	r3
	loadi	r3ll,	120
	cmp	r2,	r3
	failnz	0x01
	stop
