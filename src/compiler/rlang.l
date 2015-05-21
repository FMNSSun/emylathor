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
#include <stdint.h>
#include <string.h>
#include "glib.h"
#include "ops.h"
#include "rlang.tab.h"  // automatically generated

%}

%%

\n|\r\n					{ return ENDL; }
[\t\r\n ]*#(.*)\n			{}
[\t\r\n ]*\?\?(.*)[\n]+[\t\r\n ]*	{}
[\t\r\n ]*[\-]+[0-9]+[\t\r\n ]*		{ yylval.int_value = (int32_t)g_ascii_strtoll(yytext, NULL, 10); return INT; }
[\t\r\n ]*[0-9]+[\t\r\n ]*		{ yylval.int_value = (uint32_t)g_ascii_strtoull(yytext, NULL, 10); return INT; }
[\t\r\n ]*PROCEDURE[\t\r\n ]*		{ return KEYWORD_PROCEDURE; }
[\t\r\n ]*IPROCEDURE[\t\r\n ]*		{ return KEYWORD_IPROCEDURE; }
[\t\r\n ]*PARAMETERS[\t\r\n ]*		{ return KEYWORD_PARAMETERS; }
[\t\r\n ]*LOCALS[\t\r\n ]*		{ return KEYWORD_LOCALS; }
[\t\r\n ]*IS[\t\r\n ]*			{ return KEYWORD_IS; }
[\t\r\n ]*END[\t\r\n ]*			{ return KEYWORD_END; }
[\t\r\n ]*RETURN[\t\r\n ]*		{ return KEYWORD_RETURN; }
[\t\r\n ]*IF[\t\r\n ]*			{ return KEYWORD_IF; }
[\t\r\n ]*IFE[\t\r\n ]*			{ return KEYWORD_IFE; }
[\t\r\n ]*ELSE[\t\r\n ]*		{ return KEYWORD_ELSE; }
[\t\r\n ]*THEN[\t\r\n ]*		{ return KEYWORD_THEN; }
[\t\r\n ]*WHILE[\t\r\n ]*		{ return KEYWORD_WHILE; }
[\t\r\n ]*LOOP[\t\r\n ]*		{ return KEYWORD_LOOP; }
[\t\r\n ]*REG[\t\r\n ]*			{ return KEYWORD_REG; }
[\t\r\n ]*BYTE[\t\r\n ]*		{ return KEYWORD_BYTE; }
[\t\r\n ]*NONE[\t\r\n ]*		{ return KEYWORD_NONE; }
[\t\r\n ]*FAIL[\t\r\n ]*		{ return KEYWORD_FAIL; }
[\t\r\n ]*STOP[\t\r\n ]*		{ return KEYWORD_STOP; }
[\t\r\n ]*STATIC[\t\r\n ]*		{ return KEYWORD_STATIC; }
[\t\r\n ]*BYTES[\t\r\n ]*		{ return KEYWORD_BYTES; }
[\t\r\n ]*REGS[\t\r\n ]*		{ return KEYWORD_REGS; }
[\t\r\n ]*LABEL[\t\r\n ]*		{ return KEYWORD_LABEL; }
[\t\r\n ]*GOTO[\t\r\n ]*		{ return KEYWORD_GOTO; }
[\t\r\n ]*STRING[\t\r\n ]*		{ return KEYWORD_STRING; }
[\t\r\n ]*SIZEOF[\t\r\n ]*		{ return KEYWORD_SIZEOF; }
[\t\r\n ]*WIDTHOF[\t\r\n ]*		{ return KEYWORD_WIDTHOF; }
[\t\r\n ]*SAVE[\t\r\n ]*		{ return KEYWORD_SAVE; }
[\t\r\n ]*RESTORE[\t\r\n ]*		{ return KEYWORD_RESTORE; }
[\t\r\n ]*CHECKED[\t\r\n ]*		{ return KEYWORD_CHECKED; }
[\t\r\n ]*OVERFLOW[\t\r\n ]*		{ return KEYWORD_OVERFLOW; }
[\t\r\n ]*\$IRET[\t\r\n ]*		{ return KEYWORD_IRET; }
[\t\r\n ]*\$TOCTRL[\t\r\n ]*		{ return KEYWORD_TOCTRL; }
[\t\r\n ]*\$FROMCTRL[\t\r\n ]*		{ return KEYWORD_FROMCTRL; }
[\t\r\n ]*\$INPORT[\t\r\n ]*		{ return KEYWORD_INPORT; }
[\t\r\n ]*\$OUTPORT[\t\r\n ]*		{ return KEYWORD_OUTPORT; }
[\t\r\n ]*ASM~[\t\r\n ](.*)\n		{ yylval.str_value = (strdup(strchr(yytext, 126)+1)); return ASM; }
[\t\r\n ]*\([\t\r\n ]*			{ return PAREN_LEFT; }
[\t\r\n ]*\)[\t\r\n ]*			{ return PAREN_RIGHT; }
[\t\r\n ]*\[[\t\r\n ]*			{ return BRACKET_LEFT; }
[\t\r\n ]*\][\t\r\n ]*			{ return BRACKET_RIGHT; }
[\t\r\n ]*@[\t\r\n ]*			{ return AT; }
[\t\r\n ]*:[\t\r\n ]*			{ return COLON; }
[\t\r\n ]*`[\t\r\n ]*			{ return BACKTICK; }
[\t\r\n ]*;[\t\r\n ]*			{ return SEMICOLON; }
[\t\r\n ]*\.[\t\r\n ]*			{ return DOT; }
[a-z_]+[a-zA-Z0-9]*			{ yylval.str_value = (strdup(yytext)); return IDENT; }	
[\t\r\n ]*:=[\t\r\n ]*			{ return OP_ASSIGN; }
[\t\r\n ]*\+=[\t\r\n ]*			{ yylval.int_value = RL_UNYOP_PEQ; return OP_UNY; }
[\t\r\n ]*\-=[\t\r\n ]*			{ yylval.int_value = RL_UNYOP_SEQ; return OP_UNY; }
[\t\r\n ]*\/=[\t\r\n ]*			{ yylval.int_value = RL_UNYOP_DEQ; return OP_UNY; }
[\t\r\n ]*\*=[\t\r\n ]*			{ yylval.int_value = RL_UNYOP_MEQ; return OP_UNY; }
[\t\r\n ]*&=[\t\r\n ]*			{ yylval.int_value = RL_UNYOP_AEQ; return OP_UNY; }
[\t\r\n ]*\|=[\t\r\n ]*			{ yylval.int_value = RL_UNYOP_OEQ; return OP_UNY; }
[\t\r\n ]*\\=[\t\r\n ]*			{ yylval.int_value = RL_UNYOP_XEQ; return OP_UNY; }
[\t\r\n ]*NOT[\t\r\n ]*			{ yylval.int_value = RL_UNYOP_L_NOT; return OP_UNY2; }
[\t\r\n ]*~~[\t\r\n ]*			{ yylval.int_value = RL_UNYOP_NOT; return OP_UNY2; }
[\t\r\n ]*\+[\t\r\n ]*			{ yylval.int_value = RL_BINOP_ADD; return OP_BIN; }
[\t\r\n ]*\-[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SUB; return OP_BIN; }
[\t\r\n ]*\*[\t\r\n ]*			{ yylval.int_value = RL_BINOP_MUL; return OP_BIN; }
[\t\r\n ]*\/[\t\r\n ]*			{ yylval.int_value = RL_BINOP_DIV; return OP_BIN; }
[\t\r\n ]*S\+[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SADD; return OP_BIN; }
[\t\r\n ]*S\-[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SSUB; return OP_BIN; }
[\t\r\n ]*S\*[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SMUL; return OP_BIN; }
[\t\r\n ]*S\/[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SDIV; return OP_BIN; }
[\t\r\n ]*==[\t\r\n ]*			{ yylval.int_value = RL_BINOP_EQU; return OP_BIN; }
[\t\r\n ]*!=[\t\r\n ]*			{ yylval.int_value = RL_BINOP_NEQ; return OP_BIN; }
[\t\r\n ]*S==[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SEQU; return OP_BIN; }
[\t\r\n ]*S!=[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SNEQ; return OP_BIN; }
[\t\r\n ]*<<[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SHL; return OP_BIN; }
[\t\r\n ]*>>[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SHR; return OP_BIN; }
[\t\r\n ]*\\\\[\t\r\n ]*		{ yylval.int_value = RL_BINOP_XOR; return OP_BIN; }
[\t\r\n ]*\|\|[\t\r\n ]*		{ yylval.int_value = RL_BINOP_OR; return OP_BIN; }
[\t\r\n ]*&&[\t\r\n ]*			{ yylval.int_value = RL_BINOP_AND; return OP_BIN; }
[\t\r\n ]*AND[\t\r\n ]*			{ yylval.int_value = RL_BINOP_L_AND; return OP_BIN; }
[\t\r\n ]*OR[\t\r\n ]*			{ yylval.int_value = RL_BINOP_L_OR; return OP_BIN; }
[\t\r\n ]*<[\t\r\n ]*			{ yylval.int_value = RL_BINOP_LTH; return OP_BIN; }
[\t\r\n ]*>[\t\r\n ]*			{ yylval.int_value = RL_BINOP_GTH; return OP_BIN; }
[\t\r\n ]*<=[\t\r\n ]*			{ yylval.int_value = RL_BINOP_LEQ; return OP_BIN; }
[\t\r\n ]*>=[\t\r\n ]*			{ yylval.int_value = RL_BINOP_GEQ; return OP_BIN; }
[\t\r\n ]*S<[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SLTH; return OP_BIN; }
[\t\r\n ]*S>[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SGTH; return OP_BIN; }
[\t\r\n ]*S<=[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SLEQ; return OP_BIN; }
[\t\r\n ]*S>=[\t\r\n ]*			{ yylval.int_value = RL_BINOP_SGEQ; return OP_BIN; }
[\t\r\n ]*'(.*)'[\t\r\n ]* 		{ yylval.str_value = strdup(strchr(yytext, 39)); return STRING; }
[\t\r\n ]*\|[\t\r\n ]*			{ return STROKE; }
[\t\r\n ]*				{ return SPACE; }
.					{ return GARBAGE; }

%%
