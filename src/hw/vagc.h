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

#ifndef VA_H_HW_VAGC_H
#define VA_H_HW_VAGC_H

#define VAGC_RES_X 480u
#define VAGC_RES_Y 300u
#define VAGC_TEXT_BUFFER_SIZE 4000u
#define VAGC_RGB_BUFFER_SIZE 144000u
#define VAGC_BMPF_CHARS_PER_LINE 32u
#define VAGC_BMPF_CHAR_HEIGHT 11u
#define VAGC_BMPF_CHAR_WIDTH 6u

#define VAGC_PORT 16u

#define VAGC_MODE_TEXT 1u
#define VAGC_MODE_RGB 2u
#define VAGC_MODE_COLORS 3u



#include "common/macros.h"
#include "common/definitions.h"

void vagc_init();
void vagc_render_rgb_buffer(emulator_context* ectx);
void vagc_render_text_buffer(emulator_context* ectx);

#endif
