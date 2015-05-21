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

#ifndef VA_LIB_DIS_H
#define VA_LIB_DIS_H

#include <stdint.h>

/* Buffer sizes */
#define ARGUMENT_BUFFER_SIZE 256
#define INSTRUCTION_BUFFER_SIZE 256

/* Formats for the different instruction groups */
#define FMT_REG_REG_REG "%s,\t%s,\t%s"
#define FMT_REG_REG "%s,\t%s"
#define FMT_REG_REG_IMM12 "%s,\t%s,\t0x%04x"
#define FMT_REG "%s"
#define FMT_REG_IMM12 "%s,\t0x%04x"
#define FMT_REG_IMM16 "%s,\t0x%04x"
#define FMT_IMM16 "0x%04x"
#define FMT_NONE ""

/* Message to print if an invalid argument was found */
#define MSG_INVALID_ARGS "; INVALID"

typedef struct {
	uint8_t opcode;
	uint8_t prefix;
	uint8_t condition;
	uint8_t destination;
	uint8_t operand_a;
	uint8_t operand_b;
	uint16_t imm16;
	uint16_t imm12;
} instruction;

/* Function: vasmdis_disasm_instruction

   Disassembles the bytes provided as argument.
   Invalid encoded instructions are disassembled as far as possible and the
   invalid part will be replaced with '; INVALID'
   Note: vasmdis_disasm_instruction is not threadsafe - it uses an internal buffer to store the
   disassembled parts
   
   Parameters:
     ins - the pointer to the first byte of an assembled instruction

   Returns:
     the disassembled instruction as string. the arguments are delimited by tabulators
*/
char* vasmdis_disasm_instruction(uint8_t* ins);

/* Function: vasmdis_bytes_to_instruction

   Parses the raw data provided in the parameter bytes and stores it in the
   instruction struct.
   
   Parameters:
     instruction - a pointer to a instruction struct which will be used to store the parsed data
     bytes - the raw encoded data of an instruction

   Returns:
     
     Nothing. (see instruction)
*/
void vasmdis_bytes_to_instruction(instruction* instruction, uint8_t* bytes);


#endif
