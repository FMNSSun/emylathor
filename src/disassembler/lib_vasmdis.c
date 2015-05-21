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

#include "lib_vasmdis.h"
#include "common/lookup.h" 
#include "common/rawdefs.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>



static void decode_dword_prefix_ops(instruction* instruction, char* buffer_reg){
	switch(instruction->opcode) {
		case OP_STOP:	//FORMAT I
		case OP_IRET:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_NONE
			);
			break;
		case OP_ADD:	//FORMAT II
		case OP_AND:
		case OP_DIV:
		case OP_OR:
		case OP_MOD:
		case OP_MUL:
		case OP_SAR:
		case OP_SDIV:
		case OP_SHL:
		case OP_SHR:
		case OP_SMUL:
		case OP_SUB:
		case OP_XOR:
		case OP_LOADRD:
		case OP_STORERD:
		case OP_CMPXCHG:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG_REG,
				get_name_of_reg(instruction->destination),
				get_name_of_reg(instruction->operand_a),
				get_name_of_reg(instruction->operand_b)
			);
			break;
		case OP_CALL: //FORMAT V
		case OP_JMP:
		case OP_TEST:
		case OP_LOADAS:
		case OP_STOREAS:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG,
				get_name_of_reg(instruction->destination)
			);
			break;
		case OP_CMP:	//FORMAT VI
		case OP_IN:
		case OP_NOT:
		case OP_OUT:
		case OP_XCHG:
		case OP_MOV:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG,
				get_name_of_reg(instruction->operand_a),
				get_name_of_reg(instruction->operand_b)
			);
			break;
		case OP_LOADID:
		case OP_STOREID:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG_IMM12,
				get_name_of_reg(instruction->destination),
				get_name_of_reg(instruction->operand_a),
				instruction->imm12
			);
			break;
		case OP_MOVTC:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG,
				get_name_of_creg(instruction->operand_a),
				get_name_of_reg(instruction->operand_b)
			);
			break;
		case OP_XCHGC:
		case OP_MOVFC:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG,
				get_name_of_reg(instruction->operand_a),
				get_name_of_creg(instruction->operand_b)
			);
			break;
		default:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE, MSG_INVALID_ARGS);
			break;
	}
	
}

static void decode_word_prefix_ops(instruction* instruction, char* buffer_reg){
	switch(instruction->opcode) {
		case OP_LOADI:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_IMM16,
				get_name_of_reg16(instruction->destination),
				instruction->imm16
			);
			break;
		case OP_LOADID:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG_IMM12,
				get_name_of_reg16(instruction->destination),
				get_name_of_reg(instruction->operand_a),
				instruction->imm12
			);
			break;
		case OP_LOADRD:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG_REG,
				get_name_of_reg16(instruction->destination),
				get_name_of_reg(instruction->operand_a),
				get_name_of_reg(instruction->operand_b)
			);
			break;
		case OP_MOV:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG,
				get_name_of_reg16(instruction->operand_a),
				get_name_of_reg16(instruction->operand_b)
			);
			break;
		case OP_SEX:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG,
				get_name_of_reg(instruction->operand_a),
				get_name_of_reg16(instruction->operand_b)
			);
			break;
		case OP_STOREID:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG_IMM12,
				get_name_of_reg(instruction->destination),
				get_name_of_reg16(instruction->operand_a),
				instruction->imm12
			);
			break;
		case OP_STORERD:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG_REG,
				get_name_of_reg(instruction->destination),
				get_name_of_reg16(instruction->operand_a),
				get_name_of_reg(instruction->operand_b)
			);
			break;
		default:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE, MSG_INVALID_ARGS);
			break;
	}
}

static void decode_byte_prefix_ops(instruction* instruction, char* buffer_reg){
	switch(instruction->opcode){
		case OP_LOADI:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE, 
				 FMT_REG_IMM16,
				 get_name_of_reg8(instruction->destination),
				 instruction->imm16
			);
			break;
		case OP_LOADID:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG_IMM12,
				get_name_of_reg8(instruction->destination),
				get_name_of_reg(instruction->operand_a),
				instruction->imm12
			);
			break;
		case OP_LOADRD:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG_REG,
				get_name_of_reg8(instruction->destination),
				get_name_of_reg(instruction->operand_a),
				get_name_of_reg(instruction->operand_b)
			);
			break;
		case OP_MOV:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG,
				get_name_of_reg8(instruction->operand_a),
				get_name_of_reg8(instruction->operand_b)
			);
			break;
		case OP_SEX:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG,
				get_name_of_reg(instruction->operand_a),
				get_name_of_reg8(instruction->operand_b)
			);
			break;
		case OP_STOREID:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG_IMM12,
				get_name_of_reg(instruction->destination),
				get_name_of_reg8(instruction->operand_a),
				instruction->imm12
			);
			break;
		case OP_STORERD:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG_REG,
				get_name_of_reg(instruction->destination),
				get_name_of_reg8(instruction->operand_a),
				get_name_of_reg(instruction->operand_b)
			);
			break;
		default:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE, MSG_INVALID_ARGS);
			break;
	}
}

