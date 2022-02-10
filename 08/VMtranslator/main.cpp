#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <cassert>
#include "dirent.h"
#include "Parser.h"
#include "CodeWriter.h"
using namespace std;

vector<string> filefinder(string path, string const& postfix = ".vm");
bool hasEnding(string const& fullString, string const& ending = ".vm");
string getNoPostFilename(const string& inputFilename);

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
        string filename = getNoPostFilename(files[i]);
        // cout << outputFilename << endl;
        codeWriter.setFileName(filename);
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

string getNoPostFilename(const string& inputFilename) {
    return inputFilename.substr(0, inputFilename.find_last_of("."));
}