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

#ifndef VA_H_MACROS_H
#define VA_H_MACROS_H

#include "definitions.h"

/* About: macros

   Macros:
   
   + HHBYTE_FROM_DWORD - Higher high byte from dword
   + HLBYTE_FROM_DWORD - Higher low byte from dword
   + LHBYTE_FROM_DWORD - Lower high byte from dword
   + LLBYTE_FROM_DWORD - Lower low byte from dword

   + LWORD_FROM_DWORD - Low word from dword
   + HWORD_FROM_DWORD - High word from dword

   + HBYTE_FROM_WORD - High byte from word
   + LBYTE_FROM_WORD - Low byte from word

   + DWORD_FROM_WORDS(hi,lo) - Dword from high word and low word
   + WORD_FROM_BYTES(hi, lo) - Word from high byte and low byte
   + DWORD_FROM_BYTES(uh, ul, lh, ll) - Dword from upper high, upper low, lower high and lower low byte

   + SIGNED-N - Convert from unsigned two's complement to signed
   + UNSIGNED-N - Convert from signed to unsigned two's complement

   Some macros might be platform specific.
 */

#define HHBYTE_FROM_DWORD(x) (x >> 24u)
#define HLBYTE_FROM_DWORD(x) ((x >> 16u) & 0xFFu)
#define LHBYTE_FROM_DWORD(x) ((x >> 8u) & 0xFFu)
#define LLBYTE_FROM_DWORD(x) (x & 0xFFu)

#define LWORD_FROM_DWORD(x) (x & 0xFFFFu)
#define HWORD_FROM_DWORD(x) ((x >> 16u) & 0xFFFFu)

#define HBYTE_FROM_WORD(x) ((x >> 8u) & 0xFF)
#define LBYTE_FROM_WORD(x) (x & 0xFF)

#define DWORD_FROW_WORDS(hi,lo) ((hi << 16u) | lo)
#define WORD_FROM_BYTES(hi, lo) ((hi << 8u) | lo)
#define DWORD_FROM_BYTES(uh, ul, lh, ll) ((uh << 24u) | (ul << 16u) | (lh << 8u) | ll)

#define POINTER_TO_INT(p) ((uintptr_t)p)

#ifdef __i386__

#define ATOMIC_INC(x) asm volatile("lock incl %0" : "=m"(x) : "m"(x));
#define ATOMIC_DEC(x) asm volatile("lock decl %0" : "=m"(x) : "m"(x));

#else
#ifdef __x86_64__

#define ATOMIC_INC(x) asm volatile("lock incq %0" : "=m"(x) : "m"(x));
#define ATOMIC_DEC(x) asm volatile("lock decq %0" : "=m"(x) : "m"(x));

#else
#error "Sorry, there are no versions of ATOMIC_INC/ATOMIC_DEC for your target."
#endif
#endif


#if (__GNUC__ >= 4) && (__i386__ || __x86_64__)
#define SIGNED32(x) ( (sdword) (x) )
#define SIGNED16(x) ( (sword) (x) )
#define SIGNED8(x)  ( (sbyte) (x) )
#define UNSIGNED32(x) ( (dword)(sdword) (x) )
#define UNSIGNED16(x) ( (word)(sword) (x) )
#define UNSIGNED8(x)  ( (byte)(sbyte) (x) )
#else
#error "Sorry, there are no version of (UN)SIGNED-N for your target."
#endif

#include "glib.h"
#define PUTLOG(ectx, msg, ...) g_mutex_lock(ectx->log_lock); \
                               snprintf(ectx->logbuffer, ectx->logbuffer_size, msg, __VA_ARGS__); \
                               ectx->log(ectx->logbuffer); \
                               g_mutex_unlock(ectx->log_lock); 
#define PUTLOG2(ectx, msg)     g_mutex_lock(ectx->log_lock); \
                               snprintf(ectx->logbuffer, ectx->logbuffer_size, msg); \
                               ectx->log(ectx->logbuffer); \
                               g_mutex_unlock(ectx->log_lock); 

#endif
