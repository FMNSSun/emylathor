	load32	r0, @0
	loadadr r1, _exc_divbyzero
	storeid r0, r1, 0

	load32	r0, @8
	loadadr r1, _exc_dblfault
	storeid r0, r1, 0

	div	r0, r0, 0
	fail	0x01

.label _exc_divbyzero
	div	r0, r0, 0
	fail	0x02

.label _exc_dblfault
	stop
	fail	0x03
