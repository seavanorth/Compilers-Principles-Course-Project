#!/bin/bash
# windows

bison -d tiny.y
flex tiny.l
gcc -o tiny .\tiny.tab.c .\lex.yy.c .\util.c .\main.c -g

.\tiny.exe .\case_1.tny .\tiny-token.txt 0 # Lexical Analysis
.\tiny.exe .\case_1.tny .\tiny-tree.txt 1 # Syntax Analysis
