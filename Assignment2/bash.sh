#!/bin/bash

make all
mkdir -p output

for i in {0..5}
do
    echo "Running lexer on test case test${i}.oat"
    ./lexer < ./testcases/test${i}.oat > ./output/output_lexer_$i.txt
    ./scanner ./testcases/test${i}.oat > ./output/output_scanner_$i.txt
    if diff -q -w ./output/output_lexer_$i.txt ./output/output_scanner_$i.txt; then
    echo "|--Pass"
    else
        echo "|--Failed"
        diff ./output/output_lexer_$i.txt ./output/output_scanner_$i.txt
    fi
done

