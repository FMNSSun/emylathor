PROCEDURE main ( NONE )
LOCALS ( vram; vport; idx; r; ) IS
	vport := 16;
	vram := 8192;
	r := 0;
	idx := 0;

	WHILE (idx < (80 * 25)) LOOP
		@BYTE:vram|(2 * idx) := 61;
		@BYTE:vram|((2 * idx) + 1) := r;
		idx += 1;
		r += 1;
	END

	$OUTPORT vport :: vram;

	WHILE (1 == 1) LOOP
		NONE;
	END
END
