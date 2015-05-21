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

#include "vagc.h"
#include "cpu/pio.h"
#include "cpu/dma.h"
#include "cpu/interrupts.h"

#include <stdio.h>
#include <sys/time.h>
#include <stdint.h>
#include <sys/wait.h>
#include <unistd.h>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"

static uint8_t colors[16] = {
	0x00u, // Black 0
	0x30u, // Red 1
	0x0Cu, // Green 2
	0x03u, // Blue 3
	0x3Cu, // Yellow 4
	0x33u, // Magenta 5
	0x0Fu, // Cyan 6
	0x10u, // Dark red 7
	0x04u, // Dark green 8
	0x01u, // Dark blue 9
	0x14u, // Dark yellow A
	0x11u, // Dark magenta B
	0x05u, // Dark cyan C
	0x3fu, // White D
	0x2au, // Gray E
	0x15u, // Dark gray
};

static SDL_Surface* screen;
static SDL_Surface* font;
static byte* text_buffer;
static byte* rgb_buffer;
static byte await_command;
static byte mode;

//TODO: threaded??

void vagc_port_handler(int read, dword* data, emulator_context* ectx) {
	if(read) {
		*data = 0x00u;
	}
	else {	
		dword addr = *data;

		if(addr == 0x00u) {
			await_command = 1;
			return;
		}

		if(await_command) {
			if(addr == VAGC_MODE_TEXT)
				mode = VAGC_MODE_TEXT;
			else if(addr == VAGC_MODE_RGB)
				mode = VAGC_MODE_RGB;
			else if(addr == VAGC_MODE_COLORS)
				mode = VAGC_MODE_COLORS;

			await_command = 0;
			return;
		}

		if(mode == VAGC_MODE_COLORS) {
			byte idx = addr & 0x0Fu;
			byte clr = (addr >> 24) & 0xFFu;
			colors[idx] = clr;
			return;
		}

		if(mode == VAGC_MODE_TEXT)
			dma_request_transfer(addr, text_buffer, VAGC_TEXT_BUFFER_SIZE, DMA_READ, vagc_render_text_buffer, ectx);
		else if(mode == VAGC_MODE_RGB)
			dma_request_transfer(addr, rgb_buffer, VAGC_RGB_BUFFER_SIZE, DMA_READ, vagc_render_rgb_buffer, ectx);
	}
}

void vagc_init() {
	screen = SDL_SetVideoMode(480, 300, 32, SDL_SWSURFACE);	
	printf("screen->format->BytesPerPixel := %d\n", screen->format->BytesPerPixel);
	font = IMG_Load("./misc/shyxclic.png");
	text_buffer = calloc(VAGC_TEXT_BUFFER_SIZE, sizeof(byte));
	rgb_buffer = calloc(VAGC_RGB_BUFFER_SIZE, sizeof(byte));

	mode = VAGC_MODE_TEXT;

	SDL_WM_SetCaption("vagc","vagc");

	pio_register_port(VAGC_PORT, &vagc_port_handler);
}

void vagc_render_rgb_buffer(emulator_context* ectx) {
	SDL_LockSurface(screen);
	for(int y = 0; y < VAGC_RES_Y; y++) {
		for(int x = 0; x < VAGC_RES_X; x++) {
			int bufloc = (y * VAGC_RES_X) + x;
		
			byte bufpix = rgb_buffer[bufloc];

			uint32_t r, g, b;
			r = 36 * ((bufpix >> 5) & 7u);
			g = 36 * ((bufpix >> 2) & 7u);
			b = 85 * (bufpix & 3u);

			uint32_t pix = SDL_MapRGB(screen->format, r, g, b);
			((uint32_t*)screen->pixels)[x + y * screen->w] = pix;
		}
	}
	SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);

	int_fire_interrupt(INT_HW_MIN + VAGC_PORT, ectx);
}

void vagc_render_text_buffer(emulator_context* ectx) {

	SDL_LockSurface(screen);

	dword i;
	for(i = 0; i < VAGC_TEXT_BUFFER_SIZE; i+=2) {
		// Byte 1 holds the background and foreground color
		uint8_t color = (uint8_t)text_buffer[i];
		// Byte 2 holds the actual character
		unsigned char chr = text_buffer[i + 1];

		// Coordinates of the character
		int p = i / 2;

		uint8_t ty = (p / 80); 
		uint8_t tx = (p % 80);

		if(ty == 0 && tx == 0) printf("try to write %2x to %d,%d\n", chr, tx, ty);

		// Coordinates of the character in the font bitmap
		uint32_t by = (chr / VAGC_BMPF_CHARS_PER_LINE) * VAGC_BMPF_CHAR_HEIGHT;
		uint32_t bx = (chr % VAGC_BMPF_CHARS_PER_LINE) * VAGC_BMPF_CHAR_WIDTH;

		// Coordinates of the character in the destination screen
		uint32_t sx = (tx * VAGC_BMPF_CHAR_WIDTH);
		uint32_t sy = (ty * (VAGC_BMPF_CHAR_HEIGHT + 1));

		//printf("sy := %d\n", sy);

		// Foreground and background color from color table
		uint8_t fcolor = colors[color & 0xF];
		uint8_t bcolor = colors[(color >> 4u) & 0xF];

		// Calculate the RGB values
		uint32_t fr = ((fcolor >> 4u) & 0x3u) * 85u;
		uint32_t fg = ((fcolor >> 2u) & 0x3u) * 85u;
		uint32_t fb = ((fcolor >> 0u) & 0x3u) * 85u;
		uint32_t br = ((bcolor >> 4u) & 0x3u) * 85u;
		uint32_t bg = ((bcolor >> 2u) & 0x3u) * 85u;
		uint32_t bb = ((bcolor >> 0u) & 0x3u) * 85u;

		int x, y;

		// Draw the character
		for(x = 0; x < VAGC_BMPF_CHAR_WIDTH; x++) {
			for(y = 0; y < VAGC_BMPF_CHAR_HEIGHT; y++) {
				uint32_t fontpix = ((uint32_t*)font->pixels)[(bx+x) + (by+y) * font->w];
				uint8_t f;
				SDL_GetRGB(fontpix, font->format, &f, &f, &f);
				if(f == 0xFF)
					fontpix = SDL_MapRGB(screen->format, fr, fg, fb);
				else
					fontpix = SDL_MapRGB(screen->format, br, bg, bb);
				((uint32_t*)screen->pixels)[(sx+x) + (sy+y) * screen->w] = fontpix;
				//printf("sx + x := %d , sy + y := %d\n", (sx + x), (sy+y));
			}
		}

		if(ty < 25) {
			for(x = 0; x < 6; x++) {
				uint32_t fontpix = SDL_MapRGB(screen->format, br, bg, bb);
				((uint32_t*)screen->pixels)[(sx+x) + (sy + 11) * screen->w] = fontpix;
				//printf("sx + x := %d , sy + 11 := %d\n", (sx + x), (sy+11));
			}
		}
		
	} 

	printf("finish\n");

	SDL_UnlockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	int_fire_interrupt(INT_HW_MIN + VAGC_PORT, ectx);
}
