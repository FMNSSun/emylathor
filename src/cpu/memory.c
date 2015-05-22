/*
Copyright (c) 2013, Roman Muentener
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: Redistributions of
source code must retain the above copyright notice, this list of conditions and
the following disclaimer. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ’AS IS’ AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
*/

#include "memory.h"

#include "glib.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


_va_mem int mem_write_byte_p(byte b, dword addr, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Writing physical byte %02x to %08x", b, addr);
	#endif

	memory_context* ctx = ectx->memory_context;

	ctx->memory[addr] = b;

	return 1;
}

_va_mem int mem_write_word_p(word w, dword addr, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Writing physical word %04x to %08x", w, addr);
	#endif

	memory_context* ctx = ectx->memory_context;

	ctx->memory[addr + 0] = LBYTE_FROM_WORD(w);
	ctx->memory[addr + 1] = HBYTE_FROM_WORD(w);

	return 1;
}

_va_mem int mem_write_dword_p(dword d, dword addr, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Writing physical dword %08x to %08x", d, addr);
	#endif

	memory_context* ctx = ectx->memory_context;


	ctx->memory[addr + 0] = LLBYTE_FROM_DWORD(d);
	ctx->memory[addr + 1] = LHBYTE_FROM_DWORD(d);
	ctx->memory[addr + 2] = HLBYTE_FROM_DWORD(d);
	ctx->memory[addr + 3] = HHBYTE_FROM_DWORD(d);

	return 1;
}

_va_mem int mem_get_byte_p(dword addr, byte* val, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Reading physical byte from %08x", addr);
	#endif

	memory_context* ctx = ectx->memory_context;

	byte b = ctx->memory[addr];

	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Pysical byte is %02x", b);
	#endif

	*val = b;

	return 1;
}

_va_mem int mem_get_word_p(dword addr, word* val, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Reading physical word from %08x", addr);
	#endif

	memory_context* ctx = ectx->memory_context;

	byte lo = ctx->memory[addr + 0];
	byte hi = ctx->memory[addr + 1];

	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Pysical word is %04x", WORD_FROM_BYTES(hi, lo));
	#endif

	*val = WORD_FROM_BYTES(hi, lo);

	return 1;
}

_va_mem int mem_get_dword_p(dword addr, dword* val, emulator_context* ectx) {
	
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Reading physical dword from %08x", addr);
	#endif

	memory_context* ctx = ectx->memory_context;
	
	byte ll = ctx->memory[addr + 0];
	byte lh = ctx->memory[addr + 1];
	byte ul = ctx->memory[addr + 2];
	byte uh = ctx->memory[addr + 3];

	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Pysical dword is %08x", DWORD_FROM_BYTES(uh, ul, lh, ll));
	#endif

	*val = DWORD_FROM_BYTES(uh, ul, lh, ll);

	return 1;
}

_va_mem int mem_get_instruction_p(dword addr, dword* val, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Reading instruction from address %08x", addr);
	#endif

	
	return mem_get_dword_p(addr, val, ectx);
}
