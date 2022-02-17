#include <fstream>
#include <iostream>
#include <vector>

#include "dirent.h"
using namespace std;

vector<string> filefinder(string path, string const& postfix = ".jack");
bool hasEnding(string const& fullString, string const& ending = ".jack");

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Need to input file or directory!";
        return 1;
    }
    vector<string> files;
    if (hasEnding(argv[1])) {
        files.push_back(argv[1]);
    } else {
        files = filefinder(argv[1]);
    }
    cout << "File list: \n";
    for (auto file : files)
        cout << file << endl;

    return 0;
}

// get from
// https://stackoverflow.com/questions/874134/find-out-if-string-ends-with-another-string-in-c
bool hasEnding(string const& fullString, string const& ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(),
                                        ending.length(), ending));
    } else
        return false;
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
