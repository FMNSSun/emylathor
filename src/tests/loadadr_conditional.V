	load32	r0, @0
	cmp	r0, 0
	loadadriz r1, _my_label
	loadadrnz r1, _my_label2
	loadadr	r2, _my_label
	cmp r1, r2
	failnz 0x01
	stop 

.label _my_label
	fail	0x02
.label _my_label2
	fail	0x03
