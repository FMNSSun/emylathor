LOAD32 R0, @-1
LOAD32 R1, @1
CMP R0, R1	; -1 signed is bigger than 1 signed
FAILIZ 0x01
FAILIC 0x02
FAILIZIC 0x03
FAILNS 0x04	; -1 unsinged is smaller than 1 unsigned
LOAD32 R2, @-4
CMP	R0, R2  ; -1 is signed bigger than signed -4
FAILIS 0x05	; it is also bigger unsigned
FAILIC 0x06
FAILIZ 0x07
STOP
