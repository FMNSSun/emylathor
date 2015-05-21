Usage: ./rlang filename.rl > filename.V
Usually you'd want to use the ./invrlang.sh script. It feeds the file first through the C preprocessor.
It will use a temporary file so if the compiler displays an error the line number is the line number in the
temporary file. 
