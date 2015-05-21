	load32	r0, @8192
	load32	r1, @0x12345678
	storeid r0, r1l, 0
	storeid r0, r1h, 2
	mov	r2, r1
	xor	r1, r1, r1
	loadid	r1, r0, 0
	cmp	r2, r1
	failnz	0x01
	stop
