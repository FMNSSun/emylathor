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

#include "win_main.h"
#include "win_mem.h"
#include "win_pd.h"
#include "mkcontext.h"
#include "common/lookup.h"
#include "cpu/cpu.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>
#include <gio/gio.h>

static emulator_context* ectx;

int main(int argc, char* argv[]) {
	#if !defined(GLIB_VERSION_2_36)
	  g_type_init();
	#endif

	#if !GLIB_CHECK_VERSION(2,32,0)
	  g_thread_init(NULL);
	#endif

	gdk_threads_init();

	gtk_init(&argc, &argv);
	g_log_set_always_fatal(G_LOG_LEVEL_CRITICAL|G_LOG_LEVEL_WARNING);
	
	win_main* wm = win_main_new();
	fill_dummy_data(wm);
	
	gdk_threads_enter();

	gtk_main();

	gdk_threads_leave();

	return 0;
}

static void win_main_view_memory(GtkWidget* widget, gpointer data){
	win_main* wm = (win_main*)data;
	win_mem_show(wm->mem_window);
}

static void win_main_view_pd(GtkWidget* widget, gpointer data){
	win_main* wm = (win_main*)data;
	win_pd_update(wm->pd_window);
	gtk_widget_show_all(GTK_WIDGET(wm->pd_window->window));
}

static void win_main_update_gui(gpointer data) {
	//Oki doki
	win_main* wm = (win_main*)data;
	
	fragment_registers_update(wm->fragment_registers);
	fragment_code_update(wm->fragment_code);
	fragment_cache_update(wm->fragment_cache);
	fragment_stats_update(wm->fragment_stats);
	fragment_stack_update(wm->fragment_stack);
	fragment_log_update(wm->fragment_log);
	
	win_mem* mem = wm->mem_window;
	win_mem_update_gui(mem);
}

static void* win_main_run_background(void* data){
	win_main* wm = (win_main*) data;
	emulator_context* ectx = wm->ectx;

	while(g_atomic_int_get(&wm->keep_running)) {
		fragment_log_clear_log(wm->fragment_log);	
		int ret = cpu_execute(ectx, 1);
		dword ip = ectx->cpu_context->pos;

		if((ret != -1) || g_hash_table_lookup_extended(wm->fragment_code->breakpoints, GUINT_TO_POINTER(ip), NULL, NULL)) {
			printf("breakpoint reached!\n");
			gdk_threads_enter();
			wm->fragment_code->show_pos = ip;
			win_main_update_gui(wm);
			gdk_threads_leave();

			break;
		}
	}

	g_atomic_int_set(&wm->stopped, 1);
	g_atomic_int_set(&wm->keep_running, 0);

	return NULL;
}

static gboolean win_main_run(GtkWidget* widget, gpointer data) {
	win_main* wm = (win_main*) data;	

	if(g_atomic_int_get(&wm->stopped)) {
		g_atomic_int_set(&wm->stopped, 0);
		g_atomic_int_set(&wm->keep_running, 1);

		#if !GLIB_CHECK_VERSION(2,32,0)
		GThread* runner = g_thread_create(win_main_run_background, data, FALSE, NULL);
		#else
		GThread* runner = g_thread_new("runner_thread", win_main_run_background, data);
		#endif
		(void)runner;
	}
	return TRUE;
}

static gboolean win_main_stop(GtkWidget* widget, gpointer data) {
	win_main* wm = (win_main*)data;

	g_atomic_int_set(&wm->keep_running, 0);

	wm->fragment_code->show_pos = ectx->cpu_context->pos;
	win_main_update_gui(data);

	return TRUE;
}

static gboolean win_main_step(GtkWidget* widget, gpointer data) {
	win_main* wm = (win_main*)data;
	cpu_execute(ectx, 1);
	wm->fragment_code->show_pos = ectx->cpu_context->pos;
	win_main_update_gui(data);
	return TRUE;
}

static gboolean win_main_load_img(GtkWidget* widget, gpointer data) {
	GtkWidget *dialog;

	win_main* window = (win_main*)data;
	
	dialog = gtk_file_chooser_dialog_new ("Open image file",
					GTK_WINDOW(window->window),
					GTK_FILE_CHOOSER_ACTION_OPEN,
					GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		
		fragment_code_clear(window->fragment_code);
		
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		
		GFile* infile = g_file_new_for_path(filename);

		char* filedata = NULL;
		gsize length = 0;

		gboolean suc = g_file_load_contents(
			infile,
			NULL,
			&filedata,
			&length,
			NULL,
			NULL);

		if(!suc)
		{
			fprintf(stderr, "Could not read image file. Sorry.\n");
			exit(11);
		}

		memcpy(ectx->memory_context->memory + INSTRUCTIONS_START_OFFSET, filedata, length);
		ectx->cpu_context->pos = INSTRUCTIONS_START_OFFSET;
		window->fragment_stack->base = VA_INIT_SP_LOCATION;
		
		g_free (filename);
		gtk_widget_destroy (dialog);
		
		win_main_update_gui(data);
		return TRUE;
	}

	gtk_widget_destroy (dialog);
	
	return FALSE;
		
}

static gboolean win_main_sig_delete(GtkWidget* widget, GdkEvent *event, gpointer data) {
	return FALSE;
}

static void win_main_sig_destroy(GtkWidget* widget, gpointer data) {
	gtk_main_quit();
}


void fill_dummy_data(win_main* wm) {
	
	fragment_registers_fill_dummy_data(wm->fragment_registers);
	fragment_code_fill_dummy_data(wm->fragment_code);
	fragment_cache_fill_dummy_data(wm->fragment_cache);
}

