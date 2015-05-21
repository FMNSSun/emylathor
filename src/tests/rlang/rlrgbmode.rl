STATIC wait := 1;

PROCEDURE main ( NONE )
LOCALS ( vram; vport; res; idx; r; ) IS
	vport := 16;
	vram := 8192;
	res := 144000;
	idx := 0;
	
	@REG:0|144 := .isrVagc;
	
	?? Enable hardware interrupts
	$FROMCTRL `r :: cr1;
	r += 1;
	$TOCTRL cr1 :: `r;
	

	$OUTPORT vport :: 0;	?? COMMAND
	$OUTPORT vport :: 2;	?? MODE RGB
	
	?? e0 = Red Pixel = 224
	WHILE (idx < res) LOOP
		@BYTE:vram|idx := 224;
		idx += 1;
	END
	
	$OUTPORT vport :: vram;
	
	WHILE (@REG:.wait) LOOP
		NONE;
	END
	@REG:.wait := 1;
	
	idx := 0;
	
	?? green pixel = 28
	WHILE (idx < res) LOOP
		@BYTE:vram|idx := 28;
		idx += 1;
	END
	
	idx := 0;
	
	$OUTPORT vport :: vram;
	
	WHILE (@REG:.wait) LOOP
		NONE;
	END
	@REG:.wait := 1;
	
	?? blue pixel = 3
	WHILE (idx < res) LOOP
		@BYTE:vram|idx := 3;
		idx += 1;
	END
	
	$OUTPORT vport :: vram;
	
	WHILE (@REG:.wait) LOOP
		NONE;
	END
	@REG:.wait := 1;
	
	STOP;
END


IPROCEDURE isrVagc IS
	ASM~ mov r8, r0
	ASM~ mov r9, r1
	
	@REG:.wait := 0;
	
	ASM~ mov r0, r8
	ASM~ mov r1, r9
	
	$IRET;
END
