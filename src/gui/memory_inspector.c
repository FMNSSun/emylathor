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

#include <common/types.h>
#include <stdio.h>
#include <stdlib.h>
#include "cpu/mmu.h"
#include "memory_inspector.h"


void gui_mem_write_dword_p(dword d, dword addr, emulator_context* ectx) {
	
	if(addr % 4) {
		PUTLOG(ectx, "[ERROR in GUI] :: Invalid alignment, tried to write to address %08x", addr);
		return;
	}
	if(addr + 3 >= ectx->memory_context->memory_size){
		PUTLOG(ectx, "[ERROR in GUI] :: Invalid address, tried to write to address %08x", addr);
		return;
	}
	
	
	memory_context* ctx = ectx->memory_context;

	ctx->memory[addr + 0] = LLBYTE_FROM_DWORD(d);
	ctx->memory[addr + 1] = LHBYTE_FROM_DWORD(d);
	ctx->memory[addr + 2] = HLBYTE_FROM_DWORD(d);
	ctx->memory[addr + 3] = HHBYTE_FROM_DWORD(d);

}

/* Get word from cache if related cacheline present, else read from memory */
dword gui_mem_get_dword_p(dword addr, emulator_context* ectx) {

	if(addr % 4) {
		PUTLOG(ectx, "[ERROR in GUI] :: Invalid alignment, tried to read from address %08x", addr);
		return 0;
	}

	memory_context* ctx = ectx->memory_context;

	int in_cache = gui_mem_find_line(addr, ectx);
	if(in_cache >= 0){
		cache_context* chctx = &ctx->lines[in_cache];
		byte* cache = chctx->cache;

		byte ll = cache[addr + 0 - chctx->cache_start];
		byte lh = cache[addr + 1 - chctx->cache_start];
		byte ul = cache[addr + 2 - chctx->cache_start];
		byte uh = cache[addr + 3 - chctx->cache_start];
		return DWORD_FROM_BYTES(uh, ul, lh, ll);
	}else{
		if(addr + 3 >= ectx->memory_context->memory_size) {
			PUTLOG(ectx, "[ERROR in GUI] :: Invalid address, tried to read from address %08x", addr);
			return 0;
		}
		
		byte ll = ectx->memory_context->memory[addr + 0];
		byte lh = ectx->memory_context->memory[addr + 1];
		byte ul = ectx->memory_context->memory[addr + 2];
		byte uh = ectx->memory_context->memory[addr + 3];
		return DWORD_FROM_BYTES(uh, ul, lh, ll);
	}
}

int gui_mem_write_dword_v(dword out, dword addr, emulator_context* ectx) {
	
	if(!gui_mmu_is_enabled(ectx)) {
		/* MMU is not active. Just access physical memory directly */
		gui_mem_write_dword_p(out, addr, ectx);
		return 1;
	}
	gui_mem_write_dword_p(out, gui_mmu_translate_address(addr, ectx), ectx);
	return 1;
}

int gui_mem_get_dword_v(dword addr, dword* out, emulator_context* ectx) {
	
	if(!gui_mmu_is_enabled(ectx)) {
		/* MMU is not active. Just access physical memory directly */
		*out = gui_mem_get_dword_p(addr, ectx);
		return 1;
	}
	*out = gui_mem_get_dword_p(gui_mmu_translate_address(addr, ectx), ectx);
	return 1;
	
}

dword gui_mmu_get_page_entry(dword addr, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	/* Location of page directory */
	dword pd = cctx->cregs[CPU_CREG_MR1];

	/* Location of page table */
	dword pt = gui_mem_get_dword_p(pd + 4*MMU_PDBITS(addr), ectx);

	return  gui_mem_get_dword_p(pt + 4*MMU_PTBITS(addr), ectx);
}

dword gui_mmu_translate_address(dword addr, emulator_context* ectx) {
	dword entry = gui_mmu_get_page_entry(addr, ectx);
	dword frame = MMU_FRMBITS(entry);
	dword offset = MMU_OFBITS(addr);

	return ((frame << 12u) | (offset));
}

int gui_mmu_is_enabled(emulator_context* ectx) {
	return (ectx->cpu_context->cregs[CPU_CREG_MR2] & MR2_MMU_ENABLED);
}

int gui_mem_find_line(dword addr, emulator_context* ectx) {
	memory_context* ctx = ectx->memory_context;

	int i;
	int lines = ctx->cache_lines;
	for(i = 0; i < lines; i++) {
		cache_context* line = &ctx->lines[i];
		if((addr >= line->cache_start) && (addr < line->cache_start + ctx->cache_line_size)) {
			return i;
		}
	}

	return -1;
}

