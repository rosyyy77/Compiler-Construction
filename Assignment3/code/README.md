# How to Run the Program

```bash
cd /path/to/project
make all

PS D:\...> make all
g++ main.cpp grammar.cpp algorithm.cpp -o LL1Parser

PS D:\...> ./LL1Parser /path/to/the/grammar/file /path/to/the/testcases/in/token/stream/form
(e.g. ./LL1Parser ./grammar.txt ./testcases/test1-tokens.txt) 
# The output will directly show up on the terminal. Please be careful.

or

bash run_test.sh
# The result will show in the output directory.
```

