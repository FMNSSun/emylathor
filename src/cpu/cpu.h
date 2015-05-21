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

#ifndef VA_H_CPU_H
#define VA_H_CPU_H

#define _va_cpu __attribute__((hot))
#define _va_ins inline

#include "common/macros.h"
#include "common/definitions.h"

/* Function: cpu_execute

   Starts the CPU which will then in turn start decoding and executing
   instructions.

   Parameters:
     ectx - Emulator context
     step - If this is set, the cpu will only attempt to execute a single instruction

   Returns:
     0 - if stopped
     -1 - if step over
     error code else
   
   Nothing. (see ectx)
 */
int cpu_execute(emulator_context* ectx, int step);

/* Function: cpu_push_dword

   A helper function to push a dword to the "stack".

   Parameters:
     reg - Stack register
     val - value
     ectx - Emulator context

   Returns:

   1 on success, 0 otherwise
 */
int cpu_push_dword(dword* reg, dword val, emulator_context* ectx);

/* Function: cpu_pop_dword

   A helper function to pop a dword from the "stack".

   Parameters:
     reg - Stack register
     val - Pointer to the location where the dword shall be stored
     ectx - Emulator context

  Returns:

  1 on success, 0 otherwise
 */
int cpu_pop_dword(dword* reg, dword* val, emulator_context* ectx);

/* Function: cpu_get_byte_reg

   Returns the value from a BYTE register pointed to by reg.

   Parameters:
     reg - BYTE register
     regs - Pointer to the cpu registers

   Returns:

     The byte you asked for.
 */
byte cpu_get_byte_reg(byte reg, dword* regs);

/* Function: cpu_set_byte_reg

   Sets the value of a BYTE register pointed to by reg.

   Parameters:
     reg - BYTE register
     val - The value
     regs - Pointer to the cpu registers
 */
void cpu_set_byte_reg(byte reg, byte val, dword* regs);

/* Function: cpu_get_word_reg

   Returns the value from a WORD register pointed to by reg.

   Parameters:
     reg - WORD register
     regs - Pointer to the cpu registers

   Returns:

     The word you asked for.
 */
word cpu_get_word_reg(byte reg, dword* regs);

/* Function: cpu_set_word_reg

   Sets the value of a WORD register pointed to by reg.

   Parameters:
     reg - WORD register
     val - The value
     regs - Pointer to the cpu registers
 */
void cpu_set_word_reg(byte reg, word val, dword* regs);

/* Instructions (alphabetically, then by size) */

/* About: Instructions
   Functions with the ins_ prefix are CPU instructions. They all take a
   pointer to an instruction_context an an emulator_context. Please refer to
   the official documentation about instructions and the instruction format
   if you want to know what these instructions do. You can call these functions
   manually if you whish. If you do so I'm going to assume you know what you're
   doing. Note: Some of these instructions make use of inline assembly and the fact
   that x86 is little endian. Currently we only target x86 and we'd welcome
   your contribution to make these instructions available on your platform
   as well. Instructions not portable due to this have an #ifdef __i386__
   (at least the ones we are aware of). 
 */

/* Function: ins_add */
void ins_add(instruction_context* ins, emulator_context* ectx);
/* Function: ins_and */
void ins_and(instruction_context* ins, emulator_context* ectx);
/* Function: ins_call */
void ins_call(instruction_context* ins, emulator_context* ectx);
/* Function: ins_cmp */
void ins_cmp(instruction_context* ins, emulator_context* ectx);
/* Function: ins_cmpxchg */
void ins_cmpxchg(instruction_context* ins, emulator_context* ectx);
/* Function: ins_div */
void ins_div(instruction_context* ins, emulator_context* ectx);
/* Function: ins_in */
void ins_in(instruction_context* ins, emulator_context* ectx);
/* Function: ins_int */
void ins_int(instruction_context* ins, emulator_context* ectx);
/* Function: ins_jmp */
void ins_jmp(instruction_context* ins, emulator_context* ectx);
/* Function: ins_loadas */
void ins_loadas(instruction_context* ins, emulator_context* ectx);
/* Function: ins_loadi */
void ins_loadi(instruction_context* ins, emulator_context* ectx);
/* Function: ins_loadid */
void ins_loadid(instruction_context* ins, emulator_context* ectx);
/* Function: ins_loadma */
void ins_loadma(instruction_context* ins, emulator_context* ectx);
/* Function: ins_loadrd */
void ins_loadrd(instruction_context* ins, emulator_context* ectx);
/* Function: ins_mod */
void ins_mod(instruction_context* ins, emulator_context* ectx);
/* Function: ins_mov */
void ins_mov(instruction_context* ins, emulator_context* ectx);
/* Function: ins_movfc */
void ins_movfc(instruction_context* ins, emulator_context* ectx);
/* Function: ins_movtc */
void ins_movtc(instruction_context* ins, emulator_context* ectx);
/* Function: ins_mul */
void ins_mul(instruction_context* ins, emulator_context* ectx);
/* Function: ins_not */
void ins_not(instruction_context* ins, emulator_context* ectx);
/* Function: ins_or */
void ins_or(instruction_context* ins, emulator_context* ectx);
/* Function: ins_out */
void ins_out(instruction_context* ins, emulator_context* ectx);
/* Function: ins_sar */
void ins_sar(instruction_context* ins, emulator_context* ectx);
/* Function: ins_sex */
void ins_sex(instruction_context* ins, emulator_context* ectx);
/* Function: ins_sdiv */
void ins_sdiv(instruction_context* ins, emulator_context* ectx);
/* Function: ins_shl */
void ins_shl(instruction_context* ins, emulator_context* ectx);
/* Function: ins_shr */
void ins_shr(instruction_context* ins, emulator_context* ectx);
/* Function: ins_smul */
void ins_smul(instruction_context* ins, emulator_context* ectx);
/* Function: ins_sub */
void ins_sub(instruction_context* ins, emulator_context* ectx);
/* Function: ins_storeas */
void ins_storeas(instruction_context* ins, emulator_context* ectx);
/* Function: ins_storeid */
void ins_storeid(instruction_context* ins, emulator_context* ectx);
/* Function: ins_storema */
void ins_storema(instruction_context* ins, emulator_context* ectx);
/* Function: ins_storerd */
void ins_storerd(instruction_context* ins, emulator_context* ectx);
/* Function: ins_test */
void ins_test(instruction_context* ins, emulator_context* ectx);
/* Function: ins_xchg */
void ins_xchg(instruction_context* ins, emulator_context* ectx);
/* Function: ins_xchgc */
void ins_xchgc(instruction_context* ins, emulator_context* ectx);
/* Function: ins_xor */
void ins_xor(instruction_context* ins, emulator_context* ectx);


#endif
