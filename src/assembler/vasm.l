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
#define YY_DECL extern int yylex()
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "common/definitions.h"
#include "common/macros.h"
#include <common/lookup.h>
#include "vasm.tab.h"  // automatically generated


/*
 * This function is used to
 * make all characters upper case in order for
 * OpCodes, Registers etc. to be case-insensitive.
 */
char* upper( char *p )
{
	char* o = p;
	while( *p != '\0' ) {
		*p = toupper(*p);
		p++;
	}
	return o;
}

dword hexToDec32(const char *s){
	guint64 parsedLong = g_ascii_strtoull(s, NULL, 0);
	if(parsedLong > UINT32_MAX){
		fprintf(stderr, "Error on line %d: hex value is not in 32bit range: %s \n", yylineno, s);
		exit(1);
	}
	return (dword)parsedLong;
}

word hexToDec(const char *s){
	guint64 parsedLong = g_ascii_strtoull(s, NULL, 0); 
	if(parsedLong > UINT16_MAX){
		fprintf(stderr, "Error on line %d: hex value is not in 16bit range: %s \n", yylineno, s);
		exit(1);
	}
	
	return (word)parsedLong;
}

dword to32BitInt(const char *s){
	gint64 parsedLong = g_ascii_strtoll(s, NULL, 10);
	if(parsedLong > INT32_MAX || parsedLong < INT32_MIN){
		fprintf(stderr, "Error on line %d: value is not in 32bit range: %s \n", yylineno, s);
		exit(1);
	}
	return UNSIGNED32((sdword)parsedLong);
}

word to16BitInt(const char *s){
	gint64 parsedLong = g_ascii_strtoll(s, NULL, 10); 
	if(parsedLong > INT16_MAX || parsedLong < INT16_MIN){
		fprintf(stderr, "Error on line %d: value is not in 16bit range: %s \n", yylineno, s);
		exit(1);
	}
	
	return UNSIGNED16((sword)parsedLong);
}

word reg_range_to16bit_int(const char *s){
	char* str = strdup(s);
	char* start_reg = strtok(str,":");
	char* end_reg = strtok(NULL, ":");
	byte start_reg_val = get_reg_for_name(start_reg);
	byte end_reg_val = get_reg_for_name(end_reg);
	word reg_range_val = 0x00;
	reg_range_val |= start_reg_val;
	reg_range_val = reg_range_val << 4;
	reg_range_val |= end_reg_val;
	return reg_range_val;
	
}

char* remove32BitEscapeSign(char* s){
	if(s[0] == '@'){
		return &s[1];
	}else{
		return &s[0];
	}
}

%}

/* Registers 32bit */
REG32			R0|R1|R2|R3|R4|R5|R6|R7|R8|R9|RA|RB|RC|BP|SP|FLGS
/* Registers 16bit */
REG16			R0L|R0H|R1L|R1H|R2L|R2H|R3L|R3H|R4L|R4H|R5L|R5H|R6L|R6H|R7L|R7H
/* Registers 8 bit */
REG8			R0LL|R0LH|R0HL|R0HH|R1LL|R1LH|R1HL|R1HH|R2LL|R2LH|R2HL|R2HH|R3LL|R3LH|R3HL|R3HH
/* Control registers */
CREG			MR1|MR2|MR3|MR4|CR1|CR2|SSP|ST1|ST2|ST3|ST4


OP_FMT_I		IRET|STOP
OP_FMT_II		ADD|AND|DIV|OR|MOD|MUL|SAR|SDIV|SHL|SHR|SMUL|SUB|XOR
OP_FMT_IV		LOADMA|STOREMA
OP_FMT_V		CALL|JMP|TEST
OP_FMT_VI		CMP|IN|NOT|OUT|XCHG
OP_FMT_VIC		MOVTC|MOVFC
OP_FMT_VIII		FAIL|INT
OP_PSEUDO_LOADADR	LOADADR
OP_PSEUDO_LDCALL	LDCALL
OP_PSEUDO_LDJMP		LDJMP
OP_PSEUDO_PUSH		PUSH
OP_PSEUDO_POP		POP
OP_PSEUDO_LOAD32	LOAD32
OP_S_CMPXCHG		CMPXCHG
OP_S_LOADI		LOADI
OP_S_LOADID		LOADID
OP_S_LOADRD		LOADRD
OP_S_MOV		MOV
OP_S_STOREID		STOREID
OP_S_STORERD		STORERD
OP_S_SEX		SEX
OP_S_XCHGC		XCHGC
OP_S_STOREAS		STOREAS
OP_S_LOADAS		LOADAS
CONDITION		NZNC|NZNS|IZ|NZ|IS|NS|IO|NO|IP|NP|IZIS|IZIC|IC|NC|NF
D_ALIGN			\.align[ \t]+
D_LABEL			\.label[ \t]+
D_OFFSET		\.offset[ \t]+
D_BYTE			\.byte[ \t]+
D_WORD			\.word[ \t]+
D_DWORD			\.dword[ \t]+
D_AT			\.at[ \t]+

