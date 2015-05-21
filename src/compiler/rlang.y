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
#include <stdint.h>
#include "glib.h"
#include "gio/gio.h"
#include "ops.h"
#include "rlang.tab.h"  // automatically generated

// extern flex stuff
extern int yylex();
extern  int yyparse();
extern FILE *yyin;

typedef struct {
  char*  symbols[256];
  int symtbl[256];
  char* records[1024];
  int rectbl[1024];
  int cur_records;
  int cur_locals;
  int label;
  int labelstack_index;
  int labelstack[256];
  int stacksize_locals;
  int stacksize_parameters;
  int fails;
} rlang_context;

void yyerror(rlang_context* ctx, const char *s);

int find_local(rlang_context* ctx, char* symbol);
void emit_binop(int op);
void emit_unyop(int op);

%}

%locations
%error-verbose

%parse-param { rlang_context* ctx }

%union {
	uint8_t byte_value;
	uint32_t uint_value;
	int32_t int_value;
	char* str_value;
}

%token <byte_value> BYTE;
%token <uint_value> UINT;
%token <int_value> INT;
%token <str_value> IDENT
%token <str_value> STRING;
%token ENDL;
%token SPACE;

%token KEYWORD_PROCEDURE;
%token KEYWORD_IPROCEDURE;
%token KEYWORD_PARAMETERS;
%token KEYWORD_LOCALS;
%token KEYWORD_IS;
%token KEYWORD_END;
%token KEYWORD_RETURN;
%token KEYWORD_IF;
%token KEYWORD_IFE;
%token KEYWORD_ELSE;
%token KEYWORD_THEN;
%token KEYWORD_WHILE;
%token KEYWORD_LOOP;
%token KEYWORD_REG;
%token KEYWORD_BYTE;
%token KEYWORD_NONE;
%token KEYWORD_FAIL;
%token KEYWORD_STOP;
%token KEYWORD_STATIC;
%token KEYWORD_LABEL;
%token KEYWORD_GOTO;
%token KEYWORD_STRING;
%token KEYWORD_SIZEOF;
%token KEYWORD_WIDTHOF;
%token KEYWORD_RECORD;
%token KEYWORD_BYTES;
%token KEYWORD_REGS;
%token KEYWORD_TOCTRL;
%token KEYWORD_FROMCTRL;
%token KEYWORD_IRET;
%token KEYWORD_INPORT;
%token KEYWORD_OUTPORT;
%token KEYWORD_RESTORE;
%token KEYWORD_SAVE;
%token KEYWORD_CHECKED;
%token KEYWORD_OVERFLOW;

%token PAREN_LEFT;
%token PAREN_RIGHT;
%token BRACKET_LEFT;
%token BRACKET_RIGHT;
%token AT;
%token COLON;
%token BACKTICK;
%token SEMICOLON;
%token STROKE;
%token DOT;

%token OP_ASSIGN;
%token <int_value> OP_UNY;
%token <int_value> OP_UNY2;
%token <int_value> OP_BIN;

%token <str_value> TYPE;

%token <str_value> ASM;

%token GARBAGE;

%%

declarations:
  decl | procedure | declarations procedure | declarations decl
 ;

decl:
  KEYWORD_STATIC IDENT OP_ASSIGN INT SEMICOLON {
	printf(".align 4 0\n");
	printf(".label _%s\n", $2);
	printf(".dword @0x%08x\n", $4);
  }
  | KEYWORD_STATIC KEYWORD_BYTE IDENT OP_ASSIGN INT SEMICOLON {
	printf(".align 4 0\n");
	printf(".label _%s\n", $3);
	if(($5 > 255) || ($5 < 0)) {
		yyerror(ctx, "Literal too large for BYTE!");
	}
	printf(".byte 0x%02x\n", $5);
  }
  | KEYWORD_STATIC KEYWORD_STRING IDENT { 
	printf(".align 4 0\n");
	printf(".label _%s\n", $3); 
  } OP_ASSIGN strings SEMICOLON {
	printf(".byte 0x00\n");	
  }
  | KEYWORD_STATIC KEYWORD_BYTES IDENT INT SEMICOLON {
	printf(".align 4 0\n");
	printf(".label _%s\n", $3);
	for(int i = 0; i < $4; i++)
		printf(".byte 0x00\n");
  }
  | KEYWORD_STATIC KEYWORD_REGS IDENT INT SEMICOLON {
	printf(".align 4 0\n");
	printf(".label _%s\n", $3);
	for(int i = 0; i < $4; i++)
		printf(".dword 0x00\n");
  }
  | KEYWORD_STATIC KEYWORD_BYTES COLON IDENT {
	printf(".align 4 0\n");
	printf(".label _%s\n", $4);
  } COLON bytes SEMICOLON { }
  | KEYWORD_STATIC KEYWORD_REGS COLON IDENT {
	printf(".align 4 0\n");
	printf(".label _%s\n", $4);
  } COLON regs SEMICOLON { }

 ;


