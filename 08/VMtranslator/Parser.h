#ifndef _PARSER_
#define _PARSER_
#include <string>
#include <vector>

using std::string;
using std::vector;

enum class CommandType { C_ARITHMETIC, C_PUSH, C_POP, C_LABEL, C_GOTO, C_IF, C_FUNCTION, C_RETURN, C_CALL };
class Parser {
public:
    Parser(string filename);
    bool hasMoreCommands();
    CommandType commandType();
    string arg1(CommandType cmdType);
    int arg2();
private:
    vector<string> lines;
    int lineNum; // current working line number (initial -1)
};

#endif