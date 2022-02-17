#include "JackTokenizer.h"

#include <algorithm>

bool JackTokenizer::setFilename(const string& filename) {
    if (fin.is_open()) fin.close();
    if (fout.is_open()) fout.close();
    k = 0;
    content.clear();
    fin.open(filename);
    if (!fin.is_open()) {
        cerr << "Open file \"" << filename << "\" failed.";
        fin.clear();
        return false;
    }
    // remove .jack add "Tmy.xml"
    fout.open(filename.substr(0, filename.size() - 5) + "Tmy.xml");
    return true;
}

void JackTokenizer::readFile() {
    string line;
    bool isComment = false;
    while (getline(fin, line)) {
        // handle /* comment */ first
        // cout << line << endl;
        if (line.find("/*") != string::npos)
            isComment = true;
        if (line.find("*/") != string::npos) {
            isComment = false;
            continue;
        }
        if (isComment)
            continue;
        // cout << line << endl;
        // remove all spaces in the line
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        // remove // comment in the line
        size_t found = line.find("//");
        if (found != string::npos)
            line.erase(found);
        // if (!line.empty())
        //     cout << line << endl;
        content += line;
    }
    // cout << endl;
    cout << content << endl;
}

bool JackTokenizer::hasMoreTokens() {
    return true;
}

void JackTokenizer::tokenize() {
}