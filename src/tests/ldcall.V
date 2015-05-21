;
; Tests the LDCALL pseudo instruction
;
	ldcall	r0,	_my_label
	fail 0x01
.label _my_label
	stop
	fail 0x01
