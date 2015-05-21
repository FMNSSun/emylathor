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

#include "lookup.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


static char* oplkupbuf[128] = {
		(char*)OP_STOP,"STOP",	// 0x00
		(char*)OP_FAIL,"FAIL", // 0x01
		NULL, NULL, // 0x02
		NULL, NULL, // 0x03
		(char*)OP_IRET,"IRET", // 0x04
		NULL, NULL, // 0x05
		(char*)OP_LOADI,"LOADI", // 0x06
		NULL, NULL, // 0x07
		(char*)OP_STOREMA,"STOREMA", // 0x08
		(char*)OP_LOADMA,"LOADMA", // 0x09
		(char*)OP_INT,"INT", // 0x0A
		(char*)OP_JMP,"JMP", // 0x0B
		(char*)OP_CALL,"CALL", // 0x0C
		(char*)OP_STOREAS, "STOREAS", // 0x0D
		(char*)OP_LOADAS, "LOADAS", // 0x0E
		NULL, NULL, // 0x0F
		(char*)OP_SMUL,"SMUL", // 0x10
		(char*)OP_SDIV,"SDIV", // 0x11
		(char*)OP_NOT,"NOT", // 0x12
		NULL, NULL, // 0x13
		(char*)OP_SEX,"SEX", // 0x14
		(char*)OP_MOD,"MOD", // 0x15
		(char*)OP_TEST,"TEST", // 0x16
		NULL, NULL, // 0x17
		(char*)OP_CMP,"CMP", // 0x18
		(char*)OP_IN,"IN", // 0x19
		(char*)OP_MOV,"MOV", // 0x1A
		(char*)OP_CMPXCHG,"CMPXCHG", // 0x1B
		NULL,NULL, // 0x1C
		(char*)OP_MOVTC,"MOVTC", // 0x1D
		(char*)OP_MOVFC,"MOVFC", // 0x1E
		(char*)OP_SAR,"SAR", // 0x1F
		(char*)OP_XCHG,"XCHG", // 0x20
		(char*)OP_AND,"AND", // 0x21
		(char*)OP_OR,"OR", // 0x22
		(char*)OP_XOR,"XOR", // 0x23
		(char*)OP_SHL,"SHL", // 0x24
		(char*)OP_SHR,"SHR", // 0x25
		(char*)OP_LOADID,"LOADID", // 0x26
		(char*)OP_LOADRD,"LOADRD", // 0x27
		(char*)OP_STOREID,"STOREID", // 0x28
		(char*)OP_STORERD,"STORERD", // 0x29
		(char*)OP_ADD,"ADD", // 0x2A
		(char*)OP_SUB,"SUB", // 0x2B
		(char*)OP_MUL,"MUL", // 0x2C
		(char*)OP_DIV,"DIV", // 0x2D
		(char*)OP_OUT,"OUT", // 0x2E
		(char*)OP_XCHGC, "XCHGC"
	};

static char* creglkupbuf[32] = {
		(char*)CPU_CREG_MR1,"MR1",
		(char*)CPU_CREG_MR2,"MR2",
		(char*)CPU_CREG_CR1,"CR1",
		(char*)CPU_CREG_CR2,"CR2",
		(char*)CPU_CREG_SSP,"SSP",
		(char*)CPU_CREG_MR3,"MR3",
		(char*)CPU_CREG_MR4,"MR4",
		(char*)CPU_CREG_ST1,"ST1",
		(char*)CPU_CREG_ST1,"ST2",
		(char*)CPU_CREG_ST1,"ST3",
		(char*)CPU_CREG_ST1,"ST4"
	};

static char* reg32lkupbuf[32] = {
		(char*)CPU_REG_R0,"R0",
		(char*)CPU_REG_R1,"R1",
		(char*)CPU_REG_R2,"R2",
		(char*)CPU_REG_R3,"R3",
		(char*)CPU_REG_R4,"R4",
		(char*)CPU_REG_R5,"R5",
		(char*)CPU_REG_R6,"R6",
		(char*)CPU_REG_R7,"R7",
		(char*)CPU_REG_R8,"R8",
		(char*)CPU_REG_R9,"R9",
		(char*)CPU_REG_RA,"RA",
		(char*)CPU_REG_RB,"RB",
		(char*)CPU_REG_RC,"RC",
		(char*)CPU_REG_BP,"BP",
		(char*)CPU_REG_SP,"SP",
		(char*)CPU_REG_FLGS,"FLGS"
	};

