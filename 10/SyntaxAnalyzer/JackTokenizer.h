#include <fstream>
#include <iostream>
#include <set>
#include <string>
using namespace std;

enum struct Token { KEYWORD,
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
   public:
    JackTokenizer() {}
    void tokenize();
    void readFile();
    bool setFilename(const string& filename);

   private:
    static set<char> symbols;
    static set<string> keywords;
    ifstream fin;
    ofstream fout;
    string content;
    size_t k;  // next character index to read
    bool hasMoreTokens();
    Token tokenType();
    string keyword();
    char symbol();
    string identifier();
    int intVal();
    string stringVal();
};