bytes:
  byte | bytes byte
 ;

regs:
  reg | regs reg
 ;

byte: 
  INT {
	if(($1 > 255) || ($1 < 0)) {
		yyerror(ctx, "Literal too large for BYTE!");
	}
	printf(".byte 0x%02x\n", $1);
  }
 ;

reg:
  INT {
	printf(".dword @0x%08x\n", $1);
  }
 ;

strings:
  string | strings string
 ;

string:
  INT {
	if(($1 > 255) || ($1 < 0)) {
		yyerror(ctx, "Literal too large for BYTE!");
	}
	#ifdef TARGET_C
	printf(" = %d;\n", $1);
	#else
	printf(".byte %02x\n", $1);
	#endif
  }
  | STRING {
	for(int i = 1; i < strlen($1) - 1; i++) {
		unsigned char b = $1[i];
		printf(".byte 0x%02x\n", b);
	}
  }
 ;

procedure:
  ENDL { }
  | KEYWORD_IPROCEDURE IDENT { printf(".label _%s; iproc\n", $2); } KEYWORD_IS statements KEYWORD_END {

  }
  | KEYWORD_PROCEDURE IDENT {  //a b c local bp rc
	#ifdef TARGET_C
	printf("_%s:\n", $2);
	#else
	printf(".align 4 0\n");
	printf(".label _%s\n", $2);
	#endif
	ctx->cur_locals = 0;
	ctx->stacksize_parameters = 0; 
	ctx->stacksize_locals = 0;
    } PAREN_LEFT parameters PAREN_RIGHT {
    }
    KEYWORD_LOCALS PAREN_LEFT locals PAREN_RIGHT KEYWORD_IS  {

	//We have to adjust the positions
	for(int i = 0; i < ctx->stacksize_parameters / 4; i++) {
		(ctx->symtbl)[i] = 8 + ctx->stacksize_parameters + ctx->stacksize_locals - (i * 4);
	}
	for(int i = ctx->stacksize_parameters / 4; i < ctx->cur_locals; i++) {
		(ctx->symtbl)[i] = 12 + ((i - (ctx->stacksize_parameters / 4)) * 4);
	}

	/* printf("; stacksize_parameters = %d, stacksize_locals = %d\n",
		ctx->stacksize_parameters,
		ctx->stacksize_locals); */

	#ifdef TARGET_C
	printf("\tSP -= %d;\n", ctx->stacksize_locals);
	printf("\tPUSH(BP); PUSH(RC);\n");
	printf("\tBP = SP;\n");
	#else
	printf("\tsub sp, sp, %d\n", ctx->stacksize_locals);
	printf("\tpush bp\n");
	printf("\tpush rc\n");
	printf("\tmov bp, sp\n");
	#endif
    }
    statements
    KEYWORD_END {
	//printf(";---\n");
    }
  
 ;

call:
  IDENT BRACKET_LEFT cexpressions BRACKET_RIGHT {
	#ifdef TARGET_C
	int l = ctx->label++;
	printf("\tRC = (reg_t)(void*)&&_return_%d;\n", l);
	//printf("\tprintf(\"RC = %%d\\n\\n\", RC);\n");
	//printf("\tprintf(\"calling %%s\\n\\n\", \"%s\");\n", $1);
	printf("\tgoto _%s;\n", $1);
	printf("\t_return_%d:\n", l);
	//printf("\tprintf(\"returned from %s\\n\\n\");\n", $1);
	#else
    	printf("\tldcall r0, _%s\n", $1);
	#endif
  }
 ;

cexpressions:
  cexpressions exp SEMICOLON | exp SEMICOLON | KEYWORD_NONE
 ;

expression:
  exp | expression exp
 ;

