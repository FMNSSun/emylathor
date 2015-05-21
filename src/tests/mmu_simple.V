	; We can't really enable the MMU
	; before we set up at least a page directory, a page table
	; and some entries. Else the CPU would not even know where to fetch code from :).
	
	; The specification states that we have at least 4MB of memory available.
	; so let's place our page directory at 1MB = 0x100000
	; The size of a page directory is 4KB and the same applies for a page table
	; so let's put our page table at 1MB + 4KB = 0x101000

	; Since our code is loaded to 4096d = 0x1000 which means it's page directory is 
	; 0x0000 and it's page table is ((4096 >> 12) & 0x3ff) = 1
	; We'll just map the first two pages directly to their physical location

.offset	@0
.at	@0x101000
	.label		_page_table
.at	@0x100000
	.label		_page_directory

.offset	@4096
.at 	@0	
	; Ok, let's go
	
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

	movtc		mr1,	r2				; tell the mmu where our page directory is

	movfc		r4,	mr2
	or		r4,	r4,	1			; enable mmu flag
	movtc		mr2,	r4				; mmu should now be enabled
	xchg		r1,	r1
	stop
	fail		0x01
	
	
