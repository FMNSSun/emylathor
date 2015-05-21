#include "arch/va/mmu.rl"

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
			1; ?? Executable
			0;
			1; ?? Present
		];
		
	vmmSetPTEntry [
			ptAdr;
			vmmGetPTIndexOfPhyAdr [ 0; ];
			vmmGetFrameBitsOfPhyAdr [ 0; ];
			ctrlBits;
		];
		
	mmuSetPDAddress [ pdAdr; ]; ?? Register our Page Directory
		
	mmuEnable [ NONE ];
	
	STOP;
END

