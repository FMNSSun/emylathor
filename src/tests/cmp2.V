; Tests nzns and nznc

load32		r0, @1
cmp		r0, 0
jmpnzns		4
fail		0x01	; OK 1 > 0

load32		r0, @-1
cmp		r0, 0
failns		0x02	; Ok -1 < 0
failic		0x03	; Ok (-1 unsigned) > 0
failnzns	0x04	; WRONG -1 > 0 (signed)
failiz		0x05	; OK -1 != 0

load32		r0, @0
cmp		r0, 0
failnzns	0x06	; WRONG 0 > 0

load32		r0, @-4
load32		r1, @-5
cmp		r0, r1
jmpnzns		4	; OK -4 > -5
fail		0x07

load32		r0, @-4
cmp		r0, 0
failns		0x08	; WRONG -4 >= 0
jmpis		4
fail		0x09	; OK -4 < 0

load32		r0, @5
cmp		r0, 4
jmpnznc		4	
fail		0x0A	; Ok 5 > 4

load32		r0, @5
cmp		r0, 5
failnznc	0x0B	; Wrong 5 > 5

load32		r0, @-3
load32		r1, @-3
cmp		r0, r1
failnzns	0x0C	; Wrong -3 > -3
failis		0x0D	; Wrong -3 < -3
jmpns		4
fail		0x0E	; Ok, -3 >= -3
failnz		0x0F	; Wrong, -3 != -3

stop

