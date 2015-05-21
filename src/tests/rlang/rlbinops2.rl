PROCEDURE main ( NONE )
LOCALS ( a; b; ) IS
	IF((5 == 5) != 1) THEN
		FAIL;
	END

	IFE ((5 > 5) OR (5 == 5)) THEN
		NONE;
	ELSE
		FAIL;
	END

	IF ((5 AND 5) != 1) THEN
		FAIL;
	END

	IF ((5 OR 0) != 1) THEN
		FAIL;
	END

	IFE ((7 && 4) == 4) THEN
		NONE;
	ELSE
		FAIL;
	END

	IFE (9 == (8 || 1)) THEN
		NONE;
	ELSE
		FAIL;
	END

	IFE ((5 > 4) AND (3 > 2)) THEN
		NONE;
	ELSE
		FAIL;
	END

	IFE ((5 > 6) OR (7 != 7)) THEN
		FAIL;
	ELSE
		NONE;
	END

	IFE (NOT (5 != 5)) THEN
		NONE;
	ELSE
		FAIL;
	END

	a := 511; b := 511;

	IF (NOT (a == b)) THEN
		FAIL;
	END

	IF ((~~ -1) != 0) THEN
		FAIL;
	END

	STOP;
END