static char* reg16lkupbuf[32] = {
		(char*)CPU_REG16_R0L,"R0L",
		(char*)CPU_REG16_R0H,"R0H",
		(char*)CPU_REG16_R1L,"R1L",
		(char*)CPU_REG16_R1H,"R1H",
		(char*)CPU_REG16_R2L,"R2L",
		(char*)CPU_REG16_R2H,"R2H",
		(char*)CPU_REG16_R3L,"R3L",
		(char*)CPU_REG16_R3H,"R3H",
		(char*)CPU_REG16_R4L,"R4L",
		(char*)CPU_REG16_R4H,"R4H",
		(char*)CPU_REG16_R5L,"R5L",
		(char*)CPU_REG16_R5H,"R5H",
		(char*)CPU_REG16_R6L,"R6L",
		(char*)CPU_REG16_R6H,"R6H",
		(char*)CPU_REG16_R7L,"R7L",
		(char*)CPU_REG16_R7H,"R7H"
	};

static char* reg8lkupbuf[32] = {
		(char*)CPU_REG8_R0LL,"R0LL",
		(char*)CPU_REG8_R0LH,"R0LH",
		(char*)CPU_REG8_R0HL,"R0HL",
		(char*)CPU_REG8_R0HH,"R0HH",
		(char*)CPU_REG8_R1LL,"R1LL",
		(char*)CPU_REG8_R1LH,"R1LH",
		(char*)CPU_REG8_R1HL,"R1HL",
		(char*)CPU_REG8_R1HH,"R1HH",
		(char*)CPU_REG8_R2LL,"R2LL",
		(char*)CPU_REG8_R2LH,"R2LH",
		(char*)CPU_REG8_R2HL,"R2HL",
		(char*)CPU_REG8_R2HH,"R2HH",
		(char*)CPU_REG8_R3LL,"R3LL",
		(char*)CPU_REG8_R3LH,"R3LH",
		(char*)CPU_REG8_R3HL,"R3HL",
		(char*)CPU_REG8_R3HH,"R3HH",
	};

byte get_op_for_name(char* op) {
	int i;
	for(i = 1; i < 128u; i+=2) {
		if(oplkupbuf[i] == NULL)
			continue;
		if(!strcmp(oplkupbuf[i], op))
			return (byte)((dword)POINTER_TO_INT(oplkupbuf[i-1]));
	}

	fprintf(stderr, "Warning: Replaced invalid op %s with STOP\n", op);

	return OP_STOP;
}

byte get_creg_for_name(char* reg) {
	int i;
	for(i = 1; i < 32u; i+=2) {
		if(creglkupbuf[i] == NULL)
			continue;
		if(!strcmp(creglkupbuf[i], reg))
			return (byte)((dword)POINTER_TO_INT(creglkupbuf[i-1]));
	}

	fprintf(stderr, "Warning: Replaced invalid reg %s with CR1\n", reg);

	return CPU_CREG_CR1;
}

byte get_reg_for_name(char* reg) {
	int i;
	for(i = 1; i < 32u; i+=2) {
		if(reg32lkupbuf[i] == NULL)
			continue;
		if(!strcmp(reg32lkupbuf[i], reg))
			return (byte)((dword)POINTER_TO_INT(reg32lkupbuf[i-1]));
	}

	fprintf(stderr, "Warning: Replaced invalid reg %s with R0\n", reg);

	return CPU_REG_R0;
}

byte get_reg16_for_name(char* reg) {
	int i;
	for(i = 1; i < 32u; i+=2) {
		if(reg16lkupbuf[i] == NULL)
			continue;
		if(!strcmp(reg16lkupbuf[i], reg))
			return (byte)((dword)POINTER_TO_INT(reg16lkupbuf[i-1]));
	}
	fprintf(stderr, "Warning: Replaced invalid reg %s with R0L\n", reg);

	return CPU_REG16_R0L;
}

byte get_reg8_for_name(char* reg) {
	int i;
	for(i = 1; i < 32u; i+=2) {
		if(reg8lkupbuf[i] == NULL)
			continue;
		if(!strcmp(reg8lkupbuf[i], reg))
			return (byte)((dword)POINTER_TO_INT(reg8lkupbuf[i-1]));
	}

	fprintf(stderr, "Warning: Replaced invalid reg %s with R0LL\n", reg);

	return CPU_REG8_R0LL;
}

