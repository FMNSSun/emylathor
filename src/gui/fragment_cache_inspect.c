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
#include "fragment_cache_inspect.h"
#include "memory_inspector.h"
#include "fragment_cache.h"
#include "mkcontext.h"

#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>


static void spin_linenr_value_changed(GtkSpinButton *spinbutton, gpointer user_data)  {
	
	fragment_cache_inspect* fragment = (fragment_cache_inspect*)user_data;
	fragment->show_line = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(fragment->spin_linenr));
	fragment_cache_inspect_update(user_data);
}

fragment_cache_inspect* fragment_cache_inspect_new(emulator_context* ectx){
	
	fragment_cache_inspect* fragment = (fragment_cache_inspect*)g_malloc(sizeof(fragment_cache_inspect));
	fragment->ectx = ectx;
	fragment->show_line = 0;
	
	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);

	fragment->frame_cache = gtk_frame_new("Cache");
	
	//Set up the Code-TreeView
	fragment->tv_cache = gtk_tree_view_new();
	GtkWidget* scroll_cache = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroll_cache), fragment->tv_cache);
	fragment->lst_cache = gtk_list_store_new(
		17, 
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, 
		G_TYPE_STRING
	);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_cache),
		-1,
		"offset\n----------\naddress",
		renderer,
		"text", 0,
		NULL);
	for(int i = 0; i < 16; i++){
		char col_buf[3];
		snprintf(col_buf, 3, "%02x", i);
		gtk_tree_view_insert_column_with_attributes(
			GTK_TREE_VIEW(fragment->tv_cache),
			-1,
			col_buf,
			renderer,
			"text", i+1,
			NULL
		);
	}
	gtk_tree_view_set_model(GTK_TREE_VIEW(fragment->tv_cache), GTK_TREE_MODEL(fragment->lst_cache));

	//Boxes for the Cache-Frame
	GtkWidget* hb_cache = gtk_hbox_new(FALSE, 0);
	GtkWidget* vb_cache = gtk_vbox_new(FALSE, 0);

	gtk_container_add(GTK_CONTAINER(fragment->frame_cache), GTK_WIDGET(vb_cache));
	gtk_box_pack_start(GTK_BOX(vb_cache), GTK_WIDGET(scroll_cache), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb_cache), GTK_WIDGET(hb_cache), FALSE, TRUE, 0);

	//Spinner and Label for the Cache-Frame
	fragment->spin_linenr = gtk_spin_button_new_with_range(0, 15, 1);
	GtkWidget* lbl_linenr = gtk_label_new("cache line number");
	
	gtk_box_pack_start(GTK_BOX(hb_cache), GTK_WIDGET(lbl_linenr), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hb_cache), GTK_WIDGET(fragment->spin_linenr), TRUE, TRUE, 0);

	gtk_signal_connect(GTK_OBJECT (fragment->spin_linenr), "value-changed",
		GTK_SIGNAL_FUNC (spin_linenr_value_changed),
		(gpointer) fragment);
	
	return fragment;
}

void fragment_cache_inspect_update(fragment_cache_inspect* fragment){
	
	GtkTreeIter iter;

	gtk_list_store_clear(fragment->lst_cache);
	byte line_nr = fragment->show_line;
	memory_context* memctx = fragment->ectx->memory_context;
	cache_context cache_line = memctx->lines[line_nr];
	byte* cache_content = cache_line.cache;
	
	int rows = memctx->cache_line_size / 16;
	int additional_bytes = memctx->cache_line_size % 16;
	if(additional_bytes != 0){
		rows++;
	}
	
	byte values[4] = {0,0,0,0};
	dword address = cache_line.cache_start;
	dword pos = 0;
	
	for(int row = 0; row<rows; row++){
		char buf_addr[10] = "";
		gtk_list_store_append(fragment->lst_cache, &iter);
		snprintf(buf_addr, 10, "%08x", address + pos);
		gtk_list_store_set(fragment->lst_cache, &iter,
			0, buf_addr, -1
		);
		
		int dword_per_line = 4;
		if(row == rows - 1 && additional_bytes != 0){
			dword_per_line = additional_bytes / 4;
		}
		
		for(int dword_nr = 0; dword_nr < dword_per_line; dword_nr++){
			char buf_val[10];
			
			values[3] = cache_content[pos + 3 + dword_nr * 4];
			values[2] = cache_content[pos + 2 + dword_nr * 4];
			values[1] = cache_content[pos + 1 + dword_nr * 4];
			values[0] = cache_content[pos + dword_nr * 4];
			
			for(int byte_nr = 0; byte_nr < 4; byte_nr++){
				int column = dword_nr * 4 + byte_nr +1;
				snprintf(buf_val, 10, "%02x", values[byte_nr]);
				gtk_list_store_set(fragment->lst_cache, &iter,
					column, buf_val, -1
				);
			}
		}
		
		pos += 16;
	}
	
	
}

void fragment_cache_inspect_fill_dummy_data(fragment_cache_inspect* fragment){
	/*
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
	);*/
	/**/
}
