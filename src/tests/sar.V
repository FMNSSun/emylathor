; Tests the SAR instruction
	load32	r0, @4096
	sar	r0, r0, 2
	load32	r1, @1024
	cmp	r0, r1
	failnz	0x01

	load32	r0, @-4096
	sar	r0, r0, 2
	load32	r1, @-1024
	cmp	r0, r1
	failnz	0x02

	stop
