; underflow
	load32	r0, @12
	loadadr r1, _ov_isr
	storeid r0, r1, 0
	load32	r0, @3
	load32	r1, @-4
	storerd r0, r0, r1
	fail	0x01
	fail	0x01
	fail	0x01
.label _ov_isr
	stop
	fail	0x02
	
