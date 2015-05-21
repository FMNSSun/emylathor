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

#include "emit.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

emit_context* emit_create_context(dword size, byte fixed_length) {
	emit_context* ctx = malloc(sizeof(emit_context));

	if(ctx == NULL)
		return NULL;

	ctx->buf = calloc(size, sizeof(byte));

	if(ctx->buf == NULL) {
		free(ctx);
		return NULL;
	}

	ctx->labels = g_hash_table_new(g_str_hash, g_str_equal);

	if(ctx->labels == NULL) {
		free(ctx->buf);
		free(ctx);
		return NULL;
	}

	ctx->labelrefs = NULL;
	ctx->size = size;
	ctx->pos = 0u;
	ctx->highest_pos = ctx->pos;
	ctx->offset = 0u;
	ctx->buf_len_fixed = fixed_length;

	return ctx;
}

void emit_label(char* name, emit_context* ctx) {
#ifdef DEBUG
	printf("emit_label at %08x with %s\n", ctx->pos + ctx->offset, name);
#endif
	

	gpointer orig_key = NULL;
	gpointer value = NULL;
	
	if(g_hash_table_lookup_extended(ctx->labels, name, &orig_key, &value)) {
		char msg[50];
		snprintf (msg, 50, "Label %s already used!", name);
		yyerror(ctx,msg);
	}

	g_hash_table_insert(ctx->labels, name, GUINT_TO_POINTER(ctx->pos + ctx->offset));
}

void emit_label_ref(char* name, byte reg, byte cond, emit_context* ctx) {
#ifdef DEBUG
	printf("emit_label_ref at %08x with %s\n", ctx->pos, name);
#endif
	
	label_ref* elr = malloc(sizeof(label_ref));

	if(elr == NULL) {
		yyerror(ctx, "Insufficient memory for emit_label_ref\n");
	}

	elr->name = name;
	elr->pos = ctx->pos;
	elr->reg = reg;
	elr->cond = cond;

	ctx->labelrefs = g_slist_append(ctx->labelrefs, elr);

	if(ctx->labelrefs == NULL) {
		yyerror(ctx, "Insufficient memory for emit_label_ref\n");
	}
	
	handle_exceeding_buffer(ctx, 8);
	ctx->pos += 8;
	set_highest_position(ctx);
}

dword emit_label_res(char* name, emit_context* ctx) {

	gpointer orig_key = NULL;
	gpointer value = NULL;
	
	if(!g_hash_table_lookup_extended(ctx->labels, name, &orig_key, &value)) {
		char msg[50];
		snprintf (msg, 50, "Label %s could not be found!", name);
		yyerror(ctx,msg);
	}
	
	return (dword)GPOINTER_TO_UINT(value);
}

void emit_byte(byte b, emit_context* ctx) {
	handle_exceeding_buffer(ctx, 1);

	ctx->buf[ctx->pos++] = b;
	
	set_highest_position(ctx);
}

/*
  0x00 is used for unused regions of an instruction. Please refer
  to the va documentation to read more about the instruction formats. 
*/

void emit_fmt_i(byte prefix, byte condition, byte opcode, emit_context* ctx) {
	
	handle_exceeding_buffer(ctx, 4);
	
	ctx->buf[ctx->pos++] = (prefix << 6u) | opcode;
	ctx->buf[ctx->pos++] = (condition << 4u) | 0x00u;
	ctx->buf[ctx->pos++] = 0x00u;
	ctx->buf[ctx->pos++] = 0x00u;
	
	set_highest_position(ctx);
}

void emit_fmt_ii(byte prefix, byte condition, byte opcode, byte dst, byte a, byte b, emit_context* ctx) {
	
	handle_exceeding_buffer(ctx, 4);
	
	ctx->buf[ctx->pos++] = (prefix << 6u) | opcode;
	ctx->buf[ctx->pos++] = (condition << 4u) | dst;
	ctx->buf[ctx->pos++] = (a << 4u) | b;
	ctx->buf[ctx->pos++] = 0x00u; //unused
	
	set_highest_position(ctx);
}

void emit_fmt_iii(byte prefix, byte condition, byte opcode, byte dst, byte a, word imm12, emit_context* ctx) {
	
	handle_exceeding_buffer(ctx, 4);
	

	ctx->buf[ctx->pos++] = (prefix << 6u) | opcode;
	ctx->buf[ctx->pos++] = (condition << 4u) | dst;
	ctx->buf[ctx->pos++] = (a << 4u) | (imm12 >> 8u);
	ctx->buf[ctx->pos++] = (imm12 & 0x00FFu);
	
	set_highest_position(ctx);
}

