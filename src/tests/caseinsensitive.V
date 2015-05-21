; Example program to test lower case support :)
; Also: This tests some random stuff.
	load32	r0, @0
	load32	r1, @0
	cmp	r0, r1	
	jmpiz	4		; Skip the fail if r0 == r1
	fail	0x01		; rc should now point to this
	load32	r0, @12
	loadadr	r3, _foo	; Load address of _foo
	call	r3		; Call _foo
	stop			; STOP
	fail	0x02		; should not come here

.label _foo
	div	r0, r0, 2	; Divide by two
	cmp	r0, 6		; Check result
	failnz	0x03		; fail if not correct
	jmp	rc		; return
	fail	0x04		; should not come here
