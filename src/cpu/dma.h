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

#ifndef VA_H_DMA_H
#define VA_H_DMA_H

#include "common/macros.h"
#include "common/definitions.h"

#define _va_dma

#define DMA_READ 0
#define DMA_WRITE 1

typedef void (*dma_callback)(emulator_context* ectx);

/* Typedef: dma_transfer

   A request to the DMA to transfer memory (read or write).

   Struct members:
     addr - Address in memory (source or destination address / always physical)
     size - How many bytes to transfer
     buffer - Buffer to read to or write from
     type - Type (DMA_READ or DMA_WRITE)
     callback - A callback (will be called when the transfer was completed)
 */
typedef struct {
	dword addr;
	dword size;
	byte* buffer;
	int type;
	dma_callback callback;
} dma_transfer;

/* Function: dma_request_transfer

   Called by several threads. Called to request a transfer.

   Parameters:
     See struct dma_transfer

   Returns:
     1 - if request was successfully queued
     0 - on error
 */
int dma_request_transfer(dword addr, byte* buffer, dword size, int type, dma_callback callback, emulator_context* ectx);

/* Function: dma_background

   DMA Background Thread

   Parameters:
     ectx - Emulator context
 */
void* dma_background(void* ectx);

#endif
