	load32	r0, @8192
	load32	r1, @0x12345678
	load32  r2, @0		; r2 is the displacement
	mov	r3, r1

	; write r1 bytewise to mem[r0]
	storerd	r0, r1ll, r2
	add	r2, r2, 1
	storerd r0, r1lh, r2
	add     r2, r2, 1
	storerd r0, r1hl, r2
	add	r2, r2, 1
	storerd r0, r1hh, r2

	; clear / load / compare
	xor	r1, r1, r1
	loadid  r1, r0, 0
	cmp	r1, r3
	failnz	0x01

	; write r1 wordwise to mem[r0]
	xor	r2, r2, r2
	storerd r0, r1l, r2
	add	r2, r2, 2
	storerd r0, r1h, r2

	; clear / load / compare
	xor	r1, r1, r1
	loadid	r1, r0, 0
	cmp	r1, r3
	failnz	0x02

	xor	rc, rc, rc
	storeid r0, rc, 0	; clear mem[8192]

	; test negative displacement
	mov	ra, r0
	add	ra, ra, 4
	load32  rb, @-4
	storerd	ra, r1, rb	; displacement -4 to ra where ra = r0 + 4
	xor	r1, r1, r1
	loadid	r1, r0, 0
	cmp	r1, r3
	failnz	0x03
	
