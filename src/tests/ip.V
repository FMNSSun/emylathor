;
; Tests the return address of call 0.
; Requires an offset of 4096
;
.offset @4096
	call	0
	load32	r0,	@4096		; rc should point to this instruction
					; which must reside at offset + 4 bytes
	sub	rc,	rc,	r0	; subtract offset
	cmp	rc,	4		; compare with the expected number 4
	failnz	0x01
	stop

