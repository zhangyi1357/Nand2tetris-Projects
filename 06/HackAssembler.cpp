// Hack Compiler with symbols
#include <string>
#include <fstream>
#include <map>
#include <algorithm>
#include <iostream>
#include <queue>
using namespace std;

enum struct CommandType { A_COMMAND, C_COMMAND, L_COMMAND };

class SymbolTable
{
private:
    map<string, int> table;
public:
    void init() {
        addEntry("SP", 0);
        addEntry("LCL", 1);
        addEntry("ARG", 2);
        addEntry("THIS", 3);
        addEntry("THAT", 4);
        addEntry("R0", 0);
        addEntry("R1", 1);
        addEntry("R2", 2);
        addEntry("R3", 3);
        addEntry("R4", 4);
        addEntry("R5", 5);
        addEntry("R6", 6);
        addEntry("R7", 7);
        addEntry("R8", 8);
        addEntry("R9", 9);
        addEntry("R10", 10);
        addEntry("R11", 11);
        addEntry("R12", 12);
        addEntry("R13", 13);
        addEntry("R14", 14);
        addEntry("R15", 15);
        addEntry("SCREEN", 16384);
        addEntry("KBD", 24576);
    }
    void addEntry(string symbol, int address);
    bool contains(string symbol);
    int getAddress(string symbol);
};

void SymbolTable::addEntry(string symbol, int address) {
    table.insert(make_pair(symbol, address));
}

bool SymbolTable::contains(string symbol) {
    return table.count(symbol) > 0;
}

int SymbolTable::getAddress(string symbol) {
    return table[symbol];
}

class Parser
{
private:
    vector<string> lines;
    int lineNum;
public:
    Parser(string fileName);
    bool hasMoreCommands();
    CommandType commandType();
    string symbol();
    string dest();
    string comp();
    string jump();
    void buildPreTable(SymbolTable& table);
};

Parser::Parser(string fileName) :lineNum(-1) {
    fstream fin(fileName);
    string line;
    while (getline(fin, line))
    {
        // Remove all the spaces in this line
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        size_t found = line.find_first_of("/");
        if (found != string::npos)
            line.erase(found);
        if (!line.empty())
            lines.push_back(line);
    }
    fin.close();
}

void Parser::buildPreTable(SymbolTable& table) {
    // (symbol) can't be last line
    for (int i = 0, cnt = 0; i < lines.size() - 1; i++) {
        if (lines[i][0] == '(')
            table.addEntry(lines[i].substr(1, lines[i].size() - 2), cnt);
        else
            cnt++;
    }
}

bool Parser::hasMoreCommands() {
    return (++lineNum < lines.size()) ? true : false;
}

CommandType Parser::commandType() {
    if (lines[lineNum][0] == '@')
        return CommandType::A_COMMAND;
    else if (lines[lineNum][0] == '(')
        return CommandType::L_COMMAND;
    else
        return CommandType::C_COMMAND;
}

string Parser::symbol() {
    return lines[lineNum].substr(1);
}

string Parser::dest() {
    string dest;
    size_t found = lines[lineNum].find_first_of("=");
    if (found != string::npos)
        dest = lines[lineNum].substr(0, found);
    return dest;
}

string Parser::comp() {
    string comp;
    size_t eq = lines[lineNum].find_first_of("=");
    size_t sc = lines[lineNum].find_first_of(";");
    size_t start = (eq == string::npos) ? 0 : eq + 1;
    size_t len = (sc == string::npos) ? string::npos : sc - start;
    return lines[lineNum].substr(start, len);
}

string Parser::jump() {
    string jump;
    size_t found = lines[lineNum].find_first_of(";");
    if (found != string::npos)
        jump = lines[lineNum].substr(found + 1);
    return jump;
}

