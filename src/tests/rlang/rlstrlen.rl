#include "std/string.rl"

STATIC STRING str5 := 'abcde';
STATIC STRING str7 := '1234567';

PROCEDURE main ( NONE )
LOCALS ( NONE ) IS
  IF ( strlen [ .str5; ] != 5) THEN
    FAIL;
  END

  IF ( strlen [ .str7; ] != 7) THEN
    FAIL;
  END

  STOP;
END
