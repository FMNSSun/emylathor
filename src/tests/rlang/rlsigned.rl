PROCEDURE main ( NONE )
LOCALS ( NONE ) IS
	IF (-1 S!= -1) THEN
		FAIL;
	END

	IF (-1 S== 1) THEN
		FAIL;
	END

	IF (-1 S> 0) THEN
		FAIL;
	END

	IF (-1 S< -9) THEN
		FAIL;
	END

	IF (-2 S<= -3) THEN
		FAIL;
	END

	IF (-4 S>= -3) THEN
		FAIL;
	END

	IF (NOT ( (-3 S* -3) S== 9 ) ) THEN
		FAIL;
	END

	IF (NOT ( (-3 S* 3) S== -9 ) ) THEN
		FAIL;
	END

	IF (NOT ( (-3 S+ 3) S== 0 ) ) THEN
		FAIL;
	END

	IF (NOT ( (-3 S+ -3) S== -6 ) ) THEN
		FAIL;
	END

	IF (NOT ( (-9 S/ -3) S== 3 ) ) THEN
		FAIL;
	END

	IF (NOT ( (-9 S/ 3) S== -3 ) ) THEN
		FAIL;
	END

	IF (NOT ( (-3 S- -4) S== 1 ) ) THEN
		FAIL;
	END

	STOP;
END
