#include "CodeWriter.h"
#include <iostream>
#include <cassert>
using namespace std;

CodeWriter::CodeWriter(string pathname) : labelNum(0) {
    // pathname: either ".\*\" or ".\*\xxx.vm"
    string outputFilename;
    if (hasEnding(pathname, ".vm")) {
        outputFilename = getNoPostFilename(pathname);
    }
    else {
        pathname.erase(pathname.size() - 1); // remove last character
        int found = pathname.find_last_of("\\");
        filename = (found == string::npos) ? pathname : pathname.substr(found + 1);
        outputFilename = pathname + "\\" + filename;
    }
    cout << outputFilename << endl;
    fout.open(outputFilename + ".asm");
}

CodeWriter::~CodeWriter() {
    fout.close();
}

void CodeWriter::writeInit() {
    fout << "@256\nD=A\n@SP\nM=D\n"; // comment this out in no init situation
    fout << "@START\n0;JMP\n";
    fout << "(MAKETRUE)\n" << decSP() << "M=-1\n" << incSP() << getReturn();
    fout << "(START)\n";
    writeCall("init", 0); // comment this out in no init situation
}

void CodeWriter::setFileName(string filename) {
    size_t slash = filename.find_last_of("\\");
    this->filename = (slash == string::npos) ? filename : filename.substr(slash + 1);
    for (int i = 0; i < this->filename.size(); i++)
        this->filename[i] = toupper(this->filename[i]);
    fout << "(_" << this->filename << "_)\n";
}

void CodeWriter::close() {
    fout << "(END)\n@END\n0;JMP\n"; // write infinite loop
}

string CodeWriter::decSP() { return "@SP\nAM=M-1\n"; }

string CodeWriter::incSP() { return "@SP\nAM=M+1\n"; }

string CodeWriter::getSP() { return "@SP\nA=M\n"; }

string CodeWriter::setReturn() { return  "@RETURN" + to_string(labelNum) + "\nD=A\n@R15\nM=D\n"; }

string CodeWriter::getReturn() { return  "@R15\nA=M\n0;JMP\n"; }

void CodeWriter::writeArithmetic(string cmd) {
    if (cmd == "add" || cmd == "sub" || cmd == "and" || cmd == "or")
    {
        fout << decSP() << "D=M\n" << decSP();
        if (cmd == "add")      fout << "M=D+M\n";
        else if (cmd == "sub") fout << "M=M-D\n";
        else if (cmd == "and") fout << "M=D&M\n";
        else                   fout << "M=D|M\n";
        fout << incSP();
    }
    else if (cmd == "neg" || cmd == "not") {
        fout << decSP();
        if (cmd == "neg") fout << "M=-M\n";
        else              fout << "M=!M\n";
        fout << incSP();
    }
    else if (cmd == "eq" || cmd == "gt" || cmd == "lt") {
        fout << setReturn();
        fout << decSP() << "D=M\n" << decSP() << "D=M-D\nM=0\n" << incSP() << "@MAKETRUE\n";
        if (cmd == "eq")      fout << "D;JEQ\n";
        else if (cmd == "gt") fout << "D;JGT\n";
        else                  fout << "D;JLT\n";
        fout << "(RETURN" << labelNum++ << ")\n";
    }
    else {
        cout << "Invalid Arithmetic OP\n";
    }
}

