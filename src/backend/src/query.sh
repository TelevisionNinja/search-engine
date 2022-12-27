#! /bin/bash
# Example shell script for processing queries
# ./retrieve.sh Dog_Cat bird invertedFile
# Outputs:
# input Query: Dog_Cat bird 
# invertedFile directory: invertedFile
# The query is:  dog cat bird 
#    dog
#    cat
#    bird

flex query.lex
g++ -o query lex.yy.c -lfl
./query $@