win_main* win_main_new() {
	win_main* wm = g_malloc(sizeof(win_main));

	ectx = emu_init();
	wm->ectx = ectx;

	g_atomic_int_set(&wm->stopped, 1);
	g_atomic_int_set(&wm->keep_running, 0);

	//Create Main Window and connect signals
	wm->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(wm->window), "emulathor | Main");
	
	gtk_window_set_default_size(GTK_WINDOW(wm->window), 640, 480);
	g_signal_connect(
		wm->window,
		"delete-event",
		G_CALLBACK (win_main_sig_delete),
		NULL
	);
	g_signal_connect(
		wm->window,
		"destroy",
		G_CALLBACK (win_main_sig_destroy),
		NULL
	);

	// Create the Menu
	GtkWidget* menu_file = gtk_menu_new();
	GtkWidget* item_ldimg = gtk_menu_item_new_with_mnemonic("Load _image file");
	
	GtkWidget* menu_cpu = gtk_menu_new();
	GtkWidget* item_step = gtk_menu_item_new_with_mnemonic("_Step");
	GtkWidget* item_run = gtk_menu_item_new_with_mnemonic("_Run");
	GtkWidget* item_stop = gtk_menu_item_new_with_mnemonic("S_top");

	GtkWidget* menu_view = gtk_menu_new();
	GtkWidget* item_memory = gtk_menu_item_new_with_mnemonic("_Memory");
	GtkWidget* item_pd = gtk_menu_item_new_with_mnemonic("_Page Directory");
	
	gtk_menu_append(GTK_MENU(menu_file), item_ldimg);
	gtk_menu_append(GTK_MENU(menu_cpu), item_step);
	gtk_menu_append(GTK_MENU(menu_cpu), item_run);
	gtk_menu_append(GTK_MENU(menu_cpu), item_stop);
	gtk_menu_append(GTK_MENU(menu_view), item_memory);
	gtk_menu_append(GTK_MENU(menu_view), item_pd);
	
	GtkWidget* menu_bar = gtk_menu_bar_new();

	GtkWidget* item_file = gtk_menu_item_new_with_mnemonic("_File");
	GtkWidget* item_cpu = gtk_menu_item_new_with_mnemonic("_CPU");
	GtkWidget* item_view = gtk_menu_item_new_with_mnemonic("_View");
	
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_file), menu_file);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_cpu), menu_cpu);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(item_view), menu_view);

	gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), item_file);
	gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), item_cpu);
	gtk_menu_bar_append(GTK_MENU_BAR(menu_bar), item_view);

	gtk_signal_connect(GTK_OBJECT (item_ldimg), "activate",
		GTK_SIGNAL_FUNC (win_main_load_img),
		(gpointer) wm);

	gtk_signal_connect(GTK_OBJECT (item_step), "activate",
		GTK_SIGNAL_FUNC (win_main_step),
		(gpointer) wm);

	gtk_signal_connect(GTK_OBJECT (item_stop), "activate",
		GTK_SIGNAL_FUNC (win_main_stop),
		(gpointer) wm);

	gtk_signal_connect(GTK_OBJECT (item_run), "activate",
		GTK_SIGNAL_FUNC (win_main_run),
		(gpointer) wm);
	
	gtk_signal_connect(GTK_OBJECT (item_memory), "activate",
		GTK_SIGNAL_FUNC (win_main_view_memory),
		(gpointer) wm);

	gtk_signal_connect(GTK_OBJECT (item_pd), "activate",
		GTK_SIGNAL_FUNC (win_main_view_pd),
		(gpointer) wm);

	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);

	//Create the main boxes
	GtkWidget* vbm = gtk_vbox_new(FALSE, 0);
	GtkWidget* hbu = gtk_hbox_new(FALSE, 0);
	GtkWidget* hbl = gtk_hbox_new(FALSE, 0);

	gtk_box_pack_start(GTK_BOX(vbm), menu_bar, FALSE, TRUE, 0);

	
	//Add the hboxes to the main vbox
	gtk_box_pack_start(GTK_BOX(vbm), GTK_WIDGET(hbu), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbm), GTK_WIDGET(hbl), TRUE, TRUE, 0);
	
	wm->fragment_registers = fragment_registers_new(ectx->cpu_context);
	wm->fragment_code = fragment_code_new(&(wm->stopped), ectx, GTK_WINDOW(wm->window));
	wm->fragment_cache = fragment_cache_new(ectx->memory_context);
	wm->fragment_stats = fragment_stats_new(ectx->statistic_context);
	wm->fragment_stack = fragment_stack_new(ectx, GTK_WINDOW(wm->window));
	wm->fragment_log = fragment_log_new(ectx);
	
	wm->fragment_code->fragment_refreshed_callback = fragment_code_refreshed;
	wm->fragment_code->wm = wm;
	
	// Pack the frames
	gtk_box_pack_start(GTK_BOX(hbu), GTK_WIDGET(wm->fragment_registers->frame_regs), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbu), GTK_WIDGET(wm->fragment_code->frame_code), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbu), GTK_WIDGET(wm->fragment_stack->frame_stack), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbl), GTK_WIDGET(wm->fragment_stats->frame_stats), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbl), GTK_WIDGET(wm->fragment_cache->frame_cache), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbm), GTK_WIDGET(wm->fragment_log->frame_log), TRUE, TRUE, 0);
	
	gtk_container_add(GTK_CONTAINER(wm->window), GTK_WIDGET(vbm));

	gtk_widget_show_all(wm->window);

	wm->mem_window = win_mem_new(ectx);
	wm->pd_window = win_pd_new(ectx);
	
	return wm;
}

void fragment_code_refreshed(void* window){
	win_main* wm = (win_main*) window;
	
	fragment_registers_update(wm->fragment_registers);
}
