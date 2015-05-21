STATIC STRING msg := 'emulathor';
STATIC vram := 8192;
STATIC BYTE blackWhite := 13;
STATIC vport := 16;

PROCEDURE main ( NONE )
LOCALS ( NONE) IS
	printStr [ #msg; ];

	WHILE (1 && 1) LOOP
		NONE;
	END
END

PROCEDURE printStr ( ptr; )
LOCALS ( vramPtr; idx; ) IS
	vramPtr := (@REG:#vram);
	idx := 0;

	IF (ptr == 0) THEN
		FAIL;
	END

	WHILE ((@BYTE:ptr) != 0) LOOP
		@BYTE:vramPtr|(2 * idx) := (@BYTE:#blackWhite);
		@BYTE:vramPtr|((2 * idx) + 1) := (@BYTE:ptr);
		ptr += 1;
		idx += 1;
	END

	$OUTPORT (@REG:#vport) :: vramPtr;

	RETURN idx;
END