HEX_32BIT		(0x[0-9A-F]{8})|(@0x[0-9A-F]+)
HEX			0x[0-9A-F]+
%%
\n|\r\n			{ return ENDL; }
[\t ]*			{ return SPACE; }

{HEX_32BIT}		{ yylval.ival32 = hexToDec32(remove32BitEscapeSign(yytext)); return INT32;}
{HEX}			{ yylval.ival = hexToDec(yytext); return INT;}
[\-]*[0-9]+		{ yylval.ival = to16BitInt(yytext); return INT; }
{REG32}:{REG32}		{ yylval.ival = reg_range_to16bit_int(yytext); return INT; }
@[\-]*[0-9]+		{ yylval.ival32 = to32BitInt(remove32BitEscapeSign(yytext)); return INT32; }
[ \t\n]*,[ \t\n]*	{ return COMMA; }
{CREG}			{ yylval.sval = upper(strdup(yytext)); return CREG; }

{REG8}			{ yylval.sval = upper(strdup(yytext)); return REG8; }
{REG16}			{ yylval.sval = upper(strdup(yytext)); return REG16; }
{REG32}			{ yylval.sval = upper(strdup(yytext)); return REG32; }	
{OP_FMT_I}		{ yylval.sval = upper(strdup(yytext)); return OP_FMT_I; }
{OP_FMT_II}		{ yylval.sval = upper(strdup(yytext)); return OP_FMT_II; }
{OP_FMT_IV}		{ yylval.sval = upper(strdup(yytext)); return OP_FMT_IV; }
{OP_FMT_V}		{ yylval.sval = upper(strdup(yytext)); return OP_FMT_V; }
{OP_FMT_VI}		{ yylval.sval = upper(strdup(yytext)); return OP_FMT_VI; }
{OP_FMT_VIC}		{ yylval.sval = upper(strdup(yytext)); return OP_FMT_VIC; }
{OP_FMT_VIII}		{ yylval.sval = upper(strdup(yytext)); return OP_FMT_VIII; }
{OP_S_CMPXCHG}		{ yylval.sval = upper(strdup(yytext)); return OP_S_CMPXCHG; }
{OP_S_LOADI}		{ yylval.sval = upper(strdup(yytext)); return OP_S_LOADI; }
{OP_S_LOADID}		{ yylval.sval = upper(strdup(yytext)); return OP_S_LOADID; }
{OP_S_LOADRD}		{ yylval.sval = upper(strdup(yytext)); return OP_S_LOADRD; } 
{OP_S_MOV}		{ yylval.sval = upper(strdup(yytext)); return OP_S_MOV; }
{OP_S_SEX}		{ yylval.sval = upper(strdup(yytext)); return OP_S_SEX; }
{OP_S_STOREID}		{ yylval.sval = upper(strdup(yytext)); return OP_S_STOREID; }
{OP_S_STORERD}		{ yylval.sval = upper(strdup(yytext)); return OP_S_STORERD; }
{OP_S_XCHGC}		{ yylval.sval = upper(strdup(yytext)); return OP_S_XCHGC; }
{OP_S_STOREAS}		{ yylval.sval = upper(strdup(yytext)); return OP_S_STOREAS; }
{OP_S_LOADAS}		{ yylval.sval = upper(strdup(yytext)); return OP_S_LOADAS; }
{OP_PSEUDO_LOADADR}	{ yylval.sval = upper(strdup(yytext)); return OP_PSEUDO_LOADADR; }
{OP_PSEUDO_LDCALL}	{ yylval.sval = upper(strdup(yytext)); return OP_PSEUDO_LDCALL; }
{OP_PSEUDO_LDJMP}	{ yylval.sval = upper(strdup(yytext)); return OP_PSEUDO_LDJMP; }
{OP_PSEUDO_PUSH}	{ yylval.sval = upper(strdup(yytext)); return OP_PSEUDO_PUSH; }
{OP_PSEUDO_POP}		{ yylval.sval = upper(strdup(yytext)); return OP_PSEUDO_POP; }
{OP_PSEUDO_LOAD32}	{ yylval.sval = upper(strdup(yytext)); return OP_PSEUDO_LOAD32; }
{D_ALIGN}		{ return DIRECTIVE_ALIGN; }
{D_LABEL}		{ return DIRECTIVE_LABEL; }
{D_OFFSET}		{ return DIRECTIVE_OFFSET; }
{D_BYTE}		{ return DIRECTIVE_BYTE; }
{D_WORD}		{ return DIRECTIVE_WORD; }
{D_DWORD}		{ return DIRECTIVE_DWORD; }
{D_AT}			{ return DIRECTIVE_AT; }
\.bytes			{ return DIRECTIVE_BYTES; }
\.end			{ return DIRECTIVE_END; }
{CONDITION}		{ yylval.sval = upper(strdup(yytext)); return CONDITION; }
;(.*)\n			{ /* comment */ }
_[a-z_]+[0-9a-zA-Z_]*	{ yylval.sval = upper(strdup(yytext)); return IDENT; }
.			{ return GARBAGE; }
%%
