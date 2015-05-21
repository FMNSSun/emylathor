	load32	r0, @16
	loadadr r1, _il_isr
	storeid r0, r1, 0	
	int	2
	fail	0x01
	fail	0x01
	fail	0x01
.label _il_isr
	stop
	fail	0x02
	
