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

#ifndef VA_H_MEMORY_H
#define VA_H_MEMORY_H

#include "common/macros.h"
#include "common/definitions.h"

#define _va_mem __attribute__((hot))

/* Function: MEM_LOCK_BUS

   Locks the memory bus. Cache and DMA have to sync memory accesses. This function
   is only called by the cache. 

   Parameters:
     ectx - Emulator context
     ctx - Memory context
 */
void MEM_LOCK_BUS(emulator_context* ectx, memory_context* ctx);

/* Function: MEM_UNLOCK_BUS

   Unlocks the memory bus. Cache and DMA have to sync memory accesses. This function is
   only called by the cache. 

   Parameters:
     ectx - Emulator context
     ctx - Memory context
 */
void MEM_UNLOCK_BUS(emulator_context* ectx, memory_context* ctx);

/* Function: mem_cache_miss

   This function is called when an address was not found in the cache.

   Parameters:
     addr - The address that was not found in the cache
     ectx - Emulator context
 */
void mem_cache_miss(dword addr, emulator_context* ectx);

/* Function: mem_cache_load

   Loads data from memory into a cache line. It will load CACHE_LINE_SIZE bytes
   from the start address into the cache line. 

   Parameters:
     line - The cache line
     addr - The start address
     writeback - If set the load requests a writeback (that is, it calls <mem_cache_writeback>)
     ectx - Emulator context
 */
void mem_cache_load(int line, dword addr, int writeback, emulator_context* ectx);

/* Function: mem_cache_writeback

   Performs a writeback from a cache line to memory (if required).
   This function checks the wb_required flag of the cache line to determine
   if a writeback is required (which it is not if the cache line has not been
   modified).

   Parameters:
     line - The cache line
     addr - The address in whose favor the writeback should occur
     ectx - Emulator context
 */
void mem_cache_writeback(int line, dword addr, emulator_context* ectx);

/* Function: mem_find_line

   Tries to locate the cache line where a given memory block resides.

   Paremeters:
     addr - The address
     ectx - Emulator context

   Returns:
     -1 - if not found in cache
     x  - Number of cache line
 */
int mem_find_line(dword addr, emulator_context* ectx);

/* Function: mem_ensure_cache

   Ensures that an address is present in the cache. If it is not present in the cache
   a cache miss occurs (and <mem_cache_miss> is called) as well as the address is automatically
   loaded into the cache by replacing a cache line. 

   Parameters:
     addr - The address
     ectx - Emulator context
 */
int mem_ensure_cache(dword addr, emulator_context* ectx);

/* Function: mem_least_used_line

   Returns the currently least used line. This is the replacement policy for cache lines.

   Parameters:
     ectx - Emulator context
 */
int mem_least_used_line(emulator_context* ectx);

/* Function: mem_write_byte_p

   Writes a byte to a physical address in memory. This function internally
   calls all the required cache logic.

   Parameters:
     b - The byte
     addr - The address
     ectx - Emulator context
 */
void mem_write_byte_p(byte b, dword addr, emulator_context* ectx);

/* Function: mem_write_word_p

   Writes a word to a physical address in memory. This function internally
   calls <mem_write_byte_p>.

   Parameters:
     b - The word
     addr - The address
     ectx - Emulator context
 */
void mem_write_word_p(word b, dword addr, emulator_context* ectx);

/* Function: mem_write_dword_p

   Writes a dword to a physical address in memory. This function internally
   calls <mem_write_byte_p>.

   Parameters:
     b - The dword
     addr - The address
     ectx - Emulator context
 */
void mem_write_dword_p(dword b, dword addr, emulator_context* ectx);

/* Function: mem_get_byte_v

  Reads a byte from a virtual address. This function calls the necessary
  mmu logic (namely <mmu_translate_address>, <mmu_page_present> and 
  <mmu_is_enabled>). 

  Parameters:
    addr - The address
    out - A pointer to a byte. The byte read will be written to the location pointed to by out.
    ectx - Emulator context

  Returns:
    0 - On error
    1 - On success

  If this function returns 0 it indicates that the read could not be carried out (most likely
  due to a page not present in memory). Instructions should immediately abort execution
  when this happens. This function may fire a page not present exception (an interrupt).
 */
int mem_get_byte_v(dword addr, byte* out, emulator_context* ectx);

/* Function: mem_get_word_v

   Reads a word from a virtual address. This function internally calls
   <mem_get_byte_v>. 

   Parameters:
    addr - The address
    out - A pointer to a word. The word read will be written to the location pointed to by out.
    ectx - Emulator context

   For the return value please read the documentation for <mem_get_byte_v>.
 */
int mem_get_word_v(dword addr, word* out, emulator_context* ectx);

/* Function: mem_get_dword_v

   Reads a dword from a virtual address. This function internally calls
   <mem_get_byte_v>. 

   Parameters:
    addr - The address
    out - A pointer to a dword. The dword read will be written to the location pointed to by out.
    ectx - Emulator context

   For the return value please read the documentation for <mem_get_byte_v>.
 */
int mem_get_dword_v(dword addr, dword* out, emulator_context* ectx);

/* Function: mem_write_byte_v

   Writes a byte to a virtual address. This function calls the necessary
   mmu logic (namely <mmu_translate_address>, <mmu_page_present> and 
   <mmu_is_enabled>). 

  Parameters:
    b - The byte
    addr - The address
    ectx - Emulator context

  Returns:
    0 - On error
    1 - On success

  If this function returns 0 it indicates that the write could not be carried out (most likely
  due to a page not present in memory). Instructions should immediately abort execution
  when this happens. This function may fire a page not present exception (an interrupt).
 */
int mem_write_byte_v(byte b, dword addr, emulator_context* ectx);

/* Function: mem_write_word_v

   Writes a word to a virtual address. This function internally calls
   <mem_write_byte_v>.

  Parameters:
    b - The word
    addr - The address
    ectx - Emulator context

  For the return value please read the documentation for <mem_write_byte_v>.
 */
int mem_write_word_v(word b, dword addr, emulator_context* ectx);

/* Function: mem_write_dword_v

   Writes a dword to a virtual address. This function internally calls
   <mem_write_byte_v>.

  Parameters:
    b - The dword
    addr - The address
    ectx - Emulator context

  For the return value please read the documentation for <mem_write_byte_v>.
 */
int mem_write_dword_v(dword b, dword addr, emulator_context* ectx);

/* Function: mem_get_byte_p

   Reads a byte from a physical address. This function internally calls the required
   cache logic. 

   Parameters:
     addr - The address
     ectx - Emulator context

   Returns:
   
   The byte read. 
 */
byte mem_get_byte_p(dword addr, emulator_context* ectx);

/* Function: mem_get_word_p

   Reads a word from a physical address. This function internally calls <mem_get_byte_p>.

   Parameters:
     addr - The address
     ectx - Emulator context

   Returns:
   
   The word read. 
 */
word mem_get_word_p(dword addr, emulator_context* ectx);

/* Function: mem_get_dword_p

   Reads a dword from a physical address. This function internally calls <mem_get_byte_p>.

   Parameters:
     addr - The address
     ectx - Emulator context

   Returns:
   
   The dword read. 
 */
dword mem_get_dword_p(dword addr, emulator_context* ectx);

/* Function: mem_get_instruction_v

   Fetch an instruction from memory (MMU_ACCESS_MODE_EXEC)

   Parameters see mem_get_dword_v
 */
int mem_get_instruction_v(dword addr, dword* out, emulator_context* ectx);

#endif
