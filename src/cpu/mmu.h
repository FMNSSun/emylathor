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

#ifndef VA_H_MMU_H
#define VA_H_MMU_H

#include "common/macros.h"
#include "common/definitions.h"

#define _va_mmu

#define MMU_PDBITS(x) ((x >> 22u) & 0x3ffu) 
#define MMU_PTBITS(x) ((x >> 12u) & 0x3ffu)
#define MMU_OFBITS(x) (x & 0xfffu)
#define MMU_FRMBITS(x) ((x >> 12u) & 0xfffffu)
#define MMU_PTCTRLBITS(x) (x & 0xfffu)
#define MMU_BLANK_PTCTRLBITS(x) (x & ~0xfffu)

#define MMU_PTCTRLBIT_PRESENT 0x01u
#define MMU_PTCTRLBIT_PROTECTED 0x02u
#define MMU_PTCTRLBIT_EXECUTABLE_P 0x04u
#define MMU_PTCTRLBIT_WRITE_U 0x08u
#define MMU_PTCTRLBIT_WRITE_P 0x10u
#define MMU_PTCTRLBIT_USED 0x20u
#define MMU_PTCTRLBIT_EXECUTABLE_U 0x40u
#define MMU_PTCTRLBIT_READ_U 0x80u

#define MMU_ERR_SUCCESS 0x00u
#define MMU_ERR_PAGE_NOT_PRESENT 0x01u
#define MMU_ERR_ACCESS_VIOLATION 0x02u
#define MMU_ERR_NOT_EXECUTABLE 0x03u
#define MMU_ERR_ENTRY_MISSING 0x04u

#define MMU_ACCESS_MODE_READ 0x00u
#define MMU_ACCESS_MODE_WRITE 0x01u
#define MMU_ACCESS_MODE_EXEC 0x02u

/* Function: mmu_ensure_page

   Ensures that a page can be accessed (with a given mode).
   Mode may be one of MMU_ACCESS_MODE_READ, MMU_ACCESS_MODE_WRITE or
   MMU_ACCESS_MODE_EXEC.

   This function fires Page Faults if necessary. 

   Parameters:
     addr - Virtual address
     ectx - Emulator context
     mode - Access mode

   Returns:
     1 - if access may be carried out
     0 - if access must not be carried out. (Missing permissions, Page not present)
 */
int mmu_ensure_page(dword addr, emulator_context* ectx, int mode);

/* Function: mmu_translate_address

   Translates a virtual address to a physical address. This function assumes that the page is present.

   Parameters:
     addr - The (virtual) address
     ectx - Emulator context

   Returns:

   The physical address.
 */
dword mmu_translate_address(dword addr, emulator_context* ectx);

/* Function: mmu_is_enabled

   Returns:
     0 - if the MMU is disabled (see MR* registers)
     1 - if the MMU is enabled
 */
int mmu_is_enabled(emulator_context* ectx);

#endif
