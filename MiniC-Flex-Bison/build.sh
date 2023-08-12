#!/bin/bash
# windows

bison -d minic.y
flex minic.l
gcc -o minic .\minic.tab.c .\lex.yy.c .\util.c .\main.c -g

.\minic.exe .\case_1.mic .\minic-token.txt 0 # lexical analysis
.\minic.exe .\case_1.mic .\minic-tree.txt 1 # syntax analysis
