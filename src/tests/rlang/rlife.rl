PROCEDURE main ( NONE )
LOCALS ( NONE ) IS
  IFE (5 == 4) THEN
    FAIL;
  ELSE 
    IFE (1 == 1) THEN
      STOP;
    ELSE
      FAIL;
    END
  END
END
