#ifndef LL1
#define LL1

#include "grammar.hpp"
#include <map>
#include <stack>
#include <utility> // For std::pair

typedef std::map<std::string,std::set<std::string>> SymbolSetMap;
typedef std::map<std::pair<std::string, std::string>, int> LL1Table;

class GrammarAnalysis{
private:
    std::set<std::string> terminals;
    std::set<std::string> nonterminals;
    std::vector<std::vector<std::string>> production_rules;
    std::set<std::string> nullable_set;
    SymbolSetMap first_set;
    SymbolSetMap follow_set;

private:
    void compute_nullable_set();
    void compute_first_set();
    void compute_follow_set();
    
public:
    GrammarAnalysis()=default;
    ~GrammarAnalysis()=default;
    GrammarAnalysis(Grammar & g);

    LL1Table construct_LL1_Table();

    void LL1Parsing(std::string input,LL1Table & table);

    
    int valueInTable(const LL1Table& table, const std::string& key1, const std::string& key2);
    std::set<std::string> First(std::vector<std::string> rhs);
    bool isAllNull(std::vector<std::string> rhs);


    /* For debug*/
    void print_nullable_set();
    void print_first_set();
    void print_follow_set();
    void print_first_set(Grammar& g);
    void printLL1Table(const LL1Table& table);
};



void printStack(std::stack<std::string> S);

void printVector(const std::vector<std::string>& v, int start, int end);

#endif