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
#include "fragment_mem.h"
#include "memory_inspector.h"
#include "../common/types.h"

#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>

#include "gui_helper.h"

static void fragment_mem_btn_addr_clicked(GtkWidget* widget, gpointer data){
	fragment_mem* fragment = (fragment_mem*)data;
	
	fragment->pos = parse_address(fragment->window,gtk_entry_get_text(GTK_ENTRY(fragment->entry_addr)), 0, 16, fragment->ectx->memory_context->memory_size);
	
	fragment_mem_update(data);
}

fragment_mem* fragment_mem_new(emulator_context* ectx, GtkWindow* window, dword pos){
	
	fragment_mem* fragment = (fragment_mem*)g_malloc(sizeof(fragment_mem));
	fragment->ectx = ectx;
	fragment->pos = pos;
	fragment->window = window;
	
	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);

	fragment->frame_mem = gtk_frame_new("Memory");
	
	//Set up the Code-TreeView
	fragment->tv_mem = gtk_tree_view_new();
	GtkWidget* scroll_mem = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroll_mem), fragment->tv_mem);
	fragment->lst_mem = gtk_list_store_new(
		17, 
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
		G_TYPE_STRING
	);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_mem),
		-1,
		"offset\n----------\naddress",
		renderer,
		"text", 0,
		NULL);
	for(int i = 0; i < 16; i++){
		char col_buf[3];
		snprintf(col_buf, 3, "%02x", i);
		gtk_tree_view_insert_column_with_attributes(
			GTK_TREE_VIEW(fragment->tv_mem),
			-1,
			col_buf,
			renderer,
			"text", i+1,
			NULL
		);
	}
	gtk_tree_view_set_model(GTK_TREE_VIEW(fragment->tv_mem), GTK_TREE_MODEL(fragment->lst_mem));

	//Boxes for the Code-Frame
	GtkWidget* hb_mem = gtk_hbox_new(FALSE, 0);
	GtkWidget* vb_mem = gtk_vbox_new(FALSE, 0);

	gtk_container_add(GTK_CONTAINER(fragment->frame_mem), GTK_WIDGET(vb_mem));
	gtk_box_pack_start(GTK_BOX(vb_mem), GTK_WIDGET(scroll_mem), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb_mem), GTK_WIDGET(hb_mem), FALSE, TRUE, 0);

	//Entry and Buttons for the Code-Frame
	fragment->entry_addr = gtk_entry_new();
	fragment->btn_addr = gtk_button_new_with_label("Set address");
	
	gtk_box_pack_start(GTK_BOX(hb_mem), GTK_WIDGET(fragment->entry_addr), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hb_mem), GTK_WIDGET(fragment->btn_addr), FALSE, TRUE, 0);

	
	gtk_signal_connect(GTK_OBJECT (fragment->btn_addr), "clicked",
		GTK_SIGNAL_FUNC (fragment_mem_btn_addr_clicked),
		(gpointer) fragment);

	return fragment;
}

void fragment_mem_update(fragment_mem* fragment){
	
	GtkTreeIter iter;

	emulator_context* ectx = fragment->ectx;
	gtk_list_store_clear(fragment->lst_mem);
	
	byte values[4] = {0,0,0,0};
	dword dword_value;
	dword address = fragment->pos;
	
	for(int row = 0; row<16; row++){
		char buf_addr[10];
		gtk_list_store_append(fragment->lst_mem, &iter);
		snprintf(buf_addr, 10, "%08x", address);
		gtk_list_store_set(fragment->lst_mem, &iter,
			0, buf_addr, -1
		);
		
		for(int dword_nr = 0; dword_nr < 4; dword_nr++){
			char buf_val[10];
			gui_mem_get_dword_v(address + dword_nr * 4, &dword_value, fragment->ectx);
			values[3] = ectx->memory_context->memory[address + 3 + dword_nr * 4];
			values[2] = ectx->memory_context->memory[address + 2 + dword_nr * 4];;
			values[1] = ectx->memory_context->memory[address + 1 + dword_nr * 4];;
			values[0] = ectx->memory_context->memory[address + dword_nr * 4];;
			
			for(int byte_nr = 0; byte_nr < 4; byte_nr++){
				int column = dword_nr * 4 + byte_nr +1;
				snprintf(buf_val, 10, "%02x", values[byte_nr]);
				gtk_list_store_set(fragment->lst_mem, &iter,
					column, buf_val, -1
				);
			}
		}
		
		address += 16;
	}
	
	
}

void fragment_mem_fill_dummy_data(fragment_mem* fragment){
	
	GtkTreeIter iter;
	gtk_list_store_append(fragment->lst_mem, &iter);
	gtk_list_store_set(fragment->lst_mem, &iter,
		0, "",
		1, "1000",
		2, "C001BAB3",
		3, "XOR R0, R0, R0",
		-1
	);
	gtk_list_store_append(fragment->lst_mem, &iter);
	gtk_list_store_set(fragment->lst_mem, &iter,
		0, "",
		1, "1004",
		2, "CAFEBAB3",
		3, "XOR R0, R1, R0",
		-1
	);
	/**/
}
