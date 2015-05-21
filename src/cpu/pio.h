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

#ifndef VA_H_PIO_H
#define VA_H_PIO_H

#include "common/macros.h"
#include "common/definitions.h"

/* Typedef: port_handler

   Function pointer. 

   > void (*port_handler)(int read, dword* data, emulator_context* ectx);

   Parameters:
     read - 1 if read access, 0 if write access
     data - Pointer to a dword
     ectx - Emulator context

   On read the port handler has to write to *data, on write it has to read from *data.
  */
typedef void (*port_handler)(int read, dword* data, emulator_context* ectx);

/* Function: pio_init

   Initializes port I/O. Must be called before doing pio_read or pio_write.
 */
void pio_init();

/* Function: pio_register_port

   Registers a port.

   Parameters:
     no - port number (0 - 127)
     hndl - port handler (function pointer)
 */
void pio_register_port(byte no, port_handler hndl);

/* Function: pio_read

   Read from an I/O port.

   Parameters:
     no - Port number (must not exceed 127).
     ectx - Emulator context
    
   Returns:
     A dword read from the port.
 */
dword pio_read(byte no, emulator_context* ectx);

/* Function: pio_read

   Write to an I/O port.

   Parameters:
     no - Port number (must not exceed 127).
     data - DWORD data to write to the port.
     ectx - Emulator context
 */
void pio_write(byte no, dword data, emulator_context* ectx);

/* Function: pio_null

   The port null is a simple dword latch.
 */
void pio_null(int read, dword* data, emulator_context* ectx);

#endif
