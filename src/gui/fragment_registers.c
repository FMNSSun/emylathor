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

#include "fragment_registers.h"


fragment_registers* fragment_registers_new(cpu_context* cctx){
	fragment_registers *fragment = (fragment_registers*) g_malloc(sizeof(fragment_registers));
	fragment->show_creg = FALSE;
	fragment->cctx = cctx;
	
	GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
	g_object_set(renderer, "family", "monospace", NULL);
	
	GtkWidget* frame_regs = gtk_frame_new("Registers");
	fragment->frame_regs = frame_regs;
	
	//Set up the Registers-TreeView
	fragment->tv_regs = gtk_tree_view_new();
	GtkWidget* scroll_regs = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(scroll_regs), fragment->tv_regs);
	fragment->lst_regs = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_regs),
		-1,
		"Register",
		renderer,
		"text", 0,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_regs),
		-1,
		"Hex",
		renderer,
		"text", 1,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_regs),
		-1,
		"Unsigned",
		renderer,
		"text", 2,
		NULL);
	gtk_tree_view_insert_column_with_attributes(
		GTK_TREE_VIEW(fragment->tv_regs),
		-1,
		"Signed",
		renderer,
		"text", 3,
		NULL);
	
	gtk_tree_view_set_model(GTK_TREE_VIEW(fragment->tv_regs), GTK_TREE_MODEL(fragment->lst_regs));

	//Boxes for the Registers-Frame
	GtkWidget* hb_regs = gtk_hbox_new(FALSE, 0);
	GtkWidget* vb_regs = gtk_vbox_new(FALSE, 0);

	gtk_container_add(GTK_CONTAINER(frame_regs), GTK_WIDGET(vb_regs));
	gtk_box_pack_start(GTK_BOX(vb_regs), GTK_WIDGET(scroll_regs), TRUE, TRUE, 0);

	//Buttons for the Registers-Frame
	fragment->btn_dword = gtk_button_new_with_label("Show Main registers");
	fragment->btn_ctrl = gtk_button_new_with_label("Show Control registers");
	gtk_signal_connect(GTK_OBJECT (fragment->btn_dword), "clicked",
		GTK_SIGNAL_FUNC (fragment_registers_btn_dword_clicked),
		(gpointer) fragment);
	gtk_signal_connect(GTK_OBJECT (fragment->btn_ctrl), "clicked",
		GTK_SIGNAL_FUNC (fragment_registers_btn_ctrl_clicked),
		(gpointer) fragment);
	
	
	gtk_box_pack_start(GTK_BOX(hb_regs), GTK_WIDGET(fragment->btn_dword), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hb_regs), GTK_WIDGET(fragment->btn_ctrl), TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vb_regs), GTK_WIDGET(hb_regs), FALSE, TRUE, 0);

	
	
	
	return fragment;
}

void fragment_registers_update(gpointer data){
	fragment_registers* fragment = (fragment_registers*) data;
	
	gtk_list_store_clear(fragment->lst_regs);
	
	GtkTreeIter iter;

	dword* regs = fragment->cctx->regs;
	dword* cregs = fragment->cctx->cregs;

	int reg_count = 16u;
	if(fragment->show_creg) reg_count = 7u;
	
	for(int i = 0; i < reg_count; i++) {
		char buffer_hex[10];
		char buffer_u[128];
		char buffer_s[128];
		char buffer_reg_name[10];
		
		
		if(fragment->show_creg){
			snprintf(buffer_reg_name, 10, "%s", get_name_of_creg(i));
			snprintf(buffer_hex, 10, "%08x", cregs[i]);
			snprintf(buffer_u, 128, "%u", UNSIGNED32(cregs[i]));
			snprintf(buffer_s, 128, "%d", UNSIGNED32(cregs[i]));
		}else{
			snprintf(buffer_hex, 10, "%08x", regs[i]);
			snprintf(buffer_u, 128, "%u", UNSIGNED32(regs[i]));
			snprintf(buffer_s, 128, "%d", UNSIGNED32(regs[i]));
			snprintf(buffer_reg_name, 10, "%s", get_name_of_reg(i));
		}
		
		
		gtk_list_store_append(fragment->lst_regs, &iter);
		gtk_list_store_set(fragment->lst_regs, &iter,
			0, buffer_reg_name,
			1, buffer_hex,
			2, buffer_u,
			3, buffer_s,
			-1
		);
	}
	
}

void fragment_registers_btn_dword_clicked(GtkWidget* widget, gpointer data){
	fragment_registers* fragment = (fragment_registers*)data;
	fragment->show_creg = FALSE;
	
	fragment_registers_update(data);
}

void fragment_registers_btn_ctrl_clicked(GtkWidget* widget, gpointer data){
	fragment_registers* fragment = (fragment_registers*)data;
	fragment->show_creg = TRUE;
	
	fragment_registers_update(data);
}

void fragment_registers_fill_dummy_data(fragment_registers* fragment){
	GtkTreeIter iter;
	gtk_list_store_append(fragment->lst_regs, &iter);
	gtk_list_store_set(fragment->lst_regs, &iter,
		0, "R0",
		1, "DEADBEEF",
		2, "12345",
		3, "-12345",
		-1
	);
}