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

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include "emit.h"
#include "common/lookup.h"
#include "gio/gio.h"
#include "vasm.tab.h"  // automatically generated

// extern flex stuff
extern int yylex();
extern  int yyparse();
extern FILE *yyin;
 
void yyerror( emit_context* emctx, const char *s);
void checkIfValid12Bit(word immidiateData, emit_context* ectx);
%}

%locations
%error-verbose
%parse-param { emit_context* emctx }

// Union to hold the tokens
%union {
	word ival;
	dword ival32;
	char *sval;
}

// Symbols
%token <ival> INT
%token <ival32> INT32
%token <sval> IDENT;
%token <sval> STRING
%token <sval> REG32;
%token <sval> REG16;
%token <sval> REG8;
%token <sval> CREG;
%token <sval> REG_RANGE;
%token <sval> OP_FMT_I;
%token <sval> OP_FMT_II;
%token <sval> OP_FMT_IV;
%token <sval> OP_FMT_V;
%token <sval> OP_FMT_VI;
%token <sval> OP_FMT_VIC;
%token <sval> OP_FMT_VIII;
%token <sval> OP_S_CMPXCHG;
%token <sval> OP_S_LOADI;
%token <sval> OP_S_LOADID;
%token <sval> OP_S_LOADRD;
%token <sval> OP_S_MOV;
%token <sval> OP_S_SEX;
%token <sval> OP_S_STOREID;
%token <sval> OP_S_STORERD;
%token <sval> OP_S_XCHGC;
%token <sval> OP_S_LOADAS;
%token <sval> OP_S_STOREAS;
%token <sval> OP_PSEUDO_LOADADR;
%token <sval> OP_PSEUDO_LDCALL;
%token <sval> OP_PSEUDO_LDJMP;
%token <sval> OP_PSEUDO_POP;
%token <sval> OP_PSEUDO_PUSH;
%token <sval> OP_PSEUDO_LOAD32;
%token <sval> OP_GRP_PSEUDO;
%token <sval> CONDITION;
%token COMMA;
%token ENDL;
%token SPACE;
%token GARBAGE;
%token DIRECTIVE_ALIGN;
%token DIRECTIVE_LABEL;
%token DIRECTIVE_OFFSET;
%token DIRECTIVE_BYTE;
%token DIRECTIVE_WORD;
%token DIRECTIVE_DWORD;
%token DIRECTIVE_AT;
%token DIRECTIVE_BYTES;
%token DIRECTIVE_END;

%%
// vasm grammar
exps:
  exp | exps exp
 ;

bytes:
  byte | bytes byte
 ;

byte:
	ENDL { }
	| SPACE {}
	| INT {
		if(($1 > 0xFFu) || ($1 < 0x00u))
			yyerror(emctx, "Invalid byte constant for in .bytes!");

		emit_byte((byte)$1, emctx);
	}

	
