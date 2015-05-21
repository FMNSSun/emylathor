/*
 * Copyright (c) 2013, Roman Müntener
 *
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * 'AS IS' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "vtimer.h"
#include "cpu/pio.h"
#include "cpu/interrupts.h"

#include <stdio.h>
#include <unistd.h>

static dword delay = 1000;
static int await_command = 0;
static int timer_enabled = 0;

void vtimer_port_handler(int read, dword* data, emulator_context* ectx) {
	if(read) {
		*data = delay;
	}
	else {	
		dword command = *data;

		if(command == 0) {
			await_command = 1;
			return;
		}
		
		if(await_command) {
			if(command == VTIMER_ENABLE)
				timer_enabled = 1;
			else if(command == VTIMER_DISABLE)
				timer_enabled = 0;
			await_command = 0;
			return;
		}

		delay = *data;
	}
}

void* vtimer_background(void* ptr) {
	emulator_context* ectx = (emulator_context*)ptr;

	while(1) {
		usleep((useconds_t)(delay*1000));
		if(timer_enabled) {
			int_fire_interrupt(INT_HW_MIN + VTIMER_PORT, ectx);
		}
	}

	return NULL;
}

void vtimer_init(emulator_context* ectx) {
	#if !GLIB_CHECK_VERSION(2,32,0)
	GThread* vtimert = g_thread_create(vtimer_background, ectx, FALSE, NULL);
	#else
	GThread* vtimert = g_thread_new("vtimer_thread",vtimer_background, ectx);
	#endif
	(void)vtimert;
	pio_register_port(VTIMER_PORT, &vtimer_port_handler);
}
