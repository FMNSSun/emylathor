CC = gcc

CFLAGS_GLIB = `pkg-config --cflags glib-2.0` `pkg-config --cflags gio-2.0`
LFLAGS_GLIB = `pkg-config --libs glib-2.0` `pkg-config --libs gio-2.0`

CFLAGS_GUI = `pkg-config --cflags gtk+-2.0` -g
LFLAGS_GUI = `pkg-config --libs gtk+-2.0`

CFLAGS = -std=gnu99 -pedantic -Wall -Werror -Isrc/ $(CFLAGS_GLIB) $(CFLAGS_GUI) -O3 -DDUMP_INT -DDUMP_ALL

LFLAGS = -lSDL -lSDL_image $(LFLAGS_GLIB) $(LFLAGS_GUI)

OBJECTS = bin/decode.o bin/memory.o bin/mmu.o bin/cpu.o bin/dump.o bin/interrupts.o bin/lookup.o bin/dma.o bin/pio.o bin/mkcontext.o
OBJECTS_HW = bin/hw/vagc.o bin/hw/devm.o bin/hw/vkbc.o bin/hw/vtimer.o

OBJECTS_GUI = bin/gui/win_main.o bin/gui/fragment_code.o bin/gui/fragment_registers.o bin/gui/fragment_cache.o bin/gui/fragment_stats.o bin/gui/fragment_stack.o bin/gui/fragment_log.o bin/gui/memory_inspector.o bin/gui/fragment_mem.o bin/gui/win_mem.o bin/gui/win_pd.o bin/gui/win_pt.o bin/gui/fragment_cache_inspect.o bin/gui/gui_helper.o

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

bin/hw/vagc.o: src/hw/vagc.c src/hw/vagc.h $(COMMON_SRC)
	${CC} -c -o bin/hw/vagc.o src/hw/vagc.c $(CFLAGS) $(LFGLAS)

bin/hw/devm.o: src/hw/devm.c src/hw/devm.h $(COMMON_SRC)
	${CC} -c -o bin/hw/devm.o src/hw/devm.c $(CFLAGS) $(LFGLAS)

bin/hw/vkbc.o: src/hw/vkbc.c src/hw/vkbc.h $(COMMON_SRC)
	${CC} -c -o bin/hw/vkbc.o src/hw/vkbc.c $(CFLAGS) $(LFGLAS)

bin/hw/vtimer.o: src/hw/vtimer.c src/hw/vtimer.h $(COMMON_SRC)
	${CC} -c -o bin/hw/vtimer.o src/hw/vtimer.c $(CFLAGS) $(LFGLAS)

bin/gui/fragment_registers.o: src/gui/fragment_registers.c src/gui/fragment_registers.h $(COMMON_SRC)
	${CC} -c -o bin/gui/fragment_registers.o src/gui/fragment_registers.c $(CFLAGS) $(LFLAGS)

bin/gui/fragment_code.o: src/gui/fragment_code.c src/gui/fragment_code.h $(COMMON_SRC)
	${CC} -c -o bin/gui/fragment_code.o src/gui/fragment_code.c $(CFLAGS) $(LFLAGS)

bin/gui/fragment_stats.o: src/gui/fragment_stats.c src/gui/fragment_stats.h $(COMMON_SRC)
	${CC} -c -o bin/gui/fragment_stats.o src/gui/fragment_stats.c $(CFLAGS) $(LFLAGS)

bin/gui/fragment_cache.o: src/gui/fragment_cache.c src/gui/fragment_cache.h $(COMMON_SRC)
	${CC} -c -o bin/gui/fragment_cache.o src/gui/fragment_cache.c $(CFLAGS) $(LFLAGS)

bin/gui/fragment_stack.o: src/gui/fragment_stack.c src/gui/fragment_stack.h $(COMMON_SRC)
	${CC} -c -o bin/gui/fragment_stack.o src/gui/fragment_stack.c $(CFLAGS) $(LFLAGS)

bin/gui/fragment_log.o: src/gui/fragment_log.c src/gui/fragment_log.h $(COMMON_SRC)
	${CC} -c -o bin/gui/fragment_log.o src/gui/fragment_log.c $(CFLAGS) $(LFLAGS)

bin/gui/fragment_cache_inspect.o: src/gui/fragment_cache_inspect.c src/gui/fragment_cache_inspect.h $(COMMON_SRC)
	${CC} -c -o bin/gui/fragment_cache_inspect.o src/gui/fragment_cache_inspect.c $(CFLAGS) $(LFLAGS)