byte get_cond_for_name(char* cond) {
	if(!strcmp(cond,"IS"))
		return COND_IS;
	else if(!strcmp(cond,"NS"))
		return COND_NS;
	else if(!strcmp(cond,"IZ"))
		return COND_IZ;
	else if(!strcmp(cond,"NZ"))
		return COND_NZ;
	else if(!strcmp(cond,"IP"))
		return COND_IP;
	else if(!strcmp(cond,"NP"))
		return COND_NP;
	else if(!strcmp(cond,"IO"))
		return COND_IO;
	else if(!strcmp(cond,"NO"))
		return COND_NO;
	else if(!strcmp(cond,"IZIS"))
		return COND_IZIS;
	else if(!strcmp(cond,"NZNS"))
		return COND_NZNS;
	else if(!strcmp(cond,"IZIC"))
		return COND_IZIC;
	else if(!strcmp(cond,"NZNC"))
		return COND_NZNC;
	else if(!strcmp(cond,"IC"))
		return COND_IC;
	else if(!strcmp(cond,"NC"))
		return COND_NC;
	else if(!strcmp(cond,"NF"))
		return COND_NF;

	fprintf(stderr, "Warning: Replaced invalid condition %s with COND_ALWAYS\n", cond);
	

	return COND_ALWAYS;
}

char* get_name_of_op(byte prefix, byte op) {
	int i;
	for(i = 0; i < 128u; i+=2) {
		if((byte)(dword)POINTER_TO_INT(oplkupbuf[i]) == op) {
			if(oplkupbuf[i+1] == NULL)
				return "N/A";
			
			return oplkupbuf[i+1];
		}
	}

	return "N/A";
}

char* get_name_of_creg(byte reg) {
	int i;
	for(i = 0; i < 32u; i+=2) {
		if((byte)(dword)POINTER_TO_INT(creglkupbuf[i]) == reg) {
			if(creglkupbuf[i+1] == NULL)
				return "N/A";
			
			return creglkupbuf[i+1];
		}
	}

	return "N/A";
}

char* get_name_of_reg(byte reg) {
	int i;
	for(i = 0; i < 32u; i+=2) {
		if((byte)(dword)POINTER_TO_INT(reg32lkupbuf[i]) == reg) {
			if(reg32lkupbuf[i+1] == NULL)
				return "N/A";
			
			return reg32lkupbuf[i+1];
		}
	}

	return "N/A";
}

char* get_name_of_reg8(byte reg) {
	int i;
	for(i = 0; i < 32u; i+=2) {
		if((byte)(dword)POINTER_TO_INT(reg8lkupbuf[i]) == reg) {
			if(reg8lkupbuf[i+1] == NULL)
				return "N/A";
			
			return reg8lkupbuf[i+1];
		}
	}

	return "N/A";
}

char* get_name_of_reg16(byte reg) {
	int i;
	for(i = 0; i < 32u; i+=2) {
		if((byte)(dword)POINTER_TO_INT(reg16lkupbuf[i]) == reg) {
			if(reg16lkupbuf[i+1] == NULL)
				return "N/A";
			
			return reg16lkupbuf[i+1];
		}
	}

	return "N/A";
}

char* get_name_of_prefix(byte prefix) {
	switch(prefix) {
		case PREFIX_DWORD:
			return "DWORD";
		case PREFIX_WORD:
			return "WORD";
		case PREFIX_BYTE:
			return "BYTE";
		case PREFIX_IMM:
			return "IMM";
		default:
			return "N/A";
	}
	return "N/A";
}

char* get_name_of_cond(byte cond) {
	switch(cond) {
		case COND_ALWAYS:
			return "";
		case COND_IS:
			return "IS";
		case COND_NS:
			return "NS";
		case COND_IZ:
			return "IZ";
		case COND_NZ:
			return "NZ";
		case COND_IO:
			return "IO";
		case COND_NO:
			return "NO";
		case COND_IP:
			return "IP";
		case COND_NP:
			return "NP";
		case COND_IZIS:
			return "IZIS";
		case COND_IZIC:
			return "IZIC";
		case COND_NZNS:
			return "NZNS";
		case COND_NZNC:
			return "NZNC";
		case COND_IC:
			return "IC";
		case COND_NC:
			return "NC";
		case COND_NF:
			return "NF";
	}
	return "N/A";
}