class Code {
private:
    map<string, string> compRes, jumpRes, destRes;
public:
    void init() {
        destRes[""] = "000";
        destRes["M"] = "001";
        destRes["D"] = "010";
        destRes["MD"] = destRes["DM"] = "011";
        destRes["A"] = "100";
        destRes["AM"] = destRes["MA"] = "101";
        destRes["AD"] = destRes["DA"] = "110";
        destRes["AMD"] = destRes["ADM"] = destRes["MAD"] = destRes["MDA"] = destRes["DAM"] = destRes["DMA"] = "111";
        compRes["0"] = "0101010";
        compRes["1"] = "0111111";
        compRes["-1"] = "0111010";
        compRes["D"] = "0001100";
        compRes["A"] = "0110000";
        compRes["!D"] = "0001101";
        compRes["!A"] = "0110001";
        compRes["-D"] = "0001111";
        compRes["-A"] = "0110011";
        compRes["D+1"] = compRes["1+D"] = "0011111";
        compRes["A+1"] = compRes["1+A"] = "0110111";
        compRes["D-1"] = "0001110";
        compRes["A-1"] = "0110010";
        compRes["D+A"] = compRes["A+D"] = "0000010";
        compRes["D-A"] = "0010011";
        compRes["A-D"] = "0000111";
        compRes["D&A"] = compRes["A&D"] = "0000000";
        compRes["D|A"] = compRes["A|D"] = "0010101";
        compRes["M"] = "1110000";
        compRes["!M"] = "1110001";
        compRes["-M"] = "1110011";
        compRes["M+1"] = compRes["1+M"] = "1110111";
        compRes["M-1"] = "1110010";
        compRes["D+M"] = compRes["M+D"] = "1000010";
        compRes["D-M"] = "1010011";
        compRes["M-D"] = "1000111";
        compRes["D&M"] = compRes["M&D"] = "1000000";
        compRes["D|M"] = compRes["M|D"] = "1010101";
        jumpRes[""] = "000";
        jumpRes["JGT"] = "001";
        jumpRes["JEQ"] = "010";
        jumpRes["JGE"] = "011";
        jumpRes["JLT"] = "100";
        jumpRes["JNE"] = "101";
        jumpRes["JLE"] = "110";
        jumpRes["JMP"] = "111";
    }
    string dest(string str);
    string comp(string str);
    string jump(string str);
};

string Code::dest(string str) {
    return destRes[str];
}

string Code::comp(string str) {
    return compRes[str];
}

string Code::jump(string str) {
    return jumpRes[str];
}

int main(int argc, char** argv) {
    // cout << argv[1] << endl;
    string inputFileName(argv[1]);
    string outputFileName;
    for (int i = 0; i < inputFileName.length();i++)
        if (inputFileName[i] != '.')
            outputFileName += inputFileName[i];
        else
            break;
    outputFileName += ".hack";
    ofstream fout(outputFileName);
    Parser parser(inputFileName);
    SymbolTable table;
    table.init();
    parser.buildPreTable(table);
    int varBase = 16;
    Code code;
    code.init();
    while (parser.hasMoreCommands()) {
        CommandType cmd = parser.commandType();
        if (cmd == CommandType::A_COMMAND) {
            string symbol = parser.symbol();

            int num;
            if (isdigit(symbol[0]))
                num = stoi(symbol);
            else {
                if (table.contains(symbol))
                    num = table.getAddress(symbol);
                else {
                    table.addEntry(symbol, varBase);
                    num = varBase++;
                }
            }
            // cout << symbol << num << endl;
            fout << '0';
            for (int i = 14; i >= 0; i--)
                fout << ((num >> i) & 1);
            fout << '\n';
        }
        else if (cmd == CommandType::C_COMMAND) {
            string dest = code.dest(parser.dest());
            string comp = code.comp(parser.comp());
            string jump = code.jump(parser.jump());
            fout << "111" << comp << dest << jump << '\n';
        }
        else // L_COMMAND
            continue;
    }
    return 0;
}