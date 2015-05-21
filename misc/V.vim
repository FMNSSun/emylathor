" Vim syntax file
" Language:	VA Assembler
" Maintainer:	Roman Müntener
" Last Change:	2013 May

" For version 5.x: Clear all syntax items
" For version 6.0 and later: Quit when a syntax file was already loaded
if version < 600
  syntax clear
elseif exists("b:current_syntax")
  finish
endif

syn case match


" registers
syn match asmRegister "R0"
syn match asmRegister "R1"
syn match asmRegister "R2"
syn match asmRegister "R3"

" opcodes
syn match asmOp "ADD"
syn match asmOp "AND"
syn match asmOp "CMP"
syn match asmOp "FAIL"
syn match asmOp "LOADI"
syn match asmOp "LOAD32"
syn match asmOp "STOP"

" conditions
syn match asmCond "NZ"
syn match asmCond "IZ"

syn match asmComment		";.*"

syn match asmDirective		"\.[a-z][a-z]\+"

" Define the default highlighting.
" For version 5.7 and earlier: only when not done already
" For version 5.8 and later: only when an item doesn't have highlighting yet
if version >= 508 || !exists("did_asm_syntax_inits")
  if version < 508
    let did_asm_syntax_inits = 1
    command -nargs=+ HiLink hi link <args>
  else
    command -nargs=+ HiLink hi def link <args>
  endif

  " The default methods for highlighting.  Can be overridden later
  HiLink asmComment	Comment
  HiLink asmDirective	Statement

  HiLink asmCond	PreCondit

  HiLink asmOp          Identifier
  HiLink asmRegister	Type

  delcommand HiLink
endif

let b:current_syntax = "vasm"

" vim: ts=8
