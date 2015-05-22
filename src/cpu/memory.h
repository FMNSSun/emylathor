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



/* Function: mem_write_byte_p

   Writes a byte to a physical address in memory. This function internally
   calls all the required cache logic.

   Parameters:
     b - The byte
     addr - The address
     ectx - Emulator context
 */
int mem_write_byte_p(byte b, dword addr, emulator_context* ectx);

/* Function: mem_write_word_p

   Writes a word to a physical address in memory. This function internally
   calls <mem_write_byte_p>.

   Parameters:
     b - The word
     addr - The address
     ectx - Emulator context
 */
int mem_write_word_p(word b, dword addr, emulator_context* ectx);

/* Function: mem_write_dword_p

   Writes a dword to a physical address in memory. This function internally
   calls <mem_write_byte_p>.

   Parameters:
     b - The dword
     addr - The address
     ectx - Emulator context
 */
int mem_write_dword_p(dword b, dword addr, emulator_context* ectx);

/* Function: mem_get_byte_p

   Reads a byte from a physical address. This function internally calls the required
   cache logic. 

   Parameters:
     addr - The address
     val - Pointer where to place value   
     ectx - Emulator context 

   Returns:
   
   The byte read. 
 */
int mem_get_byte_p(dword addr, byte* val, emulator_context* ectx);

/* Function: mem_get_word_p

   Reads a word from a physical address. This function internally calls <mem_get_byte_p>.

   Parameters:
     addr - The address
     val - Pointer where to place value   
     ectx - Emulator context

   Returns:
   
   The word read. 
 */
int mem_get_word_p(dword addr, word* val, emulator_context* ectx);

/* Function: mem_get_dword_p

   Reads a dword from a physical address. This function internally calls <mem_get_byte_p>.

   Parameters:
     addr - The address
     val - Pointer where to place value   
     ectx - Emulator context

   Returns:
   
   The dword read. 
 */
int mem_get_dword_p(dword addr, dword* val, emulator_context* ectx);

/* Function: mem_get_instruction_p

   Fetch an instruction from memory

   Parameters see mem_get_dword_p
 */
int mem_get_instruction_p(dword addr, dword* val, emulator_context* ectx);

#endif
