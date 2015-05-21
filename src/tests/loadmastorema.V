;	Tests the LOADMA and STOREMA instructions
	XOR	R0, R0, R0
	LOADI	R0LL, 200
	XOR	R2, R2, R2	; Blank registers
	XOR	R3, R3, R3
	XOR	R4, R4, R4
	XOR	R5, R5, R5
	ADD	R2, R2, 1	; Put some magic values in them
	ADD	R3, R3, 2
	ADD	R4, R4, 3
	ADD	R5, R5, 4
	STOREMA	R0, 37		; 37 = 0x25 = R2,R3,R4,R5
	XOR	R1, R1, R1
	LOAD32	R1, @184		; Address should be 200 - 16
	CMP	R0, R1
	FAILNZ	0x01
	XOR	R2, R2, R2	; Blank the registers again
	XOR	R3, R3, R3
	XOR	R4, R4, R4
	XOR	R5, R5, R5
	LOADMA	R0, 37		; Try to load them again
	LOAD32	R1, @200		; Address should be 200 again
	CMP	R0, R1		
	FAILNZ	0x02
	LOAD32	R1, @1		; R2 must contain magic value 1
	CMP	R1, R2
	FAILNZ	0x03
	ADD	R1, R1, 1	; R3 must contain magic value 2
	CMP	R1, R3
	FAILNZ	0x04
	ADD	R1, R1, 1	; R4 must contain magic value 3
	CMP	R1, R4
	FAILNZ	0x05
	ADD	R1, R1, 1	; R5 must contain magic value 4
	CMP	R1, R5
	FAILNZ	0x06
	ADD	R1, R1, 1 	; Magic value should now be 5
	LOAD32	R0, @5
	CMP	R0, R1
	FAILNZ	0x07
	STOP
