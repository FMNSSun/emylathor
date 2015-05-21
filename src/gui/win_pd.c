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

#include "win_pd.h"
#include "memory_inspector.h"
#include "common/definitions.h"

#include <stdlib.h>

void win_pd_update(win_pd* wp) {
	emulator_context* ectx = wp->ectx;
	
	GtkTreeIter iter;

	gtk_list_store_clear(wp->lst_pd);

	dword pd = ectx->cpu_context->cregs[CPU_CREG_MR1]; 

	for(int i = 0; i < 1024; i++) {
		char buffer_pd[10];
		char buffer_pta[10];

		snprintf(buffer_pd, 10, "%08x", (i * 4));
		snprintf(buffer_pta, 10, "%08x", gui_mem_get_dword_p(pd + (i * 4), ectx));

		gtk_list_store_append(wp->lst_pd, &iter);
		gtk_list_store_set(wp->lst_pd, &iter,
			0, buffer_pd,
			1, buffer_pta,
			-1
		);
	}
}

static gboolean win_pd_sig_delete(GtkWidget* widget, GdkEvent *event, gpointer data) {
	gtk_widget_hide(widget);
	return TRUE;
}

static void win_pd_inspect_pt(GtkTreeView* tv, GtkTreePath* path, GtkTreeViewColumn* col, gpointer data) {
	win_pd* wp = (win_pd*) data;

	GtkTreeModel *model;
	GtkTreeIter   iter;
 
  	g_print ("A row has been double-clicked!\n");
 
    	model = gtk_tree_view_get_model(tv);

	dword addr = 0x00u;
 
    	if (gtk_tree_model_get_iter(model, &iter, path)) {
       		gchar *contents;
 
       		gtk_tree_model_get(model, &iter, 1, &contents, -1);
 		addr = (dword)g_ascii_strtoull(contents, NULL, 16);
		
		g_print ("Double-clicked row contains %s with %d\n", contents, addr);
 
       		g_free(contents);
	
	
    	}

	win_pt_update(wp->win_pt, addr);

	gtk_widget_show_all(wp->win_pt->window);
}

win_pd* win_pd_new(emulator_context* ectx) {
	win_pd* wp = g_malloc(sizeof(win_pd));

	wp->ectx = ectx;

	//Top-Level Window
	wp->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(wp->window), "emulathor | Page Directory");
	gtk_window_set_default_size(GTK_WINDOW(wp->window), 640, 480);

	g_signal_connect(
		wp->window,
		"delete-event",
		G_CALLBACK (win_pd_sig_delete),
		NULL
	);

	wp->win_pt = win_pt_new(ectx);

	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);

	
	//Set up the Code-TreeView
	wp->tv_pd = gtk_tree_view_new();
	GtkWidget* scroll_pd = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroll_pd), wp->tv_pd);
	wp->lst_pd = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pd),
		-1,
		"Offset",
		renderer,
		"text", 0,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pd),
		-1,
		"Page Table Address",
		renderer,
		"text", 1,
		NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(wp->tv_pd), GTK_TREE_MODEL(wp->lst_pd));

	gtk_container_add(GTK_CONTAINER(wp->window), GTK_WIDGET(scroll_pd));

	g_signal_connect(wp->tv_pd, "row-activated", (GCallback) win_pd_inspect_pt,
		(gpointer) wp);

	win_pd_update(wp);

	return wp;
}
