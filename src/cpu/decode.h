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

#ifndef VA_H_DECODE_H
#define VA_H_DECODE_H

#include "common/macros.h"
#include "common/definitions.h"

#define _va_dec __attribute__((hot))

/* Function: dec_fetch_ins

   Fetches the next instruction. This function does all the necessary decoding.
   It internally fetches more data (like the register part or immediate data part) if necessary. 
   This function updates the position of the cpu_context in the emulator_context. It reads
   from the memory in the memory_context from emulator_context and uses the virtual address functions
   of memory.c. Therefore a call to this function might fire a page fault.

   Parameteres:
     ectx - Emulator context

   Returns:
  
   A pointer to an instruction context. The instruction context is statically allocated and therefore it's contents
   is overwritten by the next call to <dec_fetch_ins>. If you need the data of the preceeding call still after another
   call to this function you should (well, must) copy the data by yourself. You also should not free the context
   returned by this function (as it is reused by later calls to this function). If the function returns NULL
   there has been an error. This function fires the necessary page faults or invalid opcode exceptions if necessary. 
 */
instruction_context* dec_fetch_ins(emulator_context* ectx);

#endif
