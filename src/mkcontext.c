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

#include "mkcontext.h"
#include "cpu/interrupts.h"
#include "hw/devm.h"
#include "cpu/dma.h"
#include "cpu/pio.h"

#include <stdlib.h>
#include <stdio.h>

#include "common/definitions.h"

inline void* emalloc(size_t x) {
	void* p = malloc(x);
	if(p == NULL) {
		fprintf(stderr, "OUT OF MEMORY!");
		exit(10);
	}
	return p;
}

inline void* ecalloc(size_t num, size_t size) {	
	void* p = calloc(num, size);
	if(p == NULL) {
		fprintf(stderr, "OUT OF MEMORY!");
		exit(10);
	}
	return p;
}

void log_stdout(char* log) {
	printf("%s\n",log);
}

emulator_context* emu_create_context(
	dword memsize,
	dword cachelines,
	dword cachelinesize
) {
	/* Check overridden default settings */
	if(!memsize)
		memsize = VA_DEFAULT_MEMORY_SIZE;
	if(!cachelines)
		cachelines = VA_DEFAULT_CACHE_LINES;
	if(!cachelinesize)
		cachelinesize = VA_DEFAULT_CACHE_LINE_SIZE;

	
	/* Allocate contexts */
	emulator_context* ectx = emalloc(sizeof(emulator_context));
	ectx->cpu_context = emalloc(sizeof(cpu_context));
	ectx->memory_context = emalloc(sizeof(memory_context));
	ectx->statistic_context = emalloc(sizeof(statistic_context));
	
	/* Set up memory context */
	ectx->memory_context->memory = ecalloc(memsize,sizeof(byte));
	ectx->memory_context->memory_size = memsize;
	ectx->memory_context->lines = ecalloc(cachelines, sizeof(cache_context));
	ectx->memory_context->cache_lines = cachelines;
	ectx->memory_context->cache_line_size = cachelinesize;
	
	/* Set up statistic context */
	ectx->statistic_context->cache_miss_count = 0;
	ectx->statistic_context->cache_hits = 0;
	ectx->statistic_context->interrupt_count = 0;
	ectx->statistic_context->instruction_count = 0;

	/* Initialize the ITT */
	for(int i = 0; i < INT_HW_MIN; i++) {
		ectx->memory_context->memory[INT_ITT + i] = (byte)i;
	}


	/* Set up cpu context */
	ectx->cpu_context->pos = 0u;
	ectx->cpu_context->cregs = emalloc(sizeof(dword) * 16u);
	ectx->cpu_context->regs = emalloc(sizeof(dword) * 16u);

	ectx->cpu_context->regs[CPU_REG_SP] = VA_INIT_SP_LOCATION;
	ectx->cpu_context->cregs[CPU_CREG_SSP] = VA_INIT_SSP_LOCATION;
	ectx->cpu_context->cregs[CPU_CREG_CR1] = CR1_PRIV_MODE;
	ectx->cpu_context->cregs[CPU_CREG_MR2] = 0x00u;

	/* Thread init */
	#if !GLIB_CHECK_VERSION(2,32,0)
		g_thread_init(NULL); //init glib's threads
		ectx->memory_context->bus_lock = g_mutex_new();
		ectx->log_lock = g_mutex_new();
	#else
		static GMutex bus_mutex;
		static GMutex log_mutex;
		ectx->memory_context->bus_lock = &bus_mutex;
		ectx->log_lock = &log_mutex;
	#endif
	
	
	/* Create cache lines */
	for(int i = 0; i < cachelines; i++) {
		ectx->memory_context->lines[i].cache = ecalloc(cachelinesize, sizeof(byte));
		ectx->memory_context->lines[i].cache_start = i * cachelinesize;
		ectx->memory_context->lines[i].hit_count = 0;
		ectx->memory_context->lines[i].wb_required = 0;
	}

	/* No interrupts yet, please */
	ectx->cpu_context->ints = ecalloc(INT_HW_MIN, sizeof(byte));

	ectx->cpu_context->int_present = 0;

	/* Set up dma context */
	ectx->dma_context = emalloc(sizeof(dma_context));
	ectx->dma_context->transfers = g_async_queue_new();

	ectx->logbuffer = emalloc(sizeof(char) * 512);
	ectx->logbuffer_size = 512;
	ectx->log = &log_stdout;

	return ectx;
}

emulator_context* emu_init() {
	emulator_context* ectx = emu_create_context(0, 0, 0);

	// Start DMA thread
	
	#if !GLIB_CHECK_VERSION(2,32,0)
	GThread* dmat = g_thread_create(dma_background, ectx, FALSE, NULL);
	#else
	GThread* dmat = g_thread_new("dma_thread",dma_background, ectx);
	#endif
	
	(void)dmat;

	pio_init();
	dev_init_devices(ectx);

	return ectx;
}
