/**
 * --------------------------------------
 * CUHK-SZ CSC4180: Compiler Construction
 * Assignment 2: Oat v.1 Scanner
 * --------------------------------------
 * Author: Mr.Liu Yuxuan
 * Position: Teaching Assisant
 * Date: February 27th, 2024
 * Email: yuxuanliu1@link.cuhk.edu.cn
 * 
 * File: scanner.cpp
 * ------------------------------------------------------------
 * This file implements scanner function defined in scanner.hpp
 */

#include <cctype>
#include "scanner.hpp"

DFA::~DFA() {
    for (auto state : states) {
        state->transition.clear();
        delete state;
    }
}

void DFA::print() {
    printf("DFA:\n");
    for (auto state : states)
        state->print();
}

/**
 * EPSILON NFA
 * (Start) -[EPSILON]-> (End)
 */
NFA::NFA() {
    start = new State();
    end = new State();
    start->transition[EPSILON] = {end};
}

/**
 * NFA for a single character
 * (Start) -[c]-> (End)
 * It acts as the unit of operations like union, concat, and kleene star
 * @param c: a single char for NFA
 * @return NFA with only one char
 */
NFA::NFA(char c) {
    start = new State();
    end = new State();
    start->transition[c] = {end};
}

NFA::~NFA() {
    for (auto state : iter_states()) {
        state->transition.clear();
        delete state;
    }
}

/**
 * Concat a string of char
 * Start from the NFA of the first char, then merge all following char NFAs
 * @param str: input string
 * @return
 */
NFA* NFA::from_string(std::string str) {
    // TODO
    NFA* nfa=new NFA();
    nfa->start->transition.clear();
    NFA::State* current=nfa->start;

    for (size_t i=0;i<str.length()-1;++i){
        char c=str[i];
        NFA::State* next=new NFA::State();
        current->transition[c].insert(next);
        current=next;
    }

    if (!str.empty()){
        current->transition[str.back()].insert(nfa->end);
    }

    return nfa;
}

/**
 * RegExp: [a-zA-Z]
 * @return
 */
NFA* NFA::from_letter() {
    // TODO
    NFA* nfa=new NFA();
    nfa->start->transition.clear();
    State* startState = nfa->start;
    State* endState = nfa->end;

    for(char c = 'a'; c <= 'z'; ++c){
        startState->transition[c].insert(endState);
    }

    for(char c = 'A'; c <= 'Z'; ++c){
        startState->transition[c].insert(endState);
    }
    return nfa;
}

/**
 * RegExp: [0-9]
 * @return
 */
NFA* NFA::from_digit() {
    // TODO
    NFA* nfa=new NFA();
    nfa->start->transition.clear();
    State* startState = nfa->start;
    State* endState = nfa->end;

    for(char c = '0'; c <= '9'; ++c){
        startState->transition[c].insert(endState);
    }
    return nfa;
}

/**
 * NFA for any char (ASCII 0-127)
 */
NFA* NFA::from_any_char() {
    // TODO
    NFA* nfa=new NFA();
    nfa->start->transition.clear();
    State* startState = nfa->start;
    State* endState = nfa->end;

    for(char c = 0; c <= 126; ++c){
        startState->transition[c].insert(endState);
    }
    return nfa;
}
/**
 * Concatanat two NFAs
 * @param from: NFA pointer to be concated after the current NFA
 * @return: this -> from
 */
void NFA::concat(NFA* from) {
    // TODO
    this->end->transition[EPSILON].insert(from->start);
    this->end=from->end;
}

/**
 * Set Union with another NFA
 * @param
 */
void NFA::set_union(NFA* from) {
    // TODO
    State* new_start=new NFA::State;
    State* new_end=new NFA::State;
    new_start->transition[EPSILON].insert(this->start);
    new_start->transition[EPSILON].insert(from->start);
    this->start=new_start;

    from->end->transition[EPSILON].insert(new_end);
    this->end->transition[EPSILON].insert(new_end);
    this->end=new_end;

}

/**
 * Set Union with a set of NFAs
 */
void NFA::set_union(std::set<NFA*> set) {
    // TODO
    State* new_start=new NFA::State;
    State* new_end=new NFA::State;
    new_start->transition[EPSILON].insert(this->start);
    this->start=new_start;
    this->end->transition[EPSILON].insert(new_end); 
    this->end=new_end;

    for(NFA* nfa: set){
        new_start->transition[EPSILON].insert(nfa->start);
        nfa->end->transition[EPSILON].insert(new_end);
    }

}

/**
 * Kleene Star Operation
 */
void NFA::kleene_star() {
    // TODO
    State* new_start=new NFA::State;
    State* new_end=new NFA::State;
    this->end->transition[EPSILON].insert(this->start);
    new_start->transition[EPSILON].insert(this->start);
    new_start->transition[EPSILON].insert(new_end);
    this->end->transition[EPSILON].insert(new_end);
    this->start=new_start;
    this->end=new_end;
}

/**
 * Determinize NFA to DFA by subset construction
 * @return DFA
 */
