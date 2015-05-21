; Tests the MOVTC and MOVFC instructions
	load32	r0, @10
	movtc	cr2, r0
	load32	r0, @0
	load32	r1, @10
	movfc	r0, cr2
	cmp	r0, r1
	failnz	0x01
	stop
