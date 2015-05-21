/*
 * Copyright (c) 2013, Roman Müntener
 *
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "dma.h"
#include "memory.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

_va_dma int dma_request_transfer(dword addr, byte* buffer, dword size, int type, dma_callback callback, emulator_context* ectx) {
	dma_transfer* t = malloc(sizeof(dma_transfer));

	if(t == NULL) {
		return 0;
	}

	t->addr = addr;
	t->buffer = buffer;
	t->type = type;
	t->callback = callback;
	t->size = size;

	g_async_queue_push(ectx->dma_context->transfers, t);

	return 1;
}

_va_dma void* dma_background(void* eectx) {
	emulator_context* ectx = (emulator_context*)eectx;
	dma_context* dctx = ectx->dma_context;
	memory_context* mctx = ectx->memory_context;

	//TODO: Callback?
	//TODO: Handle cache writebacks and cache invalidates automatically.

	while(1) {
		#if DUMP_DMA || DUMP_ALL
		PUTLOG2(ectx, "[DMA] :: Waiting for requests...");
		#endif

		/* pop blocks until data is available */
		dma_transfer* t = g_async_queue_pop(dctx->transfers);

		dword addr = t->addr;
		byte* buffer = t->buffer;
		int type = t->type;
		dma_callback cbck = t->callback;
		dword size = t->size;
		
		dword blocks = (t->size / mctx->cache_line_size) + 1;

		// Perform cache writebacks on read transfers
		if(type == 0) {
			for(dword i = 0; i < blocks; i++) {
				int ln = mem_find_line(addr + i * mctx->cache_line_size, ectx);
				if(ln >= 0) { //we need to write back...
					mem_cache_writeback(ln, addr, ectx);
				}
			}
		}

		#if DUMP_DMA || DUMP_ALL
		PUTLOG2(ectx, "[DMA] :: Got request...");
		PUTLOG(ectx, "[DMA] :: addr = %08x, type = %d, size = %08x, buffer = %p", addr, type, size, buffer);
		#endif

		/* Lock the bus */
		#if DUMP_ALL
		PUTLOG2(ectx, "[DMA] :: Locking bus...");
		#endif
		g_mutex_lock(mctx->bus_lock);
		#if DUMP_ALL
		PUTLOG2(ectx, "[DMA] :: Locked bus...");
		#endif

		/* Transfer data */
		if(type == 0) { // READ
			memcpy(buffer, mctx->memory + addr, (size_t) size);
		} 
		else if(type == 1) { // WRITE
			memcpy(mctx->memory + addr, buffer, (size_t) size);
		}
	
		/* Unlock the bus */
		#if DUMP_ALL
		PUTLOG2(ectx, "[DMA] :: Unlocking bus...");
		#endif
		g_mutex_unlock(mctx->bus_lock);

		// Perform cache loads on write transfers
		if(type == 1) {
			for(dword i = 0; i < blocks; i++) {
				int ln = mem_find_line(addr + i * mctx->cache_line_size, ectx);
				if(ln >= 0) { //we need to reload that line...
					mem_cache_load(ln, addr, 0, ectx); //0 = no implicit writeback
				}
			}
		}

		free(t); //don't forget to free that

		/* Call the callback */
		cbck(ectx);
	}

	return NULL;
}
