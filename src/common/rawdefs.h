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

#ifndef VA_H_RAWDEFS_H
#define VA_H_RAWDEFS_H

// Control registers
#define CPU_CREG_MR1 0x00u
#define CPU_CREG_MR2 0x01u
#define CPU_CREG_MR3 0x05u
#define CPU_CREG_MR4 0x06u
#define CPU_CREG_CR1 0x02u
#define CPU_CREG_CR2 0x03u
#define CPU_CREG_SSP 0x04u
#define CPU_CREG_ST1 0x07u
#define CPU_CREG_ST2 0x08u
#define CPU_CREG_ST3 0x09u
#define CPU_CREG_ST4 0x0Au

#define MR2_MMU_ENABLED 0x01u

#define CR1_INT_ENABLED	0x01u
#define CR1_PRIV_MODE   0x02u
#define CR1_EXCEPTION	0x04u
#define CR1_FAILURE	0x08u

#define FLGS_Z	0x01u
#define FLGS_C	0x02u
#define FLGS_O	0x04u
#define FLGS_P	0x08u
#define FLGS_S	0x10u
#define FLGS_E	0x20u

/* 13 General purpose registers
    1 Base pointer
    1 Stack pointer
    1 Flags register
 */
#define CPU_REG_R0 0x00u
#define CPU_REG_R1 0x01u
#define CPU_REG_R2 0x02u
#define CPU_REG_R3 0x03u
#define CPU_REG_R4 0x04u
#define CPU_REG_R5 0x05u
#define CPU_REG_R6 0x06u
#define CPU_REG_R7 0x07u
#define CPU_REG_R8 0x08u
#define CPU_REG_R9 0x09u
#define CPU_REG_RA 0x0Au
#define CPU_REG_RB 0x0Bu
#define CPU_REG_RC 0x0Cu
#define CPU_REG_BP 0x0Du
#define CPU_REG_SP 0x0Eu
#define CPU_REG_FLGS 0x0Fu

/* R0 - R7 are available for
 * for word instructions
 */
#define CPU_REG16_R0L	0x00u
#define CPU_REG16_R0H	0x01u
#define CPU_REG16_R1L	0x02u
#define CPU_REG16_R1H	0x03u
#define CPU_REG16_R2L	0x04u
#define CPU_REG16_R2H	0x05u
#define CPU_REG16_R3L	0x06u
#define CPU_REG16_R3H	0x07u
#define CPU_REG16_R4L	0x08u
#define CPU_REG16_R4H	0x09u
#define CPU_REG16_R5L	0x0Au
#define CPU_REG16_R5H	0x0Bu
#define CPU_REG16_R6L	0x0Cu
#define CPU_REG16_R6H	0x0Du
#define CPU_REG16_R7L	0x0Eu
#define CPU_REG16_R7H	0x0Fu

/* R0 - R3 are available for
 * byte instructions
 */
#define CPU_REG8_R0LL	0x00u
#define CPU_REG8_R0LH	0x01u
#define CPU_REG8_R0HL	0x02u
#define CPU_REG8_R0HH	0x03u
#define CPU_REG8_R1LL	0x04u
#define CPU_REG8_R1LH	0x05u
#define CPU_REG8_R1HL	0x06u
#define CPU_REG8_R1HH	0x07u
#define CPU_REG8_R2LL	0x08u
#define CPU_REG8_R2LH	0x09u
#define CPU_REG8_R2HL	0x0Au
#define CPU_REG8_R2HH	0x0Bu
#define CPU_REG8_R3LL	0x0Cu
#define CPU_REG8_R3LH	0x0Du
#define CPU_REG8_R3HL	0x0Eu
#define CPU_REG8_R3HH	0x0Fu

/**
 * Opcodes
 */

#define OP_STOP		0x00u
#define OP_FAIL		0x01u
#define OP_IRET		0x04u
#define OP_LOADI	0x06u
#define OP_STOREMA	0x08u
#define OP_LOADMA	0x09u
#define OP_INT		0x0Au
#define OP_JMP		0x0Bu
#define OP_CALL		0x0Cu
#define OP_STOREAS	0x0Du
#define OP_LOADAS	0x0Eu
#define OP_SMUL		0x10u
#define OP_SDIV		0x11u
#define OP_NOT		0x12u
#define OP_SEX		0x14u
#define OP_MOD		0x15u
#define OP_TEST		0x16u
#define OP_CMP		0x18u
#define OP_IN		0x19u
#define OP_MOV		0x1Au
#define OP_CMPXCHG	0x1Bu
#define OP_MOVTC	0x1Du
#define OP_MOVFC	0x1Eu
#define OP_SAR		0x1Fu
#define OP_XCHG		0x20u
#define OP_AND		0x21u
#define OP_OR		0x22u
#define OP_XOR		0x23u
#define OP_SHL		0x24u
#define OP_SHR		0x25u
#define OP_LOADID	0x26u
#define OP_LOADRD	0x27u	/* TODO */
#define OP_STOREID	0x28u
#define OP_STORERD	0x29u
#define OP_ADD		0x2Au
#define OP_SUB		0x2Bu
#define OP_MUL		0x2Cu
#define OP_DIV		0x2Du
#define OP_OUT		0x2Eu
#define OP_XCHGC	0x2Fu

/**
 * Groups
 */
//TODO: Remove this. Groups are not required for decoding anymore.
#define OPGRP_I		0x00u
#define OPGRP_II	0x01u
#define OPGRP_III	0x02u

/**
 * Prefixes
 */
#define PREFIX_DWORD	0x00u
#define PREFIX_WORD	0x01u
#define PREFIX_BYTE	0x02u
#define PREFIX_IMM	0x03u

/**
 * Conditions
 */
#define COND_ALWAYS	0x00u
#define COND_IZ		0x01u
#define COND_NZ		0x02u
#define COND_IS		0x03u
#define COND_NS		0x04u
#define COND_IO		0x05u
#define COND_NO		0x06u
#define COND_IP		0x07u
#define COND_NP		0x08u
#define COND_IZIS	0x09u
#define COND_IZIC	0x0Au
#define COND_NZNS	0x0Bu
#define COND_NZNC	0x0Cu
#define COND_IC		0x0Du
#define COND_NC		0x0Eu
#define COND_NF		0x0Fu

#endif
