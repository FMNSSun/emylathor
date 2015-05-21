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

#ifndef WIN_MAIN_H
#define WIN_MAIN_H

#include <gtk/gtk.h>
#include <glib.h>
#include "common/definitions.h"
#include "fragment_registers.h"
#include "fragment_code.h"
#include "fragment_cache.h"
#include "fragment_stats.h"
#include "fragment_stack.h"
#include "fragment_log.h"
#include "win_mem.h"
#include "win_pd.h"

typedef struct {
	GtkWidget* window;
	win_mem* mem_window;
	win_pd* pd_window;
	emulator_context* ectx;
	fragment_code* fragment_code;
	fragment_registers* fragment_registers;
	fragment_cache* fragment_cache;
	fragment_stats* fragment_stats;
	fragment_stack* fragment_stack;
	fragment_log* fragment_log;
	gint keep_running;
	gint stopped;
} win_main;

win_main* win_main_new();
void fill_dummy_data(win_main* wm);
void fragment_code_refreshed();

#endif
