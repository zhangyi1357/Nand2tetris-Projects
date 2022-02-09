#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <cassert>
#include "dirent.h"
using namespace std;

vector<string> filefinder(string path, string const& postfix = ".vm");
bool hasEnding(string const& fullString, string const& ending = ".vm");
string getOutputFilename(const string& inputFilename);
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

class CodeWriter {
public:
    CodeWriter() {};
    void setFileName(string filename);
    void writeArithmetic(string cmd);
    void writePushPop(CommandType cmdType, string segment, int index);
    void close();
private:
    ofstream fout;
};

int main(int argc, char** argv) {
    vector<string> files;
    string path = argv[1];
    if (hasEnding(path))
        files.push_back(path);
    else
        files = filefinder(path);
    CodeWriter codeWriter;
    for (int i = 0; i < files.size(); i++) {
        Parser parser(files[i]);
        string outputFilename = getOutputFilename(files[i]);
        // cout << outputFilename << endl;
        codeWriter.setFileName(outputFilename);
        while (parser.hasMoreCommands()) {
            CommandType cmdType = parser.commandType();
            if (cmdType == CommandType::C_ARITHMETIC) {
                string cmd = parser.arg1(cmdType);
                codeWriter.writeArithmetic(cmd);
            }
            else if (cmdType == CommandType::C_PUSH || cmdType == CommandType::C_POP) {
                codeWriter.writePushPop(cmdType, parser.arg1(cmdType), parser.arg2());
            }
            else {
                cout << "Not finished yet." << endl;
            }
        }
        codeWriter.close();
    }
    return 0;
}

// get from
// https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
bool hasEnding(string const& fullString, string const& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}
// get from 
// https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
vector<string> filefinder(string path, string const& postfix) {
    vector<string> files;
    DIR* directory = opendir(path.c_str());
    struct dirent* direntStruct;
    if (directory != NULL) {
        while (direntStruct = readdir(directory)) {
            if (hasEnding(direntStruct->d_name, postfix)) {
                files.push_back(path + direntStruct->d_name);
                // cout << path + direntStruct->d_name << endl;
            }
        }
        closedir(directory);
    }
    return files;
}

string getOutputFilename(const string& inputFilename) {
    return inputFilename.substr(0, inputFilename.find_last_of(".")) + ".asm";
}

Parser::Parser(string filename) :lineNum(-1) {
    fstream fin(filename);
    string line;
    while (getline(fin, line))
    {
        size_t found;
        // Remove comment
        found = line.find_first_of("/");
        if (found != string::npos)
            line.erase(found);
        // Remove leading and ending spaces
        found = line.find_first_not_of(" ");
        if (found != string::npos)
            line.erase(0, found);
        found = line.find_last_not_of(" ");
        if (found != string::npos)
            line.erase(found + 1);
        // if line is empty drop it
        if (!line.empty()) {
            lines.push_back(line);
            cout << line << endl;
        }
    }
    fin.close();
}

bool Parser::hasMoreCommands() {
    return (++lineNum < lines.size()) ? true : false;
}

CommandType Parser::commandType() {
    string cmd = lines[lineNum].substr(0, lines[lineNum].find_first_of(" "));
    CommandType cmdType;
    if (cmd == "add" || cmd == "sub" || cmd == "neg" ||
        cmd == "eq" || cmd == "gt" || cmd == "lt" ||
        cmd == "and" || cmd == "or" || cmd == "not")
        cmdType = CommandType::C_ARITHMETIC;
    else if (cmd == "push")
        cmdType = CommandType::C_PUSH;
    else if (cmd == "pop")
        cmdType = CommandType::C_POP;
    else if (cmd == "label")
        cmdType == CommandType::C_LABEL;
    else if (cmd == "goto")
        cmdType == CommandType::C_GOTO;
    else if (cmd == "if-goto")
        cmdType == CommandType::C_IF;
    else if (cmd == "function")
        cmdType == CommandType::C_FUNCTION;
    else if (cmd == "call")
        cmdType == CommandType::C_CALL;
    else if (cmd == "return")
        cmdType == CommandType::C_RETURN;
    else
        cout << "not valid cmdtype: " << cmd;
    return cmdType;
}

string Parser::arg1(CommandType cmdType) {
    assert(cmdType != CommandType::C_RETURN);
    string arg1;
    if (cmdType == CommandType::C_ARITHMETIC)
        arg1 = lines[lineNum];
    else {
        size_t firstSpace = lines[lineNum].find_first_of(" ");
        size_t lastSpace = lines[lineNum].find_last_of(" ");
        if (lastSpace != firstSpace)
            arg1 = lines[lineNum].substr(firstSpace + 1, lastSpace - firstSpace - 1);
        else
            arg1 = lines[lineNum].substr(firstSpace + 1, string::npos);
    }
    cout << "arg1: " << arg1 << endl;
    return arg1;
}

int Parser::arg2() {
    size_t lastSpace = lines[lineNum].find_last_of(" ");
    string arg2 = lines[lineNum].substr(lastSpace + 1);
    // cout << arg2 << ' ' << arg2.size() << endl;
    return stoi(arg2);
}

void CodeWriter::setFileName(string filename) {
    fout.open(filename);
}

void CodeWriter::close() {
    fout << "(END)\n@END\n0;JMP\n";
    fout.close();
}

void CodeWriter::writeArithmetic(string cmd) {
    if (cmd == "add") {
        fout << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=D+M\n@SP\nM=M+1\n";
    }
}

void CodeWriter::writePushPop(CommandType cmdType, string segment, int index) {
    assert(cmdType == CommandType::C_PUSH || cmdType == CommandType::C_POP);
    cout << "segment: " << segment << segment.size() << endl;
    if (cmdType == CommandType::C_PUSH) {
        if (segment == "constant")
            fout << "@" << index << "\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
        else
            cout << "push: only constant now.\n";
    }
    else {
        cout << "Pop not finished yet" << endl;
    }
}