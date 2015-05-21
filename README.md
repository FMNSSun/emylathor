THIS IS A COPY OF bitbucket.org/mroman_/emulathor (which was a school project years ago). 
However, some things have been removed (and most likely more stuff will be removed as well). 

# About

The goal of this project is to develop an emulator for a simple computer including simple hardware. The emulator is intended
for demonstration purposes.


# Developers

Please document functionality in the LaTeX documentation and code in the naturaldocs format. Before committing please run
the tests. To run the tests just run:

```
make tests-v
```

If a test fails please make a bug report (unless it is your bug and you fix it right away before committing). All test cases
produce outputs in the bin/logs directory. (You may have to set DUMP-Macros in the Makefile for a more verbose output).

```
CFLAGS = -Wall -Werror -Isrc/ $(CFLAGS_GLIB) -lpthread -DDUMP_INS -DDUMP_REGS # -DDUMP_ALL
```

The tests itself are located in src/tests/ and must have the ending *.V

You can also run a single test only by doing:

```./stest.sh sdiv.V```