void CodeWriter::writePushPop(CommandType cmdType, string segment, int index) {
    assert(cmdType == CommandType::C_PUSH || cmdType == CommandType::C_POP);
    // cout << "segment: " << segment << segment.size() << endl;
    if (cmdType == CommandType::C_PUSH) {
        // put the value need to be pushed in D
        if (segment == "constant")
            fout << "@" << index << "\nD=A\n";
        else if (segment == "static") {
            fout << "@" << filename << "." << index << "\nD=M\n";
        }
        else {
            if (segment == "pointer" || segment == "temp") {
                if (segment == "pointer") fout << "@THIS\n";
                else                      fout << "@R5\n";
                fout << "D=A\n";
            }
            else {
                if (segment == "local")         fout << "@LCL\n";
                else if (segment == "argument") fout << "@ARG\n";
                else if (segment == "this")     fout << "@THIS\n";
                else                            fout << "@THAT\n";
                fout << "D=M\n";
            }
            fout << "@" << index << "\nA=D+A\nD=M\n";
        }
        // push D to stack
        fout << getSP() << "M=D\n" << incSP();

    }
    else {
        if (segment == "static") {
            fout << decSP() << "D=M\n";
            fout << "@" << filename << "." << index << "\nM=D\n";
        }
        else {
            if (segment == "pointer" || segment == "temp") {
                if (segment == "pointer") fout << "@THIS\n";
                else                      fout << "@R5\n";
                fout << "D=A\n@";
            }
            else {
                if (segment == "local")         fout << "@LCL\n";
                else if (segment == "argument") fout << "@ARG\n";
                else if (segment == "this")     fout << "@THIS\n";
                else                            fout << "@THAT\n";
                fout << "D=M\n@";
            }
            // R13 is the address of RAM
            fout << index << "\nD=D+A\n" << "@R13\nM=D\n" << decSP() << "D=M\n" << "@R13\nA=M\nM=D\n";
        }
    }
}

void CodeWriter::writeGoto(string label) {
    // fout << "\n";
    fout << "@" << functionName << "$" << label << "\n0;JMP\n";
    // fout << "\n";
}

void CodeWriter::writeLabel(string label) {
    // fout << "\n";
    fout << "(" << functionName << "$" << label << ")\n";
    // fout << "\n";
}

void CodeWriter::writeIf(string label) {
    // fout << "\n";
    fout << decSP() << "D=M\n@" << functionName << "$" << label << "\nD;JNE\n";
    // fout << "\n";
}

void CodeWriter::writeCall(string functionName, int numArgs) {
    fout << "\n";
    // push RETURNlabelNum
    fout << "@RETURN" << labelNum << "\nD=A\n" << getSP() << "M=D\n" << incSP();
    // push LCL
    fout << "@LCL\nD=M\n" << getSP() << "M=D\n" << incSP();
    // push ARG
    fout << "@ARG\nD=M\n" << getSP() << "M=D\n" << incSP();
    // push THIS
    fout << "@THIS\nD=M\n" << getSP() << "M=D\n" << incSP();
    // push THAT
    fout << "@THAT\nD=M\n" << getSP() << "M=D\n" << incSP();
    // ARG = SP - n - 5
    fout << "@SP\nD=M\n@" << numArgs << "\nD=D-A\n@5\nD=D-A\n@ARG\nM=D\n";
    // LCL = SP
    fout << "@SP\nD=M\n@LCL\nM=D\n";
    // goto f
    fout << "@" << functionName << "\n0;JMP\n";
    // (RETURNlabelNum)
    fout << "(RETURN" << labelNum++ << ")\n";
    fout << "\n";
}

void CodeWriter::writeReturn() {
    fout << "\n";
    // FRAME = LCL
    fout << "@LCL\nD=M\n@FRAME\nM=D\n";
    // RET = *(FRAME - 5)
    fout << "@FRAME\nD=M\n@5\nA=D-A\nD=M\n@RET\nM=D\n";
    // *ARG = pop()
    fout << decSP() << "D=M\n@ARG\nA=M\nM=D\n";
    // SP = ARG + 1
    fout << "@ARG\nD=M+1\n@SP\nM=D\n";
    // THAT = *(FRAME - 1)
    fout << "@FRAME\nD=M\n@1\nA=D-A\nD=M\n@THAT\nM=D\n";
    // THIS = *(FRAME - 2)
    fout << "@FRAME\nD=M\n@2\nA=D-A\nD=M\n@THIS\nM=D\n";
    // ARG = *(FRAME - 3)
    fout << "@FRAME\nD=M\n@3\nA=D-A\nD=M\n@ARG\nM=D\n";
    // LCL = *(FRAME - 4)
    fout << "@FRAME\nD=M\n@4\nA=D-A\nD=M\n@LCL\nM=D\n";
    // goto RET
    fout << "@RET\nA=M\n0;JMP\n";
    fout << "\n";
}

void CodeWriter::writeFunction(string functionName, int numLocals) {
    fout << "\n";
    this->functionName = functionName;
    fout << "(" << functionName << ")\n";
    for (int i = 0; i < numLocals; i++)
        writePushPop(CommandType::C_PUSH, "constant", 0);
    fout << "\n";
}