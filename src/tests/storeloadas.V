	; magic value
	load32		r0,	0x12345678
	movfc 		r1,	ssp		; save ssp
	sub		r3,	r1,	4
	storeas		r0			; push r0
	movfc		r2,	ssp		; get ssp
	cmp		r2,	r3		; ssp should now be ssp - 4
	failnz		0x01
	xor		r0,	r0,	r0	; clear r0
	loadas		r0			; pop r0
	movfc		r2,	ssp		; get ssp
	cmp		r1,	r2		; ssp should be as it was before
	failnz		0x02
	load32		r4,	0x12345678
	cmp		r0, 	r4
	failnz		0x03
	stop
