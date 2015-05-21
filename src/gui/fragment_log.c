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

#include "fragment_log.h"
#include <string.h>
#include <stdlib.h>

static GQueue* log_queue;
gint debugsync = 0;

void fragment_log_clear_log(fragment_log* fragment) {
	g_mutex_lock(fragment->ectx->log_lock);

	if(g_atomic_int_get(&debugsync)) {
		printf("Sync failure 0\n");
		exit(98);
	}

	g_atomic_int_set(&debugsync, 1);

	printf("log_clear\n");
	char* msg;

	while((msg = g_queue_pop_head(fragment->log_queue)) != NULL) {
		free(msg);
	}
	printf("log_clear end\n");

	if(!g_atomic_int_get(&debugsync)) {
		printf("Sync failure 0\n");
		exit(98);
	}


	g_atomic_int_set(&debugsync, 0);

	g_mutex_unlock(fragment->ectx->log_lock);
}

void fragment_log_update(fragment_log* fragment) {
	g_mutex_lock(fragment->ectx->log_lock);

	if(g_atomic_int_get(&debugsync)) {
		printf("Sync failure 1\n");
		exit(98);
	}

	g_atomic_int_set(&debugsync, 1);
	
	printf("log_update\n");
	GtkTreeIter iter;	
	gtk_list_store_clear(fragment->lst_log);
	char* msg;

	while((msg = g_queue_pop_head(fragment->log_queue)) != NULL) {
		gtk_list_store_append(fragment->lst_log, &iter);
		gtk_list_store_set(fragment->lst_log, &iter,
			0, msg,
			-1
		);
		free(msg);
	}
	printf("log_update end\n");

	if(!g_atomic_int_get(&debugsync)) {
		printf("Sync failure 1\n");
		exit(98);
	}

	g_atomic_int_set(&debugsync, 0);

	g_mutex_unlock(fragment->ectx->log_lock);
}

void gui_log(char* c) {
	if(g_atomic_int_get(&debugsync)) {
		printf("Sync failure 2\n");
		exit(98);
	}

	g_atomic_int_set(&debugsync, 1);

	g_queue_push_head(log_queue, strdup(c));

	if(!g_atomic_int_get(&debugsync)) {
		printf("Sync failure 2\n");
		exit(98);
	}
	g_atomic_int_set(&debugsync, 0);
}

fragment_log* fragment_log_new(emulator_context* ectx) {
	fragment_log* fragment = (fragment_log*)g_malloc(sizeof(fragment_log));

	fragment->log_queue = g_queue_new();
	fragment->ectx = ectx;
	log_queue = fragment->log_queue;
	ectx->log = &gui_log;
	
	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);
	
	fragment->frame_log = gtk_frame_new("Log");
	
	//Set up the log-Treeview
	fragment->tv_log = gtk_tree_view_new();
	GtkWidget* scroll_log = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroll_log), fragment->tv_log);
	fragment->lst_log = gtk_list_store_new(1, G_TYPE_STRING);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_log),
		-1,
		"Message",
		renderer,
		"text", 0,
		NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(fragment->tv_log), GTK_TREE_MODEL(fragment->lst_log));

	gtk_container_add(GTK_CONTAINER(fragment->frame_log), scroll_log);

	
	return fragment;
}
