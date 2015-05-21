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

#include "dump.h"
#include "common/lookup.h"
#include "decode.h"

#include <stdio.h>

void dump_registers(emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	printf("[REG] :: R0 = %08x, R1 = %08x, R2 = %08x\n", 
		cctx->regs[CPU_REG_R0],
		cctx->regs[CPU_REG_R1],
		cctx->regs[CPU_REG_R2]);
	printf("[REG] :: R3 = %08x, R4 = %08x, R5 = %08x\n", 
		cctx->regs[CPU_REG_R3],
		cctx->regs[CPU_REG_R4],
		cctx->regs[CPU_REG_R5]);
	printf("[REG] :: R6 = %08x, R7 = %08x, R8 = %08x\n", 
		cctx->regs[CPU_REG_R6],
		cctx->regs[CPU_REG_R7],
		cctx->regs[CPU_REG_R8]);
	printf("[REG] :: R9= %08x, RA = %08x, RB = %08x, RC = %08x\n", 
		cctx->regs[CPU_REG_R9],
		cctx->regs[CPU_REG_RA],
		cctx->regs[CPU_REG_RB],
		cctx->regs[CPU_REG_RC]);
	printf("[REG] :: BP = %08x, SP = %08x, FLGS = %08x\n", 
		cctx->regs[CPU_REG_BP],
		cctx->regs[CPU_REG_SP],
		cctx->regs[CPU_REG_FLGS]);
	printf(" Flags: %s%s%s%s\n",
		cctx->regs[CPU_REG_FLGS] & FLGS_O ? "O" : "-",
		cctx->regs[CPU_REG_FLGS] & FLGS_C ? "C" : "-",
		cctx->regs[CPU_REG_FLGS] & FLGS_Z ? "Z" : "-",
		cctx->regs[CPU_REG_FLGS] & FLGS_S ? "S" : "-");

	printf("[REG] :: CR1 = %08x, CR2 = %08x, SSP = %08x\n", 
		cctx->cregs[CPU_CREG_CR1],
		cctx->cregs[CPU_CREG_CR2],
		cctx->cregs[CPU_CREG_SSP]);

	printf("[REG] :: MR1 = %08x, MR2 = %08x, MR3 = %08x, MR4 = %08x\n", 
		cctx->cregs[CPU_CREG_MR1],
		cctx->cregs[CPU_CREG_MR2],
		cctx->cregs[CPU_CREG_MR3],
		cctx->cregs[CPU_CREG_MR4]);
}

