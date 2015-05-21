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
#include "mmu.h"
#include "interrupts.h"

#include "glib.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

inline void MEM_LOCK_BUS(emulator_context* ectx, memory_context* ctx) {
	#if DUMP_ALL
	PUTLOG2(ectx, "[MEM] :: locking bus_lock");
	#endif
	g_mutex_lock(ctx->bus_lock); //lock the bus.
	#if DUMP_ALL
	PUTLOG2(ectx, "[MEM] :: locked bus_lock");
	#endif
}

inline void MEM_UNLOCK_BUS(emulator_context* ectx, memory_context* ctx) {
	#if DUMP_ALL
	PUTLOG2(ectx, "[MEM] :: unlocking bus_lock");
	#endif
	g_mutex_unlock(ctx->bus_lock);
}

_va_mem void mem_cache_miss(dword addr, emulator_context* ectx) {
	
	ectx->statistic_context->cache_miss_count++;
	#if DUMP_ALL || DUMP_CACHE
	PUTLOG(ectx, "[CACHE] :: Miss for address := %08x", addr);
	#endif
}

_va_mem int mem_least_used_line(emulator_context* ectx) {
	memory_context* ctx = ectx->memory_context;

	int min_hits = 0xFFFFu;
	int min_line = 0u;

	/**
	 * This searches for the line with the lowest
	 * hit count. Also it decrases the hit count of every
         * line. Why? Consider a highly used line with a hit
	 * count of 250. But now the line is never used.
	 * Due to its very high hit count, it might never be
	 * replaced even if it is never used again.
	 * This makes such lines slowly available again. 
	 */
	int i;
	for(i = 0; i < ctx->cache_lines; i++) {
		if(ctx->lines[i].hit_count < min_hits) {
			min_hits = ctx->lines[i].hit_count;
			min_line = i;
		}
		if(ctx->lines[i].hit_count > 2)
				ctx->lines[i].hit_count /= 2;
	}

	return min_line;
}

_va_mem void mem_cache_load(int line, dword addr, int writeback, emulator_context* ectx) {
	memory_context* ctx = ectx->memory_context;
	byte* memory = ctx->memory;
	cache_context* chctx = &ctx->lines[line];
	byte* cache = chctx->cache;

	/* Write back the cache */
	if(writeback)
		mem_cache_writeback(line, addr, ectx);

	#if DUMP_ALL || DUMP_CACHE
	PUTLOG(ectx, "[CACHE] :: Cache Load for address %08x to line %d", addr, line);
	#endif

	/* Calculate line start address */
	while(addr % ctx->cache_line_size)
		addr--;


	#if DUMP_ALL || DUMP_CACHE
	PUTLOG(ectx, "[CACHE] :: Cache Line Start is %08x (line := %d)", addr, line);
	#endif	

	MEM_LOCK_BUS(ectx, ctx);

	/* Load the Cache */
	if(!(addr + ctx->cache_line_size >= ctx->memory_size)) {
		int i;
		for(i = 0; i < ctx->cache_line_size; i++)
			cache[i] = memory[addr + i];
	}

	MEM_UNLOCK_BUS(ectx, ctx);
	
	chctx->cache_start = addr;
	chctx->wb_required = 0;
	chctx->hit_count = 1;
}

_va_mem void mem_cache_writeback(int line, dword addr, emulator_context* ectx) {
	memory_context* ctx = ectx->memory_context;
	byte* memory = ctx->memory;
	cache_context* chctx = &ctx->lines[line];
	byte* cache = chctx->cache;	

	#if DUMP_ALL || DUMP_CACHE
	PUTLOG(ectx, "[CACHE] :: Cache Writeback request to %08x (line := %d) for address := %08x", chctx->cache_start, line, addr);
	#endif
	if(!chctx->wb_required) {
		#if DUMP_ALL || DUMP_CACHE
		PUTLOG2(ectx, "[CACHE] :: Writeback not required. Cache has not been modified.");
		#endif
		return;
	}
	else {
		#if DUMP_ALL || DUMP_CACHE
		PUTLOG2(ectx, "[CACHE] :: Writeback required. Cache has been modified.");
		#endif
	}
	dword i;
	addr = chctx->cache_start;

	MEM_LOCK_BUS(ectx, ctx);

	/* Writeback the cache */
	if(!(addr + ctx->cache_line_size >= ctx->memory_size)) {
		for(i = 0; i < ctx->cache_line_size; i++)
			memory[addr + i] = cache[i];
	}

	MEM_UNLOCK_BUS(ectx, ctx);
}

