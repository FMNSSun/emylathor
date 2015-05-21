/*
Copyright (c) 2013, Roman Muentener
All rights reserved.
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: Redistributions of
source code must retain the above copyright notice, this list of conditions and
the following disclaimer. Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ’AS IS’ AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
POSSIBILITY OF SUCH DAMAGE.
*/

#include <common/types.h>
#include "common/macros.h"
#include "common/definitions.h"

#ifndef VA_H_MEMORY_INSPECTOR_H
#define VA_H_MEMORY_INSPECTOR_H

void gui_mem_write_dword_p(dword d, dword addr, emulator_context* ectx);
dword gui_mem_get_dword_p(dword addr, emulator_context* ectx);
int gui_mem_write_dword_v(dword out, dword addr, emulator_context* ectx);
int gui_mem_get_dword_v(dword addr, dword* out, emulator_context* ectx);
dword gui_mmu_get_page_entry(dword addr, emulator_context* ectx);
dword gui_mmu_translate_address(dword addr, emulator_context* ectx);
int gui_mmu_is_enabled(emulator_context* ectx);
int gui_mem_find_line(dword addr, emulator_context* ectx);

#endif