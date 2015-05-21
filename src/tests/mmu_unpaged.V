;
; Please see mmu_simple.V on how the setup works
;

.offset	@0
.at	@0x101000
	.label		_page_table
.at	@0x100000
	.label		_page_directory

.offset	@4096
.at 	@0	
	; Ok, let's go

	load32		r0, 	@20
	loadadr 	r1, 	_pg_isr
	storeid 	r0, 	r1, 	0
	
	loadadr		r0,	_page_table			; Address of the page table
	load32		r1,	@0x000001			; Frame bits of page at 4096
	loadadr		r2,	_page_directory			; Address of the page directory
	storeid		r2,	r0,	0			; Set up the pointer to our page table
	shl		r1,	r1,	12			; We need to shift the frame bits
								; to the left (due to control bits)
	xor		r3,	r3,	r3
	or		r3,	r3,	4			; set the executable flag
	or		r3,	r3,	1			; set the page present flag
	or		r1,	r1,	r3
	storeid		r0,	r1,	4			; write entry

	load32		r1,	@0
	shl		r1,	r1, 12

	xor		r3,	r3,	r3
	or		r3,	r3,	4			; set the executable flag
	or		r3,	r3,	1			; set the page present flag
	or		r3,	r3,	16			; set the write flag
	or		r1,	r1,	r3
	storeid		r0,	r1,	0			; write entry

	movtc		mr1,	r2				; tell the mmu where our page directory is

	movfc		r4,	mr2
	or		r4,	r4,	1			; enable mmu flag
	movtc		mr2,	r4				; mmu should now be enabled
	
								; ok... let's access an adresss with no page table set
	load32		r7,	@0x10000000
	loadid		r7,	r7,	0			; this should produce an error

	fail		0x01
	
.label	_pg_isr
	movfc		ra,	mr4
	cmp		ra,	4
	failnz		0x02
	stop
