; test raw emit directives
; by emitting instructions
; add r1, r1, 1 = ea011001
xor	r1,	r1,	r1
add	r1,	r1,	1

.byte 0xea
.byte 0x01
.byte 0x10
.byte 0x01		; these four bytes are add r1, r1, 1

.word 0x01ea
.word 0x0110		; these two words are add r1, r1, 1

.dword 0x011001ea 	; this dword is add r1, r1, 1

cmp	r1,	4	; we expect 4
failnz	0x01
stop
