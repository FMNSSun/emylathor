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

#include "win_pt.h"
#include "memory_inspector.h"
#include "common/definitions.h"

void win_pt_update(win_pt* wp, dword addr) {
	emulator_context* ectx = wp->ectx;
	
	GtkTreeIter iter;

	gtk_list_store_clear(wp->lst_pt);

	dword pt = addr; 

	for(int i = 0; i < 1024; i++) {
		char buffer_pt[10];
		char buffer_pte[10];
		char buffer_frm[10];

		dword pte = gui_mem_get_dword_p(pt + (i * 4), ectx);

		snprintf(buffer_pt, 10, "%08x", (i * 4));
		snprintf(buffer_pte, 10, "%08x", pte);
		snprintf(buffer_frm, 10, "%08x", pte >> 12u);

		gtk_list_store_append(wp->lst_pt, &iter);
		gtk_list_store_set(wp->lst_pt, &iter,
			0, buffer_pt,
			1, buffer_pte,
			2, buffer_frm,
			3, (pte >> 7) & 1 ? "X" : "",
			4, (pte >> 6) & 1 ? "X" : "",
			5, (pte >> 5) & 1 ? "X" : "",
			6, (pte >> 4) & 1 ? "X" : "",
			7, (pte >> 3) & 1 ? "X" : "",
			8, (pte >> 2) & 1 ? "X" : "",
			9, (pte >> 1) & 1 ? "X" : "",
			10, (pte >> 0) & 1 ? "X" : "",
			-1
		);
	}
}

static gboolean win_pt_sig_delete(GtkWidget* widget, GdkEvent *event, gpointer data) {
	gtk_widget_hide(widget);
	return TRUE;
}

win_pt* win_pt_new(emulator_context* ectx) {
	win_pt* wp = g_malloc(sizeof(win_pt));

	wp->ectx = ectx;

	//Top-Level Window
	wp->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(wp->window), "emulathor | Page Table");
	gtk_window_set_default_size(GTK_WINDOW(wp->window), 640, 480);

	g_signal_connect(
		wp->window,
		"delete-event",
		G_CALLBACK (win_pt_sig_delete),
		NULL
	);

	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);

	
	//Set up the Code-TreeView
	wp->tv_pt = gtk_tree_view_new();
	GtkWidget* scroll_pt = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroll_pt), wp->tv_pt);
	wp->lst_pt = gtk_list_store_new(11, 
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
		G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
		G_TYPE_STRING);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pt),
		-1,
		"Offset",
		renderer,
		"text", 0,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pt),
		-1,
		"Page Table Entry",
		renderer,
		"text", 1,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pt),
		-1,
		"Frame Bits",
		renderer,
		"text", 2,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pt),
		-1,
		"RU",
		renderer,
		"text", 3,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pt),
		-1,
		"EU",
		renderer,
		"text", 4,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pt),
		-1,
		"U",
		renderer,
		"text", 5,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pt),
		-1,
		"WP",
		renderer,
		"text", 6,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pt),
		-1,
		"WU",
		renderer,
		"text", 7,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pt),
		-1,
		"EP",
		renderer,
		"text", 8,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pt),
		-1,
		"Prot",
		renderer,
		"text", 9,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(wp->tv_pt),
		-1,
		"Present",
		renderer,
		"text", 10,
		NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(wp->tv_pt), GTK_TREE_MODEL(wp->lst_pt));

	gtk_container_add(GTK_CONTAINER(wp->window), GTK_WIDGET(scroll_pt));

	return wp;
}
