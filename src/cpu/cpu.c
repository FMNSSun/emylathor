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

#include "cpu.h"
#include "decode.h"
#include "memory.h"
#include "dump.h"
#include "interrupts.h"
#include "dma.h"
#include "pio.h"
#include "disassembler/lib_vasmdis.h"

#include "hw/vagc.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <inttypes.h>


_va_cpu int cpu_execute(emulator_context* ectx, int step) {
	cpu_context* cctx = ectx->cpu_context;

	struct timeval t1, t2;
   	double elapsedTime;
	gettimeofday(&t1, NULL);
	uint32_t fu = 0;
	statistic_context* stctx = ectx->statistic_context;

	do {
		#if DUMP_ALL || DUMP_CPU
		PUTLOG(ectx,"[CPU] IP := %08x", ectx->cpu_context->pos);
		#endif

		if(__builtin_expect(cctx->int_present,0))
			if(int_enabled(ectx)) //Only if hardware interrupts are enabled
				int_handle_interrupt(int_icu_get(ectx), ectx);
	
		instruction_context* ins = dec_fetch_ins(ectx);

		stctx->instruction_count++;

		#ifdef DOSTEP
		if(getc(stdin) == 'q')
			exit(50);
		#endif


		if(__builtin_expect(ins == NULL,0)) {
			printf("DECODE FAILED\n\n");
			continue;
		}

		// Check condition codes
		if(__builtin_expect(ins->condition != COND_ALWAYS, 0)) {
			dword flgs = cctx->regs[CPU_REG_FLGS];
			switch(ins->condition) {
				case COND_IZ:
					if(! (flgs & FLGS_Z)) continue;
					break;
				case COND_NZ:
					if(  (flgs & FLGS_Z)) continue;
					break;
				case COND_IS:
					if(! (flgs & FLGS_S)) continue;
					break;
				case COND_NS:
					if(  (flgs & FLGS_S)) continue;
					break;
				case COND_IO:
					if(! (flgs & FLGS_O)) continue;
					break;
				case COND_NO:
					if(  (flgs & FLGS_O)) continue;
					break;
				case COND_IP:
					if(! (flgs & FLGS_P)) continue;
					break;
				case COND_NP:
					if(  (flgs & FLGS_P)) continue;
					break;
				case COND_IZIS:
					if(!( (flgs & FLGS_Z) || (flgs & FLGS_S) ))
						continue;
					break;
				case COND_NZNS:
					if(  (flgs & FLGS_Z)) continue;
					if(  (flgs & FLGS_S)) continue;
					break;
				case COND_IZIC:
					if(!( (flgs & FLGS_Z) || (flgs & FLGS_C) ))
						continue;
					break;
				case COND_NZNC:
					if(  (flgs & FLGS_Z)) continue;
					if(  (flgs & FLGS_C)) continue;
					break;
				case COND_IC:
					if(! (flgs & FLGS_C)) continue;
					break;
				case COND_NC:
					if(  (flgs & FLGS_C)) continue;
					break;
				case COND_NF:
					if(  (flgs & FLGS_Z)) continue;
					if(  (flgs & FLGS_C)) continue;
					if(  (flgs & FLGS_S)) continue;
					if(  (flgs & FLGS_O)) continue;
					if(  (flgs & FLGS_P)) continue;
					if(  (flgs & FLGS_E)) continue;
					break;
			}
		}

		#if DUMP_ALL || DUMP_CPU
		PUTLOG(ectx,"[CPU] :: Executing instruction...%s","");
		#endif
		
		switch(ins->opcode) {
			case OP_ADD:
				ins_add(ins, ectx);
				break;
			case OP_AND:
				ins_and(ins, ectx);
				break;
			case OP_CALL:
				ins_call(ins, ectx);
				break;
			case OP_CMP:
				ins_cmp(ins, ectx);
				break;
			case OP_CMPXCHG:
				ins_cmpxchg(ins, ectx);
				break;
			case OP_DIV:
				ins_div(ins, ectx);
				break;
			case OP_FAIL:
				return SIGNED32(ins->imm16);
			case OP_IN:
				ins_in(ins, ectx);
				break;
			case OP_INT:
			  	ins_int(ins, ectx);
				break;
			case OP_IRET:
				int_handle_return(ectx);
				break;
			case OP_JMP:
				ins_jmp(ins, ectx);
				break;
			case OP_LOADAS:
				ins_loadas(ins, ectx);
				break;
			case OP_LOADI:
				ins_loadi(ins, ectx);
				break;
			case OP_LOADID:
				ins_loadid(ins, ectx);
				break;
			case OP_LOADMA:
				ins_loadma(ins, ectx);
				break;
			case OP_LOADRD:
				ins_loadrd(ins, ectx);
				break;
			case OP_MUL:
				ins_mul(ins, ectx);
				break;
			case OP_MOD:
				ins_mod(ins,ectx);
				break;
			case OP_MOV:
				ins_mov(ins, ectx);
				break;
			case OP_MOVFC:
				ins_movfc(ins, ectx);
				break;
			case OP_MOVTC:
				ins_movtc(ins, ectx);
				break;
			case OP_NOT:
				ins_not(ins, ectx);
				break;
			case OP_OR:
				ins_or(ins, ectx);
				break;
			case OP_OUT:
				ins_out(ins, ectx);
				break;
			case OP_SAR:
				ins_sar(ins, ectx);
				break;
			case OP_SEX:
				ins_sex(ins, ectx);
				break;
			case OP_SDIV:
				ins_sdiv(ins, ectx);
				break;
			case OP_SHL:
				ins_shl(ins, ectx);
				break;
			case OP_SHR:
				ins_shr(ins, ectx);
				break;
			case OP_SMUL:
				ins_smul(ins, ectx);
				break;
			case OP_STOP:
				//getc(stdin);
				if(fu > 0) {
					fu--;
					cctx->pos = 0;
					continue;
				}
				gettimeofday(&t2, NULL);
				elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
				elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
				PUTLOG(ectx, "executed %" PRIx64 " instructions in %lf ms", stctx->instruction_count, elapsedTime);
				PUTLOG(ectx, "Measured speed: %lf MIPS", (stctx->instruction_count / elapsedTime) / 1000.0);
				//sleep(5);
				return 0;
			case OP_SUB:
				ins_sub(ins, ectx);
				break;
			case OP_STOREAS:
				ins_storeas(ins, ectx);
				break;
			case OP_STOREID:
				ins_storeid(ins, ectx);
				break;
			case OP_STOREMA:
				ins_storema(ins, ectx);
				break;
			case OP_STORERD:
				ins_storerd(ins, ectx);
				break;
			case OP_TEST:
				ins_test(ins, ectx);
				break;
			case OP_XCHG:
				ins_xchg(ins, ectx);
				break;
			case OP_XCHGC:
				ins_xchgc(ins, ectx);
				break;
			case OP_XOR:
				ins_xor(ins, ectx);
				break;
		}

		#if DUMP_ALL || DUMP_REGS
		dump_registers(ectx);
		#endif
	} while(!step);

	return -1;
}

