#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "CodeWriter.h"
#include "Parser.h"
#include "dirent.h"
using namespace std;

vector<string> filefinder(string path, string const& postfix = ".vm");

int main(int argc, char** argv) {
    vector<string> files;
    string path = argv[1];
    if (hasEnding(path)) {
        files.push_back(path);
    } else {
        files = filefinder(path);
    }
    CodeWriter codeWriter(path);
    codeWriter.writeInit();
    for (int i = 0; i < files.size(); i++) {
        Parser parser(files[i]);
        string filename = getNoPostFilename(files[i]);
        // cout << outputFilename << endl;
        codeWriter.setFileName(filename);
        while (parser.hasMoreCommands()) {
            CommandType cmdType = parser.commandType();
            // cout << int(cmdType) << endl;
            if (cmdType == CommandType::C_ARITHMETIC)
                codeWriter.writeArithmetic(parser.arg1(cmdType));
            else if (cmdType == CommandType::C_PUSH ||
                     cmdType == CommandType::C_POP)
                codeWriter.writePushPop(cmdType, parser.arg1(cmdType),
                                        parser.arg2());
            else if (cmdType == CommandType::C_LABEL)
                codeWriter.writeLabel(parser.arg1(cmdType));
            else if (cmdType == CommandType::C_GOTO)
                codeWriter.writeGoto(parser.arg1(cmdType));
            else if (cmdType == CommandType::C_IF)
                codeWriter.writeIf(parser.arg1(cmdType));
            else if (cmdType == CommandType::C_FUNCTION)
                codeWriter.writeFunction(parser.arg1(cmdType), parser.arg2());
            else if (cmdType == CommandType::C_RETURN)
                codeWriter.writeReturn();
            else if (cmdType == CommandType::C_CALL)
                codeWriter.writeCall(parser.arg1(cmdType), parser.arg2());
            else {
                cout << "Not finished yet." << endl;
            }
        }
        codeWriter.close();
    }
    return 0;
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