exp:
	ENDL { /* empty */ }
        | SPACE { /* empty */ }

	| OP_FMT_I {
		emit_fmt_i(
			PREFIX_DWORD,
			COND_ALWAYS,
			get_op_for_name($1),
			emctx
		);
	}
	| OP_FMT_I CONDITION{
		emit_fmt_i(
			PREFIX_DWORD,
			get_cond_for_name($2),
			get_op_for_name($1),
			emctx
		);
	}

	| OP_FMT_II SPACE REG32 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_DWORD,
			COND_ALWAYS,
			get_op_for_name($1),
			get_reg_for_name($3),
			get_reg_for_name($5),
			get_reg_for_name($7),
			emctx
		);
	}
	| OP_FMT_II CONDITION SPACE REG32 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_DWORD,
			get_cond_for_name($2),
			get_op_for_name($1),
			get_reg_for_name($4),
			get_reg_for_name($6),
			get_reg_for_name($8),
			emctx
		);
	}

	| OP_FMT_II SPACE REG32 COMMA REG32 COMMA INT {	//in this case OP_FMT_II becomes Format III
		checkIfValid12Bit($7, emctx);
		emit_fmt_iii(
			PREFIX_IMM,
			COND_ALWAYS,
			get_op_for_name($1),
			get_reg_for_name($3),
			get_reg_for_name($5),
			$7,
			emctx
		);
	}
	| OP_FMT_II CONDITION SPACE REG32 COMMA REG32 COMMA INT { //in this case OP_FMT_II become Format III
		checkIfValid12Bit($8, emctx);
		emit_fmt_iii(
			PREFIX_IMM,
			get_cond_for_name($2),
			get_op_for_name($1),
			get_reg_for_name($4),
			get_reg_for_name($6),
			$8,
			emctx
		);
	}

	| OP_FMT_IV SPACE REG32 COMMA INT {
		emit_fmt_iv(
			PREFIX_IMM,
			COND_ALWAYS,
			get_op_for_name($1),
			get_reg_for_name($3),
			$5,
			emctx
		);
	}
	| OP_FMT_IV CONDITION SPACE REG32 COMMA INT {
		emit_fmt_iv(
			PREFIX_IMM,
			get_cond_for_name($2),
			get_op_for_name($1),
			get_reg_for_name($4),
			$6,
			emctx
		);
	}

	| OP_FMT_V SPACE REG32 {
		emit_fmt_v(
			PREFIX_DWORD,
			COND_ALWAYS,
			get_op_for_name($1),
			get_reg_for_name($3),
			emctx
		);
	}
	| OP_FMT_V CONDITION SPACE REG32 {
		emit_fmt_v(
			PREFIX_DWORD,
			get_cond_for_name($2),
			get_op_for_name($1),
			get_reg_for_name($4),
			emctx
		);
	}

	| OP_FMT_VI SPACE REG32 COMMA REG32 {
		emit_fmt_vi(
			PREFIX_DWORD,
			COND_ALWAYS,
			get_op_for_name($1),
			get_reg_for_name($3),
			get_reg_for_name($5),
			emctx
		);
	}
	| OP_FMT_VI CONDITION SPACE REG32 COMMA REG32 {
		emit_fmt_vi(
			PREFIX_DWORD,
			get_cond_for_name($2),
			get_op_for_name($1),
			get_reg_for_name($4),
			get_reg_for_name($6),
			emctx
		);
	}

	| OP_FMT_VIC SPACE CREG COMMA REG32 { //same as VI but with control registers
		emit_fmt_vi(
			PREFIX_DWORD,
			COND_ALWAYS,
			get_op_for_name($1),
			get_creg_for_name($3),
			get_reg_for_name($5),
			emctx
		);
	}
	| OP_FMT_VIC CONDITION SPACE CREG COMMA REG32 { //same as VI but with control registers
		emit_fmt_vi(
			PREFIX_DWORD,
			get_cond_for_name($2),
			get_op_for_name($1),
			get_creg_for_name($4),
			get_reg_for_name($6),
			emctx
		);
	}
	| OP_FMT_VIC SPACE REG32 COMMA CREG { //same as VI but with control registers
		emit_fmt_vi(
			PREFIX_DWORD,
			COND_ALWAYS,
			get_op_for_name($1),
			get_reg_for_name($3),
			get_creg_for_name($5),
			emctx
		);
	}
	| OP_FMT_VIC CONDITION SPACE REG32 COMMA CREG{ //same as VI but with control registers
		emit_fmt_vi(
			PREFIX_DWORD,
			get_cond_for_name($2),
			get_op_for_name($1),
			get_reg_for_name($4),
			get_creg_for_name($6),
			emctx
		);
	}

	| OP_FMT_VI SPACE REG32 COMMA INT {  //in the case of CMP this becomes VII
		if(strcmp($1,"CMP")) {
			fprintf(stderr, "Error: Invalid operands for instruction `%s'\n", $1);
			yyerror(emctx, "Failure...");
			exit(-1);
		}

		checkIfValid12Bit($5, emctx);
		emit_fmt_vii(
			PREFIX_IMM,
			COND_ALWAYS,
			get_op_for_name($1),
			get_reg_for_name($3),
			$5,
			emctx
		);
	}
	| OP_FMT_VI CONDITION SPACE REG32 COMMA INT { //in the case of CMP this becomes VII
		if(strcmp($1,"CMP")) {
			fprintf(stderr, "Error: Invalid operands for instruction `%s'\n", $1);
			yyerror(emctx, "Failure...");
			exit(-1);
		}

		checkIfValid12Bit($6, emctx);
		emit_fmt_vii(
			PREFIX_IMM,
			get_cond_for_name($2),
			get_op_for_name($1),
			get_reg_for_name($4),
			$6,
			emctx
		);
	}

	| OP_FMT_V SPACE INT { //in this case OP_FMT_V becomes Format VIII
		emit_fmt_viii(
			PREFIX_IMM,
			COND_ALWAYS,
			get_op_for_name($1),
			SIGNED16($3),
			emctx
		);
	}
	| OP_FMT_V CONDITION SPACE INT { //in this case OP_FMT_V becomes Format VIII
		emit_fmt_viii(
			PREFIX_IMM,
			get_cond_for_name($2),
			get_op_for_name($1),
			SIGNED16($4),
			emctx
		);
	}
	
	| OP_FMT_VIII CONDITION SPACE INT {
		 emit_fmt_viii(
			PREFIX_IMM, 
			get_cond_for_name($2), 
			get_op_for_name($1), 
			$4,
			emctx
		);
	}
	| OP_FMT_VIII SPACE INT {
		 emit_fmt_viii(
			PREFIX_IMM, 
			COND_ALWAYS, 
			get_op_for_name($1), 
			$3,
			emctx
		);
	}

	| OP_S_CMPXCHG SPACE REG32 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_DWORD,
			COND_ALWAYS,
			OP_CMPXCHG,
			get_reg_for_name($3),
			get_reg_for_name($5),
			get_reg_for_name($7),
			emctx
		);
	}
	| OP_S_CMPXCHG CONDITION SPACE REG32 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_DWORD,
			get_cond_for_name($2),
			OP_CMPXCHG,
			get_reg_for_name($4),
			get_reg_for_name($6),
			get_reg_for_name($8),
			emctx
		);
	}
	
	// Load and Store instruction require special treatment because they are
	// allowed to mix reg32 and reg8 etc.

	| OP_S_LOADI SPACE REG8 COMMA INT {
		emit_fmt_iv(
			PREFIX_BYTE,
			COND_ALWAYS,
			OP_LOADI,
			get_reg8_for_name($3),
			$5,
			emctx
		);
	}
	| OP_S_LOADI CONDITION SPACE REG8 COMMA INT {
		emit_fmt_iv(
			PREFIX_BYTE,
			get_cond_for_name($2),
			OP_LOADI,
			get_reg8_for_name($4),
			$6,
			emctx
		);
	}
	| OP_S_LOADI SPACE REG16 COMMA INT {
		emit_fmt_iv(
			PREFIX_WORD,
			COND_ALWAYS,
			OP_LOADI,
			get_reg16_for_name($3),
			$5,
			emctx
		);
	}
	| OP_S_LOADI CONDITION SPACE REG16 COMMA INT {
		emit_fmt_iv(
			PREFIX_WORD,
			get_cond_for_name($2),
			OP_LOADI,
			get_reg16_for_name($4),
			$6,
			emctx
		);
	}

	| OP_S_LOADID SPACE REG32 COMMA REG32 COMMA INT {
		checkIfValid12Bit($7, emctx);
		emit_fmt_iii(
			PREFIX_DWORD,
			COND_ALWAYS,
			OP_LOADID,
			get_reg_for_name($3),
			get_reg_for_name($5),
			$7,
			emctx
		);
	}
	| OP_S_LOADID CONDITION SPACE REG32 COMMA REG32 COMMA INT {
		checkIfValid12Bit($8, emctx);
		emit_fmt_iii(
			PREFIX_DWORD,
			get_cond_for_name($2),
			OP_LOADID,
			get_reg_for_name($4),
			get_reg_for_name($6),
			$8,
			emctx
		);
	}
	| OP_S_LOADID SPACE REG8 COMMA REG32 COMMA INT {
		checkIfValid12Bit($7, emctx);
		emit_fmt_iii(
			PREFIX_BYTE,
			COND_ALWAYS,
			OP_LOADID,
			get_reg8_for_name($3),
			get_reg_for_name($5),
			$7,
			emctx
		);
	}
	| OP_S_LOADID CONDITION SPACE REG8 COMMA REG32 COMMA INT {
		checkIfValid12Bit($8, emctx);
		emit_fmt_iii(
			PREFIX_BYTE,
			get_cond_for_name($2),
			OP_LOADID,
			get_reg8_for_name($4),
			get_reg_for_name($6),
			$8,
			emctx
		);
	}
	| OP_S_LOADID SPACE REG16 COMMA REG32 COMMA INT {
		checkIfValid12Bit($7, emctx);
		emit_fmt_iii(
			PREFIX_WORD,
			COND_ALWAYS,
			OP_LOADID,
			get_reg16_for_name($3),
			get_reg_for_name($5),
			$7,
			emctx
		);
	}
	| OP_S_LOADID CONDITION SPACE REG16 COMMA REG32 COMMA INT {
		checkIfValid12Bit($8, emctx);
		emit_fmt_iii(
			PREFIX_WORD,
			get_cond_for_name($2),
			OP_LOADID,
			get_reg16_for_name($4),
			get_reg_for_name($6),
			$8,
			emctx
		);
	}
	
	| OP_S_LOADRD SPACE REG32 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_DWORD,
			COND_ALWAYS,
			OP_LOADRD,
			get_reg_for_name($3),
			get_reg_for_name($5),
			get_reg_for_name($7),
			emctx
		);
	}
	| OP_S_LOADRD CONDITION SPACE REG32 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_DWORD,
			get_cond_for_name($2),
			OP_LOADRD,
			get_reg_for_name($4),
			get_reg_for_name($6),
			get_reg_for_name($8),
			emctx
		);
	}
	| OP_S_LOADRD SPACE REG8 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_BYTE,
			COND_ALWAYS,
			OP_LOADRD,
			get_reg8_for_name($3),
			get_reg_for_name($5),
			get_reg_for_name($7),
			emctx
		);
	}
	| OP_S_LOADRD CONDITION SPACE REG8 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_BYTE,
			get_cond_for_name($2),
			OP_LOADRD,
			get_reg8_for_name($4),
			get_reg_for_name($6),
			get_reg_for_name($8),
			emctx
		);
	}
	| OP_S_LOADRD SPACE REG16 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_WORD,
			COND_ALWAYS,
			OP_LOADRD,
			get_reg16_for_name($3),
			get_reg_for_name($5),
			get_reg_for_name($7),
			emctx
		);
	}
	| OP_S_LOADRD CONDITION SPACE REG16 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_WORD,
			get_cond_for_name($2),
			OP_LOADRD,
			get_reg8_for_name($4),
			get_reg_for_name($6),
			get_reg16_for_name($8),
			emctx
		);
	}
	
	| OP_S_MOV SPACE REG32 COMMA REG32{
		emit_fmt_vi(
			PREFIX_DWORD,
			COND_ALWAYS,
			get_op_for_name($1),
			get_reg_for_name($3),
			get_reg_for_name($5),
			emctx
		);
	}
	| OP_S_MOV CONDITION SPACE REG32 COMMA REG32 {
		emit_fmt_vi(
			PREFIX_DWORD,
			get_cond_for_name($2),
			get_op_for_name($1),
			get_reg_for_name($4),
			get_reg_for_name($6),
			emctx
		);
	}
	| OP_S_MOV SPACE REG16 COMMA REG16{
		emit_fmt_vi(
			PREFIX_WORD,
			COND_ALWAYS,
			get_op_for_name($1),
			get_reg16_for_name($3),
			get_reg16_for_name($5),
			emctx
		);
	}
	| OP_S_MOV CONDITION SPACE REG16 COMMA REG16 {
		emit_fmt_vi(
			PREFIX_WORD,
			get_cond_for_name($2),
			get_op_for_name($1),
			get_reg16_for_name($4),
			get_reg16_for_name($6),
			emctx
		);
	}
	| OP_S_MOV SPACE REG8 COMMA REG8{
		emit_fmt_vi(
			PREFIX_BYTE,
			COND_ALWAYS,
			get_op_for_name($1),
			get_reg8_for_name($3),
			get_reg8_for_name($5),
			emctx
		);
	}
	| OP_S_MOV CONDITION SPACE REG8 COMMA REG8 {
		emit_fmt_vi(
			PREFIX_BYTE,
			get_cond_for_name($2),
			get_op_for_name($1),
			get_reg8_for_name($4),
			get_reg8_for_name($6),
			emctx
		);
	}
	
	
	| OP_S_SEX CONDITION SPACE REG32 COMMA REG8 {
		emit_fmt_vi(
			PREFIX_BYTE,
			get_cond_for_name($2),
			OP_SEX,
			get_reg_for_name($4),
			get_reg8_for_name($6),
			emctx
		);
	}
	| OP_S_SEX CONDITION SPACE REG32 COMMA REG16 {
		emit_fmt_vi(
			PREFIX_WORD,
			get_cond_for_name($2),
			OP_SEX,
			get_reg_for_name($4),
			get_reg16_for_name($6),
			emctx
		);
	}
	| OP_S_SEX SPACE REG32 COMMA REG8 {
		emit_fmt_vi(
			PREFIX_BYTE,
			COND_ALWAYS,
			OP_SEX,
			get_reg_for_name($3),
			get_reg8_for_name($5),
			emctx
		);
	}
	| OP_S_SEX SPACE REG32 COMMA REG16 {
		emit_fmt_vi(
			PREFIX_WORD,
			COND_ALWAYS,
			OP_SEX,
			get_reg_for_name($3),
			get_reg16_for_name($5),
			emctx
		);
	}
	
	| OP_S_STOREID SPACE REG32 COMMA REG32 COMMA INT {
		checkIfValid12Bit($7, emctx);
		emit_fmt_iii(
			PREFIX_DWORD,
			COND_ALWAYS,
			OP_STOREID,
			get_reg_for_name($3),
			get_reg_for_name($5),
			$7,
			emctx
		);
	}
	| OP_S_STOREID CONDITION SPACE REG32 COMMA REG32 COMMA INT {
		checkIfValid12Bit($8, emctx);
		emit_fmt_iii(
			PREFIX_DWORD,
			get_cond_for_name($2),
			OP_STOREID,
			get_reg_for_name($4),
			get_reg_for_name($6),
			$8,
			emctx
		);
	}
	| OP_S_STOREID SPACE REG32 COMMA REG8 COMMA INT {
		checkIfValid12Bit($7, emctx);
		emit_fmt_iii(
			PREFIX_BYTE,
			COND_ALWAYS,
			OP_STOREID,
			get_reg_for_name($3),
			get_reg8_for_name($5),
			$7,
			emctx
		);
	}
	| OP_S_STOREID CONDITION SPACE REG32 COMMA REG8 COMMA INT {
		checkIfValid12Bit($8, emctx);
		emit_fmt_iii(
			PREFIX_BYTE,
			get_cond_for_name($2),
			OP_STOREID,
			get_reg_for_name($4),
			get_reg8_for_name($6),
			$8,
			emctx
		);
	}

	| OP_S_STOREID SPACE REG32 COMMA REG16 COMMA INT {
		checkIfValid12Bit($7, emctx);
		emit_fmt_iii(
			PREFIX_WORD,
			COND_ALWAYS,
			OP_STOREID,
			get_reg_for_name($3),
			get_reg16_for_name($5),
			$7,
			emctx
		);
	}
	| OP_S_STOREID CONDITION SPACE REG32 COMMA REG16 COMMA INT {
		checkIfValid12Bit($8, emctx);
		emit_fmt_iii(
			PREFIX_WORD,
			get_cond_for_name($2),
			OP_STOREID,
			get_reg_for_name($4),
			get_reg16_for_name($6),
			$8,
			emctx
		);
	}

	| OP_S_STORERD SPACE REG32 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_DWORD,
			COND_ALWAYS,
			OP_STORERD,
			get_reg_for_name($3),
			get_reg_for_name($5),
			get_reg_for_name($7),
			emctx
		);
	}
	
	| OP_S_STORERD SPACE REG32 COMMA REG16 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_WORD,
			COND_ALWAYS,
			OP_STORERD,
			get_reg_for_name($3),
			get_reg16_for_name($5),
			get_reg_for_name($7),
			emctx
		);
	}
	| OP_S_STORERD SPACE REG32 COMMA REG8 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_BYTE,
			COND_ALWAYS,
			OP_STORERD,
			get_reg_for_name($3),
			get_reg8_for_name($5),
			get_reg_for_name($7),
			emctx
		);
	}
	| OP_S_STORERD CONDITION SPACE REG32 COMMA REG32 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_DWORD,
			get_cond_for_name($2),
			OP_STORERD,
			get_reg_for_name($4),
			get_reg_for_name($6),
			get_reg_for_name($8),
			emctx
		);
	}
	| OP_S_STORERD CONDITION SPACE REG32 COMMA REG16 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_WORD,
			get_cond_for_name($2),
			OP_STORERD,
			get_reg_for_name($4),
			get_reg16_for_name($6),
			get_reg_for_name($8),
			emctx
		);
	}
	| OP_S_STORERD CONDITION SPACE REG32 COMMA REG8 COMMA REG32 {
		emit_fmt_ii(
			PREFIX_BYTE,
			get_cond_for_name($2),
			OP_STORERD,
			get_reg_for_name($4),
			get_reg8_for_name($6),
			get_reg_for_name($8),
			emctx
		);
	}

	|OP_S_XCHGC SPACE REG32 COMMA CREG {
		emit_fmt_vi(
			PREFIX_DWORD,
			COND_ALWAYS,
			OP_XCHGC,
			get_reg_for_name($3),
			get_creg_for_name($5),
			emctx
		);
	}
	| OP_S_XCHGC CONDITION SPACE REG32 COMMA CREG {
		emit_fmt_vi(
			PREFIX_DWORD,
			get_cond_for_name($2),
			OP_XCHGC,
			get_reg_for_name($4),
			get_reg8_for_name($6),
			emctx
		);
	}

	| OP_S_STOREAS SPACE REG32 {
		emit_fmt_v(
			PREFIX_DWORD,
			COND_ALWAYS,
			OP_STOREAS,
			get_reg_for_name($3),
			emctx
		);
	}
	| OP_S_LOADAS SPACE REG32 {
		emit_fmt_v(
			PREFIX_DWORD,
			COND_ALWAYS,
			OP_LOADAS,
			get_reg_for_name($3),
			emctx
		);
	}
	

	| OP_PSEUDO_LOADADR SPACE REG32 COMMA IDENT {
		if(get_reg_for_name($3) > CPU_REG_R7) yyerror(emctx, "LOADADR is only possible with WORD accessible registers\n");
		emit_label_ref($5, get_reg_for_name($3), COND_ALWAYS,  emctx);
	}
	| OP_PSEUDO_LOADADR CONDITION SPACE REG32 COMMA IDENT {
		if(get_reg_for_name($4) > CPU_REG_R7) yyerror(emctx, "LOADADR is only possible with WORD accessible registers\n");
		emit_label_ref($6, get_reg_for_name($4), get_cond_for_name($2),  emctx);
	}

	| OP_PSEUDO_LDCALL SPACE REG32 COMMA IDENT {
		if(get_reg_for_name($3) > CPU_REG_R7) yyerror(emctx, "LDCALL is only possible with WORD accessible registers\n");
		emit_label_ref($5, get_reg_for_name($3), COND_ALWAYS, emctx);
		emit_fmt_v(
			PREFIX_DWORD,
			COND_ALWAYS,
			OP_CALL,
			get_reg_for_name($3),
			emctx
		);		
	}
	| OP_PSEUDO_LDCALL CONDITION SPACE REG32 COMMA IDENT {
		if(get_reg_for_name($4) > CPU_REG_R7) yyerror(emctx, "LDCALL is only possible with WORD accessible registers\n");
		emit_label_ref($6, get_reg_for_name($4), get_cond_for_name($2), emctx);
		emit_fmt_v(
			PREFIX_DWORD,
			get_cond_for_name($2),
			OP_CALL,
			get_reg_for_name($4),
			emctx
		);		
	}

	| OP_PSEUDO_LDJMP SPACE REG32 COMMA IDENT {
		if(get_reg_for_name($3) > CPU_REG_R7) yyerror(emctx, "LDJMP is only possible with WORD accessible registers\n");
		emit_label_ref($5, get_reg_for_name($3), COND_ALWAYS, emctx);
		emit_fmt_v(
			PREFIX_DWORD,
			COND_ALWAYS,
			OP_JMP,
			get_reg_for_name($3),
			emctx
		);		
	}
	| OP_PSEUDO_LDJMP CONDITION SPACE REG32 COMMA IDENT {
		if(get_reg_for_name($4) > CPU_REG_R7) yyerror(emctx, "LDJMP is only possible with WORD accessible registers\n");
		emit_label_ref($6, get_reg_for_name($4), get_cond_for_name($2), emctx);
		emit_fmt_v(
			PREFIX_DWORD,
			get_cond_for_name($2),
			OP_JMP,
			get_reg_for_name($4),
			emctx
		);		
	}


	| OP_PSEUDO_PUSH SPACE REG32 {
		byte imm = (get_reg_for_name($3) << 4u) | (get_reg_for_name($3));
		emit_fmt_iv(
			PREFIX_IMM,
			COND_ALWAYS,
			OP_STOREMA,
			get_reg_for_name("SP"),
			imm,
			emctx
		);
	}
	| OP_PSEUDO_PUSH CONDITION SPACE REG32 {
		byte imm = (get_reg_for_name($4) << 4u) | (get_reg_for_name($4));
		emit_fmt_iv(
			PREFIX_IMM,
			get_cond_for_name($2),
			OP_STOREMA,
			get_reg_for_name("SP"),
			imm,
			emctx
		);
	}

	| OP_PSEUDO_POP SPACE REG32 {
		byte imm = (get_reg_for_name($3) << 4u) | (get_reg_for_name($3));
		emit_fmt_iv(
			PREFIX_IMM,
			COND_ALWAYS,
			OP_LOADMA,
			get_reg_for_name("SP"),
			imm,
			emctx
		);
	}
	| OP_PSEUDO_POP CONDITION SPACE REG32 {
		byte imm = (get_reg_for_name($4) << 4u) | (get_reg_for_name($4));
		emit_fmt_iv(
			PREFIX_IMM,
			get_cond_for_name($2),
			OP_LOADMA,
			get_reg_for_name("SP"),
			imm,
			emctx
		);
	}

	| OP_PSEUDO_LOAD32 SPACE REG32 COMMA INT32 {
		byte reg = get_reg_for_name($3) * 2u;

		if(get_reg_for_name($3) > CPU_REG_R7) {
			emit_fmt_vi(
				PREFIX_DWORD,
				COND_ALWAYS,
				OP_XCHG,
				CPU_REG_R7,
				get_reg_for_name($3),
				emctx
			);
			reg = CPU_REG_R7 * 2u;
		}
		
		dword val = $5;
		emit_fmt_iv(
			PREFIX_WORD,
			COND_ALWAYS,
			OP_LOADI,
			reg,
			LWORD_FROM_DWORD(val),
			emctx
		);
		emit_fmt_iv(
			PREFIX_WORD,
			COND_ALWAYS,
			OP_LOADI,
			reg + 1u,
			HWORD_FROM_DWORD(val),
			emctx
		);

		if(get_reg_for_name($3) > CPU_REG_R7) {
			emit_fmt_vi(
				PREFIX_DWORD,
				COND_ALWAYS,
				OP_XCHG,
				CPU_REG_R7,
				get_reg_for_name($3),
				emctx
			);
		}
	}
	| OP_PSEUDO_LOAD32 CONDITION SPACE REG32 COMMA INT32 {
		byte reg = get_reg_for_name($4) * 2u;

		if(get_reg_for_name($4) > CPU_REG_R7) {
			emit_fmt_vi(
				PREFIX_DWORD,
				get_cond_for_name($2),
				OP_XCHG,
				CPU_REG_R7,
				get_reg_for_name($4),
				emctx
			);
			reg = CPU_REG_R7 * 2u;
		}
		
		dword val = $6;
		emit_fmt_iv(
			PREFIX_WORD,
			get_cond_for_name($2),
			OP_LOADI,
			reg,
			LWORD_FROM_DWORD(val),
			emctx
		);
		emit_fmt_iv(
			PREFIX_WORD,
			get_cond_for_name($2),
			OP_LOADI,
			reg + 1u,
			HWORD_FROM_DWORD(val),
			emctx
		);

		if(get_reg_for_name($4) > CPU_REG_R7) {
			emit_fmt_vi(
				PREFIX_DWORD,
				get_cond_for_name($2),
				OP_XCHG,
				CPU_REG_R7,
				get_reg_for_name($4),
				emctx
			);
		}
	}	
	
	| DIRECTIVE_ALIGN INT SPACE INT{
		while(emctx->pos % $2)
			emit_byte($4, emctx);
		printf("%d-aligned with %x to %08x\n", $2, $4, emctx->pos);
	}

	| DIRECTIVE_LABEL IDENT {
		emit_label($2, emctx);
	}

	| DIRECTIVE_OFFSET INT32 {
		emctx->offset = $2;
	}

	| DIRECTIVE_BYTE INT {
		if(($2 > 0xFFu) || ($2 < 0x00u))
			yyerror(emctx, "Invalid byte constant for .byte!");

		emit_byte((byte)$2, emctx);
	}

	| DIRECTIVE_WORD INT {
		emit_byte(LBYTE_FROM_WORD($2), emctx);
		emit_byte(HBYTE_FROM_WORD($2), emctx);
	}

	| DIRECTIVE_DWORD INT32 {
		emit_byte(LLBYTE_FROM_DWORD($2), emctx);
		emit_byte(LHBYTE_FROM_DWORD($2), emctx);
		emit_byte(HLBYTE_FROM_DWORD($2), emctx);
		emit_byte(HHBYTE_FROM_DWORD($2), emctx);
	}

	| DIRECTIVE_AT INT32 {
		emctx->pos = $2;
	}

	| DIRECTIVE_BYTES bytes DIRECTIVE_END {}
	;
