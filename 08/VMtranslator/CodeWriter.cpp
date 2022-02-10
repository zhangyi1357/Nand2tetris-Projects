#include "CodeWriter.h"
#include <iostream>
#include <cassert>
using namespace std;

CodeWriter::CodeWriter(string pathname) : labelNum(0), programCnt(0) {
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

void CodeWriter::writeInit() {
    fout << "@256\nD=A\n@SP\nM=D\n"; // initialize RAM[0] = 256
    fout << "@START\n0;JMP\n";
    fout << "(MAKETRUE)\n" << decSP() << "M=-1\n" << incSP() << getReturn();
    fout << "(START)\n";
    // fout << "@_MAIN_\n0;JMP\n"; // Single file compile doesn't need this
}

void CodeWriter::setFileName(string filename) {
    size_t slash = filename.find_last_of("\\");
    this->filename = (slash == string::npos) ? filename : filename.substr(slash + 1);
    for (int i = 0; i < this->filename.size(); i++)
        this->filename[i] = toupper(this->filename[i]);
    fout << "(_" << this->filename << "_)\n";
}

void CodeWriter::close() {
    fout << "(END)\n@END\n0;JMP\n";
    fout.close();
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
    fout << "\n";
    fout << "@" << label << "\n0;JMP\n";
    fout << "\n";
}

void CodeWriter::writeLabel(string label) {
    fout << "\n";
    fout << "(" << label << ")\n";
    fout << "\n";
}

void CodeWriter::writeIf(string label) {
    fout << "\n";
    fout << decSP() << "D=M\n@" << label << "\nD;JNE\n";
    fout << "\n";
}
