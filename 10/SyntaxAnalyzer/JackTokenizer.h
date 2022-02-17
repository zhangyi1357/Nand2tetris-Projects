#include <fstream>
#include <iostream>
#include <string>
using namespace std;

enum struct TokenType { KEYWORD,
                        SYMBOL,
                        IDENTIFIER,
                        INT_CONST,
                        STRING_CONST };

enum struct KeyWord { CLASS,
                      METHOD,
                      INT,
                      FUNCTION,
                      BOOLEAN,
                      CONSTRUCTOR,
                      CHAR,
                      VOID,
                      VAR,
                      STATIC,
                      FIELD,
                      LET,
                      DO,
                      IF,
                      ELSE,
                      WHILE,
                      RETURN,
                      TRUE,
                      FALSE,
                      KNULL,
                      THIS
};

class JackTokenizer {
   private:
    ifstream fin;

   public:
    JackTokenizer(const string& filename);
    bool hasMoreTokens();
    void advance();
    TokenType tokenType();
    KeyWord keyword();
    char symbol();
    string identifier();
    int intVal();
    string stringVal();
};