/**
 * --------------------------------------
 * CUHK-SZ CSC4180: Compiler Construction
 * Assignment 1: Micro Language Compiler
 * --------------------------------------
 * Author: Mr.Liu Yuxuan
 * Position: Teaching Assisant
 * Date: January 25th, 2024
 * Email: yuxuanliu1@link.cuhk.edu.cn
 * 
 * This file defines the LLVM IR Generator class, which generate LLVM IR (.ll) file given the AST from parser.
 */



#include "ir_generator.hpp"

void IR_Generator::export_ast_to_llvm_ir(Node* node) {
    /* TODO: your program here */
    gen_llvm_ir(node);
}

void IR_Generator::gen_llvm_ir(Node* node) {
    /* TODO: Your program here */
    if(!node) return;

    switch (node->symbol_class){
        //unused symbols
        case SymbolClass::START:
        case SymbolClass::SCANEOF:
        case SymbolClass::BEGIN_:
        case SymbolClass::END:
        case SymbolClass::LPAREN:
        case SymbolClass::RPAREN:
        case SymbolClass::SEMICOLON:
        case SymbolClass::COMMA:
        //symbols that should not appear
        case SymbolClass::STATEMENT:
        case SymbolClass::ID_LIST:
        case SymbolClass::EXPRESSION_LIST:
        case SymbolClass::EXPRESSION:
        case SymbolClass::PRIMARY:
            out<<"Non terminal symbol."<<std::endl;
            break;
        //
        case SymbolClass::PROGRAM:
            IR_Generator::generateProgramNode(node);
            break;
        case SymbolClass::STATEMENT_LIST:
            IR_Generator::generateStatmentNode(node);
            break;
        case SymbolClass::READ:
            IR_Generator::generateReadNode(node);
            break;
        case SymbolClass::WRITE:
            IR_Generator::generateWriteNode(node);
            break;
        case SymbolClass::ASSIGNOP:
            IR_Generator::generateAssignOPNode(node);
            break;
        case SymbolClass::PLUSOP:
            IR_Generator::generatePlusOPNode(node);
            break;
        case SymbolClass::MINUSOP:
            IR_Generator::generateMinusOPNode(node);
            break;
        case SymbolClass::INTLITERAL:
            IR_Generator::generateIntliteralNode(node);
            break;
        case SymbolClass::ID:
            IR_Generator::generateIDNode(node);
            break;
        default:
            out<<"Unknown symbol.\n";
            break;
    }
}

void IR_Generator::generateProgramNode(Node* node){
    out<<"; Declare printf"<<std::endl;
    out<<"declare i32 @printf(i8*, ...)"<<std::endl;
    out<<std::endl;
    out<<";Declare scanf"<<std::endl;
    out<<"declare i32 @scanf(i8*, ...)"<<std::endl;
    out<<std::endl;
    out<<"define i32 @main() {"<<std::endl;
    for (auto child:node->children){
        IR_Generator::gen_llvm_ir(child);
    }
	out<<"\tret i32 0\n}"<<std::endl;
}

void IR_Generator::generateStatmentNode(Node* node){
    for (auto child:node->children){
        IR_Generator::gen_llvm_ir(child);
    }
}

void IR_Generator::generateReadNode(Node* node){
    //allocate the space
    int allocated_space=node->children.size()*3;

    // construct string
    std::string format_string = "%d";
    for(size_t i = 0; i < node->children.size()-1; ++i) {
        format_string += " %d";
    }
    format_string += "\\00";

    //load id_list
    for(auto child:node->children){
        if(!contains(IR_Generator::id_table,child->lexeme)){
            IR_Generator::id_table.push_back(child->lexeme);
            out<<"\t%"<<child->lexeme<<" = alloca i32"<<std::endl;
        }  
    }

    out<<"\t"<<R"(%_scanf_format_1 = alloca [)"<<allocated_space<<R"( x i8])"<<std::endl;
    out<<"\t"<<"store ["<<allocated_space<<R"( x i8] c")"<<format_string<<R"(", [)"<<allocated_space<<R"( x i8]* %_scanf_format_1)"<<std::endl;
    out<<"\t"<<R"(%_scanf_str_1 = getelementptr [)"<<allocated_space<<R"( x i8], [)"<<allocated_space<<R"( x i8]* %_scanf_format_1, i32 0, i32 0)"<<std::endl;


    out<<"\t"<<R"(call i32 (i8*, ...) @scanf(i8* %_scanf_str_1)";
    for(auto child:node->children){
        out<<", i32* %"<<child->lexeme;
    }
    out<<")"<<std::endl;
}

