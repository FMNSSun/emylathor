; Tests the SHR instruction
	load32	r0, @4
	shr	r1, r0, 1
	failic	0x01
	cmp	r1, 2
	failnz	0x02
	load32	r3, @3
	shr	r1, r0, r3
	failic	0x03
	cmp	r1, 0
	failnz	0x04
	load32	r0, @5
	shr	r1, r0, 2
	failnc	0x05
	stop