_va_mem int mem_find_line(dword addr, emulator_context* ectx) {
	memory_context* ctx = ectx->memory_context;

	int i;
	int lines = ctx->cache_lines;
	for(i = 0; i < lines; i++) {
		cache_context* line = &ctx->lines[i];
		if((addr >= line->cache_start) && (addr < line->cache_start + ctx->cache_line_size)) {
			#if DUMP_ALL || DUMP_CACHE
			PUTLOG(ectx, "[CACHE] :: Address %08x in cache line %d with hit count %d", addr, i, line->hit_count);
			#endif
			line->hit_count++;
			ectx->statistic_context->cache_hits++;
			return i;
		}
	}

	return -1; //not in cache.... :(
}

_va_mem int mem_ensure_cache(dword addr, emulator_context* ectx) {
	int i = mem_find_line(addr, ectx);

	if(i >= 0)
		return i; //already in Cache. Nothing to do here.

	mem_cache_miss(addr, ectx);

	i = mem_least_used_line(ectx);

	#if DUMP_ALL || DUMP_CACHE
	PUTLOG(ectx, "[CACHE] :: Replacing line %d due to lowest hit count (was %d)", i, ectx->memory_context->lines[i].hit_count);
	#endif

	mem_cache_load(i, addr, 1, ectx);
	return i;
}

_va_mem void mem_write_byte_p(byte b, dword addr, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Writing physical byte %02x to %08x", b, addr);
	#endif

	memory_context* ctx = ectx->memory_context;

	int in_cache = mem_ensure_cache(addr, ectx);
	cache_context* chctx = &ctx->lines[in_cache];
	chctx->cache[addr - chctx->cache_start] = b;
	chctx->wb_required = 1;
}

_va_mem void mem_write_word_p(word w, dword addr, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Writing physical word %04x to %08x", w, addr);
	#endif

	if(addr % 2) {
		#if DUMP_ALL || DUMP_MEM
		PUTLOG2(ectx, "[MEM] :: Invalid alignment!");
		#endif
		int_fire_interrupt(INT_EXC_ALIGFAULT, ectx);
		return;
	}

	memory_context* ctx = ectx->memory_context;

	int in_cache = mem_ensure_cache(addr, ectx);
	cache_context* chctx = &ctx->lines[in_cache];
	byte* cache = chctx->cache;

	cache[addr + 0 - chctx->cache_start] = LBYTE_FROM_WORD(w);
	cache[addr + 1 - chctx->cache_start] = HBYTE_FROM_WORD(w);

	chctx->wb_required = 1;
}

_va_mem void mem_write_dword_p(dword d, dword addr, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Writing physical dword %08x to %08x", d, addr);
	#endif

	if(addr % 2) {
		#if DUMP_ALL || DUMP_MEM
		PUTLOG2(ectx, "[MEM] :: Invalid alignment!");
		#endif
		int_fire_interrupt(INT_EXC_ALIGFAULT, ectx);
		return;
	}

	memory_context* ctx = ectx->memory_context;

	int in_cache = mem_ensure_cache(addr, ectx);
	cache_context* chctx = &ctx->lines[in_cache];
	byte* cache = chctx->cache;

	cache[addr + 0 - chctx->cache_start] = LLBYTE_FROM_DWORD(d);
	cache[addr + 1 - chctx->cache_start] = LHBYTE_FROM_DWORD(d);
	cache[addr + 2 - chctx->cache_start] = HLBYTE_FROM_DWORD(d);
	cache[addr + 3 - chctx->cache_start] = HHBYTE_FROM_DWORD(d);

	chctx->wb_required = 1;
}

_va_mem byte mem_get_byte_p(dword addr, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Reading physical byte from %08x", addr);
	#endif

	memory_context* ctx = ectx->memory_context;

	int in_cache = mem_ensure_cache(addr, ectx);
	cache_context* chctx = &ctx->lines[in_cache];

	byte b = chctx->cache[addr - chctx->cache_start];

	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Pysical byte is %02x", b);
	#endif

	return b;
}

_va_mem word mem_get_word_p(dword addr, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Reading physical word from %08x", addr);
	#endif

	if(addr % 2) {
		#if DUMP_ALL || DUMP_MEM
		PUTLOG2(ectx, "[MEM] :: Invalid alignment!");
		#endif
		int_fire_interrupt(INT_EXC_ALIGFAULT, ectx);
		return 0x00u;
	}

	memory_context* ctx = ectx->memory_context;

	int in_cache = mem_ensure_cache(addr, ectx);
	cache_context* chctx = &ctx->lines[in_cache];
	byte* cache = chctx->cache;

	byte lo = cache[addr + 0 - chctx->cache_start];
	byte hi = cache[addr + 1 - chctx->cache_start];

	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Pysical word is %04x", WORD_FROM_BYTES(hi, lo));
	#endif

	return WORD_FROM_BYTES(hi, lo);
}

