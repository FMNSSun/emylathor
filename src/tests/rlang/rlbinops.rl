PROCEDURE main ( NONE )
LOCALS ( a; b; ) IS
	# Equals
	IFE (5 == 5) THEN
		NONE;
	ELSE
		FAIL;
	END

	# Not Equals
	IFE (5 != 6) THEN
		NONE;
	ELSE
		FAIL;
	END

	IFE (5 != 5) THEN
		FAIL;
	ELSE
		NONE;
	END

	# Greater
	IFE (5 > 4) THEN
		NONE;
	ELSE
		FAIL;
	END

	IF (5 > 6) THEN
		FAIL;
	END

	IF (5 > 5) THEN
		FAIL;
	END

	# Greater Equal

	IFE (5 >= 4) THEN
		NONE;
	ELSE
		FAIL;
	END

	IF (5 >= 6) THEN
		FAIL;
	END

	IFE (5 >= 5) THEN
		NONE;
	ELSE
		FAIL;
	END

	# Smaller
	IFE (77 < 88) THEN
		NONE;
	ELSE
		FAIL;
	END

	IF (77 < 66) THEN
		FAIL;
	END

	IF (77 < 77) THEN
		FAIL;
	END

	# Smaller Equal
	IFE (77 <= 88) THEN
		NONE;
	ELSE
		FAIL;
	END

	IF (77 <= 66) THEN
		FAIL;
	END

	IFE (77 <= 77) THEN
		NONE;
	ELSE
		FAIL;
	END

	STOP;
END