bin/gui/win_main.o: src/gui/win_main.c src/gui/win_main.h bin/mkcontext.o
	${CC} -c -o bin/gui/win_main.o src/gui/win_main.c $(CFLAGS) $(LFLAGS)

bin/gui/memory_inspector.o: src/gui/memory_inspector.c src/gui/memory_inspector.h $(COMMON_SRC)
	${CC} -c -o bin/gui/memory_inspector.o src/gui/memory_inspector.c $(CFLAGS) $(LFLAGS)

bin/lib_vasmdis.o: src/disassembler/lib_vasmdis.c src/disassembler/lib_vasmdis.h
	${CC} -c -o bin/lib_vasmdis.o src/disassembler/lib_vasmdis.c -Isrc/ $(CFLAGS_GLIB) $(LFLAGS_GLIB)

bin/gui/win_mem.o: src/gui/win_mem.c src/gui/win_mem.h bin/mkcontext.o
	${CC} -c -o bin/gui/win_mem.o src/gui/win_mem.c $(CFLAGS) $(LFLAGS)

bin/gui/fragment_mem.o: src/gui/fragment_mem.c src/gui/fragment_mem.h
	${CC} -c -o bin/gui/fragment_mem.o src/gui/fragment_mem.c $(CFLAGS) $(LFLAGS)

bin/gui/win_pd.o: src/gui/win_pd.c src/gui/win_pd.h bin/mkcontext.o
	${CC} -c -o bin/gui/win_pd.o src/gui/win_pd.c $(CFLAGS) $(LFLAGS)

bin/gui/win_pt.o: src/gui/win_pt.c src/gui/win_pt.h bin/mkcontext.o
	${CC} -c -o bin/gui/win_pt.o src/gui/win_pt.c $(CFLAGS) $(LFLAGS)

bin/gui/gui_helper.o: src/gui/gui_helper.c src/gui/gui_helper.h
	${CC} -c -o bin/gui/gui_helper.o src/gui/gui_helper.c $(CFLAGS) $(LFGLAS)

gui: $(OBJECTS_GUI) $(OBJECTS) $(OBJECTS_HW) bin/lib_vasmdis.o Makefile
	${CC} -o gui $(OBJECTS_GUI) $(OBJECTS) $(OBJECTS_HW) bin/lib_vasmdis.o $(CFLAGS) $(LFLAGS)

va: $(OBJECTS) $(OBJECTS_HW) bin/emulator.o bin/lib_vasmdis.o 
	${CC} -o va bin/emulator.o $(OBJECTS) $(OBJECTS_HW) bin/lib_vasmdis.o $(CFLAGS) $(LFLAGS)

vasm: bin/lookup.o bin/emit.o src/assembler/vasm.l src/assembler/vasm.y
	bison -d src/assembler/vasm.y -o bin/vasm.tab.c -v
	flex  --case-insensitive --yylineno -o bin/lex.y.c src/assembler/vasm.l
	${CC} bin/vasm.tab.c bin/lex.y.c -lfl -o vasm bin/emit.o bin/lookup.o $(CFLAGS) $(LFLAGS) -lfl -Isrc/assembler/ -Wno-unused

compiler: src/compiler/rlang.l src/compiler/rlang.y
	bison --report=state --verbose -d src/compiler/rlang.y -o bin/rlang.tab.c -v -t
	flex --yylineno -o bin/lex.y.c src/compiler/rlang.l
	${CC} bin/rlang.tab.c bin/lex.y.c -lfl -o rlang $(CFLAGS) $(LFLAGS) -lfl -Isrc/compiler/ -Wno-unused

vdis: bin/lib_vasmdis.o bin/lookup.o src/disassembler/vdis.c
vdis: bin/lib_vasmdis.o bin/lookup.o src/disassembler/vdis.c src/disassembler/vdis.h
	${CC} -o vdis src/disassembler/vdis.c bin/lib_vasmdis.o bin/lookup.o $(CFLAGS_GLIB) $(LFLAGS_GLIB) -Isrc/

.PHONY : tests-v
tests-v: va vasm
	./test.sh

.PHONY : tests-r
tests-r: va vasm compiler
	./testrl.sh

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
	-mkdir bin/gui

.PHONY : all
all: clean va vasm tests-v