DFA* NFA::to_DFA() {
    // TODO
    DFA* dfa=new DFA();
    DFA::State* dfa_start=new DFA::State();
    std::set<NFA::State*> nfa_start=NFA::epsilon_closure(this->start);
    std::map<DFA::State*,std::set<NFA::State*>> state_map;  // 1-to-1 relations between DFA and NFAs
    state_map[dfa_start]=nfa_start;
    std::queue<DFA::State*> unmarkedStates;
    unmarkedStates.push(dfa_start);

    while(!unmarkedStates.empty()){
        DFA::State* DFAState=unmarkedStates.front();
        unmarkedStates.pop();
        dfa->states.push_back(DFAState);
        // transitions for the DFA state
        std::set<NFA::State*> NFAStates=state_map[DFAState];
        std::set<char> inputSymbols;
        for(NFA::State* state:NFAStates){
            for(auto& trans:state->transition){
                if (trans.first != EPSILON) { // 排除ε转移
                    inputSymbols.insert(trans.first);
                }
            }
        }
        // if there is new DFA state
        for(char c:inputSymbols){
            std::set<NFA::State*> successor=NFA::move(NFAStates,c);
            std::set<NFA::State*> all_closure;
            std::vector<unsigned int> id_list;
            for (NFA::State* single : successor) {
                std::set<NFA::State*> single_closure = NFA::epsilon_closure(single);
                //merge the two sets, the same id should appear once
                for(NFA::State* state:single_closure){
                    auto it=std::find(id_list.begin(),id_list.end(),state->id);
                    if(it==id_list.end()){
                        id_list.push_back(state->id);
                        all_closure.insert(state);
                    }
                }
            }

            bool existed=false; //whether the DFA state already exists
            DFA::State* newState;
            for(const auto& pair : state_map){
                if(pair.second == all_closure){
                    existed=true;
                    newState=pair.first;
                    break;
                }
            }
            if(!existed){
                newState=new DFA::State();
                state_map[newState]=all_closure;
                unmarkedStates.push(newState);
            }
            DFAState->transition[c]=newState;
        }
    }

    // label the accepted
    for(auto pair : state_map){
        DFA::State* dfaState=pair.first;
        std::set<NFA::State*> nfaStates=pair.second;

        unsigned int precedence=0;
        for(auto state:nfaStates){
            if(state->accepted && state->accepted > precedence){
                precedence=state->accepted;
                dfaState->accepted=true;
                dfaState->token_class=state->token_class;
            }
        }
    }
    
    return dfa;
}

/**
 * Get the closure of the given Nstates set
 * It means all the Nstates can be reached with the given Nstates set without any actions
 * @param state: State* , the starting state of getting EPSILON closure
 * @return {set<State&>} The closure of state
 */
std::set<NFA::State*> NFA::epsilon_closure(NFA::State* state) {
    // TODO
    std::set<NFA::State*> closure;
    std::set<NFA::State*> newStates;

    closure.insert(state);
    newStates.insert(state);

    // continue if only there are new states
    while (!newStates.empty()) {
        std::set<NFA::State*> tempStates;
        for (auto s : newStates) {
            auto children = move(closure, EPSILON);
            for (auto child : children) {
                if (closure.find(child) == closure.end()) { //
                    closure.insert(child);
                    tempStates.insert(child);
                }
            }
        }
        newStates = tempStates;
    }

    return closure;
}


/**
 * Get the set of neighbor states from the closure of starting state through char c
 * @param closure
 * @param c
 * @return
 */
std::set<NFA::State*> NFA::move(std::set<NFA::State*> closure, char c) {
    // TODO
    std::set<NFA::State*> results;
    for(NFA::State* state:closure){
        auto it=state->transition.find(c);
        if(it!=state->transition.end()){
            results.insert(it->second.begin(),it->second.end());
        }
    }
    return results;
}

void NFA::print() {
    printf("NFA:\n");
    for (auto state : iter_states())
        state->print();
}

/**
 * BFS Traversal
 */
std::vector<NFA::State*> NFA::iter_states() {
    std::vector<State*> states{};
    states.emplace_back(start);
    std::queue<State*> states_to_go{};
    states_to_go.emplace(start);
    std::set<State*> visited_states = {start};
    while(!states_to_go.empty()) {
        State* state = states_to_go.front();
        states_to_go.pop();
        for (auto trans : state->transition) {
            for (auto neighbor : trans.second) {
                if (visited_states.find(neighbor) == visited_states.end()) {
                    states_to_go.emplace(neighbor);
                    visited_states.insert(neighbor);
                    states.emplace_back(neighbor);
                }
            }
        }
    }
    return states;
}

/**
 * Constructor: Scanner
 * @usage: Scanner origin;
 *         Scanner scanner(reserved_word_strs, token_strs, reserced_word_num, token_num);
 * --------------------------------------------------------------------------------------
 * Create a Scanner object. The default constructor will not be used, and the second form
 * will create the NFA and DFA machines based on the given reserved words and tokens
 */
Scanner::Scanner() {
    nfa = new NFA();
}

/**
 * Given a filename of a source program, print all the tokens of it
 * @param {string} filename
 * @return 0 for success, -1 for failure
 */ 