exp:
  ENDL {}
  | SPACE {}
  | call {
	#ifdef TARGET_C
	printf("\tPUSH(R0); //watatata\n");
	#else
	printf("\tpush r0\n");
	#endif
  }
  | IDENT {
	int la = find_local(ctx, $1);
	#ifdef TARGET_C
	printf("\tR0 = *((reg_t*)BP + %d); //var %s\n", la / 4, $1);
	printf("\tPUSH(R0);\n");
	#else
	printf("\tloadid r0, bp, %d ; var %s\n", la, $1);
	printf("\tpush r0\n");
	#endif
  }
  | INT {
	#ifdef TARGET_C
	printf("\tR0 = %d;\n", $1);
	printf("\tPUSH(R0);\n");
	#else
	printf("\tload32 r0, @0x%08x\n", $1);
	printf("\tpush r0\n");
	#endif
  }
  | KEYWORD_CHECKED expression {
	printf("\tmov r7, flgs\n");
  }
  | KEYWORD_OVERFLOW {
	printf("\tmov r0, r7\n");
	printf("\tshr r0, r0, 1\n");
	printf("\tand r0, r0, 3\n");
	printf("\ttest r0\n");
	printf("\txor r0, r0, r0\n");
	printf("\taddnz r0, r0, 1\n");
	printf("\tpush r0\n");
  }
  | KEYWORD_SIZEOF KEYWORD_REG {
	#ifdef TARGET_C
	printf("\tR0 = 4;\n");
	printf("\tPUSH(R0);\n");
	#else
	printf("\tload32 r0, @4\n");
	printf("\tpush r0\n");
	#endif
  }
  | KEYWORD_SIZEOF KEYWORD_BYTE {
	#ifdef TARGET_C
	printf("\tR0 = 1;\n");
	printf("\tPUSH(R0);\n");
	#else
	printf("\tload32 r0, @1\n");
	printf("\tpush r0\n");
	#endif
  }
  | KEYWORD_WIDTHOF KEYWORD_REG {
	#ifdef TARGET_C
	printf("\tR0 = 32;\n");
	printf("\tPUSH(R0);\n");
	#else
	printf("\tload32 r0, @32\n");
	printf("\tpush r0\n");
	#endif
  }
  | KEYWORD_WIDTHOF KEYWORD_BYTE {
	#ifdef TARGET_C
	printf("\tR0 = 8;\n");
	printf("\tPUSH(R0);\n");
	#else
	printf("\tload32 r0, @8\n");
	printf("\tpush r0\n");
	#endif
  }
  | PAREN_LEFT OP_UNY2 expression PAREN_RIGHT {
	#ifdef TARGET_C
	printf("\tPOP(R0);\n");
	#else
	printf("\tpop r0\n");
	#endif

	emit_unyop($2);

	#ifdef TARGET_C
	printf("\tPUSH(R0);\n");
	#else
	printf("\tpush r0\n");
	#endif
  }
  | PAREN_LEFT expression OP_BIN expression PAREN_RIGHT {
	#ifdef TARGET_C
	printf("\tPOP(R1);\n");
	printf("\tPOP(R0);\n");
	#else
	printf("\tpop r1\n");
	printf("\tpop r0\n");
	#endif

	emit_binop($3);	

	#ifdef TARGET_C
	printf("\tPUSH(R0);\n");
	#else
	printf("\tpush r0\n");
	#endif
  }
  | PAREN_LEFT AT KEYWORD_REG COLON expression PAREN_RIGHT {
	#ifdef TARGET_C
	printf("\tPOP(R0);\n");
	printf("\tR0 = *((reg_t*)R0);\n");
	printf("\tPUSH(R0);\n");
	#else
	printf("\tpop r0\n");
	printf("\tloadid r0, r0, 0\n");
	printf("\tpush r0\n");
	#endif
  }
  | PAREN_LEFT AT KEYWORD_REG COLON expression STROKE expression PAREN_RIGHT {
	#ifdef TARGET_C
	printf("\tPOP(R2);\n");
	printf("\tPOP(R0);\n");
	printf("\tR0 = *((reg_t*)R0 + R2);\n");
	printf("\tPUSH(R0);\n");
	#else
	printf("\tpop r2\n");
	printf("\tpop r0\n");
	printf("\tmul r2, r2, 4\n");
	printf("\tadd r0, r0, r2\n");
	printf("\tloadid r0, r0, 0\n");
	printf("\tpush r0\n");
	#endif
  }
  | PAREN_LEFT AT KEYWORD_BYTE COLON expression STROKE expression PAREN_RIGHT {
	#ifdef TARGET_C
	printf("\tPOP(R2);\n");
	printf("\tPOP(R0);\n");
	printf("\tR0 = (*((byte_t*)R0 + R2)) & BYTE_MASK;\n");
	printf("\tPUSH(R0);\n");
	#else
	printf("\tpop r2\n");
	printf("\tpop r0\n");
	printf("\tadd r0, r0, r2\n");
	printf("\txor r1, r1, r1\n");
	printf("\tloadid r1ll, r0, 0\n");
	printf("\tpush r1\n");
	#endif
  }
  | PAREN_LEFT AT KEYWORD_BYTE COLON expression PAREN_RIGHT {
	#ifdef TARGET_C
	printf("\tPOP(R0);\n");
	printf("\tR0 = (*((byte_t*)R0)) & BYTE_MASK;\n");
	printf("\tPUSH(R0);\n");
	#else
	printf("\tpop r0\n");
	printf("\txor r1, r1, r1\n");
	printf("\tloadid r1ll, r0, 0\n");
	printf("\tpush r1\n");
	#endif
  }
  | BACKTICK IDENT {
	int la = find_local(ctx, $2);
	#ifdef TARGET_C
	printf("\tR0 = BP;\n");
	printf("\tR0 += %d;\n", la);
	printf("\tPUSH(R0);\n");
	#else
	printf("\tmov r0, bp\n");
	printf("\tadd r0, r0, %d\n", la);
	printf("\tpush r0\n");
	#endif
  }
  | DOT IDENT {
	#ifdef TARGET_C
	printf("\tR0 = &&_%s;\n", $2);
	printf("\tPUSH(R0);\n");
	#else
	printf("\tloadadr r0, _%s ; DOT\n", $2);
	printf("\tpush r0\n");	
	#endif
  }
 ;

