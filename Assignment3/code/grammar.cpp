#include "grammar.hpp"


/**
 * Initialize the Grammar class.
 * 
 * @param filename The grammar file to be read. 
*/
Grammar::Grammar(const std::string filename){
    std::ifstream file(filename);
    std::string line;
    
    // 尝试读取文件的第一行
    if (getline(file, line) && !line.empty()) {
        std::istringstream iss(line);
        std::string firstSymbol;
        iss >> firstSymbol; // 提取第一行的第一个词语

        // 构建特殊规则，假设firstSymbol是起始符号
        std::string specialRule = "S ::= " + firstSymbol + " $";
        add_production_rule(specialRule);

        // 处理第一行剩余的部分
        add_production_rule(line);
    }

    while(getline(file, line)){
        if(line.empty()) {
            continue; // 如果line是空的，跳过当前迭代
        }
        add_production_rule(line);
    }

    update_terminals();
    
}

/**
 * Add a production rule into the grammar.
 * The rule should be of the form: prog ::= decl prog
*/
void Grammar::add_production_rule(const std::string line){
    std::istringstream iss(line);
    std::string word;
    std::vector<std::string> words;

    /* separate the words by whitespace */
    while(iss >> word){
        words.push_back(word);
    }

    /* words: {'prog','::=','decl','prog'} */
    if(words.size()<3 | words[1] != "::="){
        throw std::invalid_argument("Production rule format is invalid:"+line);
        return;
    }

    // add the nonterminals
    this->nonterminals.insert(words[0]);
    this->symbols.insert(words[0]);

    // add the symbols
    for(int i=2;i<words.size();i++){
        this->symbols.insert(words[i]);
    }

    // add the production rule
    std::vector<std::string> production_rule;
    production_rule.push_back(words[0]);
    for(int i=2;i<words.size();i++){
        production_rule.push_back(words[i]);
    }
    this->production_rules.push_back(production_rule);

}

void Grammar::update_terminals(){
    std::set_difference(this->symbols.begin(),this->symbols.end(),
    this->nonterminals.begin(),this->nonterminals.end(),
    std::inserter(this->terminals,this->terminals.begin()));
}


std::set<std::string> Grammar::get_terminals(){
    return this->terminals;
}

std::set<std::string> Grammar::get_nonterminals(){
    return this->nonterminals;
}

std::vector<std::vector<std::string>> Grammar::get_production_rules(){
    return this->production_rules;
}


/**
 * To check the production rules of the grammar.
*/
void Grammar::print_production_rules() {
    for (const auto& rule : this->production_rules) {
        for (const auto& symbol : rule) { 
            std::cout << symbol << " ";
        }
        std::cout << std::endl;
    }
}

void Grammar::print_terminals(){
    for(const auto& t : this->terminals){
        std::cout << t << " ";
    }
    std::cout<<std::endl;
}

void Grammar::print_nonterminals(){
    for(const auto& n : this->nonterminals){
        std::cout << n << " ";
    }
    std::cout<<std::endl;
}