_va_mem dword mem_get_dword_p(dword addr, emulator_context* ectx) {
	
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Reading physical dword from %08x", addr);
	#endif

	if(addr % 2) {
		#if DUMP_ALL || DUMP_MEM
		PUTLOG2(ectx, "[MEM] :: Invalid alignment!");
		#endif
		int_fire_interrupt(INT_EXC_ALIGFAULT, ectx);
		return 0x00u;
	}

	memory_context* ctx = ectx->memory_context;

	int in_cache = mem_ensure_cache(addr, ectx);
	cache_context* chctx = &ctx->lines[in_cache];
	byte* cache = chctx->cache;

	byte ll = cache[addr + 0 - chctx->cache_start];
	byte lh = cache[addr + 1 - chctx->cache_start];
	byte ul = cache[addr + 2 - chctx->cache_start];
	byte uh = cache[addr + 3 - chctx->cache_start];

	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Pysical dword is %08x", DWORD_FROM_BYTES(uh, ul, lh, ll));
	#endif

	return DWORD_FROM_BYTES(uh, ul, lh, ll);
}

_va_mem int mem_write_byte_v(byte out, dword addr, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Writing byte (%02x) to virtual address %08x", out, addr);
	#endif

	if(!mmu_is_enabled(ectx)) {
		/* MMU is not active. Just access physical memory directly */
		mem_write_byte_p(out, addr, ectx);
		return 1;
	}
	if(!mmu_ensure_page(addr, ectx, MMU_ACCESS_MODE_WRITE)) {
		return 0;
	}
	else {
		mem_write_byte_p(out, mmu_translate_address(addr, ectx), ectx);
		return 1;
	}
}

_va_mem int mem_write_word_v(word out, dword addr, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Writing word to virtual address %08x", addr);
	#endif

	if(!mmu_is_enabled(ectx)) {
		/* MMU is not active. Just access physical memory directly */
		mem_write_word_p(out, addr, ectx);
		return 1;
	}
	if(!mmu_ensure_page(addr, ectx, MMU_ACCESS_MODE_WRITE)) {
		return 0;
	}
	else {
		mem_write_word_p(out, mmu_translate_address(addr, ectx), ectx);
		return 1;
	}
}

_va_mem int mem_write_dword_v(dword out, dword addr, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Writing dword (%08x) to virtual address %08x", out, addr);
	#endif

	if(!mmu_is_enabled(ectx)) {
		/* MMU is not active. Just access physical memory directly */
		mem_write_dword_p(out, addr, ectx);
		return 1;
	}
	if(!mmu_ensure_page(addr, ectx, MMU_ACCESS_MODE_WRITE)) {
		return 0;
	}
	else {
		mem_write_dword_p(out, mmu_translate_address(addr, ectx), ectx);
		return 1;
	}
}

_va_mem int mem_get_byte_v(dword addr, byte* out, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Reading byte from virtual address %08x", addr);
	#endif

	if(!mmu_is_enabled(ectx)) {
		/* MMU is not active. Just access physical memory directly */
		*out = mem_get_byte_p(addr, ectx);
		return 1;
	}
	if(!mmu_ensure_page(addr, ectx, MMU_ACCESS_MODE_READ)) {
		return 0;
	}
	else {
		*out = mem_get_byte_p(mmu_translate_address(addr, ectx), ectx);
		return 1;
	}
}

_va_mem int mem_get_word_v(dword addr, word* out, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Reading word from virtual address %08x", addr);
	#endif

	if(!mmu_is_enabled(ectx)) {
		/* MMU is not active. Just access physical memory directly */
		*out = mem_get_word_p(addr, ectx);
		return 1;
	}
	if(!mmu_ensure_page(addr, ectx, MMU_ACCESS_MODE_READ)) {
		return 0;
	}
	else {
		*out = mem_get_word_p(mmu_translate_address(addr, ectx), ectx);
		return 1;
	}
}

_va_mem int mem_get_dword_v(dword addr, dword* out, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Reading dword from virtual address %08x", addr);
	#endif

	if(!mmu_is_enabled(ectx)) {
		/* MMU is not active. Just access physical memory directly */
		*out = mem_get_dword_p(addr, ectx);
		return 1;
	}
	if(!mmu_ensure_page(addr, ectx, MMU_ACCESS_MODE_READ)) {
		return 0;
	}
	else {
		*out = mem_get_dword_p(mmu_translate_address(addr, ectx), ectx);
		return 1;
	}
}

_va_mem int mem_get_instruction_v(dword addr, dword* out, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_MEM
	PUTLOG(ectx, "[MEM] :: Reading instruction from virtual address %08x", addr);
	#endif

	if(!mmu_is_enabled(ectx)) {
		/* MMU is not active. Just access physical memory directly */
		*out = mem_get_dword_p(addr, ectx);
		return 1;
	}
	if(!mmu_ensure_page(addr, ectx, MMU_ACCESS_MODE_EXEC)) {
		return 0;
	}
	else {
		*out = mem_get_dword_p(mmu_translate_address(addr, ectx), ectx);
		return 1;
	}
}