void IR_Generator::generateWriteNode(Node* node) {
    // allocate space
    int allocated_space = node->children.size() * 3 + 1; // 对于每个%d加上\n\0

    // construct string
    std::string format_string = "%d";
    for(size_t i = 0; i < node->children.size()-1; ++i) {
        format_string += " %d";
    }
    format_string += "\\0A\\00";

    out << "\t" << R"(%_printf_format_1 = alloca [)" << allocated_space << R"( x i8])" << std::endl;
    out << "\t" << R"(store [)" << allocated_space << R"( x i8] c")" << format_string << R"(", [)" << allocated_space << R"( x i8]* %_printf_format_1)" << std::endl;
    out << "\t" << R"(%_printf_str_1 = getelementptr [)" << allocated_space << R"( x i8], [)" << allocated_space << R"( x i8]* %_printf_format_1, i32 0, i32 0)" << std::endl;

    std::string expr_list="";
    for(auto child:node->children) {
        if(child->symbol_class==SymbolClass::INTLITERAL){
            expr_list+=", i32 "+child->lexeme;
        } else{
            IR_Generator::gen_llvm_ir(child);
            expr_list += ", i32 %_tmp_" + std::to_string(IR_Generator::local_tmp);          
        }
    }

    out << "\t" << R"(call i32 (i8*, ...) @printf(i8* %_printf_str_1)";
    out<<expr_list;
    out << ")" << std::endl;
}


void IR_Generator::generateAssignOPNode(Node* node){
    Node* lhs=node->children[0];
    Node* rhs=node->children[1];
    //if rhs is INTLITERAL(why don't declare it first whatever it appears for the first time?)
    if(rhs->symbol_class==SymbolClass::INTLITERAL){
        if(!contains(IR_Generator::id_table,lhs->lexeme)){
            IR_Generator::id_table.push_back(lhs->lexeme);
            out<<"\t"<<"%"<<lhs->lexeme<<" = alloca i32\n";
            out<<"\tstore i32 "<<rhs->lexeme<<", i32* %"<<lhs->lexeme<<std::endl;
            return;
        } else{
            out<<"\tstore i32 "<<rhs->lexeme<<", i32* %"<<lhs->lexeme<<std::endl;
            return;
        }
    } else{
        if(!contains(IR_Generator::id_table,lhs->lexeme)){
            IR_Generator::id_table.push_back(lhs->lexeme);
            IR_Generator::gen_llvm_ir(rhs);
            out<<"\t"<<"%"<<lhs->lexeme<<" = alloca i32\n";
            out<<"\tstore i32 %_tmp_"<<IR_Generator::local_tmp<<", i32* %"<<lhs->lexeme<<std::endl;
            return;
        } else{
            IR_Generator::gen_llvm_ir(rhs);
            out<<"\tstore i32 %_tmp_"<<IR_Generator::local_tmp<<", i32* %"<<lhs->lexeme<<std::endl;
            return;
        }
        
    }
}

