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

#include <common/types.h>
#include <stdlib.h>

#include "common/lookup.h"

#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>

#include "fragment_stack.h"
#include "memory_inspector.h"
#include "gui_helper.h"


fragment_stack* fragment_stack_new(emulator_context* ectx, GtkWindow* window){
	fragment_stack *fragment = (fragment_stack*) g_malloc(sizeof(fragment_stack));
	fragment->ectx = ectx;
	fragment->base = ectx->cpu_context->regs[CPU_REG_SP];
	fragment->window = window;
	
	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);
	
	//Create the frames
	fragment->frame_stack = gtk_frame_new("Stack");

	// Set up the stack 
	fragment->tv_stack = gtk_tree_view_new();
	GtkWidget* scroll_stack = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroll_stack), fragment->tv_stack);
	fragment->lst_stack = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_stack),
		-1,
		"Address",
		renderer,
		"text", 0,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_stack),
		-1,
		"Hex Value",
		renderer,
		"text", 1,
		NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(fragment->tv_stack), GTK_TREE_MODEL(fragment->lst_stack));

	GtkWidget* vb_stack = gtk_vbox_new(FALSE, 0);
	GtkWidget* hb_stack = gtk_hbox_new(FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vb_stack), GTK_WIDGET(scroll_stack), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb_stack), GTK_WIDGET(hb_stack), FALSE, TRUE, 0);

	fragment->entry_base = gtk_entry_new();
	fragment->btn_base = gtk_button_new_with_label("Set base");
	gtk_signal_connect(GTK_OBJECT (fragment->btn_base), "clicked",
		GTK_SIGNAL_FUNC (fragment_stack_btn_base_clicked),
		(gpointer) fragment);
	

	gtk_box_pack_start(GTK_BOX(hb_stack), GTK_WIDGET(fragment->entry_base), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hb_stack), GTK_WIDGET(fragment->btn_base), FALSE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(fragment->frame_stack), GTK_WIDGET(vb_stack));
	
	return fragment;
}

void fragment_stack_update(gpointer data){
	fragment_stack* fragment = (fragment_stack*) data;
	
	
	GtkTreeIter iter;
	
	dword base = fragment->base;
	dword bp = fragment->ectx->cpu_context->regs[CPU_REG_SP];
	
	dword item_count;
	if (bp >= base) {
		item_count = 0;
	}else{
		item_count = (base - bp)/4;
	}
	
	gtk_list_store_clear(fragment->lst_stack);
	
	for(int i = 0; i < item_count; i++) {
		char buffer_addr[10];
		char buffer_hex[128];
		dword value;
		gui_mem_get_dword_v(base - (4 * i), &value, fragment->ectx);
		snprintf(buffer_addr, 10, "%08x", base - (4*i));
		snprintf(buffer_hex, 128, "%08x", 
			 value
		);
		
		gtk_list_store_append(fragment->lst_stack, &iter);
		gtk_list_store_set(fragment->lst_stack, &iter,
			0, buffer_addr,
			1, buffer_hex,
			-1
		);
	}
	
}

void fragment_stack_btn_base_clicked(GtkWidget* widget, gpointer data){
	fragment_stack* fragment = (fragment_stack*)data;
	
	fragment->base = parse_address(
		fragment->window,
		gtk_entry_get_text(GTK_ENTRY(fragment->entry_base)),
		0,
		4,
		fragment->ectx->memory_context->memory_size - 1
	);
	
	fragment_stack_update(fragment);
}