%%

void label_resolution(emit_context* emctx) {
	GSList* ptr = emctx->labelrefs;
	printf("Resolving labels [%08x]...\n", (unsigned int)GPOINTER_TO_INT(ptr));
	while(ptr != NULL) {
		label_ref* lr = ptr->data;
		printf("%s at %08x\n", lr->name, lr->pos);
		dword val = emit_label_res(lr->name, emctx);
		byte reg = lr->reg * 2;
		byte cond = lr->cond;
		printf(" resolved to %08x\n", val);
		printf(" Generating LOADI chain to reg %x\n", reg);

		emctx->pos = lr->pos;

		emit_fmt_iv(
			PREFIX_WORD,
			cond,
			OP_LOADI,
			reg,
			LWORD_FROM_DWORD(val),
			emctx
			);
		emit_fmt_iv(
			PREFIX_WORD,
			cond,
			OP_LOADI,
			reg + 1u,
			HWORD_FROM_DWORD(val),
			emctx
		);

		ptr = g_slist_next(ptr);
	}
}

void usage(char* name) {
	printf("Usage\n");
	printf(" %s --asm <input file> --img <output file>\n", name);
	printf("\tOptions:\n");
	printf("\t -f, --fixed-buffer <buffer_size> \t the encoded buffer length is fixed to buffer_size\n");
	printf("\t                                  \t in bytes.\n");
	printf("\t -h, --help \t\t\t\t shows the help\n");
	printf("\t -i, --img <output_file> \t\t writes the bytecode to a file\n");
	printf("\t -v, --asm <input_file> \t\t specifies the assembler file to encode\n");
	printf("\n\n");
}

