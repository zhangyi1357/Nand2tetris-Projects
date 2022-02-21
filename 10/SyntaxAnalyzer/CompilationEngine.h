#ifndef _COMPILATIONENGINE_
#define _COMPILATIONENGINE_
#include <string>

#include "JackTokenizer.h"
using namespace std;

class CompilationEngine {
   private:
    JackTokenizer *tokenizer;
    ofstream fout;

   public:
    CompilationEngine() {}
    Token getTokenType();
    bool setFileName(string filename, JackTokenizer *tokenizer);
    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileSubroutineBody();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();
    bool checkType(Token type);
    void output(string label, string content);
    void outputError(string where, string name);
    void compileTypename();
    void compileSymbol(string where, char symbol);
};
#endif  // !_COMPILATIONENGINE_