/*
	Every DWORD register is divided into 4 BYTE registers.
	Therefore (reg / 4) will yield the DWORD register where a BYTE
	register is contained in.

	(reg % 4) yields which byte of the DWORD register is meant.
	To select that byte we shift it by (reg % 4) * 8 to the right and apply
	a 0xFFu AND-mask.

	I.e reg = 9 -> 9 / 4 = 2 -> R2
	               9 % 4 = 1 -> R2LH

	0x11223344 >> (1 * 8) -> 0x00112233
				   & 0xFFu -> 0x33

	The mechanism for WORD registers is the same except that we divide by two
	and multiply by 16.

	For setting a byte we do almost the same. We first apply an AND mask
        (which we get by ~(0xFF << ((reg % 4) * 8))) which clears the target byte in the DWORD register.
        Then we shift the value to be written to that byte to the same position (<< ((reg % 4) * 8)) and
        or it. 

	I.e reg = 9 -> 9 / 4 = 2 -> R2
	               9 % 4 = 1 -> R2LH
            value = 0x55

	0x11223344 & ~(0xFF << (1 * 8)) -> 0x11220044
        val << (1 * 8) -> 0x5500
        0x11220044 | (0x5500) -> 0x11225544

        The same mechanism is used for WORD registers too.
*/

