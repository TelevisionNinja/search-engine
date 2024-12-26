#! /bin/bash
echo
echo 'compiling'
flex index.lex
g++ -o ../bin/index lex.yy.c -lfl
