PROCEDURE sub ( a; b; )
LOCALS ( NONE) IS
  RETURN (a - b);
END

PROCEDURE mul ( a; b; )
LOCALS ( NONE ) IS
  RETURN (a * b);
END

PROCEDURE submul ( a; b; )
LOCALS ( c; ) IS
  c := mul [ 
		sub [
			mul [ a; a; ]; 
			mul [ b; b; ]; 
		];
		2; 
	];
  c += 1;			?? increment C

  @REG:8192 := c;		?? write C to address 8192
  c := 0;			?? zero c
  c := (@REG:(8190 + 2));	?? read C from address 8190 + 2 = 8192
  RETURN c;
END

PROCEDURE main ( NONE )
LOCALS ( NONE ) IS
  IF (submul [ 5; 4; ] != 19) THEN
    FAIL;
  END
  STOP;
END