_va_cpu byte cpu_get_byte_reg(byte reg, dword* regs) {
	dword i = regs[reg / 4];
	i = (i >> ((reg % 4) * 8)) & 0xFFu;
	return (byte)i;
}

_va_cpu void cpu_set_byte_reg(byte reg, byte val, dword* regs) {
	dword i = regs[reg / 4];
	i = i & ~(0xFF << ((reg % 4) * 8));
	i = i | (val << ((reg % 4) * 8));
	regs[reg / 4] = i;
}

_va_cpu word cpu_get_word_reg(byte reg, dword* regs) {
	dword i = regs[reg / 2];
	i = (i >> ((reg % 2) * 16)) & 0xFFFFu;
	return (word)i;
}

_va_cpu void cpu_set_word_reg(byte reg, word val, dword* regs) {
	dword i = regs[reg / 2];
	i = i & ~(0xFFFF << ((reg % 2) * 16));
	i = i | (val << ((reg % 2) * 16));
	regs[reg / 2] = i;
}

_va_cpu int cpu_push_dword(dword* reg, dword val, emulator_context* ectx) {

	if(!mem_write_dword_v(val, *reg, ectx)) {
		return 0;
	}

	if(*reg < 4) {
		int_fire_interrupt(INT_EXC_ADDROVERF, ectx);
		return 1;
	}

	*reg -=4;

	return 1;
}

_va_cpu int cpu_pop_dword(dword* reg, dword* val, emulator_context* ectx) {
	
	dword sp = *reg;

	if(*reg >= (DWORD_MAX - 4)) {
		int_fire_interrupt(INT_EXC_ADDROVERF, ectx);
		return 1;
	}

	*reg += 4;

	if(!mem_get_dword_v(*reg, val, ectx)) {
		*reg = sp;
		return 0;
	}


	return 1;
}

_va_ins void ins_add(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	dword a = cctx->regs[ins->opa];
	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];

	byte o, c;

	/* TODO: Make this more portable */
	#if __i386__ || __x86_64__
	asm volatile (
			"addl %%ebx, %%eax;"
			"setob %%dl;"
			"setcb %%cl;"
		 	:"=d"(o),"=c"(c),"=a"(a)
			:"a"(a), "b"(b));
	#else
	#error "There's no version of ins_add for your target."
	#endif

	cctx->regs[ins->dst] = a;

	if(o)
		cctx->regs[CPU_REG_FLGS] |= FLGS_O;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_O;

	if(c)
		cctx->regs[CPU_REG_FLGS] |= FLGS_C;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_C;
}

_va_ins void ins_and(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	dword a = cctx->regs[ins->opa];
	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];

	cctx->regs[ins->dst] = a & b;
}

_va_ins void ins_call(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	// Save position
	cctx->regs[CPU_REG_RC] = cctx->pos;	
	// Adjust position
	if(ins->prefix == PREFIX_IMM)
		cctx->pos += SIGNED16(ins->imm16);
	else
		cctx->pos = cctx->regs[ins->dst];
}

_va_ins void ins_cmp(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword a = cctx->regs[ins->opa];
	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];

	if(a == b)
		cctx->regs[CPU_REG_FLGS] |= FLGS_Z;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_Z;

	if(a < b)
		cctx->regs[CPU_REG_FLGS] |= FLGS_C;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_C;

	/* Signed comparision */
	sdword as = SIGNED32(a);
	sdword bs = SIGNED32(b);

	if(as < bs)
		cctx->regs[CPU_REG_FLGS] |= FLGS_S;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_S;
}

_va_ins void ins_cmpxchg(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword dest = cctx->regs[ins->dst];
	
	
	if(ins->prefix == PREFIX_DWORD) {
		dword memValue;
		
		int success = mem_get_dword_v(dest, &memValue, ectx);
		if(!success) return;
		
		dword a = cctx->regs[ins->opa];
		dword b = cctx->regs[ins->opb];
		
		if(memValue == a) {
			success = mem_write_dword_v(b, dest, ectx);
			if(!success) return;
			cctx->regs[CPU_REG_FLGS] |= FLGS_Z;
			
		} else {
			cctx->regs[ins->opa] = memValue;
			cctx->regs[CPU_REG_FLGS] &= ~FLGS_Z;
		}
		
	} 
	
}

