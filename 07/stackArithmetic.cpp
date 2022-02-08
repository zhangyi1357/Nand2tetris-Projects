#include <iostream>
#include <string>
#include <vector>
#include "dirent.h"
using namespace std;

vector<string> filefinder(string path, string const& postfix = ".vm");
bool hasEnding(string const& fullString, string const& ending = ".vm");

int main(int argc, char** argv) {
    vector<string> files;
    string path = argv[1];
    if (hasEnding(path))
        files.push_back(path);
    else
        files = filefinder(path);

    return 0;
}

bool hasEnding(string const& fullString, string const& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    }
    else {
        return false;
    }
}

vector<string> filefinder(string path, string const& postfix) {
    vector<string> files;
    DIR* directory = opendir(path.c_str());
    struct dirent* direntStruct;
    if (directory != NULL) {
        while (direntStruct = readdir(directory)) {
            if (hasEnding(direntStruct->d_name, postfix)) {
                files.push_back(path + direntStruct->d_name);
                cout << path + direntStruct->d_name << endl;
            }
        }
        closedir(directory);
    }
    return files;
}