statements:
  statement | statements statement
 ;

statement:
  ENDL {}
  | SPACE {}
  | call SEMICOLON {}
  | KEYWORD_NONE SEMICOLON {}
  | KEYWORD_RESTORE SEMICOLON {
	printf("mov r0, r8\n");
	printf("mov r1, r9\n");
	printf("mov r7, ra\n");
	printf("mov r2, rb\n");
  }
  | KEYWORD_SAVE SEMICOLON {
	printf("mov r8, r0\n");
	printf("mov r9, r1\n");
	printf("mov ra, r7\n");
	printf("mov rb, r2\n");
  }
  | KEYWORD_GOTO IDENT SEMICOLON {
	#ifdef TARGET_C
	printf("\tgoto _%s;\n", $2);
	#else
	printf("\tldjmp r0, _%s\n", $2);
	#endif
  }
  | KEYWORD_GOTO COLON expression SEMICOLON {
	printf("\tpop r0\n");
	printf("\tjmp r0\n");
  }
  | KEYWORD_LABEL IDENT SEMICOLON {
	#ifdef TARGET_C
	printf("_%s:\n", $2);
	#else
	printf(".label _%s\n", $2);
	#endif
  }
  | KEYWORD_FAIL SEMICOLON {
	#ifdef TARGET_C
	printf("\treturn %d;\n", ++ctx->fails);
	#else
	printf("\tfail 0x%02x\n", ++ctx->fails);
	#endif
  }
  | KEYWORD_STOP SEMICOLON {
	#ifdef TARGET_C
	printf("\treturn 0;\n");
	#else
	printf("\tstop\n");
	#endif
  }
  | KEYWORD_RETURN expression SEMICOLON {
	#ifdef TARGET_C
	printf("\tPOP(R0);\n");
	printf("\tSP = BP;\n");
	printf("\tPOP(RC);\n");
	printf("\tPOP(BP);\n");
	printf("\tSP += %d;\n", ctx->stacksize_locals);
	printf("\tSP += %d;\n", ctx->stacksize_parameters);
	//printf("\tprintf(\"trying to return to %%d\\n\\n\", RC);");
	printf("\tgoto *(void*)RC;\n");
	#else
	printf("\tpop r0\n");
	printf("\tmov sp, bp\n");
	printf("\tpop rc\n");
	printf("\tpop bp\n");
	printf("\tadd sp, sp, %d\n", ctx->stacksize_locals);
	printf("\tadd sp, sp, %d\n", ctx->stacksize_parameters);
	printf("\tjmp rc\n");
	#endif
  }
  | KEYWORD_WHILE {
	ctx->labelstack[ctx->labelstack_index++] = ctx->label++;
	#ifdef TARGET_C
	printf("_while_%d_start:\n", ctx->labelstack[ctx->labelstack_index - 1]);
	#else
	printf(".label _while_%d_start\n", ctx->labelstack[ctx->labelstack_index - 1]);
	#endif
  } expression  {
	ctx->labelstack[ctx->labelstack_index++] = ctx->label++;
	#ifdef TARGET_C
	printf("\tPOP(R0)\n");
	printf("\tif(R0 == 0) goto _while_%d_end;\n", ctx->labelstack[ctx->labelstack_index -1]); 
	#else
        printf("\tpop r0\n");
 	printf("\ttest r0\n");
	printf("\tldjmpiz r0, _while_%d_end\n", ctx->labelstack[ctx->labelstack_index -1]);
	#endif
  } KEYWORD_LOOP statements KEYWORD_END {
	#ifdef TARGET_C
	printf("\tgoto _while_%d_start;\n", ctx->labelstack[ctx->labelstack_index -2]);
	printf("_while_%d_end:\n", ctx->labelstack[ctx->labelstack_index -1]);
	#else
	printf("\tldjmp r0, _while_%d_start\n", ctx->labelstack[ctx->labelstack_index -2]);
	printf(".label _while_%d_end\n", ctx->labelstack[ctx->labelstack_index -1]);
	#endif
	ctx->labelstack_index -= 2;
  }
  | KEYWORD_IF expression KEYWORD_THEN {
	ctx->labelstack[ctx->labelstack_index++] = ctx->label++;
	#ifdef TARGET_C
	printf("\tPOP(R0);\n");
	printf("\tif(R0 == 0) goto _if_%d_end;\n", ctx->labelstack[ctx->labelstack_index -1]);
	#else
        printf("\tpop r0\n");
 	printf("\ttest r0\n");
	printf("\tldjmpiz r0, _if_%d_end\n", ctx->labelstack[ctx->labelstack_index -1]);
	#endif
  } statements KEYWORD_END {
	#ifdef TARGET_C
	printf("_if_%d_end:\n", ctx->labelstack[--(ctx->labelstack_index)]);
	#else
	printf(".label _if_%d_end\n", ctx->labelstack[--(ctx->labelstack_index)]);
	#endif
  }
  | KEYWORD_IFE expression KEYWORD_THEN {
	ctx->labelstack[ctx->labelstack_index++] = ctx->label++;
	ctx->labelstack[ctx->labelstack_index++] = ctx->label++;
	#ifdef TARGET_C
	printf("\tPOP(R0);\n");
	printf("\tif(R0 == 0) goto _if_%d_else;\n", ctx->labelstack[ctx->labelstack_index -1]);
	#else
        printf("\tpop r0\n");
 	printf("\ttest r0\n");
	printf("\tldjmpiz r0, _if_%d_else\n", ctx->labelstack[ctx->labelstack_index -1]);
	#endif
  } statements {
	#ifdef TARGET_C
	printf("\tgoto _if_%d_end;\n", ctx->labelstack[ctx->labelstack_index -2]);
	#else
        printf("\tldjmp r0, _if_%d_end\n", ctx->labelstack[ctx->labelstack_index -2]);
	#endif
  } KEYWORD_ELSE {
	#ifdef TARGET_C
	printf("_if_%d_else:\n", ctx->labelstack[--(ctx->labelstack_index)]);
	#else
        printf(".label _if_%d_else\n", ctx->labelstack[--(ctx->labelstack_index)]);
	#endif
  }
    statements KEYWORD_END {
	#ifdef TARGET_C
	printf("_if_%d_end:\n", ctx->labelstack[--(ctx->labelstack_index)]);
	#else
	printf(".label _if_%d_end\n", ctx->labelstack[--(ctx->labelstack_index)]);
	#endif
  }
  | AT KEYWORD_REG COLON expression OP_ASSIGN expression SEMICOLON {
	#ifdef TARGET_C
	printf("\tPOP(R1);\n");
	printf("\tPOP(R0);\n");
	printf("\t*((reg_t*)R0) = R1;\n");
	#else
	printf("\tpop r1\n");
	printf("\tpop r0\n");
	printf("\tstoreid r0, r1, 0\n");
	#endif
  }
  | AT KEYWORD_BYTE COLON expression OP_ASSIGN expression SEMICOLON {
	#ifdef TARGET_C
	printf("\tPOP(R1);\n");
	printf("\tPOP(R0);\n");
	printf("\t*((byte_t*)R0) = (byte_t)(R1 & BYTE_MASK);\n");
	#else
	printf("\tpop r1\n");
	printf("\tpop r0\n");
	printf("\tstoreid r0, r1ll, 0\n");
	#endif
  }
  | AT KEYWORD_REG COLON expression STROKE expression OP_ASSIGN expression SEMICOLON {
	#ifdef TARGET_C
	printf("\tPOP(R1);\n");
	printf("\tPOP(R2);\n");
	printf("\tPOP(R0);\n");
	printf("\t*((reg_t*)R0 + R2) = R1;\n");
	#else
	printf("\tpop r1\n");
	printf("\tpop r2\n");
	printf("\tpop r0\n");
	printf("\tmul r2, r2, 4\n");
	printf("\tadd r0, r0, r2\n");
	printf("\tstoreid r0, r1, 0\n");
	#endif
  }
  | AT KEYWORD_BYTE COLON expression STROKE expression OP_ASSIGN expression SEMICOLON {
	#ifdef TARGET_C
	printf("\tPOP(R1);\n");
	printf("\tPOP(R2);\n");
	printf("\tPOP(R0);\n");
	printf("\t*((byte_t*)R0 + R2) = (byte_t)(R1 & BYTE_MASK);\n");
	#else
	printf("\tpop r1\n");
	printf("\tpop r2\n");
	printf("\tpop r0\n");
	printf("\tadd r0, r0, r2\n");
	printf("\tstoreid r0, r1ll, 0\n");
	#endif
  }
  | IDENT OP_ASSIGN expression SEMICOLON {
	int la = find_local(ctx, $1);
	#ifdef TARGET_C
	printf("\tPOP(R0);\n");
	printf("\t*((reg_t*)BP + %d) = R0;\n", la / 4);
	#else
	printf("\tpop r0\n");
	printf("\tstoreid bp, r0, %d\n", la);
	#endif
  }
  | IDENT OP_UNY expression SEMICOLON {
	int la = find_local(ctx, $1);

	#ifdef TARGET_C
	printf("\tR0 = *((reg_t*)BP + %d);\n", la / 4);
	printf("\tPOP(R1);\n");
	#else
	printf("\tloadid r0, bp, %d\n", la);
	printf("\tpop r1\n");
	#endif

	emit_unyop($2);

	#ifdef TARGET_C
	printf("\t*((reg_t*)BP + %d) = R0;\n", (la / 4));
	#else
	printf("\tstoreid bp, r0, %d\n", la);
	#endif
  }
  | KEYWORD_IRET SEMICOLON {
	printf("\tiret\n");
  }
  | KEYWORD_FROMCTRL expression COLON COLON IDENT SEMICOLON {
	printf("\tpop r0\n");
	printf("\tmovfc r4, %s\n", $5);
	printf("\tstoreid r0, r4, 0\n");
  }
  | KEYWORD_TOCTRL IDENT COLON COLON expression SEMICOLON {
	printf("\tpop r0\n");
	printf("\tloadid r4, r0, 0\n");
	printf("\tmovtc %s, r4\n", $2);
  }
  | KEYWORD_INPORT expression COLON COLON expression SEMICOLON {
	printf("\tpop r1\n");
	printf("\tpop r0\n");
	printf("\tin r4, r1\n");
	printf("\tstoreid r0, r4, 0\n");
  }
  | KEYWORD_OUTPORT expression COLON COLON expression SEMICOLON {
	printf("\tpop r1\n");
	printf("\tpop r0\n");
	printf("\tout r0, r1\n");
  }
  | ASM {
	printf("%s",$1);
  }
 ;

