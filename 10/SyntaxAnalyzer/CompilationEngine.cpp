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
    return tokenizer->hasMoreTokens() && tokenizer->tokenType() == type;
}

inline void CompilationEngine::outputError(string where, string name) {
    cerr << where << "should be " << name << endl;
    fout << "Compilation failed here!" << endl;
}

inline void CompilationEngine::output(string label, string content) {
    fout << "<" << label << "> " << content << " </" << label << ">\n";
}

void CompilationEngine::compileSymbol(string where, char symbol) {
    if (!checkType(Token::SYMBOL) || tokenizer->symbol() != symbol)
        outputError(where, "symbol " + symbol);
    output("symbol", "" + symbol);
}

void CompilationEngine::compileClass() {
    string where = "compileClass: ";
    fout << "<class>" << endl;

    // class
    if (!checkType(Token::KEYWORD) || tokenizer->keyword() != "class")
        outputError(where, "keyword class");
    output("keyword", "class");

    // className
    if (!checkType(Token::IDENTIFIER))
        outputError(where, "identifier");
    output("identifier", tokenizer->identifier());

    // {
    if (!checkType(Token::SYMBOL) || tokenizer->symbol() != '{')
        outputError(where, "symbol {");
    output("symbol", "{");

    // classVarDec* & subroutineDec*
    while (tokenizer->tokenType() == Token::KEYWORD) {
        string word = tokenizer->keyword();
        if (word == "field" || word == "static") {
            fout << "<classVarDec>" << endl;
            output("keyword", word);
            compileClassVarDec();
            fout << "</classVarDec>" << endl;
        } else if (word == "constructor" || word == "function" || word == "method" || word == "void") {
            fout << "<subroutineDec>" << endl;
            output("keyword", word);
            compileSubroutine();
            fout << "</subroutineDec>" << endl;
        } else
            outputError(where, "keyword for classVarDec or subroutineDec");
    }

    // }
    if (!checkType(Token::SYMBOL) || tokenizer->symbol() != '}')
        outputError(where, "symbol }");
    output("symbol", "}");
    fout << "</class>" << endl;
}

void CompilationEngine::compileTypename() {
    string where = "compileTypename: ";
    if (!checkType(Token::IDENTIFIER) && !checkType(Token::KEYWORD))
        outputError(where, "identifier or keyword");
    Token type = tokenizer->tokenType();
    if (type == Token::IDENTIFIER)
        output("identifier", tokenizer->identifier());
    else {
        string word = tokenizer->keyword();
        if (word != "int" && word != "char" && word != "boolean" && word != "void")
            outputError(where, "keyword int or char or boolean");
        output("keyword", word);
    }
}

void CompilationEngine::compileClassVarDec() {
    string where = "compileClassVarDec: ";

    // typeName
    // some problem here (void can also pass)
    compileTypename();

    // varName
    if (!checkType(Token::IDENTIFIER))
        outputError(where, "identifier");
    output("identifier", tokenizer->identifier());

    // (',' varName)*
    while (checkType(Token::SYMBOL)) {
        char ch = tokenizer->symbol();
        if (ch == ';')
            break;
        if (ch != ',')
            outputError(where, "symbol ,");
        output("symbol", ",");
        if (!checkType(Token::IDENTIFIER))
            outputError(where, "identifier");
        output("identifier", tokenizer->identifier());
    }

    // ;
    output("symbol", ";");
}
void CompilationEngine::compileSubroutine() {
    string where = "compileSubroutine: ";

    // (void | type)
    compileTypename();

    // subroutineName
    if (!checkType(Token::IDENTIFIER))
        outputError(where, "identifier");
    output("identifier", tokenizer->identifier());

    // (
    if (!checkType(Token::SYMBOL) || tokenizer->symbol() != '(')
        outputError(where, "symbol (");
    output("symbol", "(");

    // parameterList
    fout << "<parameterList>" << endl;
    compileParameterList();
    fout << "</parameterList>" << endl;

    // check has been done by compileParameterList
    output("symbol", ")");

    // subroutineBody
    fout << "<subroutineBody>" << endl;
    compileSubroutineBody();
    fout << "</subroutineBody>" << endl;
}

void CompilationEngine::compileParameterList() {
    string where = "compileParameterList: ";
    // typeName
    // some problem here (void can also pass)
    compileTypename();

    // varName
    if (!checkType(Token::IDENTIFIER))
        outputError(where, "identifier");
    output("identifier", tokenizer->identifier());

    // (',' varName)*
    while (checkType(Token::SYMBOL)) {
        char ch = tokenizer->symbol();
        if (ch == ')')
            break;
        if (ch != ',')
            outputError(where, "symbol ,");
        output("symbol", ",");
        if (!checkType(Token::IDENTIFIER))
            outputError(where, "identifier");
        output("identifier", tokenizer->identifier());
    }
}

void CompilationEngine::compileSubroutineBody() {
    string where = "compileSubroutineBody: ";

    // {
    if (!checkType(Token::SYMBOL) || tokenizer->symbol() != '{')
        outputError(where, "symbol {");
    output("symbol", "{");

    // {
    if (!checkType(Token::SYMBOL) || tokenizer->symbol() != '{')
        outputError(where, "symbol {");
    output("symbol", "{");
}