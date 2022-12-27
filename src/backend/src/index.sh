#! /bin/bash
echo
echo 'compiling'
flex index.lex
g++ -o index lex.yy.c -lfl

echo
echo 'indexing'
./index $1 $2

ls -lt $2
