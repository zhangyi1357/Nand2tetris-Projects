#include "JackTokenizer.h"

JackTokenizer::JackTokenizer(const string& filename) : fin(filename) {
    if (!fin.is_open())
        cerr << "Open file \"" << filename << "\" failed.";
    fin.clear();
}

