#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
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
    string arg1();
    string arg2();
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

            }
            else if (cmdType == CommandType::C_PUSH || cmdType == CommandType::C_POP) {

            }
            else {
                cout << "not finished yet." << endl;
            }
        }
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
            line.erase(found);
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