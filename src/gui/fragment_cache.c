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

#include "fragment_cache.h"


fragment_cache* fragment_cache_new(memory_context* memctx){
	fragment_cache* fragment = (fragment_cache*)g_malloc(sizeof(fragment_cache));
	fragment->memctx = memctx;
	
	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);
	
	fragment->frame_cache = gtk_frame_new("Cache");
	
	//Set up the Cache-Treeview
	fragment->tv_cache = gtk_tree_view_new();
	GtkWidget* scroll_cache = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroll_cache), fragment->tv_cache);
	fragment->lst_cache = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_cache),
		-1,
		"Line No.",
		renderer,
		"text", 0,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_cache),
		-1,
		"Address",
		renderer,
		"text", 1,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_cache),
		-1,
		"Hit-Count",
		renderer,
		"text", 2,
		NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(fragment->tv_cache), GTK_TREE_MODEL(fragment->lst_cache));

	gtk_container_add(GTK_CONTAINER(fragment->frame_cache), scroll_cache);

	
	return fragment;
}

void fragment_cache_update(fragment_cache* fragment){
	GtkTreeIter iter;
	
	gtk_list_store_clear(fragment->lst_cache);
	
	for(int i = 0; i < fragment->memctx->cache_lines; i++) {
		cache_context* chctx = &fragment->memctx->lines[i];
		
		char buffer_num[10];
		char buffer_addr[10];
		char buffer_hits[10];
		snprintf(buffer_num, 10, "%d", i);
		snprintf(buffer_addr, 10, "%08x", chctx->cache_start);
		snprintf(buffer_hits, 10, "%d", chctx->hit_count);

		gtk_list_store_append(fragment->lst_cache, &iter);
		gtk_list_store_set(fragment->lst_cache, &iter,
			0, buffer_num,
			1, buffer_addr,
			2, buffer_hits,
			-1
		);		
	}
	
}

void fragment_cache_fill_dummy_data(fragment_cache* fragment){
	GtkTreeIter iter;
	gtk_list_store_append(fragment->lst_cache, &iter);
	gtk_list_store_set(fragment->lst_cache, &iter,
		0, "#1",
		1, "1000",
		2, "12",
		-1
	);
}

