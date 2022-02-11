// Hack Compiler without symbols

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
using namespace std;

enum struct CommandType { A_COMMAND,
                          C_COMMAND,
                          L_COMMAND };

class Parser {
   private:
    ifstream fin;
    string line;

   public:
    Parser(string fileName);
    ~Parser();
    bool hasMoreCommands();
    void advance();
    CommandType commandType();
    string symbol();
    string dest();
    string comp();
    string jump();
};

Parser::Parser(string fileName) {
    fin.open(fileName);
}

Parser::~Parser() {
    fin.close();
}

bool Parser::hasMoreCommands() {
    while (getline(fin, line)) {
        line.erase(remove(line.begin(), line.end(), ' '), line.end());
        if (line.empty() || line[0] == '/')
            continue;
        else
            return true;
    }
    return false;
}

void Parser::advance() {
    // Remove comments
    size_t found = line.find_first_of("/");
    if (found != string::npos)
        line.erase(found);
    // cout << line << endl;
}

CommandType Parser::commandType() {
    if (line[0] == '@')
        return CommandType::A_COMMAND;
    else if (line[0] == '(')
        return CommandType::L_COMMAND;
    else
        return CommandType::C_COMMAND;
}

string Parser::symbol() {
    return line.substr(1);
}

string Parser::dest() {
    string dest;
    size_t found = line.find_first_of("=");
    if (found != string::npos)
        dest = line.substr(0, found);
    return dest;
}

string Parser::comp() {
    string comp;
    size_t eq = line.find_first_of("=");
    size_t sc = line.find_first_of(";");
    size_t start = (eq == string::npos) ? 0 : eq + 1;
    size_t len = (sc == string::npos) ? string::npos : sc - start;
    return line.substr(start, len);
}

string Parser::jump() {
    string jump;
    size_t found = line.find_first_of(";");
    if (found != string::npos)
        jump = line.substr(found + 1);
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
    for (int i = 0; i < inputFileName.length(); i++)
        if (inputFileName[i] != '.')
            outputFileName += inputFileName[i];
        else
            break;
    outputFileName += ".hack";
    ofstream fout(outputFileName);
    Parser parser(inputFileName);
    Code code;
    code.init();
    while (parser.hasMoreCommands()) {
        parser.advance();
        CommandType cmd = parser.commandType();
        if (cmd == CommandType::A_COMMAND || cmd == CommandType::L_COMMAND) {
            string symbol = parser.symbol();
            if (cmd == CommandType::A_COMMAND) {
                int num = stoi(symbol);
                fout << '0';
                for (int i = 14; i >= 0; i--)
                    fout << ((num >> i) & 1);
                fout << '\n';
            } else {
                fout << "not finished yet.\n";
            }
        } else {  // (cmd == CommandType::C_COMMAND)
            string dest = code.dest(parser.dest());
            string comp = code.comp(parser.comp());
            string jump = code.jump(parser.jump());
            fout << "111" << comp << dest << jump << '\n';
        }
    }
    return 0;
}