#ifndef _CODEWRITER_
#define _CODEWRITER_
#include <fstream>
#include <string>

#include "Parser.h"
using std::ofstream;
using std::string;

class CodeWriter {
   public:
    CodeWriter(string pathname);
    ~CodeWriter();
    void writeInit();
    void setFileName(string filename);
    void writeArithmetic(string cmd);
    void writePushPop(CommandType cmdType, string segment, int index);
    void writeLabel(string label);
    void writeGoto(string label);
    void writeIf(string label);
    void writeCall(string functionName, int numArgs);
    void writeReturn();
    void writeFunction(string functionName, int numLocals);
    void close();

   private:
    ofstream fout;
    int labelNum;  // return label number (initial 0)
    string filename;
    string functionName;
    string decSP();
    string incSP();
    string getSP();
    string getReturn();
    string setReturn();
};

#endif