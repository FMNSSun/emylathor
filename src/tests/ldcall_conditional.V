	xor		r0,	r0,	r0
	cmp		r0,	0
	ldcallnz	r0, _fail
	ldcalliz	r0, _stop
	fail		0x02
.label _fail
	fail	0x01
.label _stop
	stop
