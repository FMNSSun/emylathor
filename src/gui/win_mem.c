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

#include "win_mem.h"
#include "fragment_mem.h"
#include "fragment_cache_inspect.h"

#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>

static gboolean win_mem_sig_delete(GtkWidget* widget, GdkEvent *event, gpointer data) {
	gtk_widget_hide(widget);
	return TRUE;
}


void win_mem_update_gui(gpointer data) {
	
	win_mem* wm = (win_mem*)data;
	fragment_mem_update(wm->fragment_mem);
	fragment_cache_inspect_update(wm->fragment_cache_inspect);
	
}

void win_mem_fill_dummy_data(win_mem* wm) {
	
}

void win_mem_show(win_mem* wm) {
	gtk_widget_show_all(wm->window);
}

win_mem* win_mem_new(emulator_context* ectx) {
	win_mem* wm = g_malloc(sizeof(win_mem));
	
	wm->ectx = ectx;
	
	//Create Memory Window and connect signals
	wm->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(wm->window), "emulathor | Memory and cache inspection");
	gtk_window_set_default_size(GTK_WINDOW(wm->window), 640, 480);
	g_signal_connect(
		wm->window,
		"delete-event",
		G_CALLBACK (win_mem_sig_delete),
		NULL
	);
	/*g_signal_connect(
		wm->window,
		"destroy",
		G_CALLBACK (win_mem_sig_destroy),
		NULL
	);*/
	
	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);

	//Create the main boxes
	GtkWidget* vbm = gtk_vbox_new(FALSE, 0);
	GtkWidget* hbu = gtk_hbox_new(FALSE, 0);
	GtkWidget* hbl = gtk_hbox_new(FALSE, 0);

	
	//Add the hboxes to the main vbox
	gtk_box_pack_start(GTK_BOX(vbm), GTK_WIDGET(hbu), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbm), GTK_WIDGET(hbl), TRUE, TRUE, 0);
	
	
	//add frames
	wm->fragment_mem = fragment_mem_new(ectx, GTK_WINDOW(wm->window), 0);
	wm->fragment_cache_inspect = fragment_cache_inspect_new(ectx);
	
	// Pack the frames
	gtk_box_pack_start(GTK_BOX(hbu), GTK_WIDGET(wm->fragment_mem->frame_mem), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbl), GTK_WIDGET(wm->fragment_cache_inspect->frame_cache), TRUE, TRUE, 0);
	
	gtk_container_add(GTK_CONTAINER(wm->window), GTK_WIDGET(vbm));

	return wm;
}