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

STATIC timerPort := 2;
STATIC timerCmdEnable := 1;
STATIC timerCmdDisable := 2;

?? @FUNCTION timerEnable
??  @RETURN 0
?? Enables the Timer
PROCEDURE timerEnable ( NONE )
LOCALS ( NONE ) IS
	$OUTPORT (@REG:.timerPort) :: 0;
	$OUTPORT (@REG:.timerPort) :: (@REG:.timerCmdEnable);
	
	RETURN 0;
END

?? @FUNCTION timerDisable
??  @RETURN 0
?? Disables the Timer
PROCEDURE timerDisable ( NONE )
LOCALS ( NONE ) IS
	$OUTPORT (@REG:.timerPort) :: 0;
	$OUTPORT (@REG:.timerPort) :: (@REG:.timerCmdDisable);
	
	RETURN 0;
END

?? @FUNCTION timerSetDelay
??   @PARAM delay Delay in Milliseconds
??  @RETURN 0
?? Sets the Delay of the Timer
PROCEDURE timerSetDelay ( delay; )
LOCALS ( NONE ) IS
	$OUTPORT (@REG:.timerPort) :: delay;
END

?? @FUNCTION timerGetDelay
??  @RETURN The Delay of the Timer in Milliseconds
PROCEDURE timerGetDelay ( NONE )
LOCALS ( delay; ) IS
	$INPORT `delay :: (@REG:.timerPort);
	RETURN delay;
END

