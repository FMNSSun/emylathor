; Tests a combination of XOR and CMP
;  this test is actually just a demo on how
;  to write tests.
	XOR	R0, R0, R0	; Clear R0
	XOR	R1, R1, R1	; Clear R1
	CMP	R0, R1
	FAILNZ	0x01		; Fail if not equal
	STOP			; Stop with success
