STATIC ivt := 0;
STATIC keyCode := 0;

PROCEDURE main ( NONE )
LOCALS ( r; ) IS
	?? Register Int 128 (== keyboard)
	@REG:(@REG:.ivt)|128 := .isrVkbc;

	@BYTE:1025 := 0;

	?? Enable hardware interrupts
	$FROMCTRL `r :: cr1;
	r += 1;
	$TOCTRL cr1 :: `r;

	?? Keyup 't' = 45
	WHILE ((@REG:.keyCode) != 45) LOOP
		?? Busy wait
		NONE;
	END

	STOP;
END

IPROCEDURE isrVkbc IS
	ASM~ mov r8, r0
	ASM~ mov r9, r1
	
	?? Keybord is behind port number 1
	$INPORT .keyCode :: 1;

	ASM~ mov r0, r8
	ASM~ mov r1, r9

	$IRET; 
END
