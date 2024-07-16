# CSC4180-A2-Report

## How to Run the Compiler

```bash
cd path/to/project
make all

# Flex Scanner
./lexer < ./testcases/test0.oat
# Handwritten Scanner
./scanner ./testcases/test0.oat

# A bash script also provided
bash bash.sh
120090102@c2d52c9b1339:~/Assignment2$ bash bash.sh
g++ main.cpp scanner.cpp tokens.cpp -o scanner
Running lexer on test case test0.oat
|--Pass
Running lexer on test case test1.oat
|--Pass
Running lexer on test case test2.oat
|--Pass
Running lexer on test case test3.oat
|--Pass
Running lexer on test case test4.oat
|--Pass
Running lexer on test case test5.oat
|--Pass
```

## Design and Implementation

To implement a scanner by hand, the general idea is: Lexical Specification->Regular expressions->NFA->DFA->Scanner. 

The design and implementation is based on the template given by TA (Very clear template, great TA).  

**NFA** is composed of a group of states with a branch of transitions pointing to each other. For each token, we represent it using regular expressions. The basic formation from regular expressions to a branch of NFA states are: `NFA()`, `NFA(char c)`, `from_string`, `from_letter`, `from_digit`, `from_any_character` and `concat`, `set_union`, `kleene_star`. By repeating using these methods, we can form complicated regular expressions.

 **DFA** is composed of a group of states. For each state, certain character points to the only next state. DFA is derived from NFA by subset construction. To be simple, for certain NFA state, its epsilon-closure is thought to be a state in DFA.

**Scanning** part is illustrated in detail in Q & A.

## Q & A

1. **Why we choose regular expression to represent lexical specification?**

In compiler design, regular expressions are concise notations used to define and recognize patterns in source code. Regular expressions help in defining the syntax of programming languages, facilitating the transformation of human-readable code into machine-readable forms during compilation.

Moreover, many programming languages set built-in tools supporting regular expressions, making them a universal tool for text processing.

2. **Why NFA is more suitable than DFA to recognize regular expression, and how did you enable NFA to recognize different kinds of regular expression?**

- NFAs can be constructed directly from a regular expression using simple algorithms. The structure built from concatenation, alternation and Kleene stars naturally correspond to that of regular expressions.  
- NFAs allow a non-deterministic and flexible way for lexical specification, which corresponds to regular expressions: the same input symbol lead to multiple paths for lexical analysis.

3. **How did you enable your scanner to always recognize the longest match and the most precedent match?**

- **the longest match**: We implement the longest match by backtracking. When the scanner arrives at an accepted state, it will not stop immediately. Instead, it keeps the record of the latest accepted state. Only if the scanner comes across an error during scanning what is no transition under the character will the scanner pop out the record and backtrack to the latest accepted state.

```C++
// A simplified version of int scan()
int Scanner::scan(std::string &filename) {
    std::string tmp="";	// the tmp variable stores the scanned but unaccepted characters
    std::string backtrackString="";
    std::streampos backtrackPoint;	
    DFA::State* backtrackState=nullptr;	
	// Record the state to backtrack
	// backtrackString: the string already scanned at the latest accepted state
    // backtrackPoint: the backtracking position at the input file
    // backtrackState: the latest accpeted state to backtrack
    DFA::State* currentState=dfa->states[0];
    while(infile.get(ch)){
        if(currentState->transition.find(ch) != currentState->transition.end()){   
            currentState=currentState->transition[ch];
            tmp+=ch;
            if(currentState->accepted){	// record the accepted state
                if(currentState->token_class==COMMENT|currentState->token_class==STRINGLITERAL){
                    // special cases: COMMENT, STRINGLITERAL 
                    std::cout<<token_class_to_str(currentState->token_class)<<" "<<tmp<<std::endl;
                    currentState=dfa->states[0];
                    tmp="";
                    continue;
                }
                backtrackPoint=infile.tellg();
                backtrackState=currentState;
                backtrackString+=tmp;
                tmp="";
            }
        } else{		// transition fails
            if(backtrackString==""){	// if the backtrackingString is null, no accepted state for backtracking
                if(!isspace(static_cast<unsigned char>(ch))){	// ignore the wihtespace
                    std::cerr<<"Unrecognized token.\n";
                    return 1;
                } else continue;
            }else{	// pop out the token and return to the backtrackpoint
                std::cout<<token_class_to_str(backtrackState->token_class)<<" "<<backtrackString<<std::endl;
                infile.seekg(backtrackPoint);
                currentState=dfa->states[0];
                backtrackString="";
            }
        }
    }
    // the process for the last character
    return 0;
}
```

The COMMENT and STRINGLITERAL token classes are two special cases. They both use `static NFA* from_any_char()`. This function adds transitions for all Ascii from 0-127 (126, actually, an unstoppable loop for 127, I have no idea for it),  including `*/` and `"`. This will produces an infinite loop because the scanner continues looking for the next COMMENT and STRINGLITERAL state. I do not know how to modify DFA to avoid the problem. However, I think according to the regular expression, as long as the scanner arrives at the first accepted state for COMMENT and STRINGLITERAL, it returns the token.

- **the most precedent match:** When merging the NFA states for the same DFA state, the highest precedence reserves.

4. **Why do we still need to convert NFA into DFA for lexical analysis in most cases?**

DFAs have a deterministic operation, meaning for every state and input symbol, there is exactly one transition to the next state. This deterministic behavior simplifies the implementation of the lexical analyzer, making it more efficient during execution. It allows for a straightforward, one-pass scan of the input without the need for backtracking or maintaining multiple possible states at each step.

## Modifications

- Some tokens are not added to the scanner. More token class such as TRUE and FALSE are added just for passing the test cases. These classes do not cover all the tokens specified in Oat v1.