int Scanner::scan(std::string &filename) {
    // TODO
    std::ifstream infile(filename);
    if(!infile){
        std::cerr<<"Cannot open file.\n";
        return 1;
    }
    char ch;
    std::string tmp="";
    std::string backtrackString="";
    std::streampos backtrackPoint;
    DFA::State* backtrackState=nullptr;
    DFA::State* currentState=dfa->states[0];
    while(infile.get(ch)){
        if(currentState->transition.find(ch) != currentState->transition.end()){   
            currentState=currentState->transition[ch];
            tmp+=ch;
            if(currentState->accepted){
                if(currentState->token_class==COMMENT|currentState->token_class==STRINGLITERAL){
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

        } else{
            if(backtrackString==""){
                if(!isspace(static_cast<unsigned char>(ch))){
                    std::cerr<<"Unrecognized token.\n";
                    return 1;
                } else continue;
            }else{
                std::cout<<token_class_to_str(backtrackState->token_class)<<" "<<backtrackString<<std::endl;
                infile.seekg(backtrackPoint);
                currentState=dfa->states[0];
                backtrackString="";
            }
        }
    }

    if (infile.eof()) {
        if(currentState->accepted){
            std::cout<<token_class_to_str(backtrackState->token_class)<<" "<<backtrackString<<std::endl;
        }
    } else if (infile.fail()) {
        std::cout << "\nRead error before reaching end of file.\n";
    }
    infile.close();
    return 0;
}

/**
 * Add string tokens, usually for reserved words, punctuations, and operators
 * @param token_str: exact string to match for token recognition
 * @param token_class
 * @param precedence: precedence of token, especially for operators
 * @return
 */
void Scanner::add_token(std::string token_str, TokenClass token_class, unsigned int precedence) {
    auto keyword_nfa = NFA::from_string(token_str);
    keyword_nfa->set_token_class_for_end_state(token_class, precedence);
    nfa->set_union(keyword_nfa);
}

/**
 * Token: ID (Identifier)
 * RegExp: [a-zA-Z][a-zA-Z0-9_]*
 * @param token_class
 * @param precedence
 * @return
 */
void Scanner::add_identifier_token(TokenClass token_class, unsigned int precedence) {
    // TODO: mimic how add_token do
    NFA* identifier_nfa=NFA::from_letter();
    identifier_nfa->NFA::set_union(NFA::from_digit());
    NFA* tmp=new NFA('_');
    identifier_nfa->NFA::set_union(tmp);
    identifier_nfa->NFA::kleene_star();

    auto letter_nfa=NFA::from_letter();
    letter_nfa->NFA::concat(identifier_nfa);
    identifier_nfa=letter_nfa;
    identifier_nfa->set_token_class_for_end_state(token_class,precedence);
    nfa->set_union(identifier_nfa);
}

/**
 * Token: INTEGER
 * RegExp: [1-9][0-9]+  //Does it have any mistake here? should be [0-9][0-9]*
 * Negative integer is recognized by unary operator MINUS
 * @param token_class
 * @param precedence
 * @return
 */
void Scanner::add_integer_token(TokenClass token_class, unsigned int precedence) {
    // TODO: mimic how add_token do
    std::set<NFA*> set;
    for(char c='0';c<'9';c++){
        NFA* tmp=new NFA(c);
        set.insert(tmp);
    }
    NFA* integer_nfa=new NFA('9');
    integer_nfa->set_union(set);

    NFA* digit_nfa=NFA::from_digit();
    digit_nfa->NFA::kleene_star();
    integer_nfa->NFA::concat(digit_nfa);
    integer_nfa->set_token_class_for_end_state(token_class,precedence);
    nfa->set_union(integer_nfa);
}

/**
 * Token Class: STRINGLITERAL
 * RegExp: "(.|")*" or  "(.)*"
 * @param token_class
 * @param precedence
 * @return
 */
void Scanner::add_string_token(TokenClass token_class, unsigned int precedence) {
    // TODO: mimic how add_token do
    NFA* string_nfa=NFA::from_any_char();
    string_nfa->kleene_star();

    NFA* quotation_nfa=new NFA(34);
    quotation_nfa->concat(string_nfa);
    string_nfa=quotation_nfa;
    string_nfa->concat(new NFA(34));
    string_nfa->set_token_class_for_end_state(token_class, precedence);
    nfa->set_union(string_nfa);
}

/**
 * Token Class: COMMENT
 * RegExp: //\s(.)* or  /*(.)*\*\/
 * @param token_class
 * @param precedence
 * @return
 */
void Scanner::add_comment_token(TokenClass token_class, unsigned int precedence) {
    // TODO: mimic how add_token do
    NFA* comet_nfa=NFA::from_any_char();
    comet_nfa->kleene_star();

    NFA* prefix_nfa=NFA::from_string("/*");
    prefix_nfa->concat(comet_nfa);
    comet_nfa=prefix_nfa;
    comet_nfa->concat(NFA::from_string("*/"));
    comet_nfa->set_token_class_for_end_state(token_class, precedence);
    nfa->set_union(comet_nfa);
}
