#include "JackTokenizer.h"

#include <algorithm>

set<char> JackTokenizer::symbols = {'{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*',
                                    '/', '&', '|', '<', '>', '=', '~'};

set<string> JackTokenizer::keywords = {"class", "constructor", "function", "method", "field",
                                       "static", "var", "int", "char", "boolean", "void", "true",
                                       "false", "null", "this", "let", "do", "if", "else", "while",
                                       "return"};

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
        // line.erase(remove(line.begin(), line.end(), ' '), line.end());
        // remove // comment in the line
        size_t found = line.find("//");
        if (found != string::npos)
            line.erase(found);
        // if (!line.empty())
        //     cout << line << endl;
        content += line;
    }
    // cout << endl;
    // cout << content << endl;
}

bool JackTokenizer::hasMoreTokens() {
    while (k < content.size() && content[k] == ' ') ++k;
    return k < content.size();
}

Token JackTokenizer::tokenType() {
    if (symbols.count(content[k]))
        return Token::SYMBOL;
    else if (content[k] == '"')
        return Token::STRING_CONST;
    else if (isdigit(content[k]))
        return Token::INT_CONST;
    else {
        string token;
        size_t cur = k;
        while (cur < content.size() && isalpha(content[cur])) {
            token += content[cur];
            ++cur;
        }
        if (cur >= content.size() || !ispunct(content[k]))
            if (keywords.count(token))
                return Token::KEYWORD;
        return Token::IDENTIFIER;
    }
}

char JackTokenizer::symbol() {
    return content[k++];
}

string JackTokenizer::keyword() {
    string word;
    while (k < content.size() && isalpha(content[k])) {
        word += content[k];
        ++k;
    }
    return word;
}

int JackTokenizer::intVal() {
    int val = 0;
    while (k < content.size() && isdigit(content[k])) {
        val = val * 10 + content[k] - '0';
        ++k;
    }
    return val;
}

string JackTokenizer::stringVal() {
    string str;
    ++k;  // skip first "
    while (k < content.size() && content[k] != '"') {
        str += content[k];
        ++k;
    }
    ++k;
    return str;
}

string JackTokenizer::identifier() {
    string iden;
    while (k < content.size()) {
        char ch = content[k];
        if (isalnum(ch) || ch == '_') {
            iden += ch;
            ++k;
        } else
            break;
    }
    return iden;
}

void JackTokenizer::tokenize() {
    fout << "<tokens>\n";
    while (hasMoreTokens()) {
        Token type = tokenType();
        switch (type) {
            case Token::KEYWORD: {
                string word = keyword();
                fout << "<keyword> " << word << " </keyword>\n";
                break;
            }
            case Token::SYMBOL: {
                char ch = symbol();
                string out;
                if (ch == '<')
                    out = "&lt;";
                else if (ch == '>')
                    out = "&gt;";
                else if (ch == '&')
                    out = "&amp;";
                else
                    out += ch;
                fout << "<symbol> " << out << " </symbol>\n";
                break;
            }
            case Token::INT_CONST: {
                int val = intVal();
                fout << "<integerConstant> " << val << " </integerConstant>\n";
                break;
            }
            case Token::STRING_CONST: {
                string str = stringVal();
                fout << "<stringConstant> " << str << " </stringConstant>\n";
                break;
            }
            case Token::IDENTIFIER: {
                string iden = identifier();
                fout << "<identifier> " << iden << " </identifier>\n";
                break;
            }
        }
    }
    fout << "</tokens>\n";
}