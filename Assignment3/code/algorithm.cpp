#include "algorithm.hpp"



/**
 * Constructor
*/
GrammarAnalysis::GrammarAnalysis(Grammar & g){
    this->terminals=g.get_terminals();
    this->nonterminals=g.get_nonterminals();
    this->production_rules=g.get_production_rules();
}


/**
 * 
*/
void GrammarAnalysis::compute_nullable_set(){
    
    /* Does any nonterminal derive lambda directly? */
    for(auto & p : this->production_rules){
        if ( p.size()<=2 && p[1]=="\'\'" ){
            this->nullable_set.insert(p[0]);
        }
    }

    bool ischanged=true;
    while(ischanged){
        ischanged=false;    
        /* For a production rule A ::= Y1 Y2 Y3, can each part be lambda*/
        for(auto& p:this->production_rules){
            /* if the nonterminal already derives lambda */
            if(this->nullable_set.find(p[0]) != nullable_set.end()){
                continue;
            }

            bool isAllNull=true;
            for(int i=1;i<p.size();i++){
                if(this->nullable_set.find(p[i]) == nullable_set.end()){
                    isAllNull=false;
                }
            }

            if(isAllNull){
                this->nullable_set.insert(p[0]);
                ischanged=true;
            }
        }
    }
}

void GrammarAnalysis::compute_first_set(){
    /* Initialize the first set */
    for(auto T:this->terminals){
        this->first_set[T].insert(T);
    }

    bool isChanged=true;

    while(isChanged){
        isChanged=false;

        for(auto p: this->production_rules){    
            /* For production rule X -> Y1 Y2 ... Yk */
            std::string X=p[0];
            int size_before=first_set[X].size();
            int i=1;
            while(i<p.size()){
                std::string Y=p[i];
                this->first_set[X].insert(first_set[Y].begin(),first_set[Y].end());
                if(this->nullable_set.find(Y) == nullable_set.end()) break;
                i++;
            }
            int size_after=first_set[X].size();
            if(size_after-size_before!=0) isChanged=true;
        }
    }
}

void GrammarAnalysis::compute_follow_set(){
    bool isChanged=true;
    int iteration=0;
    while(isChanged){
        iteration++;
        isChanged=false;

        for(auto p: this->production_rules){    
            /* For a production rule: X ::= Y1 Y2 Y3 ... Yk */
            for(int i=1;i<p.size();++i){
                if(terminals.find(p[i]) != terminals.end() | p[i]=="\'\'") continue;
                bool tillEnd=true;
                int size_before=follow_set[p[i]].size();
                for(int j=i+1;j<p.size();++j){
                    follow_set[p[i]].insert(first_set[p[j]].begin(),first_set[p[j]].end());

                    if(nullable_set.find(p[j]) == nullable_set.end()){
                        tillEnd=false;
                        break;
                    }
                }
                if(tillEnd){
                    follow_set[p[i]].insert(follow_set[p[0]].begin(),follow_set[p[0]].end());

                }
                int size_after=follow_set[p[i]].size();
                if(size_after-size_before!=0) isChanged=true;
            }
        }
    }
    follow_set.erase("\'\'");
}

std::set<std::string> GrammarAnalysis::First(std::vector<std::string> rhs){
    std::set<std::string> result;

    bool tillEnd=true;
    for(auto T:rhs){
        result.insert(first_set[T].begin(),first_set[T].end());
        if(nullable_set.find(T) == nullable_set.end()) break;
    }

    return result;
}

bool GrammarAnalysis::isAllNull(std::vector<std::string> rhs){
    std::set<std::string> result;

    bool isAllNull=true;
    for(auto T:rhs){
        if(nullable_set.find(T) == nullable_set.end() and T != "\'\'"){
            return !isAllNull;
        }
    }

    return isAllNull;
}


