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

#include "mmu.h"
#include "memory.h"
#include "interrupts.h"

#include <stdio.h>

//TODO: Emulate an address translation cache?

_va_mmu dword mmu_get_page_entry(dword addr, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	/* Location of page directory */
	dword pd = cctx->cregs[CPU_CREG_MR1];

	#if DUMP_ALL || DUMP_MMU
	PUTLOG(ectx, "[MMU] :: Loading page entry from %08x", pd);
	#endif

	/* Location of page table */
	dword pt = mem_get_dword_p(pd + 4*MMU_PDBITS(addr), ectx);

	#if DUMP_ALL || DUMP_MMU
	PUTLOG(ectx, "[MMU] :: Loaded page entry for virtual address %08x. pd := %08x, pt := %08x",
		addr, pd, pt);
	#endif

	return  mem_get_dword_p(pt + 4*MMU_PTBITS(addr), ectx);
}

_va_mmu int mmu_ensure_page(dword addr, emulator_context* ectx, int mode) {
	cpu_context* cctx = ectx->cpu_context;

	/* Location of Page directory */
	dword pd = cctx->cregs[CPU_CREG_MR1]; //mr1 holds the physical address of the page directory
	/* Location of Page table */
	dword pt = mem_get_dword_p(pd + 4*MMU_PDBITS(addr), ectx);

	#if DUMP_ALL || DUMP_MMU
	PUTLOG(ectx, "[MMU] ensure_page: addr := %08x, pd := %08x, pt = %08x", addr, pd, pt);
	#endif

	if(pt == 0x00u) {
		cctx->cregs[CPU_CREG_MR4] = MMU_ERR_ENTRY_MISSING;
		goto mmu_abort;
	}

	/* Page entry */
	dword entry = mem_get_dword_p(pt + 4*MMU_PTBITS(addr), ectx);

	/* Control bits */
	word ctrl_bits = MMU_PTCTRLBITS(entry);

	#if DUMP_ALL || DUMP_MMU
	PUTLOG(ectx, "[MMU] ensure_page: pd := %08x, pt = %08x, entry = %08x, ctrlbits = %08x", pd, pt, entry, ctrl_bits);
	#endif

	//Is the page present?
	if(!(ctrl_bits & MMU_PTCTRLBIT_PRESENT)) {
		cctx->cregs[CPU_CREG_MR4] = MMU_ERR_PAGE_NOT_PRESENT;
		goto mmu_abort;
	}

	//Is it protected but we are not in privilege mode?
	if(ctrl_bits & MMU_PTCTRLBIT_PROTECTED) {
		if(!(cctx->cregs[CPU_CREG_CR1] & CR1_PRIV_MODE)) {
			cctx->cregs[CPU_CREG_MR4] = MMU_ERR_ACCESS_VIOLATION;
			goto mmu_abort;
		}
	}

	//Ok.. are we in privileged mode or in unprivileged mode?
	if(cctx->cregs[CPU_CREG_CR1] & CR1_PRIV_MODE) { // priviliged mode
		//Are we writing but the write flag is not set?
		if(mode == MMU_ACCESS_MODE_WRITE) {
			if(!(ctrl_bits & MMU_PTCTRLBIT_WRITE_P)) {
				cctx->cregs[CPU_CREG_MR4] = MMU_ERR_ACCESS_VIOLATION;
				goto mmu_abort;
			}
		}

		//Are we reading but the read unprivileged flag is set?
		if(mode == MMU_ACCESS_MODE_READ){
			if((ctrl_bits & MMU_PTCTRLBIT_READ_U)) {
				cctx->cregs[CPU_CREG_MR4] = MMU_ERR_ACCESS_VIOLATION;
				goto mmu_abort;
			}
		}

		//Is it not executable but we are fetching code?
		if(!(ctrl_bits & MMU_PTCTRLBIT_EXECUTABLE_P)) {
			if(mode == MMU_ACCESS_MODE_EXEC) {
				cctx->cregs[CPU_CREG_MR4] = MMU_ERR_NOT_EXECUTABLE;
				goto mmu_abort;
			}
		}
	} else { // unprivileged mode
		//Are we writing but the write flag is not set?
		if(mode == MMU_ACCESS_MODE_WRITE) {
			if(!(ctrl_bits & MMU_PTCTRLBIT_WRITE_U)) {
				cctx->cregs[CPU_CREG_MR4] = MMU_ERR_ACCESS_VIOLATION;
				goto mmu_abort;
			}
		}

		//Is it not executable but we are fetching code?
		if(!(ctrl_bits & MMU_PTCTRLBIT_EXECUTABLE_U)) {
			if(mode == MMU_ACCESS_MODE_EXEC) {
				cctx->cregs[CPU_CREG_MR4] = MMU_ERR_NOT_EXECUTABLE;
				goto mmu_abort;
			}
		}
	}

	//Set the used bit
	ctrl_bits = ctrl_bits | MMU_PTCTRLBIT_USED;

	//Update the page table entry
	entry = MMU_BLANK_PTCTRLBITS(entry) | ctrl_bits;
	mem_write_dword_p(entry, pt + 4*MMU_PTBITS(addr), ectx);
	
	return 1;

mmu_abort:

	#if DUMP_ALL || DUMP_MMU
	PUTLOG(ectx, "[MMU] :: abort due to err := %d", cctx->cregs[CPU_CREG_MR4]);
	#endif

	cctx->cregs[CPU_CREG_MR3] = addr;
	int_fire_interrupt(INT_EXC_PAGEFAULT, ectx);
	return 0;
}

_va_mmu dword mmu_translate_address(dword addr, emulator_context* ectx) {
	dword entry = mmu_get_page_entry(addr, ectx);
	dword frame = MMU_FRMBITS(entry);
	dword offset = MMU_OFBITS(addr);

	#if DUMP_ALL || DUMP_MMU
	PUTLOG(ectx, "[MMU] :: Frame:= %08x Offset = %08x, Vaddr = %08x, Phaddr = %08x",
		frame, offset,  addr, ((frame << 12u | offset)));
	#endif

	return ((frame << 12u) | (offset));
}

_va_mmu int mmu_is_enabled(emulator_context* ectx) {
	return (ectx->cpu_context->cregs[CPU_CREG_MR2] & MR2_MMU_ENABLED);
}