locals:
  local | locals local;
 ;

local:
  IDENT SEMICOLON{
	ctx->symbols[ctx->cur_locals] = $1;
	ctx->cur_locals++;
	ctx->stacksize_locals += 4;
  }
  | KEYWORD_NONE { }
 ;

parameters:
  parameter | parameters parameter;
 ;

parameter:
  IDENT SEMICOLON{
	ctx->symbols[ctx->cur_locals] = $1;
	ctx->cur_locals++;
	ctx->stacksize_parameters += 4;
  }
  | KEYWORD_NONE { }
 ;

%%

void emit_unyop(int op) {
	switch(op) {
		case RL_UNYOP_PEQ:
			#ifdef TARGET_C
			printf("\tR0 += R1;\n");
			#else
			printf("\tadd r0, r0, r1\n");
			#endif
			break;
		case RL_UNYOP_SEQ:
			#ifdef TARGET_C
			printf("\tR0 -= R1;\n");
			#else
			printf("\tsub r0, r0, r1\n");
			#endif
			break;
		case RL_UNYOP_MEQ:
			#ifdef TARGET_C
			printf("\tR0 *= R1;\n");
			#else
			printf("\tmul r0, r0, r1\n");
			#endif
			break;
		case RL_UNYOP_DEQ:
			#ifdef TARGET_C
			printf("\tR0 /= R1;\n");
			#else
			printf("\tdiv r0, r0, r1\n");
			#endif
			break;
		case RL_UNYOP_L_NOT:
			#ifdef TARGET_C
			printf("\tif(R0) R0 = 0; else R0 = 1;\n");
			#else
			printf("\ttest r0\n");
			printf("\txor r0, r0, r0\n");
			printf("\toriz r0, r0, 1\n");
			#endif
			break;
		case RL_UNYOP_NOT:
			#ifdef TARGET_C
			printf("\tR0 = ~R0;\n");
			#else
			printf("\tnot r0, r0\n");
			#endif
			break;
		case RL_UNYOP_AEQ:
			printf("and r0, r0, r1\n");
			break;
		case RL_UNYOP_OEQ:
			printf("or r0, r0, r1\n");
			break;
		case RL_UNYOP_XEQ:
			printf("xor r0, r0, r1\n");
			break;
		default:
			fprintf(stderr, "Unknown operator!");
			exit(5);
	}
}

