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

#include "gui_helper.h"
#include <common/types.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <gio/gio.h>

void show_error_dialog(GtkWindow* parent_window, const char* format, ...){
	
	char buffer[256];
	va_list args;
	va_start (args, format);
	vsprintf (buffer,format, args);
	va_end (args);
	
	GtkWidget* dialog = gtk_message_dialog_new(
		parent_window,
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_OK,
		"%s",
		buffer
	);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

dword parse_address(GtkWindow* parent_window, const char* str, dword offset, dword alignment, dword max_value){
	char* endptr;
	char str_addr[10];
	strncpy(str_addr, str, 10);
	str_addr[9] = '\0';
	
	int len = strlen(str_addr);
	if(len <= 0){
		show_error_dialog(parent_window,"%s","Please provide an address!");
		return 0;
	}
	
	guint64 parsed_value = g_ascii_strtoull(str_addr, &endptr, 16);
	if(parsed_value == 0 && endptr == str_addr){
		show_error_dialog(parent_window, "Invalid address: %s", str_addr);
		return 0;
	}
	
	if(parsed_value < UINT32_MAX){
		if(offset + parsed_value >= max_value){
			parsed_value = max_value;
		}
		if(parsed_value % alignment != 0){
			show_error_dialog(parent_window, "Address is not aligned to %d bytes: %s", alignment, str_addr);
			return 0;
		}
		return parsed_value;
	} 
	return max_value;
}