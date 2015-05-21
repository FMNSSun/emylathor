CC = gcc

CFLAGS_GLIB = `pkg-config --cflags glib-2.0` `pkg-config --cflags gio-2.0`
LFLAGS_GLIB = `pkg-config --libs glib-2.0` `pkg-config --libs gio-2.0`

CFLAGS = -std=gnu99 -pedantic -Wall -Werror -Isrc/ $(CFLAGS_GLIB) -O3 -DDUMP_INT -DDUMP_ALL

LFLAGS = $(LFLAGS_GLIB) 

OBJECTS = bin/decode.o bin/memory.o bin/mmu.o bin/cpu.o bin/dump.o bin/interrupts.o bin/lookup.o bin/dma.o bin/pio.o bin/mkcontext.o
OBJECTS_HW = bin/hw/devm.o

COMMON_SRC = src/common/macros.h src/common/definitions.h src/common/rawdefs.h src/common/types.h Makefile 

bin/decode.o: src/cpu/decode.c src/cpu/decode.h $(COMMON_SRC)
	${CC} -c -o bin/decode.o src/cpu/decode.c $(CFLAGS) $(LFLAGS)

bin/memory.o: src/cpu/memory.c src/cpu/memory.h $(COMMON_SRC)
	${CC} -c -o bin/memory.o src/cpu/memory.c $(CFLAGS) $(LFLAGS)

bin/mmu.o: src/cpu/mmu.c src/cpu/mmu.h $(COMMON_SRC)
	${CC} -c -o bin/mmu.o src/cpu/mmu.c $(CFLAGS) $(LFLAGS)

bin/dma.o: src/cpu/dma.c src/cpu/dma.h $(COMMON_SRC)
	${CC} -c -o bin/dma.o src/cpu/dma.c $(CFLAGS) $(LFLAGS)

bin/cpu.o: src/cpu/cpu.c src/cpu/cpu.h $(COMMON_SRC)
	${CC} -c -o bin/cpu.o src/cpu/cpu.c $(CFLAGS) $(LFLAGS)

bin/interrupts.o: src/cpu/interrupts.c src/cpu/interrupts.h $(COMMON_SRC)
	${CC} -c -o bin/interrupts.o src/cpu/interrupts.c $(CFLAGS) $(LFLAGS)

bin/emulator.o: src/emulator.c src/emulator.h $(COMMON_SRC)
	${CC} -c -o bin/emulator.o src/emulator.c $(CFLAGS) $(LFLAGS) -Isrc/cpu/

bin/emit.o: src/assembler/emit.c src/assembler/emit.h $(COMMON_SRC)
	${CC} -c -o bin/emit.o src/assembler/emit.c $(CFLAGS) $(LFLAGS)

bin/assembler.o: src/cpu/assembler.c $(COMMON_SRC)
	${CC} -c -o bin/assembler.o src/cpu/assembler.c $(CFLAGS) $(LFLAGS) -Isrc/

bin/dump.o: src/cpu/dump.c src/cpu/dump.h $(COMMON_SRC)
	${CC} -c -o bin/dump.o src/cpu/dump.c $(CFLAGS) $(LFLAGS)

bin/lookup.o: src/common/lookup.c src/common/lookup.h $(COMMON_SRC)
	${CC} -c -o bin/lookup.o src/common/lookup.c $(CFLAGS) $(LFLAGS)

bin/pio.o: src/cpu/pio.c src/cpu/pio.h $(COMMON_SRC)
	${CC} -c -o bin/pio.o src/cpu/pio.c $(CFLAGS) $(LFLAGS)

bin/mkcontext.o: src/mkcontext.c src/mkcontext.h $(COMMON_SRC)
	${CC} -c -o bin/mkcontext.o src/mkcontext.c $(CFLAGS) $(LFLAGS)

bin/hw/devm.o: src/hw/devm.c src/hw/devm.h $(COMMON_SRC)
	${CC} -c -o bin/hw/devm.o src/hw/devm.c $(CFLAGS) $(LFGLAS)

bin/lib_vasmdis.o: src/disassembler/lib_vasmdis.c src/disassembler/lib_vasmdis.h
	${CC} -c -o bin/lib_vasmdis.o src/disassembler/lib_vasmdis.c -Isrc/ $(CFLAGS_GLIB) $(LFLAGS_GLIB)

va: $(OBJECTS) $(OBJECTS_HW) bin/emulator.o bin/lib_vasmdis.o 
	${CC} -o va bin/emulator.o $(OBJECTS) $(OBJECTS_HW) bin/lib_vasmdis.o $(CFLAGS) $(LFLAGS)

vasm: bin/lookup.o bin/emit.o src/assembler/vasm.l src/assembler/vasm.y
	bison -d src/assembler/vasm.y -o bin/vasm.tab.c -v
	flex  --case-insensitive --yylineno -o bin/lex.y.c src/assembler/vasm.l
	${CC} bin/vasm.tab.c bin/lex.y.c -lfl -o vasm bin/emit.o bin/lookup.o $(CFLAGS) $(LFLAGS) -lfl -Isrc/assembler/ -Wno-unused

vdis: bin/lib_vasmdis.o bin/lookup.o src/disassembler/vdis.c
vdis: bin/lib_vasmdis.o bin/lookup.o src/disassembler/vdis.c src/disassembler/vdis.h
	${CC} -o vdis src/disassembler/vdis.c bin/lib_vasmdis.o bin/lookup.o $(CFLAGS_GLIB) $(LFLAGS_GLIB) -Isrc/

.PHONY : tests-v
tests-v: va vasm
	./test.sh

.PHONY : test-vasm
test-vasm: va vasm vdis
	./vgen.py bin/tests/generated.V bin/tests/pseudo_instr_gen.V bin/tests/pseudo_instr_expected.V
	./vasm --asm bin/tests/generated.V --img bin/tests/generated.img
	./vdis --img bin/tests/generated.img --outfile bin/tests/disassembled.V
	./compare.sh bin/tests/disassembled.V bin/tests/generated.V bin/logs/test-asm.log
	./vasm --asm bin/tests/pseudo_instr_gen.V --img bin/tests/pseudo_instr_gen.img
	./vdis --img bin/tests/pseudo_instr_gen.img --outfile bin/tests/disassembled_pseudo_instr.V
	./compare.sh bin/tests/pseudo_instr_expected.V bin/tests/disassembled_pseudo_instr.V bin/logs/test-asm-pseudo.log

.PHONY : clean
clean:
	-rm -rf bin/
	-mkdir bin/
	-mkdir bin/tests
	-mkdir bin/logs
	-mkdir bin/hw

.PHONY : all
all: clean va vasm tests-v
