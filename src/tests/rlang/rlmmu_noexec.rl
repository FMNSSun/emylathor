#include "arch/va/mmu.rl"
#include "arch/va/int.rl"

?? Tests the executable flag (privileged)
?? Maps physical address 8192 to a non executable
?? page and gotos to that loctian

PROCEDURE main ( NONE ) 
LOCALS ( pdAdr; ptAdr; codeStart; ctrlBits; ) IS
	?? We place our Page Directory at 1MB = 1048576
	?? and our Page Table at 1MB + 4KB (because the Page Directory is 4KB in size) = 1052672
	pdAdr := 1048576;
	ptAdr := 1052672;
	codeStart := 4096; ?? on va code is loaded to address 4096
	
	
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
		
	?? Map 8192 as non executable
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
	
	?? Jump to 8192 provoking an exception
	GOTO : 8192;
	
	FAIL;
END

PROCEDURE checkMr4 ( NONE ) 
LOCALS ( errCode; ) IS
	$FROMCTRL `errCode :: mr4;
	
	IF (errCode != 3) THEN ?? 3 == NOT EXECUTABLE
		FAIL;
	END
	
	STOP;
END

IPROCEDURE isrPageFault IS
	SAVE;
	
	checkMr4 [ NONE ];
	
	RESTORE;
END