_va_ins void ins_div(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];

	if(b == 0u) {
		int_fire_interrupt(0u, ectx);
		return;
	}

	cctx->regs[ins->dst] = cctx->regs[ins->opa] / b;
}

_va_ins void ins_in(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	if(!(cctx->cregs[CPU_CREG_CR1] & CR1_PRIV_MODE)) {
		int_fire_interrupt(INT_EXC_PROTFAULT, ectx);
		return;
	}

	cctx->regs[ins->opa] = pio_read(cctx->regs[ins->opb & 0x7Fu], ectx);
}

_va_ins void ins_int(instruction_context* ins, emulator_context* ectx) {
	word no = ins->imm16;
	no &= 0x00FFu;

	if(!((no >= INT_SW_MIN) && (no <= INT_SW_MAX))) {
		int_fire_interrupt(INT_EXC_ILLEGALOP, ectx);
	} 
	else int_fire_interrupt(no, ectx);
}

_va_ins void ins_jmp(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	
	if(ins->prefix == PREFIX_IMM)
		cctx->pos += SIGNED16(ins->imm16);
	else
		cctx->pos = cctx->regs[ins->dst];
}

_va_ins void ins_loadas(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	if(!(cctx->cregs[CPU_CREG_CR1] & CR1_PRIV_MODE)) {
		int_fire_interrupt(INT_EXC_PROTFAULT, ectx);
		return;
	}

	cpu_pop_dword(&(cctx->cregs[CPU_CREG_SSP]), &(cctx->regs[ins->dst]), ectx);
}

_va_ins void ins_loadi(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	
	if(ins->prefix == PREFIX_BYTE){
		cpu_set_byte_reg(ins->dst, ins->imm16 & 0x00FFu, cctx->regs);
		
	} else {
		cpu_set_word_reg(ins->dst, ins->imm16, cctx->regs);
	}
}

_va_ins void ins_loadid(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	if((qword)((qword)cctx->regs[ins->opa] + (qword)ins->imm12) > DWORD_MAX) {
		int_fire_interrupt(3u, ectx);
		return;
	}

	if(ins->prefix == PREFIX_DWORD) {
		dword data;

		if(!mem_get_dword_v(cctx->regs[ins->opa] + ins->imm12, &data, ectx))
			return;

		cctx->regs[ins->dst] = data;
	}
	else if(ins->prefix == PREFIX_WORD) {
		word data;

		if(!mem_get_word_v(cctx->regs[ins->opa] + ins->imm12, &data, ectx))
			return;

		cpu_set_word_reg(ins->dst, data, cctx->regs);
	}
	else if(ins->prefix == PREFIX_BYTE) {
		byte data;

		if(!mem_get_byte_v(cctx->regs[ins->opa] + ins->imm12, &data, ectx))
			return;

		cpu_set_byte_reg(ins->dst, data, cctx->regs);
	}
}

_va_ins void ins_loadma(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	byte dst = ins->dst;
	byte immr = ins->imm16 & 0x00FFu;
	byte regstart = immr >> 4u;
	byte regend = immr & 0xFu;

	byte regp = regend;

	dword sp = cctx->regs[dst];
	dword temp = cctx->regs[dst];

	for(; regp >= regstart; regp--) {
		#if DUMP_CPU || DUMP_ALL
		PUTLOG(ectx, "[CPU] :: LOADMA popping %x\n", regp);
		#endif
		if(!cpu_pop_dword(&temp, &(cctx->regs[regp]), ectx)) {
			// Restore
			cctx->regs[dst] = sp;
			return;
		}

		if(regp == regstart)
			break;
	}

	cctx->regs[dst] = temp;
}

