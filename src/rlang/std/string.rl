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

?? @FUNCTION strlen
??   @PARAM ptr Pointer to a 0-terminated string
??  @RETURN Length of the string
PROCEDURE strlen ( ptr; )
LOCALS ( len; ) IS
	len := 0;

	WHILE ((@BYTE:ptr) != 0) LOOP
		len += 1;
		ptr += SIZEOF BYTE;
	END

	RETURN len;	
END

?? @FUNCTION strcpy
??   @PARAM src Pointer to a 0-terminated string
??   @PARAM dst Pointer to a buffer
??  @RETURN dst
?? Copies the string from src to dst. dst buffer must be big 
?? enough to hold src (incl. 0 byte). 
PROCEDURE strcpy ( src; dst; )
LOCALS ( idx; ) IS
	idx := 0;

	WHILE ((@BYTE:src|idx) != 0) LOOP
		@BYTE:dst|idx := (@BYTE:src|idx);
		
		idx += SIZEOF BYTE;
	END

	@BYTE:dst|idx := (@BYTE:src|idx); ?? copy terminating 0

	RETURN dst;
END

?? @FUNCTION strcmp
??   @PARAM src Pointer to a 0-terminated string
??   @PARAM dst Pointer to a 0-terminated string
??  @RETURN 0 if src != dst. 1 if src == dst
PROCEDURE strcmp ( src; dst; )
LOCALS ( idx; ) IS
	idx := 0;
	
	?? If they don't have the same size they can't possibly
	?? be equal.
	IF (strlen [ src; ] != strlen [ dst; ]) THEN
		RETURN 0;
	END
	
	WHILE ((@BYTE:src|idx) != 0) LOOP
		?? Bytes do not match?
		IF ((@BYTE:dst|idx) != (@BYTE:src|idx)) THEN
			RETURN 0;
		END
		
		idx += SIZEOF BYTE;
	END
	
	RETURN 1;
END
