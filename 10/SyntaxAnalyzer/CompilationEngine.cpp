#include "CompilationEngine.h"

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

void CompilationEngine::compileSymbol(string where, string symbol) {
    if (!checkType(Token::SYMBOL) || tokenizer->symbol() != symbol[0])
        outputError(where, "symbol " + symbol);
    output("symbol", symbol);
}

void CompilationEngine::compileIdentifier() {
    if (!checkType(Token::IDENTIFIER))
        outputError("compileIdentifier; ", "identifier");
    output("identifier", tokenizer->identifier());
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
    compileSymbol(where, "{");

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
    compileSymbol(where, "}");

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
    compileSymbol(where, "(");

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
    compileSymbol(where, "{");

    // varDec*
    string word;
    while (checkType(Token::KEYWORD) && (word = tokenizer->keyword()) == "var")
        compileVarDec();

    // statements
    if (word != "" && word != "var")
        compileStatements(word);

    // }
    compileSymbol(where, "}");
}

void CompilationEngine::compileVarDec() {
    string where = "compileVarDec: ";
    fout << "<varDec>" << endl;
    output("keyword", "var");

    // typeName
    compileTypename();

    // varName
    compileIdentifier();

    while (checkType(Token::SYMBOL)) {
        char ch = tokenizer->symbol();
        if (ch == ';')
            break;
        if (ch != ',')
            outputError(where, "symbol ,");
        output("symbol", ",");

        // typename
        compileTypename();
        // varName
        compileIdentifier();
    }

    output("symbol", ";");

    fout << "</varDec>" << endl;
}

void CompilationEngine::compileStatements(string word) {
    string where = "compileStatements: ";
    fout << "<statements>" << endl;
    while (true) {
        if (word == "let")
            compileLet();
        else if (word == "if")
            compileIf();
        else if (word == "while")
            compileWhile();
        else if (word == "do")
            compileDo();
        else if (word == "return")
            compileReturn();
        else
            outputError(where, "keyword let or if or while or do or return");
        if (!checkType(Token::KEYWORD))
            break;
        word = tokenizer->keyword();
    }
    fout << "</statements>" << endl;
}

void CompilationEngine::compileLet() {
    string where = "compileLet: ";
    fout << "<letStatement>" << endl;
    output("keyword", "let");
    // varname
    compileIdentifier();
    if (!checkType(Token::SYMBOL))
        outputError(where, "symbol");
    char ch = tokenizer->symbol();
    if (ch == '[') {
        output("symbol", "[");
        compileExpression();
        compileSymbol(where, "]");
        if (!checkType(Token::SYMBOL))
            outputError(where, "symbol");
        ch = tokenizer->symbol();
    }

    if (ch != '=')
        outputError(where, "symbol =");
    output("symbol", "=");

    compileExpression();

    compileSymbol(where, ";");

    fout << "</letStatement>" << endl;
}