_va_ins void ins_loadrd(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	dword* regs = cctx->regs;

	sqword opa = (sqword)cctx->regs[ins->opa];
	sqword ofs = (sqword)SIGNED32(cctx->regs[ins->opb]);

	if((opa + ofs) > DWORD_MAX || (opa + ofs) < 0) {
		int_fire_interrupt(3u, ectx);
		return;
	}

	sdword disp = SIGNED32(cctx->regs[ins->opb]);
	
	if(ins->prefix == PREFIX_DWORD) {
		dword data;
		if(!mem_get_dword_v(cctx->regs[ins->opa] + disp, &data, ectx))
			return;
		regs[ins->dst] = data;
	}
	else if(ins->prefix == PREFIX_WORD) {
		word data;
		if(!mem_get_word_v(cctx->regs[ins->opa] + disp, &data, ectx))
			return;
		cpu_set_word_reg(ins->dst, data, regs);
	}
	else if(ins->prefix == PREFIX_BYTE) {
		byte data;
		if(!mem_get_byte_v(cctx->regs[ins->opa] + disp, &data, ectx))
			return;
		cpu_set_byte_reg(ins->dst, data, regs);
	}
}

_va_ins void ins_mod(instruction_context* ins, emulator_context* ectx){
	
	cpu_context* cctx = ectx->cpu_context;
	
	dword modulo = cctx->regs[ins->opb];
	dword value = cctx->regs[ins->opa];
	
	if(modulo == 0){
		int_fire_interrupt(0u, ectx);	
		return;			
	}
	
	value = value % modulo;
	
	cctx->regs[ins->dst] = value;
}

_va_ins void ins_mov(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword di, dw;
	word w; byte b;

	switch(ins->prefix) {
		case PREFIX_IMM:
			di = ins->imm12;
			cctx->regs[ins->opa] = di;
			break;
		case PREFIX_DWORD:
			dw = cctx->regs[ins->opb];
			cctx->regs[ins->opa] = dw;
			break;
		case PREFIX_WORD:
			w = cpu_get_word_reg(ins->opb, cctx->regs);
			cpu_set_word_reg(ins->opa, w, cctx->regs);
			break;
		case PREFIX_BYTE:
			b = cpu_get_byte_reg(ins->opb, cctx->regs);
			cpu_set_byte_reg(ins->opa, b, cctx->regs);
			break;
	}
}

_va_ins void ins_movfc(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	if(!(cctx->cregs[CPU_CREG_CR1] & CR1_PRIV_MODE)) {
		int_fire_interrupt(INT_EXC_PROTFAULT, ectx);
		return;
	}

	cctx->regs[ins->opa] = cctx->cregs[ins->opb];
}

_va_ins void ins_movtc(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	if(!(cctx->cregs[CPU_CREG_CR1] & CR1_PRIV_MODE)) {
		int_fire_interrupt(INT_EXC_PROTFAULT, ectx);
		return;
	}

	cctx->cregs[ins->opa] = cctx->regs[ins->opb];

	if(cctx->cregs[CPU_CREG_CR1] & CR1_INT_ENABLED) //reload ITT cache in ITU
		int_reload_itt(ectx);
}

_va_ins void ins_mul(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];

	qword res = (qword)cctx->regs[ins->opa] * (qword)b;

	if(res > DWORD_MAX) {
		cctx->regs[CPU_REG_FLGS] |= FLGS_C;
	}
	else {
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_C;
	}

	cctx->regs[ins->dst] = (dword)res;

}

_va_ins void ins_not(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	cctx->regs[ins->opa] = ~(cctx->regs[ins->opb]);
}

_va_ins void ins_or(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	dword a = cctx->regs[ins->opa];
	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];

	cctx->regs[ins->dst] = a | b;
}

_va_ins void ins_out(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword* regs = cctx->regs;

	if(!(cctx->cregs[CPU_CREG_CR1] & CR1_PRIV_MODE)) {
		int_fire_interrupt(INT_EXC_PROTFAULT, ectx);
		return;
	}

	pio_write(regs[ins->opa] & 0x7Fu, regs[ins->opb], ectx);
}

_va_ins void ins_sar(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword a = cctx->regs[ins->opa];
	sdword as = SIGNED32(a);
	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];

	b %= 32;

	// Carry?
	if(a & 1u)
		cctx->regs[CPU_REG_FLGS] |= FLGS_C;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_C;

	cctx->regs[ins->dst] = UNSIGNED32(as >> b);
}

