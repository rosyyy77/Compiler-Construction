# ifndef GRAMMAR_HPP
# define GRAMMAR_HPP

# include <string>
# include <set>
# include <vector>
# include <fstream>
# include <sstream>
#include <stdexcept>
#include <iostream>
#include <algorithm>    //for set difference
#include <iterator>


class Grammar{

private:
    std::set<std::string> symbols;
    std::set<std::string> terminals;
    std::set<std::string> nonterminals;
    std::vector<std::vector<std::string>> production_rules;

public:
    Grammar()=default;
    ~Grammar()=default;

    Grammar(const std::string filename);
    
    void add_production_rule(const std::string line);
    void update_terminals();

/* Access functions */
    std::set<std::string> get_terminals();
    std::set<std::string> get_nonterminals();
    std::vector<std::vector<std::string>> get_production_rules();


/* Grammar analysis */
    

/* Debug Only*/
public:
    void print_production_rules();
    void print_terminals();
    void print_nonterminals();
};

#endif