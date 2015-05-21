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

#ifndef VA_H_TYPES_H
#define VA_H_TYPES_H

#include <stdint.h>

#include "glib.h"

#define BYTE_MAX 	0xFFu
#define WORD_MAX	0xFFFFu
#define DWORD_MAX	0xFFFFFFFFu

typedef uint8_t byte;
typedef int8_t sbyte;
typedef uint16_t word;
typedef int16_t sword;
typedef uint32_t dword;
typedef int32_t sdword;
typedef uint64_t qword;
typedef int64_t sqword;

/* Typedef: cache_context

   A Cache Line

   Struct members:
     cache_start - Start Address of the Cache Line
     cache - Pointer to the contents of the Cache Line
     hit_count - Hit counter
     wb_required - Flag (if set a writeback is required)
 */
typedef struct {
	dword cache_start;
	byte* cache;
	word hit_count;
	byte wb_required;
} cache_context;

/* Typedef: memory_context

   Struct members:
     bus_lock - GMutex for syncing memory access
     lines - Cache Lines (Array of cache_contexts)
     memory - RAM
     memory_size - How much RAM?
     cache_lines - How many Cache Lines?
     cache_line_size - The size of a Cache Line
 */
typedef struct {
	GMutex* bus_lock;
	cache_context* lines;
	byte* memory;
	dword memory_size;
	word cache_lines;
	word cache_line_size;
} memory_context;

/* Typedef: cpu_context

   Struct members:
     cregs - Control Registers (Array)
     regs - Main Registers (Array)
     pos - Instruction Pointer
     int_present - Flag. Set if an interrupt is present
     ints - Pending interrupts
 */  
typedef struct {
	dword* cregs;
	dword* regs;
	dword pos;
	dword int_present;
	byte* ints;
} cpu_context;

/* Typedef: dma_context
 
   Struct members:
     transfers - Pointer to a GAsyncQueue containing dma transfer requests
 */
typedef struct {
	GAsyncQueue* transfers;
} dma_context;

typedef void (*log_function)(char* msg);

/* Typedef: statistic_context

   Struct members:
     cache_miss_count - How many cache misses have occured
     cache_hits - How many cache hits have occured
     interrupt_count - How many interrupts have been fired
     instruction_count - How many instructions have been executed
 */
typedef struct {
	dword cache_miss_count;
	qword cache_hits;
	dword interrupt_count;
	qword instruction_count;
} statistic_context;

/* Typedef: emulator_context

   Struct members:
     memory_context - Pointer to the memory context
     dma_context - Pointer to the dma context
     log - Function pointer for a logging function
     char* logbuffer - Buffer used by PUTLOG
     size_t logbuffer_size - Size of logbuffer
     log_lock - Lock for accessing logbuffer
     statistic_context - Pointer to the statistics context
 */
typedef struct {
	memory_context* memory_context;
	cpu_context* cpu_context;
	dma_context* dma_context;
	log_function log;
	char* logbuffer;
	size_t logbuffer_size;
	GMutex* log_lock;
	statistic_context* statistic_context;
} emulator_context;

/* Typedef: instruction_context

   Struct members:
     opcode - 6 bit opcode
     dst - 4 bit destination operand
     opa - 4 bit operand A
     opb - 4 bit operand B
     imm8 - Immediate 8 bit
     prefix - Instruction prefix
     condition - Condition code
 */
typedef struct instruction_context {
	byte opcode;
	byte dst;
	byte opa;
	byte opb;
	word imm12;
	word imm16;
	byte prefix;
	byte condition;
} instruction_context;

/* Typedef: cpu_int

   Struct members:
     no - Interrupt number
 */
typedef struct cpu_int {
	byte no;
} cpu_int;

#endif