_va_ins void ins_sex(instruction_context* ins, emulator_context* ectx){
	
	cpu_context* cctx = ectx->cpu_context;
  
	dword value = cctx->regs[ins->opa];
	dword unmaskedValue = cctx->regs[ins->opb];
	
	if(ins->prefix == PREFIX_BYTE){
		dword maskedValue = unmaskedValue & 0x000000FFu;
		dword signFlag = maskedValue & 0x00000080;
		if(signFlag != 0){
			value = 0xFFFFFF00 | maskedValue;
		}else{
			value = maskedValue;
		}
	}else if(ins->prefix == PREFIX_WORD){
		dword maskedValue = unmaskedValue & 0x0000FFFFu;
		dword signFlag = maskedValue & 0x00008000;
		if(signFlag != 0){
			value = 0xFFFF0000 | maskedValue;
		}else{
			value = maskedValue;
		}
	}
	
	cctx->regs[ins->opa] = value;
	
}

_va_ins void ins_sdiv(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword a = cctx->regs[ins->opa];
	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];

	if(b == 0u) {
		int_fire_interrupt(0u, ectx);
		return;
	}

	sqword aq = (sqword) SIGNED32(a);
	sqword bq = (sqword) SIGNED32(b);

	sqword r = aq / bq;

	if((r > INT32_MAX) || (r < INT32_MIN))  { //overflow
		cctx->regs[CPU_REG_FLGS] |= FLGS_O;
		r = 0;
	}
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_O;

	cctx->regs[ins->dst] = UNSIGNED32((sdword)r);
}

_va_ins void ins_shl(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword a = cctx->regs[ins->opa];
	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];

	b %= 32;

	// Carry?
	if(a & 0x80000000u)
		cctx->regs[CPU_REG_FLGS] |= FLGS_C;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_C;

	cctx->regs[ins->dst] = a << b;
}

_va_ins void ins_shr(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword a = cctx->regs[ins->opa];
	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];

	b %= 32;

	// Carry?
	if(a & 1u)
		cctx->regs[CPU_REG_FLGS] |= FLGS_C;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_C;

	cctx->regs[ins->dst] = a >> b;
}

_va_ins void ins_smul(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword a = cctx->regs[ins->opa];
	dword b;

	byte o;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];
	
	/* TODO: Make this more portable */
	#if __i386__ || __x86_64__
	asm volatile (
			"imull %%ebx, %%eax;"
			"setob %%dl;"
		 	:"=d"(o),"=a"(a)
			:"a"(a), "b"(b));
	#else
	#error "There's no version of ins_smul for your target."
	#endif

	cctx->regs[ins->dst] = a;

	if(o)
		cctx->regs[CPU_REG_FLGS] |= FLGS_O;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_O;

}

_va_ins void ins_sub(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	dword a = cctx->regs[ins->opa];
	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];

	byte o, c;

	/* TODO: Make this more portable */
	#if __i386__ || __x86_64__
	asm volatile (
			"subl %%ebx, %%eax;"
			"setob %%dl;"
			"setcb %%cl;"
		 	:"=d"(o),"=c"(c),"=a"(a)
			:"a"(a), "b"(b));
	#else
	#error "There's no version of ins_sub for your target."
	#endif

	cctx->regs[ins->dst] = a;

	if(o)
		cctx->regs[CPU_REG_FLGS] |= FLGS_O;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_O;

	if(c)
		cctx->regs[CPU_REG_FLGS] |= FLGS_C;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_C;
}

_va_ins void ins_storeas(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	if(!(cctx->cregs[CPU_CREG_CR1] & CR1_PRIV_MODE)) {
		int_fire_interrupt(INT_EXC_PROTFAULT, ectx);
		return;
	}

	cpu_push_dword(&(cctx->cregs[CPU_CREG_SSP]), cctx->regs[ins->dst], ectx);
}

