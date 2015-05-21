%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "x86tov.tab.h"  // automatically generated

// extern flex stuff
extern int yylex();
extern  int yyparse();
extern FILE *yyin;
 
void yyerror(const char *s);


char* translate_reg(char* reg);
void load_offset(int o);
void load_constant(int c);
%}

%locations
%error-verbose

// Union to hold the tokens
%union {
	int ival;
	char *sval;
}

// Symbols
%token <ival> INT
%token <sval> OP_MOVL
%token <sval> OP_PUSHL
%token <sval> OP_IMULL
%token <sval> OP_RET
%token <sval> OP_LEAL
%token <sval> REG32
%token <sval> MREG32
%token SPACE;
%token COMMA;
%token ENDL;
%token GARBAGE;

%%
// vasm grammar
exps:
  exp | exps exp
 ;
exp:
	ENDL { /* empty */ }

	/* Translating mov instructions */

	// movl reg32, reg32
	| OP_MOVL SPACE REG32 COMMA REG32 {
		printf("; %s %%%s, %%%s\n",$1, $3, $5);
		
		// Simple translation
		printf("mov\t\t%s, %s\n", translate_reg($3), translate_reg($5));
	}

	// movl int, m-reg32
	| OP_MOVL SPACE INT COMMA INT MREG32 {
		printf("; %s %d, %d(%%%s)\n", $1, $3, $5, $6);

		// Ok, first we need to load the offset into a register.
		load_offset($5);
		// Ok, then we need to load the constant into a register. We use R9 for that
		load_constant($3);
		// Ok, then we can issue a store with register displacement
		printf("storerd\t\t%s, ra, r5\n", translate_reg($6));
	}

	// movl m-reg32, reg32
	| OP_MOVL SPACE INT MREG32 COMMA REG32 {
		printf("; %s %d(%%%s), %s\n", $1, $3, $4, $6);

		// Ok, first we need to load the offset into a register. We use RA for that
		load_offset($3);
		// Ok, do the load
		printf("loadrd\t\t%s, %s, ra\n", translate_reg($6), translate_reg($4));
	}

	/* Tranlating imul instructions */

	| OP_IMULL SPACE REG32 COMMA REG32 {
		printf("; %s %%%s, %%%s\n", $1, $3, $5);

		// Simple translation
		printf("mul\t\t%s, %s, %s\n", translate_reg($3), translate_reg($3), translate_reg($5));
	}

	/* Translating push instructions */

	| OP_PUSHL SPACE REG32 {
		printf("; %s %%%s\n",$1, $3);

		// Simple translation
		printf("push\t\t%s\n", translate_reg($3));
	}

	/* Misc */
	| OP_RET {
		printf("; %s\n", $1);

		// Very simple
		printf("jmp\t\trc\n");
	}

	| SPACE { /* empty */ }
	| GARBAGE { /* empty */ }
	| COMMA { /* empty */ }
	| REG32 { /* empty */ }
	| INT { /* empty */ }
	;
%%

void load_offset(int o) {
	printf("xchg\t\tra, r0\n");
	printf("load32\t\tr0, %d\n", o);
	printf("xchg\t\tra, r0\n");
}

void load_constant(int c) {
	printf("xchg\t\tr9, r0\n");
	printf("load32\t\tr0, %d\n", c);
	printf("xchg\t\tr9, r0\n");
}

char* translate_reg(char* reg) {
	if(!strcmp(reg,"ebp"))
		return "bp";
	if(!strcmp(reg,"esp"))
		return "sp";
	if(!strcmp(reg,"eax"))
		return "r0";
	if(!strcmp(reg,"ebx"))
		return "r1";
	if(!strcmp(reg,"ecx"))
		return "r2";
	if(!strcmp(reg,"edx"))
		return "r3";
 	return "n/a";
}

int main(int argc, char* argv[]) {
	FILE * file;

	file = fopen ("test.S" , "r");
	if (file == NULL) {
		perror ("Error opening file");
		exit(-1);
	}

	// file to parse
	yyin = file;

	

	do {
		yyparse();
	} while (!feof(yyin));

	fclose (file);

	

	return 0;
}

void yyerror(const char *s) {
	fprintf(stderr, "Error on line %d: %s\n", yylloc.first_line, s);
	exit(-1);
}
