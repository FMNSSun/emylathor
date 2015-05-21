#include "pio.h"

#include <stdio.h>

static port_handler ports[128];

void pio_init() {
	ports[0] = &pio_null;
}

dword pio_read(byte no, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_IO
	PUTLOG(ectx, "[PIO] :: Read from port %d", no);
	#endif

	if(ports[no] == NULL)
		return no;

	dword data;
	(ports[no])(1, &data, ectx);
	return data;
}

void pio_register_port(byte no, port_handler hndl) {
	if(no >= 128) {
		return;
	}
	ports[no] = hndl;
}

void pio_write(byte no, dword data, emulator_context* ectx) {
	#if DUMP_ALL || DUMP_IO
	PUTLOG(ectx, "[PIO] :: Write to port %d (data := %08x)", no, data);
	#endif

	if(ports[no] == NULL)
		return;

	(ports[no])(0, &data, ectx);
}

void pio_null(int read, dword* data, emulator_context* ectx) {
	static dword latch = 0x00u;

	if(read) {
		*data = latch;
	}
	else {	
		latch = *data;
	}
}