void IR_Generator::generatePlusOPNode(Node* node){
    //add int values a+b a+(b+c) ID+Expression Expression+ID
    Node* lhs=node->children[0];
    Node* rhs=node->children[1];

    if(lhs->symbol_class==SymbolClass::INTLITERAL and rhs->symbol_class==SymbolClass::INTLITERAL){  //a=1+2
        out<<"\t"<<"%_tmp_"<<++IR_Generator::local_tmp<<" = add i32 ";
        IR_Generator::gen_llvm_ir(lhs);
        out<<", ";
        IR_Generator::gen_llvm_ir(rhs);
        out<<std::endl;
    } else if(lhs->symbol_class!=SymbolClass::INTLITERAL and rhs->symbol_class==SymbolClass::INTLITERAL){   //a=_tmp_+1
        IR_Generator::gen_llvm_ir(lhs);
        out<<"\t"<<"%_tmp_"<<++IR_Generator::local_tmp<<" = add i32 ";
        out<<"%_tmp_"<<IR_Generator::local_tmp-1<<", ";
        IR_Generator::gen_llvm_ir(rhs);
        out<<std::endl;
    } else if(lhs->symbol_class==SymbolClass::INTLITERAL and rhs->symbol_class!=SymbolClass::INTLITERAL){   //a=1+_tmp_?
        IR_Generator::gen_llvm_ir(rhs);
        out<<"\t"<<"%_tmp_"<<++IR_Generator::local_tmp<<" = add i32 ";
        IR_Generator::gen_llvm_ir(lhs);
        out<<", %_tmp_"<<IR_Generator::local_tmp-1<<std::endl;
    }else{  //a=_tmp_?+_tmp_?
        IR_Generator::gen_llvm_ir(lhs);
        int tmp1=IR_Generator::local_tmp;
        IR_Generator::gen_llvm_ir(rhs);
        int tmp2=IR_Generator::local_tmp;
        out<<"\t"<<"%_tmp_"<<++IR_Generator::local_tmp<<" = add i32 "<<"%_tmp_"<<tmp1<<", %_tmp_"<<tmp2;
        out<<std::endl;
    }
}

void IR_Generator::generateMinusOPNode(Node* node){
    Node* lhs=node->children[0];
    Node* rhs=node->children[1];

    if(lhs->symbol_class==SymbolClass::INTLITERAL and rhs->symbol_class==SymbolClass::INTLITERAL){  //a=1+2
        out<<"\t"<<"%_tmp_"<<++IR_Generator::local_tmp<<" = sub i32 ";
        IR_Generator::gen_llvm_ir(lhs);
        out<<", ";
        IR_Generator::gen_llvm_ir(rhs);
        out<<std::endl;
    } else if(lhs->symbol_class!=SymbolClass::INTLITERAL and rhs->symbol_class==SymbolClass::INTLITERAL){   //a=_tmp_+1
        IR_Generator::gen_llvm_ir(lhs);
        out<<"\t"<<"%_tmp_"<<++IR_Generator::local_tmp<<" = sub i32 ";
        out<<"%_tmp_"<<IR_Generator::local_tmp-1<<", ";
        IR_Generator::gen_llvm_ir(rhs);
        out<<std::endl;
    } else if(lhs->symbol_class==SymbolClass::INTLITERAL and rhs->symbol_class!=SymbolClass::INTLITERAL){   //a=1+_tmp_?
        IR_Generator::gen_llvm_ir(rhs);
        out<<"\t"<<"%_tmp_"<<++IR_Generator::local_tmp<<" = sub i32 ";
        IR_Generator::gen_llvm_ir(lhs);
        out<<", %_tmp_"<<IR_Generator::local_tmp-1<<std::endl;
    }else{  //a=_tmp_?+_tmp_?
        IR_Generator::gen_llvm_ir(lhs);
        int tmp1=IR_Generator::local_tmp;
        IR_Generator::gen_llvm_ir(rhs);
        int tmp2=IR_Generator::local_tmp;
        out<<"\t"<<"%_tmp_"<<++IR_Generator::local_tmp<<" = sub i32 "<<"%_tmp_"<<tmp1<<", %_tmp_"<<tmp2;
        out<<std::endl;
    }
}

void IR_Generator::generateIntliteralNode(Node* node){
    out<<node->lexeme;
}

void IR_Generator::generateIDNode(Node* node){
    // if this function is called, then ID must be at rhs of ":="
    // load is needed
    out<<"\t%_tmp_"<<++IR_Generator::local_tmp<<" = load i32, i32* %"<<node->lexeme<<std::endl;
}


//other functions
bool contains(const std::vector<std::string>& vec, const std::string& value) {
    // 使用std::find查找value在vector中的位置
    auto it = std::find(vec.begin(), vec.end(), value);
    // 如果迭代器不是vector的末尾，表示找到了value
    return it != vec.end();
}