void write_file(char* filepath, emit_context* ectx){
	FILE *fp;

	fp = fopen(filepath, "w");
	if (fp == NULL) {
		fprintf(stderr, "couldn't open output file.\n");
		exit(1);
	}

	fwrite(ectx->buf, ectx->highest_pos, sizeof(byte), fp);
	fclose(fp);
}

int main(int argc, char* argv[]) {
	puts("vasm - va assembler");
	puts(" (c) Martin Bloechlinger, 2013");
	puts(" (c) Roman Muentener, 2013");
	puts("");

	#if !defined(GLIB_VERSION_2_36)
	  g_type_init();
	#endif

	static struct option long_options[] =
             {
               /* These options set a flag. */
               {"img", required_argument, 0,'i'},
               {"asm", required_argument, 0, 'v'},
               {"help", no_argument, 0, 'h'},
               {"fixed-buffer", required_argument, 0, 'f'},
               {0, 0, 0, 0}
             };
	
	int option_index = 0;
	int error = 0;
	int buffer_size = 0;
	byte fixed_size = 0;
	char* path = NULL;
	char* img = NULL;
     
	while(1){
		int c = getopt_long (argc, argv, "f:hi:v:", long_options, &option_index);
	
		/* Detect the end of the options. */
		if (c == -1)
			break;
		if (c == 0){
			c = long_options[option_index].val;
		}

		switch (c)
		{
			case 'f':
				buffer_size = atoi(optarg);
				fixed_size = 1;
				break;
			case 'h':
				usage(argv[0]);
				exit(0);
				break;
			case 'i':
				img = optarg;
				break;

			case 'v':
				path = optarg;
				break;

			case '?':
				error++;
				break;

			default:
				abort ();
		}
	}
	
	if(error > 0 || argc < 3){
		usage(argv[0]);
		exit(1);
	}

	if((path == NULL) || (img == NULL)) {
		usage(argv[0]);
		exit(1);
	}
	
	if(fixed_size == 0){
		buffer_size = DYNAMIC_BUFFER_SIZE;
	}
	
	FILE * file;
	char line [100];

	file = fopen (path , "r");
	if (file == NULL) {
		perror ("Error opening file");
		exit(-1);
	}

	// file to parse
	yyin = file;

	emit_context emctx = *emit_create_context(buffer_size, fixed_size);

	// Default offset
	emctx.offset = 4096u;

	do {
		yyparse(&emctx);
	} while (!feof(yyin));

	fclose (file);

	label_resolution(&emctx);

	if(img != NULL) {
		write_file(img, &emctx);
	}

	return 0;
}

void checkIfValid12Bit(word immidiateData, emit_context* ectx){
	if((immidiateData & 0xF000) != 0){
		yyerror(ectx, "value not in 12 bit range");
	}
}

void yyerror(emit_context* emctx, const char *s) {
	fprintf(stderr, "Error on line %d: %s\n", yylloc.first_line, s);
	fprintf(stderr, " pos := %08x\n", emctx->pos);
	exit(-1);
}
