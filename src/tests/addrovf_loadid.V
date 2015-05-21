	load32	r0, @12
	loadadr r1, _ov_isr
	storeid r0, r1, 0	
	load32	r0, @0xFFFFFFFF
	loadid	r0, r0, 1
	fail	0x01
	fail	0x01
	fail	0x01
.label _ov_isr
	stop
	fail	0x02
	
