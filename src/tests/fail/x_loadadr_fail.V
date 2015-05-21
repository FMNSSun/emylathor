; It should not be possible to load an address into registers > R7

	.label _test
	LOADADR	R8,	_test
	STOP