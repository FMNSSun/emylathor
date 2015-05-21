/*
Copyright (c) 2013, Roman Muentener
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: Redistributions of
source code must retain the above copyright notice, this list of conditions and
the following disclaimer. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ’AS IS’ AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
*/

#include "emulator.h"
#include "cpu.h"
#include "memory.h"
#include "dma.h"
#include "pio.h"
#include "mkcontext.h"
#include "hw/devm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <gio/gio.h>

_va_emu void emu_load_img(char* path, emulator_context* ectx) {
	GFile* infile = g_file_new_for_path(path);

	char* filedata = NULL;
	gsize length = 0;

	gboolean suc = g_file_load_contents(
		infile,
		NULL,
		&filedata,
		&length,
		NULL,
		NULL);

	if(!suc)
	{
		fprintf(stderr, "Could not read image file. Sorry.\n");
		exit(11);
	}

	memcpy(ectx->memory_context->memory + INSTRUCTIONS_START_OFFSET, filedata, length);
	ectx->cpu_context->pos = INSTRUCTIONS_START_OFFSET;
}

int main(int argc, char* argv[]) {
	#if !GLIB_CHECK_VERSION(2,32,0)
	  g_type_init();
	  g_thread_init(NULL);
	#endif

	if(argc < 2)
		exit(10);

	printf("%d.%d\n", glib_major_version, glib_minor_version);

	emulator_context* ectx = emu_init();
	emu_load_img(argv[1], ectx);
	return cpu_execute(ectx, 0);
}