LL1Table GrammarAnalysis::construct_LL1_Table(){
    compute_nullable_set();
    compute_first_set();
    compute_follow_set();

    LL1Table table;

    for (int i=0;i<production_rules.size()-1;++i){


        auto p=production_rules[i];
        std::string X=p[0];
        std::vector<std::string> rhs(p.begin() + 1, p.end());

        
        // debug part
        // std::cout<<"Production Rule "<<i<<": ";
        // printVector(p,0,p.size()-1);


        for ( auto T : First(rhs) ){
            if(valueInTable(table,X,T) != -1 and valueInTable(table,X,T) != i){
                //debug
                // std::cout<<"Come across error when constructing LL1 Table: Production "<<i<<std::endl;

                std::cout<<" "<<X<<" "<<T<<" "<<valueInTable(table,X,T)<<std::endl;
                return table;
            }
            // std::cout<<"Insert ("<<X<<" ,"<<T<<" ): "<<i<<" because T is in the first set of X."<<std::endl;
            table.insert({std::make_pair(X,T),i});
        }

        if(isAllNull(rhs)){
            for (auto T: follow_set[X]){
                if(valueInTable(table,X,T) != -1 and valueInTable(table,X,T) != i){
                    std::cout<<"Come across error when constructing LL1 Table: Production "<<i<<std::endl;
                    std::cout<<" "<<X<<" "<<T<<" "<<valueInTable(table,X,T)<<std::endl;
                    return table;
                }
                // std::cout<<"Insert ("<<X<<" ,"<<T<<" ): "<<i<<" because T is in the follow set of X."<<std::endl;
                table.insert({std::make_pair(X,T),i});
            }
        }



    }
    return table;
}


int GrammarAnalysis::valueInTable(const LL1Table& table, const std::string& key1, const std::string& key2) {
    // 使用std::make_pair来创建查询用的键
    auto key = std::make_pair(key1, key2);
    // 使用find方法检查键是否存在
    auto it = table.find(key);
    if (it != table.end()) {
        // 如果找到了键，返回对应的值
        return it->second;
    }
    // 如果键不存在，返回-1
    return -1;
}




/**
 * @param input the string to parse 
*/
void GrammarAnalysis::LL1Parsing(std::string input,LL1Table & table){
    /* Proccess the input */
    std::ifstream file(input);
    std::string line;
    std::string word;
    std::vector<std::string> words;

    while(getline(file, line)){
        std::istringstream iss(line);
        while(iss >> word){
            words.push_back(word);
        }
    }
    words.push_back("$");

    std::stack<std::string> stack;
    stack.push("S");

    int remaining_input=0;
    std::string r;
    while(!stack.empty()){
        std::string t;
        r=stack.top();

        printStack(stack);
        if (remaining_input<words.size()){
            t=words[remaining_input];
            std::cout<<"The remaining input: ";
            printVector(words,remaining_input,words.size()-1);
        } else {
            t="\'\'";
            std::cout<<"The remaining input: "<<std::endl;
        }
        
        if(terminals.find(r) != terminals.end()){
            if(r != t){
                std::cout<<"Error: terminals do not match."<<std::endl;
                return;
            } else{
                std::cout<<"Match "<<t<<std::endl;
                remaining_input++;
                stack.pop();
            }
        } else {
            auto key=std::make_pair(r,t);
            if(table.find(key) == table.end()){
                std::cout<<"Error: Undefined rule of "<<r<<" "<<t<<"."<<std::endl;
                return;
            }

            stack.pop();
            int i=table[std::make_pair(r,t)];
            auto production_rule=production_rules[i];

            std::cout<<"Rule: ";
            printVector(production_rule,0,production_rule.size()-1);
            

            for(auto rit = production_rule.rbegin();rit != production_rule.rend()-1;++rit){
                if(*rit != "\'\'")  stack.push(*rit);
            }

        }
        std::cout<<std::endl;
    }

    if(r=="$"){
        std::cout<<"Parsing complete!"<<std::endl;
    }
}

