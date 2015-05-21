PROCEDURE main ( NONE )
LOCALS ( NONE ) IS
  WHILE (1 == 1) LOOP
    GOTO abort;
  END
LABEL abort;
  IF ((1 << 3) != 8) THEN
    FAIL;
  END
  STOP;
END