_va_ins void ins_storeid(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	if((qword)((qword)cctx->regs[ins->dst] + (qword)ins->imm12) > DWORD_MAX) {
		int_fire_interrupt(3u, ectx);
		return;
	}

	if(ins->prefix == PREFIX_DWORD) {

		if(!mem_write_dword_v(cctx->regs[ins->opa], cctx->regs[ins->dst] + ins->imm12, ectx))
			return;

	} else if(ins->prefix == PREFIX_BYTE) {

		byte b = cpu_get_byte_reg(ins->opa, cctx->regs);

		if(!mem_write_byte_v(b, cctx->regs[ins->dst] + ins->imm12, ectx))
			return;

	} else if(ins->prefix == PREFIX_WORD) {

		word w = cpu_get_word_reg(ins->opa, cctx->regs);

		if(!mem_write_word_v(w, cctx->regs[ins->dst] + ins->imm12, ectx))
			return;

	}
}

_va_ins void ins_storema(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	byte dst = ins->dst;
	byte immr = ins->imm16 & 0x00FFu;
	byte regstart = immr >> 4u;
	byte regend = immr & 0xFu;

	dword sp = cctx->regs[dst];
	dword temp = cctx->regs[dst];

	byte regp = regstart;

	for(; regp <= regend; regp++) {
		#if DUMP_CPU || DUMP_ALL
		PUTLOG(ectx,"[CPU] :: STOREMA pushing %x\n", regp);
		#endif
		if(!cpu_push_dword(&temp, cctx->regs[regp], ectx)) {
			cctx->regs[dst] = sp;
			return;
		}
	}

	cctx->regs[dst] = temp;
}


_va_ins void ins_storerd(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	sqword dst = (sqword)cctx->regs[ins->dst];
	sqword ofs = (sqword)SIGNED32(cctx->regs[ins->opb]);

	if((dst + ofs) > DWORD_MAX || (dst + ofs) < 0) {
		int_fire_interrupt(3u, ectx);
		return;
	}

	sdword disp = SIGNED32(cctx->regs[ins->opb]);

	if(ins->prefix == PREFIX_DWORD) {

		if(!mem_write_dword_v(cctx->regs[ins->opa], cctx->regs[ins->dst] + disp, ectx))
			return;

	} else if(ins->prefix == PREFIX_BYTE) {

		byte b = cpu_get_byte_reg(ins->opa, cctx->regs);

		if(!mem_write_byte_v(b, cctx->regs[ins->dst] + disp, ectx))
			return;

	} else if(ins->prefix == PREFIX_WORD) {

		word w = cpu_get_word_reg(ins->opa, cctx->regs);

		if(!mem_write_word_v(w, cctx->regs[ins->dst] + disp, ectx))
			return;

	}
}


_va_ins void ins_test(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	dword val = cctx->regs[ins->dst];

	if(ins->prefix == PREFIX_IMM)
		val = ins->imm16;

	if(val == 0x00000000u)
		cctx->regs[CPU_REG_FLGS] |= FLGS_Z;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_Z;

	if(val & 0x80000000u)
		cctx->regs[CPU_REG_FLGS] |= FLGS_S;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_S;

	if(!(val & 0x00000001u))
		cctx->regs[CPU_REG_FLGS] |= FLGS_E;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_E;

	int bits = 0;
	for(int i = 0; i < 32; i++) {
		if(val & 1u)
			bits++;
		val >>= 1;
	}
	if(bits % 2 == 1)
		cctx->regs[CPU_REG_FLGS] |= FLGS_P;
	else
		cctx->regs[CPU_REG_FLGS] &= ~FLGS_P;
}

_va_ins void ins_xchg(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	
	dword temp = cctx->regs[ins->opa];
	cctx->regs[ins->opa] = cctx->regs[ins->opb];
	cctx->regs[ins->opb] = temp;
}

_va_ins void ins_xchgc(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	
	if(!(cctx->cregs[CPU_CREG_CR1] & CR1_PRIV_MODE)) {
		int_fire_interrupt(INT_EXC_PROTFAULT, ectx);
		return;
	}
	
	dword temp = cctx->regs[ins->opa];
	cctx->regs[ins->opa] = cctx->cregs[ins->opb];
	cctx->cregs[ins->opb] = temp;
}

_va_ins void ins_xor(instruction_context* ins, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	dword a = cctx->regs[ins->opa];
	dword b;

	if(ins->prefix == PREFIX_IMM)
		b = ins->imm12;
	else
		b = cctx->regs[ins->opb];
	
	cctx->regs[ins->dst] = a ^ b;
}