void printStack(std::stack<std::string> S) {
    std::stack<std::string> tempStack;

    // 将原栈中的元素倒入临时栈中
    while (!S.empty()) {
        tempStack.push(S.top());
        S.pop();
    }

    // 从临时栈中弹出并打印元素，这样就能逆序打印了
    std::cout << "Stack: ";
    while (!tempStack.empty()) {
        std::cout << tempStack.top() << " ";
        tempStack.pop();
    }
    std::cout << std::endl;
}

void printVector(const std::vector<std::string>& v, int start, int end) {
    // 检查开始和结束索引的有效性
    if (start < 0 || end >= v.size() || start > end) {
        std::cerr << "Invalid range" << std::endl;
        return;
    }

    for (int i = start; i <= end; ++i) {
        std::cout << v[i] << " ";
    }
    std::cout << std::endl; // 打印换行，结束输出
}



/**
 * A series of functions for debugging
*/
void GrammarAnalysis::print_nullable_set(){
    for (std::set<std::string>::const_iterator it = nullable_set.begin(); it != nullable_set.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout<<std::endl;
}

void GrammarAnalysis::print_first_set(){
    auto myMap=this->first_set;
    for (const auto& pair : myMap) {
        std::cout << pair.first << ": ";
        for (auto setIter = pair.second.begin(); setIter != pair.second.end(); ++setIter) {
            if (setIter != pair.second.begin()) {
                // 在元素之间添加逗号，除了第一个元素之前
                std::cout << ", ";
            }
            std::cout << *setIter;
        }
        std::cout << std::endl; // 每个键值对打印完毕后换行
    }
}

void GrammarAnalysis::print_first_set(Grammar& g){
    auto terminals=g.get_terminals();
    auto myMap=this->first_set;
    for (const auto& pair : myMap) {
        if(terminals.find(pair.first) != terminals.end()) continue;
        std::cout << pair.first << ": ";
        for (auto setIter = pair.second.begin(); setIter != pair.second.end(); ++setIter) {
            if (setIter != pair.second.begin()) {
                // 在元素之间添加逗号，除了第一个元素之前
                std::cout << " ";
            }
            std::cout << *setIter;
        }
        std::cout << std::endl; // 每个键值对打印完毕后换行
    }
}

void GrammarAnalysis::print_follow_set(){
    auto myMap=this->follow_set;
    for (const auto& pair : myMap) {
        std::cout << pair.first << ": ";
        for (auto setIter = pair.second.begin(); setIter != pair.second.end(); ++setIter) {
            if (setIter != pair.second.begin()) {
                // 在元素之间添加逗号，除了第一个元素之前
                std::cout << " ";
            }
            std::cout << *setIter;
        }
        std::cout << std::endl; // 每个键值对打印完毕后换行
    }
}

void GrammarAnalysis::printLL1Table(const LL1Table& table) {
    for (const auto& entry : table) {
        // entry.first 是键，类型为 std::pair<std::string, std::string>
        // entry.second 是值，类型为 int
        std::cout << "(" << entry.first.first << ", " << entry.first.second << "): " << entry.second << std::endl;
    }
}





// void compute_nullable_first_follow_set(const Grammar g){
//     std::map<std::string,bool> nullable_set;
//     std::map<std::string,std::set<std::string>> first_set;
//     std::map<std::string,std::set<std::string>> follow_set;

//     std::set<std::string> symbols;
//     symbols=g.get_symbols();
//     for(auto& S : symbols){
//         nullable_set[S]=false;
//         first_set[S]=None;
//         follow_set[S]=None;
//     }

//     std::set<std::string> terminals;
//     terminals=g.get_terminals();
//     for(auto& T : terminals){
//         first_set[T].insert(T);
//     }

//     std::vector<std::vector<std::string>> production_rules;
//     production_rules=g.get_production_rules;
//     bool change=false;
//     while(change){
//         for(auto& p:production_rules){
//             bool isAllNullable=true;
//             for(int i=1;i<p.size();i++){
//                 if(nullable_set[])
//             }
//         }
//     }
// }