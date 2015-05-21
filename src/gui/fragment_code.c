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
#include "fragment_code.h"
#include "mkcontext.h"
#include "memory_inspector.h"
#include "gui_helper.h"

#include <string.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <glib.h>
#include <gio/gio.h>


static void fragment_code_toggle_breakpoint(GtkTreeView* tv, GtkTreePath* path, GtkTreeViewColumn* col, gpointer data) {
	fragment_code* fragment = (fragment_code*) data;

	if(!g_atomic_int_get(fragment->stopped)) {
		GtkWidget* dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Can't toggle breakpoint while running!");
		gtk_dialog_run (GTK_DIALOG (dialog));
 		gtk_widget_destroy (dialog);
		return;
	}
	
	/*
	if(!g_hash_table_lookup_extended(ctx->labels, name, NULL, NULL)){

	fragment->breakpoints
	*/

	GtkTreeModel *model;
	GtkTreeIter   iter;
 
  	g_print ("A row has been double-clicked!\n");
 
    	model = gtk_tree_view_get_model(tv);
 
    	if (gtk_tree_model_get_iter(model, &iter, path)) {
       		gchar *contents;
 
       		gtk_tree_model_get(model, &iter, 1, &contents, -1);
 		dword ip = (dword)g_ascii_strtoull(contents, NULL, 16);
		
		g_print ("Double-clicked row contains %s with %d\n", contents, ip);
 
       		g_free(contents);
	
		//g_hash_table_insert(fragment->breakpoints, "huhu", "fuck you too");

		if(!g_hash_table_lookup_extended(fragment->breakpoints, GUINT_TO_POINTER(ip), NULL, NULL)) {
			g_print ("Set breakpoint\n");
			g_hash_table_insert(fragment->breakpoints, GUINT_TO_POINTER(ip), GUINT_TO_POINTER(ip));
		}
		else {
			g_print ("Remove breakpoint\n");
			g_hash_table_remove(fragment->breakpoints, GUINT_TO_POINTER(ip));
		}
    }

	fragment_code_update(fragment);
}

fragment_code* fragment_code_new(int* stopped, emulator_context* ectx, GtkWindow* window){
	
	fragment_code* fragment = (fragment_code*)g_malloc(sizeof(fragment_code));
	fragment->ectx = ectx;
	fragment->show_pos = INSTRUCTIONS_START_OFFSET;
	fragment->breakpoints = g_hash_table_new(g_direct_hash, g_direct_equal);
	fragment->frame_code = gtk_frame_new("Code");
	fragment->stopped = stopped;
	fragment->window = window;
	
	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);

	
	//Set up the Code-TreeView
	fragment->tv_code = gtk_tree_view_new();
	GtkWidget* scroll_code = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroll_code), fragment->tv_code);
	fragment->lst_code = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_code),
		-1,
		"Break",
		renderer,
		"text", 0,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_code),
		-1,
		"IP",
		renderer,
		"text", 1,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_code),
		-1,
		"Hex",
		renderer,
		"text", 2,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_code),
		-1,
		"Assembly",
		renderer,
		"text", 3,
		NULL);
	gtk_tree_view_set_model(GTK_TREE_VIEW(fragment->tv_code), GTK_TREE_MODEL(fragment->lst_code));

	//Boxes for the Code-Frame
	GtkWidget* hb_code = gtk_hbox_new(FALSE, 0);
	GtkWidget* vb_code = gtk_vbox_new(FALSE, 0);

	gtk_container_add(GTK_CONTAINER(fragment->frame_code), GTK_WIDGET(vb_code));
	gtk_box_pack_start(GTK_BOX(vb_code), GTK_WIDGET(scroll_code), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb_code), GTK_WIDGET(hb_code), FALSE, TRUE, 0);

	//Entry and Buttons for the Code-Frame
	fragment->entry_code = gtk_entry_new();
	fragment->btn_showat = gtk_button_new_with_label("Show at");
	fragment->btn_jump = gtk_button_new_with_label("Jump");
	fragment->btn_call = gtk_button_new_with_label("Call");
	
	gtk_box_pack_start(GTK_BOX(hb_code), GTK_WIDGET(fragment->entry_code), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hb_code), GTK_WIDGET(fragment->btn_showat), FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hb_code), GTK_WIDGET(fragment->btn_jump), FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hb_code), GTK_WIDGET(fragment->btn_call), FALSE, TRUE, 0);

	
	gtk_signal_connect(GTK_OBJECT (fragment->btn_jump), "clicked",
		GTK_SIGNAL_FUNC (fragment_code_btn_jmp_clicked),
		(gpointer) fragment);
	gtk_signal_connect(GTK_OBJECT (fragment->btn_call), "clicked",
		GTK_SIGNAL_FUNC (fragment_code_btn_call_clicked),
		(gpointer) fragment);
	gtk_signal_connect(GTK_OBJECT (fragment->btn_showat), "clicked",
		GTK_SIGNAL_FUNC (fragment_code_btn_showat_clicked),
		(gpointer) fragment);

	g_signal_connect(fragment->tv_code, "row-activated", (GCallback) fragment_code_toggle_breakpoint,
		(gpointer) fragment);
	
	return fragment;
}

