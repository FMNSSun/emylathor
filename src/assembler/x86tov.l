%{
#define YY_DECL extern int yylex()

#include "x86tov.tab.h"  // automatically generated
#include <string.h>
#include <stdlib.h>

#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;

char* remove_brackets(char* c) {
  int len = strlen(c);
  char* new = malloc(len - 2);
  int i;
  for(i = 1; i < len - 1; i++) {
    new[i-1] = c[i];
  }
  return new;
}

%}



REG32			%eax|%ebx|%ecx|%edx|%ebp|%esp
OP_MOVL			movl
OP_PUSHL		pushl
OP_IMULL		imull
OP_RET			ret
OP_LEAL			leal
IGND			.glob|.file|.intel_syntax|.text|.type|"(.*)"

%%
\n|\r\n			{ return ENDL; }
[\t ]*			{ return SPACE; }
$[\-]*[0-9]+		{ yylval.ival = atoi(yytext + 1); return INT; }
[\-]*[0-9]+		{ yylval.ival = atoi(yytext); return INT; }
\({REG32}\)		{ yylval.sval = remove_brackets(yytext) + 1; return MREG32; }
[ \t\n]*,[ \t\n]*	{ return COMMA; }
{REG32}			{ yylval.sval = strdup(yytext+1); return REG32; }	
{OP_MOVL}		{ yylval.sval = strdup(yytext); return OP_MOVL; }
{OP_PUSHL}		{ yylval.sval = strdup(yytext); return OP_PUSHL; }
{OP_IMULL}		{ yylval.sval = strdup(yytext); return OP_IMULL; }
{OP_RET}		{ yylval.sval = strdup(yytext); return OP_RET; }
{OP_LEAL}		{ yylval.sval = strdup(yytext); return OP_LEAL; }
IGND			;
.			{ return GARBAGE; }
%%
