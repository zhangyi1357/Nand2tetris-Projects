#ifndef _CODEWRITER_
#define _CODEWRITER_
#include <string>
#include <fstream>
#include "Parser.h"
using std::string;
using std::ofstream;

class CodeWriter {
public:
    CodeWriter() {};
    void setFileName(string filename);
    void writeArithmetic(string cmd);
    void writePushPop(CommandType cmdType, string segment, int index);
    void close();
private:
    ofstream fout;
    int labelNum; // return label number (initial 0)
    string filename;
    string decSP();
    string incSP();
    string getSP();
    string getReturn();
    string setReturn();
};

#endif