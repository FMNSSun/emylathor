#include "std/string.rl"

STATIC STRING astr5 := '12345';
STATIC STRING bstr5 := 'abcde';

PROCEDURE main ( NONE )
LOCALS ( NONE ) IS
  IF strcmp [ .astr5; .bstr5; ] THEN
    FAIL;
  END

  strcpy [ .astr5; .bstr5; ];

  IFE strcmp [ .astr5; .bstr5; ] THEN
    NONE;
  ELSE
    FAIL;
  END

  STOP;
END
