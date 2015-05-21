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

#include "vkbc.h"
#include "cpu/pio.h"
#include "cpu/interrupts.h"
#include "SDL/SDL.h"

static dword vkbc_last_state = 0x00u;
static int vkbc_fire_next = 1;

void vkbc_port_handler(int read, dword* data, emulator_context* ectx) {
	vkbc_fire_next = 1;
	if(read) {
		*data = vkbc_last_state;
	}
	else {	
		return; //nothing to do here
	}
}

void* vkbc_background(void* ptr) {
	emulator_context* ectx = (emulator_context*)ptr;

	SDL_Event event;

	while(1) {
		if(SDL_WaitEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				if(vkbc_fire_next) {
					vkbc_last_state = 0xF0000000u | event.key.keysym.scancode;
					int_fire_interrupt(VKBC_PORT + 128, ectx);
					vkbc_fire_next = 0;
				}
				printf("keydown = %08x\n", vkbc_last_state);
			}
			if (event.type == SDL_KEYUP) {
				if(vkbc_fire_next) {
					vkbc_last_state = event.key.keysym.scancode;
					int_fire_interrupt(VKBC_PORT + 128, ectx);
					vkbc_fire_next = 0;
				}
				printf("keyup = %08x\n", vkbc_last_state);
			}

			if (event.type == SDL_QUIT) {
				exit(30);
			}
		}
	}

	return NULL;
}

void vkbc_init(emulator_context* ectx) {
	#if !GLIB_CHECK_VERSION(2,32,0)
	GThread* vkbct = g_thread_create(vkbc_background, ectx, FALSE, NULL);
	#else
	GThread* vkbct = g_thread_new("vkbc_thread",vkbc_background, ectx);
	#endif
	(void)vkbct;
	pio_register_port(VKBC_PORT, &vkbc_port_handler);
}
