#include "interrupts.h"
#include "memory.h"
#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

//TODO: double fault

static byte itt_cached[128] = {0u};

_va_int void int_reload_itt(emulator_context* ectx) {

	#if DUMP_ALL || DUMP_INT
	PUTLOG2(ectx, "[INT] :: Reload ITT");
	#endif


	for(dword i = 0; i < INT_HW_MIN; i++) {
		itt_cached[i] = mem_get_byte_p(INT_ITT + i, ectx);
		#if DUMP_ALL || DUMP_INT
		PUTLOG(ectx, "[INT] :: Remapped %d to %d", i, itt_cached[i]);
		#endif
	}
}

_va_int byte int_icu_get(emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	byte* ints = cctx->ints;

	for(int i = 0; i < INT_HW_MIN; i++) {
		if(ints[i]) {
			ints[i] = 0;
			ATOMIC_DEC(cctx->int_present);
			return i + INT_HW_MIN;
		}
	}

	fprintf(stderr, "FATAL: int_icu_get\n");
	exit(-1);
	return 0;
}

_va_int void int_handle_exception(byte no, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword cr1 = cctx->cregs[CPU_CREG_CR1];

	if(cctx->cregs[CPU_CREG_CR1] & CR1_EXCEPTION)
		cctx->cregs[CPU_CREG_CR1] |= CR1_FAILURE;
	else
		cctx->cregs[CPU_CREG_CR1] |= CR1_EXCEPTION;

	/* Ok, load position of interrupt handler from IVT */
	dword pos = mem_get_dword_p(INT_IVT + sizeof(dword)*no, ectx);

	#if DUMP_INT || DUMP_ALL
	PUTLOG(ectx, "[INT] :: ISR Address of interrupt %d is %08x!", no, pos);
	#endif

	if(pos == 0x00u) {
		int_fire_interrupt(INT_EXC_EXCNOTHLD, ectx);
		return;
	}

	
	
	// Ok. Handle that exception
	#if DUMP_INT || DUMP_ALL
	PUTLOG(ectx, "[INT] :: Handling the exception %d!", no);
	#endif

	dword sp = cctx->cregs[CPU_CREG_SSP]; //save stack pointer

	cctx->cregs[CPU_CREG_CR1] |= CR1_PRIV_MODE; // enter privileged mode
	cctx->cregs[CPU_CREG_CR1] &= ~CR1_INT_ENABLED; //disable interrupts
	cctx->cregs[CPU_CREG_CR1] |= CR1_EXCEPTION; // set the exception flag
	
	/* Push CR1 to stack */
	if(!cpu_push_dword(&cctx->cregs[CPU_CREG_SSP], cr1, ectx)) {
		//TODO: fault
		cctx->cregs[CPU_CREG_SSP] = sp;
		return;
	}

	/* Push position to stack */
	if(!cpu_push_dword(&cctx->cregs[CPU_CREG_SSP], cctx->pos, ectx)) {
		//TODO: fault
		cctx->cregs[CPU_CREG_SSP] = sp;
		return;
	}


	cctx->pos = pos;
}

_va_int void int_handle_interrupt(byte no, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	/* Ok, load position of interrupt handler from IVT */
	dword pos = mem_get_dword_p(INT_IVT + sizeof(dword)*no, ectx);
	
	#if DUMP_INT || DUMP_ALL
	PUTLOG(ectx, "[INT] :: ISR Address of interrupt %d is %08x!", no, pos);
	#endif

	if(pos == 0x00u) {
		return;
	}
	
	// Ok. Handle that exception
	#if DUMP_INT || DUMP_ALL
	printf("[INT] :: Handling the interrupt %d!\n", no);
	#endif


	dword sp = cctx->cregs[CPU_CREG_SSP]; //save stack pointer
	
	dword cr1 = cctx->cregs[CPU_CREG_CR1];
	cctx->cregs[CPU_CREG_CR1] |= CR1_PRIV_MODE; // enter privieged mode
	cctx->regs[CPU_CREG_CR1] &= ~CR1_INT_ENABLED; //disable interrupts
	
	/* Push CR1 to stack */
	if(!cpu_push_dword(&cctx->cregs[CPU_CREG_SSP], cr1, ectx)) {
		//TODO: fault
		cctx->cregs[CPU_CREG_SSP] = sp;
		return;
	}

	/* Push position to stack */
	if(!cpu_push_dword(&cctx->cregs[CPU_CREG_SSP], cctx->pos, ectx)) {
		//TODO: fault
		cctx->cregs[CPU_CREG_SSP] = sp;
		return;
	}

	cctx->pos = pos;
}

_va_int void int_handle_return(emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;

	dword sp = cctx->cregs[CPU_CREG_SSP]; //save stack pointer
	
	/* Restore position */
	if(!cpu_pop_dword(&cctx->cregs[CPU_CREG_SSP], &cctx->pos, ectx)) {
		cctx->cregs[CPU_CREG_SSP] = sp;
		return;
	}

	/* Restore CR1 */
	if(!cpu_pop_dword(&cctx->cregs[CPU_CREG_SSP], &cctx->regs[CPU_CREG_CR1], ectx)) {
		cctx->cregs[CPU_CREG_SSP] = sp;
		return;
	}
}

_va_int void int_fire_interrupt(byte no, emulator_context* ectx) {
	cpu_context* cctx = ectx->cpu_context;
	byte* hw_ints = cctx->ints;

	#if DUMP_INT || DUMP_ALL
	PUTLOG(ectx, "[INT] :: Firing %d", no);
	#endif

	ectx->statistic_context->interrupt_count++;
	
	/* Hardware Interrupt Handling */
	if((no >= INT_HW_MIN) && (no <= INT_HW_MAX)) {
		//ITU Translation
		byte idx = no - INT_HW_MIN;
		byte translated = itt_cached[idx];

		if(translated > 127) { //Illegal
			int_fire_interrupt(INT_EXC_ITU_FAULT, ectx);
			return;
		}

		#if DUMP_ALL || DUMP_INT
		PUTLOG(ectx, "[INT] :: %d translated to %d", no, translated + INT_HW_MIN);
		#endif

		no = translated + INT_HW_MIN;

		if(!hw_ints[no - INT_HW_MIN]) {	//fire same interrupt only once
			hw_ints[no - INT_HW_MIN] = 1;
			ATOMIC_INC(cctx->int_present); //inform the CPU that an interrupt is present
		}

		return;
	}

	/* Exception handling (sync) */
	if((no >= INT_EXC_MIN) && (no <= INT_EXC_MAX)) {

		/* Are we already in an exception? */
		if(cctx->cregs[CPU_CREG_CR1] & CR1_EXCEPTION) {
			/* Triple fault or failure? */
			if(cctx->cregs[CPU_CREG_CR1] & CR1_FAILURE) {
				printf("TRIPLE FAULT!\n");
				exit(99);
				return;
			} else {
				no = INT_EXC_DBLEFAULT;	//fire double fault instead
			}
		}

		int_handle_exception(no, ectx);

		return;
	} 

	else { //Software Interrupt (sync)
		int_handle_interrupt(no, ectx);

		return;
	}
}

_va_int int int_enabled(emulator_context* ectx) {

	#if DUMP_INT || DUMP_ALL
	if(ectx->cpu_context->cregs[CPU_CREG_CR1] & CR1_INT_ENABLED) {
		PUTLOG2(ectx, "[INT] :: HW-INT enabled");
	}
	else {
		PUTLOG2(ectx, "[INT] :: HW-INT disabled");
	}
	#endif


	return ectx->cpu_context->cregs[CPU_CREG_CR1] & CR1_INT_ENABLED;
}
