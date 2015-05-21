	; isr setup
	load32	r0, @4
	loadadr r1, _pt_isr
	storeid r0, r1, 0		
	
	; switch to unprivileged mode
	movfc	r0,	cr1
	load32	r1,	@2
	not	r1,	r1
	and	r0,	r0,	r1
	movtc	cr1,	r0

	movtc	mr1,	r0

	fail	0x01
.label _pt_isr
	stop
	fail	0x02
