#!/bin/bash

make all

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

TESTCASES_DIR="${SCRIPT_DIR}/testcases"

OUTPUT_DIR="${SCRIPT_DIR}/outputs"
mkdir -p "$OUTPUT_DIR"

for testcase in "$TESTCASES_DIR"/*-tokens.txt; do

  filename=$(basename -- "$testcase")
  output_file="$OUTPUT_DIR/output_${filename%.*}.txt"
  
  ./LL1Parser "$SCRIPT_DIR/grammar.txt" "$testcase" > "$output_file"
done

echo "All test cases have been processed."

