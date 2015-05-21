??
?? Copyright (c) 2013, Roman Müntener
??
?? All rights reserved.
?? Redistribution and use in source and binary forms, with or without
?? modification, are permitted provided that the following conditions
?? are met:
??
?? Redistributions of source code must retain the above copyright
?? notice, this list of conditions and the following disclaimer.
?? Redistributions in binary form must reproduce the above copyright
?? notice, this list of conditions and the following disclaimer in the
?? documentation and/or other materials provided with the distribution.
??
?? THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
?? 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
?? LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
?? FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
?? COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
?? INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
?? BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
?? LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
?? CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
?? LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
?? ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
?? POSSIBILITY OF SUCH DAMAGE.
??

?? @FUNCTION mmuEnable
??  @RETURN Old value of MR2
?? Enables the MMU
PROCEDURE mmuEnable ( NONE )
LOCALS ( mmuReg; ) IS
	$FROMCTRL `mmuReg :: mr2;
	mmuReg |= 1;
	$TOCTRL mr2 :: `mmuReg;
	
	RETURN mmuReg;
END

?? @FUNCTION mmuDisable
??  @RETURN Old value of MR2
?? Disables the MMU
PROCEDURE mmuDisable ( NONE )
LOCALS ( mmuReg; ) IS
	$FROMCTRL `mmuReg :: mr2;
	mmuReg &= (~~1);
	$TOCTRL mr2 :: `mmuReg;
	
	RETURN mmuReg;
END

?? @FUNCTION mmuSetPDAddress
??   @PARAM pdAdr Address of Page Directory
??  @RETURN 0
PROCEDURE mmuSetPDAddress ( pdAdr; )
LOCALS ( NONE ) IS
	$TOCTRL mr1 :: `pdAdr;
	
	RETURN 0;
END

?? @FUNCTION vmmSetPDEntry
??   @PARAM pdAdr Address of Page Directory
??   @PARAM pdIndex Index of the Page Directory Entry
??   @PARAM ptAdr Address of Page Table
??  @RETURN pdAdr
?? Sets the specified Page Directory Entry to
?? the given Page Table Address
PROCEDURE vmmSetPDEntry( pdAdr; pdIndex; ptAdr; )
LOCALS ( NONE ) IS
	@REG:pdAdr|pdIndex := ptAdr;
	
	RETURN pdAdr;
END

?? @FUNCTION vmmSetPTEntry
??   @PARAM ptAdr Address of Page Table
??   @PARAM ptIndex Index of the Page Table Entry
??   @PARAM frameBits Frame Bits
??   @PARAM ctrlBits Control Bits
??  @RETURN ptAdr
?? Sets the specified Page Table Entry according
?? to the given Frame- and Control-Bits
PROCEDURE vmmSetPTEntry ( ptAdr; ptIndex; frameBits; ctrlBits; )
LOCALS ( NONE ) IS
	@REG:ptAdr|ptIndex := ((frameBits << 12) || ctrlBits);
	
	RETURN ptAdr;
END

?? @FUNCTION vmmGetPDIndexOfPhyAdr
??   @PARAM phyAdr Physical Address
??  @RETURN PDINDEX Bits of phyAdr
PROCEDURE vmmGetPDIndexOfPhyAdr ( phyAdr; )
LOCALS ( NONE ) IS
	RETURN (phyAdr >> 22);
END

?? @FUNCTION vmmGetPTIndexOfPhyAdr
??   @PARAM phyAdr Physical Address
??  @RETURN PTINDEX Bits of phyAdr
PROCEDURE vmmGetPTIndexOfPhyAdr ( phyAdr; )
LOCALS ( NONE ) IS
	RETURN ((phyAdr >> 12) && 1023);
END

?? @FUNCTION vmGetOffsetBitsOfPhyAdr
??   @PARAM phyAdr Physical Address
??  @RETURN OFFSET Bits of phyAdr
PROCEDURE vmmGetOffsetBitsOfPhyAdr ( phyAdr; )
LOCALS ( NONE ) IS
	RETURN (phyAdr && 4095);
END

?? @FUNCTION vmGetFrameBitsOfPhyAdr
??   @PARAM phyAdr Physical Address
??  @RETURN FRAME Bits of phyAdr
PROCEDURE vmmGetFrameBitsOfPhyAdr ( phyAdr; )
LOCALS ( NONE ) IS
	RETURN (phyAdr >> 12);
END

?? @FUNCTION vmmConControlBits
??  @PARAM ruBit - Read-Unprivileged Bit
??  @PARAM euBit - Executable-Unprivileged Bit
??  @PARAM uBit - Used Bit
??  @PARAM wpBit - Write-Privileged Bit
??  @PARAM wuBit - Write-Unprivileged Bit
??  @PARAM epBit - Executable-Privileged Bit
??  @PARAM protBit - Protected Bit
??  @PARAM presentBit - Present Bit
??  @RETURN Control Bits
?? Constructs control bits out of parameters. Each parameter
?? must either be 0 or 1 (unsigned). 
PROCEDURE vmmConControlBits ( ruBit; euBit; uBit; wpBit; wuBit; epBit; protBit; presentBit; )
LOCALS ( val; ) IS
	val := 0;
	val |= (presentBit << 0);
	val |= (protBit << 1);
	val |= (epBit << 2);
	val |= (wuBit << 3);
	val |= (wpBit << 4);
	val |= (uBit << 5);
	val |= (euBit << 6);
	val |= (ruBit << 7);
	
	RETURN val;
END
