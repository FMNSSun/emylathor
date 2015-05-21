;Tests the storeid instruction with 8bit a operand	
	load32	r0,	@4096		; target address
	loadi	r1ll,	16		; 0x10
	loadi	r1lh,	32		; 0x20
	loadi	r1hl,	48		; 0x30
	loadi	r1hh,	64		; 0x40 => mem[4096] = DWOR 0x40302010
	storeid	r0,	r1ll,	0	; write the bytes to 4096, 4096+1, 4096+2 and 4096 + 3
	add	r0,	r0,	1
	storeid	r0,	r1lh,	0
	add	r0,	r0,	1
	storeid	r0,	r1hl,	0
	add	r0,	r0,	1
	storeid	r0,	r1hh,	0
	sub	r0,	r0,	3	; set r0 back to 4096
	loadid	r2,	r0,	0	; 32bit load to R2
	load32	r3,	@1076895760	; this is 0x40302010, it's what we expect
	cmp	r2,	r3		; compare
	failnz	0x01			; fail if not equal
	stop
