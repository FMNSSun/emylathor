STATIC foobar := 0;

PROCEDURE main ( NONE )
LOCALS ( ivt; foo; ref; ) IS
  ivt := 0;
  
  @REG:ivt|0 := .isrDivByZero;
  @REG:.foobar := `ref;

  foo := (foo / 0);

  IF (ref == 5) THEN
    STOP;
  END

  FAIL;
END

IPROCEDURE isrDivByZero IS
  @REG:(@REG:.foobar) := 5;
  ASM~ iret;
END