void emit_binop(int op) {
	switch(op) {
		case RL_BINOP_ADD:
		case RL_BINOP_SADD:
			#ifdef TARGET_C
			printf("\tR0 += R1;\n");
			#else
			printf("\tadd r0, r0, r1\n");
			#endif
			break;
		case RL_BINOP_SUB:
		case RL_BINOP_SSUB:
			#ifdef TARGET_C
			printf("\tR0 -= R1;\n");
			#else
			printf("\tsub r0, r0, r1\n");
			#endif
			break;
		case RL_BINOP_MUL:
			#ifdef TARGET_C
			printf("\tR0 *= R1;\n");
			#else
			printf("\tmul r0, r0, r1\n");
			#endif
			break;
		case RL_BINOP_SMUL:
			printf("\tsmul r0, r0, r1\n");
			break;
		case RL_BINOP_DIV:
			#ifdef TARGET_C
			printf("\tR0 /= R1;\n");
			#else
			printf("\tdiv r0, r0, r1\n");
			#endif
			break;
		case RL_BINOP_SDIV:
			printf("\tsdiv r0, r0, r1\n");
			break;
		case RL_BINOP_EQU:
		case RL_BINOP_SEQU:
			#ifdef TARGET_C
			printf("\tif(R0 == R1) R0 = 1; else R0 = 0;\n");
			#else
			printf("\tcmp r0, r1\n");
			printf("\txor r0, r0, r0\n");
			printf("\toriz r0, r0, 1\n");
			#endif
			break;
		case RL_BINOP_NEQ:
		case RL_BINOP_SNEQ:
			#ifdef TARGET_C
			printf("\tif(R0 != R1) R0 = 1; else R0 = 0;\n");
			#else
			printf("\tcmp r0, r1\n");
			printf("\txor r0, r0, r0\n");
			printf("\tornz r0, r0, 1\n");
			#endif
			break;
		case RL_BINOP_SHL:
			#ifdef TARGET_C
			printf("\tR0 <<= R1;\n");
			#else
			printf("\tshl r0, r0, r1\n");
			#endif
			break;
		case RL_BINOP_SHR:
			#ifdef TARGET_C
			printf("\tR0 >>= R1;\n");			
			#else
			printf("\tshr r0, r0, r1\n");
			#endif
			break;
		case RL_BINOP_XOR:
			#ifdef TARGET_C
			printf("\tR0 ^= R1;\n");
			#else
			printf("\txor r0, r0, r1\n");
			#endif
			break;
		case RL_BINOP_OR:
			#ifdef TARGET_C
			printf("\tR0 |= R1;\n");
			#else
			printf("\tor r0, r0, r1\n");
			#endif
			break;
		case RL_BINOP_AND:
			#ifdef TARGET_C
			printf("\tR0 &= R1;\n");
			#else
			printf("\tand r0, r0, r1\n");
			#endif
			break;
		case RL_BINOP_L_AND:
			#ifdef TARGET_C
			printf("if((R0 != 0) && (R1 != 0)) R0 = 1; else R0 = 0;\n");
			#else
			printf("\txor r4, r4, r4\n");
			printf("\ttest r0\n");
			printf("\taddnz r4, r4, 1\n");
			printf("\ttest r1\n");
			printf("\taddnz r4, r4, 1\n");
			printf("\tcmp r4, 2\n");
			printf("\txor r0, r0, r0\n");
			printf("\toriz r0, r0, 1\n");
			#endif
			break;
		case RL_BINOP_L_OR:
			#ifdef TARGET_C
			printf("if((R0 != 0) || (R1 != 0)) R0 = 1; else R0 = 0;\n");
			#else
			printf("\txor r4, r4, r4\n");
			printf("\ttest r0\n");
			printf("\taddnz r4, r4, 1\n");
			printf("\ttest r1\n");
			printf("\taddnz r4, r4, 1\n");
			printf("\ttest r4\n");
			printf("\txor r0, r0, r0\n");
			printf("\tornz r0, r0, 1\n");
			#endif
			break;
		case RL_BINOP_LTH:
			#ifdef TARGET_C
			printf("if(R0 < R1) R0 = 1; else R0 = 0;\n");
			#else
			printf("\tcmp r0, r1\n");
			printf("\txor r0, r0, r0\n");
			printf("\toric r0, r0, 1\n");
			#endif
			break;
		case RL_BINOP_GTH:
			#ifdef TARGET_C
			printf("if(R0 > R1) R0 = 1; else R0 = 0;\n");
			#else
			printf("\tcmp r0, r1\n");
			printf("\txor r0, r0, r0\n");
			printf("\tornznc r0, r0, 1\n");
			#endif
			break;
		case RL_BINOP_LEQ:
			#ifdef TARGET_C
			printf("if(R0 <= R1) R0 = 1; else R0 = 0;\n");
			#else
			printf("\tcmp r0, r1\n");
			printf("\txor r0, r0, r0\n");
			printf("\torizic r0, r0, 1\n");
			break;
			#endif
		case RL_BINOP_GEQ:
			#ifdef TARGET_C
			printf("if(R0 >= R1) R0 = 1; else R0 = 0;\n");
			#else
			printf("\tcmp r0, r1\n");
			printf("\txor r0, r0, r0\n");
			printf("\tornc r0, r0, 1\n");
			#endif
			break;

		////
		case RL_BINOP_SLTH:
			printf("\tcmp r0, r1\n");
			printf("\txor r0, r0, r0\n");
			printf("\toris r0, r0, 1\n");
			break;
		case RL_BINOP_SGTH:
			printf("\tcmp r0, r1\n");
			printf("\txor r0, r0, r0\n");
			printf("\tornzns r0, r0, 1\n");
			break;
		case RL_BINOP_SLEQ:
			printf("\tcmp r0, r1\n");
			printf("\txor r0, r0, r0\n");
			printf("\torizis r0, r0, 1\n");
			break;
		case RL_BINOP_SGEQ:
			printf("\tcmp r0, r1\n");
			printf("\txor r0, r0, r0\n");
			printf("\torns r0, r0, 1\n");
			break;
		default:
			fprintf(stderr, "Unknown operator!");
			exit(5);
	}
}

