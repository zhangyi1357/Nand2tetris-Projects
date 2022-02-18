#include "CompilationEngine.h"

#include <cassert>
#include <iostream>

bool CompilationEngine::setFileName(string filename, JackTokenizer *tokenizer) {
    this->tokenizer = tokenizer;
    if (fout.is_open()) fout.close();
    // remove ".jack" add "Tmy.xml"
    fout.open(filename.substr(0, filename.size() - 5) + "_my.xml");
    if (!fout.is_open()) {
        cerr << "File " << filename.substr(0, filename.size() - 5) + "_my.xml"
             << " Can't open!";
        return false;
    }
    return true;
}

inline bool CompilationEngine::checkType(Token type) {
    assert(tokenizer->hasMoreTokens() && tokenizer->tokenType() == type);
    return true;
}

inline void CompilationEngine::output(string label, string content) {
    fout << "<" << label << "> " << content << " </" << label << ">\n";
}

void CompilationEngine::compileClass() {
    string word;
    char ch;
    fout << "<class>\n";

    checkType(Token::KEYWORD);
    assert(tokenizer->keyword() == "class");
    output("keyword", "class");

    checkType(Token::IDENTIFIER);
    word = tokenizer->identifier();
    output("identifier", word);

    checkType(Token::SYMBOL);
    assert(tokenizer->symbol() == '{');
    output("symbol", "{");

    while (tokenizer->tokenType() == Token::KEYWORD) {
        word = tokenizer->keyword();
        if (word == "field" || word == "static") {
            fout << "<classVarDec>\n";
            output("keyword", word);
            compileClassVarDec();
            fout << "</classVarDec>\n";
        } else if (word == "constructor" || word == "function" || word == "method" || word == "void") {
            fout << "<subroutineDec>\n";
            output("keyword", word);
            compileSubroutine();
            fout << "</subroutineDec>";
        } else
            cerr << "not valid type!\n";
    }

    assert(tokenizer->symbol() == '}');
    output("symbol", "}");
    fout << "</class>\n";
}

void CompilationEngine::compileClassVarDec() {
    Token type = tokenizer->tokenType();
    assert(type == Token::IDENTIFIER || type == Token::KEYWORD);
    if (type == Token::IDENTIFIER)
        output("identifier", tokenizer->identifier());
    else
        output("keyword", tokenizer->keyword());
    
}
void CompilationEngine::compileSubroutine() {
}