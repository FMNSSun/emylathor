#include "arch/va/mmu.rl"
#include "arch/va/int.rl"

?? Maps 8192 to a non-writeable page
?? and tests if an access violation page fault occurs

PROCEDURE main ( NONE ) 
LOCALS ( pdAdr; ptAdr; codeStart; ctrlBits; dummy; ) IS
	?? We place our Page Directory at 1MB = 1048576
	?? and our Page Table at 1MB + 4KB (because the Page Directory is 4KB in size) = 1052672
	pdAdr := 1048576;
	ptAdr := 1052672;
	codeStart := 4096; ?? on va code is loaded to address 4096
	
	?? Write a magic value to 8192 before enabling the mmu
	@REG:8192 := 12345;
	
	vmmSetPDEntry [ ?? Register our Page Table
			pdAdr;
			vmmGetPDIndexOfPhyAdr [ codeStart; ];
			ptAdr;
		];
		
	?? Paging Stuff for Code
	ctrlBits := vmmConControlBits [ ?? Set up our Control Bits
			0;
			0;
			0;
			0;
			0;
			1; ?? Executable
			0;
			1; ?? Present
		];
		
	vmmSetPTEntry [
			ptAdr;
			vmmGetPTIndexOfPhyAdr [ codeStart; ];
			vmmGetFrameBitsOfPhyAdr [ codeStart; ];
			ctrlBits;
		];
		
	?? Paging Stuff for Stack
	ctrlBits := vmmConControlBits [ ?? Set up our Control Bits
			0;
			0;
			0;
			1; ?? Write-Privileged
			1; ?? Write-Unprivileged
			0;
			0;
			1; ?? Present
		];
		
	vmmSetPTEntry [
			ptAdr;
			vmmGetPTIndexOfPhyAdr [ 0; ];
			vmmGetFrameBitsOfPhyAdr [ 0; ];
			ctrlBits;
		];
		
	?? Map 8192 as non writeable
	ctrlBits := vmmConControlBits [ ?? Set up our Control Bits
			0;
			0;
			0;
			0; ?? Write-Privileged
			0; ?? Write-Unprivileged
			0;
			0;
			1; ?? Present
		];
	
	
	vmmSetPtEntry [
			ptAdr;
			vmmGetPTIndexOfPhyAdr [ 8192; ];
			vmmGetFrameBitsOfPhyAdr [ 8192; ];
			ctrlBits;
		];
	
		
	mmuSetPDAddress [ pdAdr; ]; ?? Register our Page Directory
	
	?? Register isr
	intRegisterIsr [ 5; .isrPageFault; ]; ?? 5 == EXC PAGE FAULT
		
	mmuEnable [ NONE ];
	
	?? Read from that location
	dummy := (@REG:8192); ?? this must not fail
	
	IF (dummy != 12345) THEN ?? read has failed or returned an incorrect result
		FAIL;
	END
	
	?? Write to that loctaion (this MUST fail)
	@REG:8192 := 9876;
	
	
	FAIL;
END

PROCEDURE checkMr4 ( NONE ) 
LOCALS ( errCode; ) IS
	$FROMCTRL `errCode :: mr4;
	
	IF (errCode != 2) THEN ?? 2 == ACCESS VIOLATION
		FAIL;
	END
	
	STOP;
END

IPROCEDURE isrPageFault IS
	SAVE;
	
	checkMr4 [ NONE ];
	
	RESTORE;
END
