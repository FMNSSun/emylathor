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

#ifndef VA_H_LOOKUP_H
#define VA_H_LOOKUP_H

#include "common/macros.h"
#include "common/definitions.h"


/* Function: get_op_for_name

   Parameters:
     op - Mnemonic or name of the opcode

   Returns:
  
   The op code of a corresponding Mnemonic.
 */
byte get_op_for_name(char* op);

byte get_creg_for_name(char* creg);

/* Function: get_reg_for_name

   Parameters:
     reg - Mnemonic or name of the register

   Returns:
  
   The register of a corresponding Mnemonic.
 */
byte get_reg_for_name(char* reg);

/* Function: get_reg16_for_name

   Parameters:
     reg - Mnemonic or name of the register

   Returns:
  
   The register of a corresponding Mnemonic.
 */
byte get_reg16_for_name(char* reg);

/* Function: get_reg8_for_name
   
   Parameters:
     reg - Mnemonic or name of the register

   Returns:

   The register of a corresponding Mnemonic.
 */  
byte get_reg8_for_name(char* reg);

/* Function: get_cond_for_name

   Paremeters:
     cond - Mnemonic or name of the condition

   Returns:
   
   The condition code of the given Mnemonic.
 */
byte get_cond_for_name(char* cond);

char* get_name_of_op(byte prefix, byte op);
char* get_name_of_creg(byte reg);
char* get_name_of_reg(byte reg);
char* get_name_of_reg8(byte reg);
char* get_name_of_reg16(byte reg);
char* get_name_of_prefix(byte prefix);
char* get_name_of_cond(byte cond);

#endif
