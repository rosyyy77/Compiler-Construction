#include "grammar.hpp"
#include "algorithm.hpp"

int main(int argc, char const *argv[]) {
    if (argc == 3) {
        
        std::string grammar_txt = argv[1];
        
        /* Read the input LL(1) grammar file and parse its format correctly */
        Grammar* grammar=new Grammar(grammar_txt);
        GrammarAnalysis* grammar_analysis=new GrammarAnalysis(*grammar);
        
        /* grammar analysis */
        auto table = grammar_analysis->construct_LL1_Table();
        
        /* Debug */
        // grammar->print_nonterminals();
        // grammar->print_terminals();
        // grammar->print_production_rules();
        // grammar_analysis->print_nullable_set();
        // grammar_analysis->print_first_set(*grammar);
        // grammar_analysis->print_follow_set();
        // grammar_analysis->printLL1Table(table);

        std::string input_txt = argv[2];

        grammar_analysis->LL1Parsing(input_txt,table);

    } else{
        std::cout<<"Invalid parameters."<<std::endl;
    }

    return 0;
}


