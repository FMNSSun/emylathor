/*
Copyright (c) 2013, Roman Muentener
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: Redistributions of
source code must retain the above copyright notice, this list of conditions and
the following disclaimer. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ’AS IS’ AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef VA_H_INT_H
#define VA_H_INT_H

#include "common/definitions.h"
#include "common/macros.h"

#define _va_int

#define INT_IVT	0u
#define INT_ITT 1024u

#define INT_EXC_DIVBYZERO	0u
#define INT_EXC_PROTFAULT	1u
#define INT_EXC_DBLEFAULT	2u
#define INT_EXC_ADDROVERF	3u
#define INT_EXC_ILLEGALOP	4u
#define INT_EXC_PAGEFAULT	5u
#define INT_EXC_EXCNOTHLD	6u
#define INT_EXC_ITU_FAULT	7u
#define INT_EXC_ALIGFAULT	8u


#define INT_EXC_MIN		0u
#define INT_EXC_MAX		31u
#define INT_SW_MIN		32u
#define INT_SW_MAX		127u
#define INT_HW_MIN		128u
#define INT_HW_MAX		255u

/* Function: int_reload_itt

   Reloads the cached version of the ITT. This function should be
   called when the Interrupts Enabled Flag in CR1 was set. 

   Parameters:
     ectx - Emulator context
 */
void int_reload_itt(emulator_context* ectx);

/* Function: int_icu_get

   Returns the next hardware interrupt to be fired.
 */
byte int_icu_get(emulator_context* ectx);

/* Function: int_handle_exception

   Handles an exception (requires that an exception is present). If no
   exception is present this function will block. 

   Parameters:
     ectx - Emulator context
 */
void int_handle_exception(byte no, emulator_context* ectx);

/* Function: int_handle_interrupt

   Handles an interrupt (excl. exceptions) (requires that an interrupt is present). If no interrupt (excl. exception)
   is present this function will block. 

   Parameters:
     ectx - Emulator context
 */
void int_handle_interrupt(byte no, emulator_context* ectx);

/* Function: int_handle_return

   Returns from an Interrupt Service Routine. This function is called when an IRET is executed.

   Parameters:
     ectx - Emulator context
 */
void int_handle_return(emulator_context* ectx);

/* Function: int_fire_interrupt

   Fire an interrupt. The exact handling depends on the interrupt number.
   Exceptions and Software Interrupts are handled immediately. For Hardware Interrupts
   this function sets a flag indicating that a certain Hardware Interrupt is pending.

   Parameters:
     no - Interrupt Number
     ectx - Emulator Context
 */
void int_fire_interrupt(byte no, emulator_context* ectx);

/* Function: int_enabled

   Parameters:
     ectx - Emulator context

   Returns:
     0 - if not enabled
     != 0 - otherwise
 */
int int_enabled(emulator_context* ectx);

#endif
