; Test parity flags
load32	r0, @1
test	r0
failnp	0x01	; 1 has odd number of bits
load32	r0, @3
test	r0
failip	0x02	; 3 has even number of bits
load32	r0, @0xe0300c00
test	r0
failnp	0x03
load32  r0, @0xe0300c01
test	r0
failip	0x04
stop
