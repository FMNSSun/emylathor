#!/usr/bin/env bash

path=..
genbook_script=./genbook.sh
pdf_default_name=main.pdf

gui_manual_name="VA_GUI_Manual.pdf"
va_doc_name="VA_Technical_Documentation.pdf"
vasm_doc_name="VASM_Technical_Documentation.pdf"
rhailargue_doc_name="Rhailargue_Language_Specification.pdf"

if [ $# -gt 0 ]; then
	path=$1
fi
	

function copy_book {
	folder_name=$1
	pdf_name=$2
	
	cd $folder_name
	$genbook_script
	cp_path="$path/$pdf_name"
	cp $pdf_default_name $cp_path
	cd ..
}

copy_book "gui" $gui_manual_name
copy_book "va" $va_doc_name
copy_book "vasm" $vasm_doc_name
copy_book "rlangspec" $rhailargue_doc_name


