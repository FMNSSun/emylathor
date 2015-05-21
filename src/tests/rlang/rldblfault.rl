PROCEDURE main ( NONE )
LOCALS ( ivt; foo; ) IS
  ivt := 0;
  
  @REG:ivt|0 := 0;
  @REG:ivt|2 := .isrDblFault;

  foo := (foo / 0);

  FAIL;
END

IPROCEDURE isrDblFault IS
  STOP;
END
