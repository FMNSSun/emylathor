;Sandbox example program writes 'Hello ' in Black-White to VAGC.
	load32	r0, @1695369229	;He
	load32	r1, @8192
	storeid	r1, r0, 0
	add	r1, r1, 4
	load32	r0, @1812818957	;ll
	storeid	r1, r0, 0
	add	r1, r1, 4
	load32  r0, @537751309	;o 
	storeid	r1, r0, 0
	load32	r2, @0x10	; 0x10 = VAGC port
	load32  r1, @8192	; we write stuff to 8192
	out	r2, r1