void fragment_code_update(fragment_code* fragment){
	GtkTreeIter iter;

	dword pos = fragment->show_pos;
	
	gtk_list_store_clear(fragment->lst_code);
	
	for(int i = 0; i < 32u; i++) {
		char buffer_ip[10];
		char buffer_hex[128];
		char* buffer_break;

		if(!g_hash_table_lookup_extended(fragment->breakpoints, GUINT_TO_POINTER(pos + (4*i)), NULL, NULL)) {
			buffer_break = "";
		}
		else {
			buffer_break = "X";
		}
		
		dword dWordValue;
		byte values[4] = {0,0,0,0};
		
		dword address = pos + (4 * i);
		gui_mem_get_dword_v(address, &dWordValue, fragment->ectx);
		values[3] = HHBYTE_FROM_DWORD(dWordValue);
		values[2] = HLBYTE_FROM_DWORD(dWordValue);
		values[1] = LHBYTE_FROM_DWORD(dWordValue);
		values[0] = LLBYTE_FROM_DWORD(dWordValue);
		snprintf(buffer_ip, 10, "%08x", address);
		snprintf(buffer_hex, 128, "%02x%02x%02x%02x", values[0], values[1], values[2], values[3]);
		gtk_list_store_append(fragment->lst_code, &iter);
		gtk_list_store_set(fragment->lst_code, &iter,
			0, buffer_break,
			1, buffer_ip,
			2, buffer_hex,
			3, replace_tabs(vasmdis_disasm_instruction(values)),
			-1
		);
	}
	fragment->fragment_refreshed_callback(fragment->wm);
	
}

void fragment_code_btn_showat_clicked(GtkWidget* widget, gpointer data){
	fragment_code* fragment = (fragment_code*)data;

	
	fragment->show_pos = parse_address(
		fragment->window,
		gtk_entry_get_text(GTK_ENTRY(fragment->entry_code)),
		fragment->show_pos,
		4,
		fragment->ectx->memory_context->memory_size - 1
	);
	fragment_code_update(fragment);
}


void fragment_code_btn_call_clicked(GtkWidget* widget, gpointer data){
	fragment_code* fragment = (fragment_code*)data;

	if(!g_atomic_int_get(fragment->stopped)) {
		GtkWidget* dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Can't do calls while running!");
		gtk_dialog_run (GTK_DIALOG (dialog));
 		gtk_widget_destroy (dialog);
		return;
	}
	
	dword new_address = parse_address(
		fragment->window,
		gtk_entry_get_text(GTK_ENTRY(fragment->entry_code)),
		fragment->ectx->cpu_context->pos,
		4,
		fragment->ectx->memory_context->memory_size - 1
	);
	
	dword* rc_reg = &fragment->ectx->cpu_context->regs[CPU_REG_RC];
	*rc_reg = fragment->ectx->cpu_context->pos;
	
	fragment->ectx->cpu_context->pos = new_address;
	fragment->show_pos = new_address;
	fragment_code_update(fragment);
	
}

void fragment_code_btn_jmp_clicked(GtkWidget* widget, gpointer data){
	fragment_code* fragment = (fragment_code*)data;

	if(!g_atomic_int_get(fragment->stopped)) {
		GtkWidget* dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Can't do jumps while running!");
		gtk_dialog_run (GTK_DIALOG (dialog));
 		gtk_widget_destroy (dialog);
		return;
	}
	
	dword new_address = parse_address(
		fragment->window,
		gtk_entry_get_text(GTK_ENTRY(fragment->entry_code)),
		fragment->ectx->cpu_context->pos,
		4,
		fragment->ectx->memory_context->memory_size - 1
	);
	
	//const gchar* address_text = gtk_entry_get_text(GTK_ENTRY(fragment->entry_code));
	//fragment_code_jump_to_address(address_text, fragment);
	fragment->ectx->cpu_context->pos = new_address;
	fragment->show_pos = new_address;
	
	fragment_code_update(fragment);
}

void fragment_code_jump_to_address(const gchar* address, fragment_code* fragment){
	guint64 parsed_value = g_ascii_strtoull(address, NULL, 16);
	dword mem_size = fragment->ectx->memory_context->memory_size;
	dword *pos = &fragment->ectx->cpu_context->pos;
	
	if(parsed_value < UINT32_MAX){
		if(*pos + parsed_value >= mem_size){
			parsed_value = mem_size - 1;
		}
		if(parsed_value % 4 != 0) printf("[WARNING] Use addresses with multiple of 4 bytes or you get wrong commands!\n");
		*(pos) = parsed_value;
		
	} else {
		*(pos) = mem_size - 1;
	}
	fragment->show_pos = *(pos);
}

void fragment_code_fill_dummy_data(fragment_code* fragment){
	GtkTreeIter iter;
	gtk_list_store_append(fragment->lst_code, &iter);
	gtk_list_store_set(fragment->lst_code, &iter,
		0, "",
		1, "1000",
		2, "C001BAB3",
		3, "XOR R0, R0, R0",
		-1
	);
	gtk_list_store_append(fragment->lst_code, &iter);
	gtk_list_store_set(fragment->lst_code, &iter,
		0, "",
		1, "1004",
		2, "CAFEBAB3",
		3, "XOR R0, R1, R0",
		-1
	);
}

void fragment_code_clear(fragment_code* fragment){
	gtk_list_store_clear(fragment->lst_code);
}

char* replace_tabs(char* ptr) {
	char* old = ptr;
	while(*ptr) {
		if(*ptr == '\t')
			*ptr = ' ';
		ptr++;
	}
	return old;
}