static void decode_immediate_prefix_ops(instruction* instruction, char* buffer_reg){
	switch(instruction->opcode) {
		case OP_ADD:	//FORMAT III
		case OP_AND:
		case OP_DIV:
		case OP_OR:
		case OP_MOD:
		case OP_MUL:
		case OP_SAR:
		case OP_SDIV:
		case OP_SHL:
		case OP_SHR:
		case OP_SMUL:
		case OP_SUB:
		case OP_XOR:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_REG_IMM12,
				get_name_of_reg(instruction->destination),
				get_name_of_reg(instruction->operand_a),
				instruction->imm12
			);
			break;
		case OP_CMP: //FORMAT VII
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_IMM12,
				get_name_of_reg(instruction->operand_a),
				instruction->imm12
			);
			break;
		case OP_CALL:
		case OP_FAIL:	//FORMAT VIII
		case OP_INT:
		case OP_JMP:
		case OP_TEST:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_IMM16,
				instruction->imm16
			);
			break;	
		case OP_LOADMA:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_IMM16,
				get_name_of_reg(instruction->destination),
				instruction->imm16
			);
			break;
		case OP_STOREMA:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE,
				FMT_REG_IMM16,
				get_name_of_reg(instruction->destination),
				instruction->imm16
			);
			break;
		default:
			snprintf(buffer_reg, ARGUMENT_BUFFER_SIZE, MSG_INVALID_ARGS);
			break;
	}
}

static void decode_instruction_arguments(instruction* instruction, char* buffer_reg){
	switch(instruction->prefix) {
		case PREFIX_DWORD:
			decode_dword_prefix_ops(instruction, buffer_reg);
			break;
		case PREFIX_WORD:
			decode_word_prefix_ops(instruction, buffer_reg);
			break;
		case PREFIX_BYTE:
			decode_byte_prefix_ops(instruction, buffer_reg);
			break;
		case PREFIX_IMM:
			decode_immediate_prefix_ops(instruction, buffer_reg);
			break;
	}
}

char* vasmdis_disasm_instruction(uint8_t* ins) {
	//ins always must point to a buffer of at least size 4 (in bytes)
	
	
	instruction instruction; 
	
	vasmdis_bytes_to_instruction(&instruction, ins);

	/* OPCODE and CONDITION CODE are pretty straight forward */
	char* opcode_str = get_name_of_op(instruction.prefix, instruction.opcode);
	char* prefix_str = get_name_of_prefix(instruction.prefix);
	char* condition_str = get_name_of_cond(instruction.condition);

	/* registers not so much... */
	static char buffer_reg[ARGUMENT_BUFFER_SIZE];

	decode_instruction_arguments(&instruction, buffer_reg);

	static char buffer_ins[INSTRUCTION_BUFFER_SIZE];
	
	snprintf(buffer_ins, INSTRUCTION_BUFFER_SIZE, "%s%s\t\t%s",
		opcode_str, condition_str, buffer_reg);

	return buffer_ins;
}

void vasmdis_bytes_to_instruction(instruction* instruction, uint8_t* bytes){
	/* Let's decode the instruction... */
	instruction->opcode = bytes[0x00u] & 0x3Fu;			//OPCODE
	instruction->prefix = bytes[0x00u] >> 6u;   			//INSTRUCTION PREFIX
	instruction->condition = bytes[0x01u] >> 4u;			//CONDITION CODE
	instruction->destination = bytes[0x01u] & 0x0Fu; 		//DESTINATION OPERAND
	instruction->operand_a = bytes[0x02u] >> 4u;			//OPERAND A
	instruction->operand_b = bytes[0x02u] & 0x0Fu;			//OPERAND B
	
	instruction->imm16 = (bytes[0x03u] << 8u) | bytes[0x2u];	//IMMEDIATE 16 BIT DATA (little endian)
	instruction->imm12 = (instruction->operand_b << 8u) | bytes[0x3u];		//IMMEDIATE 12 BIT DATA (big endian)
}

