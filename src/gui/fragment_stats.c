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
#include <inttypes.h>

#include "common/lookup.h"

#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>

#include "fragment_stats.h"


fragment_stats* fragment_stats_new(statistic_context* statctx){
	fragment_stats *fragment = (fragment_stats*) g_malloc(sizeof(fragment_stats));
	fragment->statctx = statctx;
	
	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);
	
	fragment->frame_stats = gtk_frame_new("Statistics");
	
	//Set up the Statistics-Treeview
	fragment->tv_stats = gtk_tree_view_new();
	fragment->lst_stats = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_stats),
		-1,
		"Attribute",
		renderer,
		"text", 0,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_stats),
		-1,
		"Value",
		renderer,
		"text", 1,
		NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(fragment->tv_stats), GTK_TREE_MODEL(fragment->lst_stats));

	//Boxes for the Registers-Frame
	GtkWidget* vb_stats = gtk_vbox_new(FALSE, 0);

	//Buttons for the Registers-Frame
	fragment->btn_reset = gtk_button_new_with_label("Reset statistics");
	gtk_signal_connect(GTK_OBJECT (fragment->btn_reset), "clicked",
		GTK_SIGNAL_FUNC (fragment_stats_btn_reset_clicked),
		(gpointer) fragment);
	
	
	gtk_box_pack_start(GTK_BOX(vb_stats), GTK_WIDGET(fragment->tv_stats), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb_stats), GTK_WIDGET(fragment->btn_reset), FALSE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(fragment->frame_stats), GTK_WIDGET(vb_stats));
	
	return fragment;
}

void fragment_stats_update(gpointer data){
	fragment_stats* fragment = (fragment_stats*) data;
	
	gtk_list_store_clear(fragment->lst_stats);
	GtkTreeIter iter;
	
	char buffer_cache_miss[15];
	char buffer_instruction_count[15];
	char buffer_interrupt_count[15];
	char buffer_cache_hits[25];
	
	snprintf(buffer_cache_miss, 15, "%" PRId32, fragment->statctx->cache_miss_count);
	snprintf(buffer_instruction_count, 15, "%" PRId64, fragment->statctx->instruction_count);
	snprintf(buffer_interrupt_count, 15, "%" PRId32, fragment->statctx->interrupt_count);
	snprintf(buffer_cache_hits, 25, "%" PRId64, fragment->statctx->cache_hits);
	
	gtk_list_store_append(fragment->lst_stats, &iter);
	gtk_list_store_set(fragment->lst_stats, &iter,
			0, "Cache misses",
			1, buffer_cache_miss,
			-1
	);
	gtk_list_store_append(fragment->lst_stats, &iter);
	gtk_list_store_set(fragment->lst_stats, &iter,
			0, "Cache hits",
			1, buffer_cache_hits,
			-1
	);
	gtk_list_store_append(fragment->lst_stats, &iter);
	gtk_list_store_set(fragment->lst_stats, &iter,
			0, "Instructions executed",
			1, buffer_instruction_count,
			-1
	);
	gtk_list_store_append(fragment->lst_stats, &iter);
	gtk_list_store_set(fragment->lst_stats, &iter,
			0, "Interrupts received",
			1, buffer_interrupt_count,
			-1
	);
	
	
}

void fragment_stats_btn_reset_clicked(GtkWidget* widget, gpointer data){
	fragment_stats* fragment = (fragment_stats*)data;
	
	fragment->statctx->instruction_count = 0;
	fragment->statctx->interrupt_count = 0;
	fragment->statctx->cache_miss_count = 0;
	fragment->statctx->cache_hits = 0;
	fragment_stats_update(fragment);
	
}

void fragment_stats_fill_dummy_data(fragment_stats* fragment){
	GtkTreeIter iter;
	
	gtk_list_store_append(fragment->lst_stats, &iter);
	gtk_list_store_set(fragment->lst_stats, &iter,
		0, "Cache misses",
		1, "981",
		-1
	);
	
}