void emit_fmt_iv(byte prefix, byte condition, byte opcode, byte dst, word imm16, emit_context* ctx) {
	handle_exceeding_buffer(ctx, 4);
	

	ctx->buf[ctx->pos++] = (prefix << 6u) | opcode;
	ctx->buf[ctx->pos++] = (condition << 4u) | dst;
	ctx->buf[ctx->pos++] = (imm16 & 0x00FFu); //lower
	ctx->buf[ctx->pos++] = (imm16 >> 8u); //higher
	
	set_highest_position(ctx);
}

void emit_fmt_v(byte prefix, byte condition, byte opcode, byte dst, emit_context* ctx) {
	
	handle_exceeding_buffer(ctx, 4);

	ctx->buf[ctx->pos++] = (prefix << 6u) | opcode;
	ctx->buf[ctx->pos++] = (condition << 4u) | dst;
	ctx->buf[ctx->pos++] = 0x00u;
	ctx->buf[ctx->pos++] = 0x00u;
	
	set_highest_position(ctx);
}

void emit_fmt_vi(byte prefix, byte condition, byte opcode, byte a, byte b, emit_context* ctx) {
	
	handle_exceeding_buffer(ctx, 4);

	ctx->buf[ctx->pos++] = (prefix << 6u) | opcode;
	ctx->buf[ctx->pos++] = (condition << 4u) | 0x00u;
	ctx->buf[ctx->pos++] = (a << 4u) | b;
	ctx->buf[ctx->pos++] = 0x00u;
	
	set_highest_position(ctx);
}

void emit_fmt_vii(byte prefix, byte condition, byte opcode, byte a, word imm12, emit_context* ctx) {
	handle_exceeding_buffer(ctx, 4);
	
	ctx->buf[ctx->pos++] = (prefix << 6u) | opcode;
	ctx->buf[ctx->pos++] = (condition << 4u) | 0x00;
	ctx->buf[ctx->pos++] = (a << 4u) | (imm12 >> 8u);
	ctx->buf[ctx->pos++] = (imm12 & 0x00FFu);
	
	set_highest_position(ctx);
}

void emit_fmt_viii(byte prefix, byte condition, byte opcode, word imm16, emit_context* ctx) {
	handle_exceeding_buffer(ctx, 4);
	
	ctx->buf[ctx->pos++] = (prefix << 6u) | opcode;
	ctx->buf[ctx->pos++] = (condition << 4u) | 0x00;
	ctx->buf[ctx->pos++] = (imm16 & 0x00FFu); //lower
	ctx->buf[ctx->pos++] = (imm16 >> 8u); //higher
	
	set_highest_position(ctx);
	
}

//Required size must not be >= DYNAMIC_BUFFER_SIZE!! POSSIBLE FIXME?
void handle_exceeding_buffer(emit_context* ctx, dword required_size){
	if(!(ctx->pos + required_size - 1 >= ctx->size)){
		return;
	}
	
	if(ctx->buf_len_fixed){
		
		yyerror(ctx, "FATAL: Emit buffer too small!\n");
		exit(1);
	}else{
		
#ifdef DEBUG
		printf("allocate dynamic\n");
#endif
		int alloc_block = ctx->highest_pos / DYNAMIC_BUFFER_SIZE;
		alloc_block++;
		
#ifdef DEBUG
		printf("allocate block nr. %d\n", alloc_block);
		printf("ctx->pos := %d and ctx->highest_pos = %d\n", ctx->pos, ctx->highest_pos);
		printf("required_size := %d\n", required_size);
		printf("realloc to size := %d\n", sizeof(byte) * alloc_block * DYNAMIC_BUFFER_SIZE);
#endif
		
		ctx->buf = realloc(ctx->buf, sizeof(byte) * alloc_block * DYNAMIC_BUFFER_SIZE);
		if(ctx->buf == NULL){
			fprintf(stderr, "FATAL: Dynamic allocation of emit buffer failed, not enough memory!\n");
			exit(2);
		}
		
		memset(&ctx->buf[ctx->pos], 0, DYNAMIC_BUFFER_SIZE);
		ctx->size += DYNAMIC_BUFFER_SIZE;
	}
}

void set_highest_position(emit_context* ctx){
	if(ctx->pos > ctx->highest_pos){
		ctx->highest_pos = ctx->pos;
	}
}
