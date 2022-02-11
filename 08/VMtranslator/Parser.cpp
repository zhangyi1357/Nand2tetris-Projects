#include "Parser.h"

#include <cassert>
#include <fstream>
#include <iostream>
using namespace std;

Parser::Parser(string filename) : lineNum(-1) {
    ifstream fin(filename);
    string line;
    while (getline(fin, line)) {
        size_t found;
        // Remove comment
        found = line.find_first_of("/");
        if (found != string::npos) line.erase(found);
        // Remove leading and ending spaces
        found = line.find_first_not_of(" ");
        if (found != string::npos) line.erase(0, found);
        found = line.find_last_not_of(" ");
        if (found != string::npos) line.erase(found + 1);
        // if line is empty drop it
        if (!line.empty()) {
            lines.push_back(line);
            // cout << line << endl;
        }
    }
    fin.close();
}

bool Parser::hasMoreCommands() {
    return (++lineNum < lines.size()) ? true : false;
}

CommandType Parser::commandType() {
    string cmd = lines[lineNum].substr(0, lines[lineNum].find_first_of(" "));
    // cout << cmd << endl;
    CommandType cmdType;
    if (cmd == "add" || cmd == "sub" || cmd == "neg" || cmd == "eq" ||
        cmd == "gt" || cmd == "lt" || cmd == "and" || cmd == "or" ||
        cmd == "not")
        cmdType = CommandType::C_ARITHMETIC;
    else if (cmd == "push")
        cmdType = CommandType::C_PUSH;
    else if (cmd == "pop")
        cmdType = CommandType::C_POP;
    else if (cmd == "label")
        cmdType = CommandType::C_LABEL;
    else if (cmd == "goto")
        cmdType = CommandType::C_GOTO;
    else if (cmd == "if-goto")
        cmdType = CommandType::C_IF;
    else if (cmd == "function")
        cmdType = CommandType::C_FUNCTION;
    else if (cmd == "call")
        cmdType = CommandType::C_CALL;
    else if (cmd == "return")
        cmdType = CommandType::C_RETURN;
    else
        cout << "not valid cmdtype: " << cmd;
    // cout << int(cmdType);
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
    // cout << "arg1: " << arg1 << endl;
    return arg1;
}

int Parser::arg2() {
    size_t lastSpace = lines[lineNum].find_last_of(" ");
    string arg2 = lines[lineNum].substr(lastSpace + 1);
    // cout << arg2 << ' ' << arg2.size() << endl;
    return stoi(arg2);
}

// get from
// https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
bool hasEnding(string const& fullString, string const& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(),
                                        ending.length(), ending));
    } else {
        return false;
    }
}

string getNoPostFilename(const string& inputFilename) {
    return inputFilename.substr(0, inputFilename.find_last_of("."));
}