int find_local(rlang_context* ctx, char* symbol) {
	for(int i = 0; i < ctx->cur_locals; i++) {
		if(!strcmp(ctx->symbols[i], symbol)) {
			return ctx->symtbl[i];
		}
	}
	fprintf(stderr, "%s ...\n", symbol);
	yyerror(ctx, "Local not found :(");
	exit(-1);
}

int main(int argc, char* argv[]) {
	#if !defined(GLIB_VERSION_2_36)
	  g_type_init();
	#endif

	FILE * file;

	file = fopen (argv[1] , "r");
	if (file == NULL) {
		perror ("Error opening file");
		exit(-1);
	}

	// file to parse
	yyin = file;

	rlang_context ctx;
	ctx.cur_locals = 0;
	ctx.cur_records = 0;
	ctx.labelstack_index = 0;
	ctx.label = 0;
	ctx.fails = 0;

	#ifdef TARGET_C
	printf("#include <stdio.h>\n");
	printf("#include <stdlib.h>\n");
	printf("#include <stdint.h>\n");
	printf("typedef uint8_t byte_t;\n");
	printf("typedef uintptr_t reg_t;\n");
	printf("static reg_t SP, BP, R0, R1, R2, R4, RC;\n");
	printf("#define BYTE_MASK 0xFFu\n");
	printf("#define PUSH(x) { *(reg_t*)SP = x; SP -= sizeof(reg_t); }\n");
	printf("#define POP(x) { SP += sizeof(reg_t); x = *(reg_t*)SP; }\n");
	printf("int main(void){\n");
	printf("if(sizeof(uintptr_t) != 4) return -1;\n");
	printf("SP = (reg_t)malloc(sizeof(reg_t)*1024);\n");
	printf("SP += sizeof(reg_t) * 1023;\n");
	printf("goto _main;\n");
	#else
	printf("\tldjmp r0, _main\n");
	#endif

	do {
		yyparse(&ctx);
	} while (!feof(yyin));

	#ifdef TARGET_C
	printf("return 0;}\n");
	#endif

	exit(0);
}

void yyerror(rlang_context* ctx, const char *s) {
	fprintf(stderr, "On line %d: %s\n", yylloc.first_line, s);
	exit(-1);
}
