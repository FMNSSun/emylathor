# Writing tests

For writing tests you must/should use the FAIL instruction to signal
that a test has failed. Use STOP if the test passes. You may make use of
the conditional instructions i.e FAILIZ or FAILNZ. E.g

```
LOADI R0, 0
TEST  R0
FAILNZ ; The zero flag must be set, if not: FAIL
STOP
```
