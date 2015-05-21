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

#ifndef FRAGMENT_CODE_H
#define FRAGMENT_CODE_H


#include <gtk/gtk.h>
#include "common/types.h"
#include "disassembler/lib_vasmdis.h"

typedef struct {
	emulator_context* ectx;
	dword show_pos;
	GtkWidget* tv_code;
	GtkListStore* lst_code;
	GtkWidget* entry_code;
	GtkWidget* btn_showat;
	GtkWidget* btn_jump;
	GtkWidget* btn_call;
	GtkWidget* frame_code;
	GHashTable* breakpoints;
	void (*fragment_refreshed_callback)(void*);
	GtkWindow* window;
	void* wm;
	int* stopped;
	
} fragment_code;

fragment_code* fragment_code_new(int* stopped, emulator_context* ectx, GtkWindow* window);
void fragment_code_update(fragment_code *data);
void fragment_code_btn_jmp_clicked(GtkWidget* widget, gpointer data);
void fragment_code_btn_call_clicked(GtkWidget* widget, gpointer data);
void fragment_code_btn_showat_clicked(GtkWidget* widget, gpointer data);
void fragment_code_fill_dummy_data(fragment_code* fragment);
void fragment_code_clear(fragment_code* fragment);
char* replace_tabs(char* ptr);
void fragment_code_jump_to_address(const gchar* address, fragment_code* fragment);
#endif
