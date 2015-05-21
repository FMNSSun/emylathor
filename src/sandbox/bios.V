	ldjmp r0, _main
.align 4 0
.label _msg
.byte 0x65
.byte 0x6d
.byte 0x75
.byte 0x6c
.byte 0x61
.byte 0x74
.byte 0x68
.byte 0x6f
.byte 0x72
.byte 0x00
.align 4 0
.label _vram
.dword @0x00002000
.align 4 0
.label _blackWhite
.byte 0x0d
.align 4 0
.label _vport
.dword @0x00000010
.label _main
	sub sp, sp, 8
	push bp
	push rc
	mov bp, sp
	loadadr r0, _vram ; numsign
	push r0
	pop r0
	loadid r0, r0, 0
	push r0
	pop r0
	storeid bp, r0, 8
	loadadr r0, _msg ; numsign
	push r0
	pop r0
	xor r1, r1, r1
	loadid r1ll, r0, 0
	push r1
	pop r0
	storeid bp, r0, 12
	loadadr r0, _msg ; numsign
	push r0
	ldcall r0, _printStr
.label _while_0_start
	load32 r0, @0x00000001
	push r0
	load32 r0, @0x00000001
	push r0
	pop r1
	pop r0
	cmp r0, r1
	xor r0, r0, r0
	oriz r0, r0, 1
	push r0
	pop r0
	test r0
	ldjmpiz r0, _while_1_end
	ldjmp r0, _while_0_start
.label _while_1_end
;---
.label _printStr
	sub sp, sp, 8
	push bp
	push rc
	mov bp, sp
	loadadr r0, _vram ; numsign
	push r0
	pop r0
	loadid r0, r0, 0
	push r0
	pop r0
	storeid bp, r0, 12
	load32 r0, @0x00000002
	push r0
	pop r0
	storeid bp, r0, 16
	loadid r0, bp, 20 ; var ptr
	push r0
	load32 r0, @0x00000000
	push r0
	pop r1
	pop r0
	cmp r0, r1
	xor r0, r0, r0
	oriz r0, r0, 1
	push r0
	pop r0
	test r0
	ldjmpiz r0, _if_2_end
	fail 0x01
.label _if_2_end
.label _while_3_start
	loadid r0, bp, 20 ; var ptr
	push r0
	pop r0
	xor r1, r1, r1
	loadid r1ll, r0, 0
	push r1
	load32 r0, @0x00000000
	push r0
	pop r1
	pop r0
	cmp r0, r1
	xor r0, r0, r0
	ornz r0, r0, 1
	push r0
	pop r0
	test r0
	ldjmpiz r0, _while_4_end
	loadid r0, bp, 12 ; var vramPtr
	push r0
	load32 r0, @0x00000002
	push r0
	loadid r0, bp, 16 ; var idx
	push r0
	pop r1
	pop r0
	mul r0, r0, r1
	push r0
	loadadr r0, _blackWhite ; numsign
	push r0
	pop r0
	xor r1, r1, r1
	loadid r1ll, r0, 0
	push r1
	pop r1
	pop r2
	pop r0
	add r0, r0, r2
	storeid r0, r1ll, 0
	loadid r0, bp, 12 ; var vramPtr
	push r0
	load32 r0, @0x00000002
	push r0
	loadid r0, bp, 16 ; var idx
	push r0
	pop r1
	pop r0
	mul r0, r0, r1
	push r0
	load32 r0, @0x00000001
	push r0
	pop r1
	pop r0
	add r0, r0, r1
	push r0
	loadid r0, bp, 20 ; var ptr
	push r0
	pop r0
	xor r1, r1, r1
	loadid r1ll, r0, 0
	push r1
	pop r1
	pop r2
	pop r0
	add r0, r0, r2
	storeid r0, r1ll, 0
	load32 r0, @0x00000001
	push r0
	loadid r0, bp, 20
	pop r1
	add r0, r0, r1
	storeid bp, r0, 20
	load32 r0, @0x00000001
	push r0
	loadid r0, bp, 16
	pop r1
	add r0, r0, r1
	storeid bp, r0, 16
	ldjmp r0, _while_3_start
.label _while_4_end
	loadadr r0, _vport ; numsign
	push r0
	pop r0
	loadid r0, r0, 0
	push r0
	loadid r0, bp, 12 ; var vramPtr
	push r0
	pop r1
	pop r0
	out r0, r1
	loadid r0, bp, 16 ; var idx
	push r0
	pop r0
	mov sp, bp
	pop rc
	pop bp
	add sp, sp, 8
	add sp, sp, 4
	jmp rc
;---

