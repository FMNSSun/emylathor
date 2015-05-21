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

#ifndef VA_H_EMIT_H
#define VA_H_EMIT_H

#include "common/macros.h"
#include "common/definitions.h"

#include <glib.h>

#define DYNAMIC_BUFFER_SIZE	512

typedef struct {
	byte* buf;
	byte buf_len_fixed;
	dword pos;
	dword highest_pos;
	dword offset;
	dword size;
	GHashTable* labels;
	GSList* labelrefs;
} emit_context;

typedef struct {
	dword pos;
	byte reg;
	char* name;
	byte cond;
} label_ref;

/* Function: emit_create_context

   Create an emit context with a buffer of a pre-initialized size.

   Parameters:
     size - Size
     fixed_length - 0 = dynamically reallocate memory, 1 = error when buffer exceeds size
   Returns:

   Pointer to an emit context.
 */
emit_context* emit_create_context(dword size, byte fixed_length);

/* Function: emit_label

   Emits a label.

   Parameters:
     name - Name of the label
     ctx - Emit context
*/
void emit_label(char* name, emit_context* ctx);

/* Function: emit_label_ref

   Creates a reference to a label.

   Parameters:
     name - Name of the label
     reg - Target register
     cond - Condition code
     ctx - Emit context
 */
void emit_label_ref(char* name, byte reg, byte cond, emit_context* ctx);

/* Function: emit_label_res

   Label resolution.

    Parameters:
      name - Name of the label to find
      ctx - Emit context
 */
dword emit_label_res(char* name, emit_context* ctx);

/* About: Emit functions

   Emit functions emit (or encode) instructions to a buffer.
   Please refer to the documentation about instructions and instruction format.
 */

/* Function: emit_byte

   Emit a raw byte

   Parameters:
     b - The raw byte
     ctx - Emit context
 */
void emit_byte(byte b, emit_context* ctx);

void emit_fmt_i(byte prefix, byte condition, byte opcode, emit_context* ctx);
void emit_fmt_ii(byte prefix, byte condition, byte opcode, byte dst, byte a, byte b, emit_context* ctx);
void emit_fmt_iii(byte prefix, byte condition, byte opcode, byte dst, byte a, word imm12, emit_context* ctx);
void emit_fmt_iv(byte prefix, byte condition, byte opcode, byte dst, word imm16, emit_context* ctx);
void emit_fmt_v(byte prefix, byte condition, byte opcode, byte dst, emit_context* ctx);
void emit_fmt_vi(byte prefix, byte condition, byte opcode, byte a, byte b, emit_context* ctx);
void emit_fmt_vii(byte prefix, byte condition, byte opcode, byte a, word imm12, emit_context* ctx);
void emit_fmt_viii(byte prefix, byte condition, byte opcode, word imm16, emit_context* ctx);
void set_highest_position(emit_context* ctx);
void handle_exceeding_buffer(emit_context* ctx, dword required_size);
extern void yyerror( emit_context* emctx, const char *s);
#endif
