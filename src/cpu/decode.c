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

#include "decode.h"
#include "memory.h"
#include "common/lookup.h"

#include <stdlib.h>
#include <stdio.h>

_va_dec instruction_context* dec_fetch_ins(emulator_context* ectx) {
	static instruction_context ctx;
	static instruction_context* ictx = &ctx;

	#if DUMP_ALL || DUMP_DECODE
	PUTLOG(ectx, "[DECODING] :: Decoding instruction...%s","");
	#endif

	byte b1, b2, b3, b4;

	dword idata;

	cpu_context* cctx = ectx->cpu_context;

	if(!mem_get_instruction_v(cctx->pos, &idata, ectx))
		return NULL;

	cctx->pos += 4; //advance instruction pointer


	byte* ibytes = (byte*)&idata;
	b1 = ibytes[0]; b2 = ibytes[1];
	b3 = ibytes[2]; b4 = ibytes[3];

	#if DUMP_ALL || DUMP_DECODE
	PUTLOG(ectx, "[DECODING] :: Byte group is %02x%02x%02x%02x",b1, b2, b3, b4);
	#endif


	// Decode opcode and prefix
	ictx->prefix = (b1 >> 6u);
	ictx->opcode = b1 & 0x3Fu; // throw away prefix bits

	// Decode condition and destination operand
	ictx->condition = (b2 >> 4u);
	ictx->dst = (b2 & 0xFu);

	// Decode operands
	ictx->opa = (b3 >> 4u);
	ictx->opb = (b3 & 0xFu);

	ictx->imm16 = WORD_FROM_BYTES(b4, b3);
	ictx->imm12 = (ictx->opb << 8u) | b4;

	#if DUMP_ALL || DUMP_DECODE
	PUTLOG(ectx, "[DECODING] :: Instruction valid...%s","");
	PUTLOG(ectx, "[DECODING] :: ... opcode = 0x%x (%s), prefix = %0x (%s), condition = %0x, (%s)", 
		ictx->opcode,
		get_name_of_op(ictx->prefix, ictx->opcode),
		ictx->prefix,
		get_name_of_prefix(ictx->prefix),
		ictx->condition,
		get_name_of_cond(ictx->condition));
	PUTLOG(ectx, "[DECODING] :: ... dst = %d, opa = %d, opb = %d", ictx->dst, ictx->opa, ictx->opb);
	PUTLOG(ectx, "[DECODING] :: ... imm16 = 0x%x, imm12 = 0x%x", ictx->imm16, ictx->imm12);
	#endif

	return ictx;
}
