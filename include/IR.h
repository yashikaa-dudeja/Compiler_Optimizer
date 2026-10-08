#pragma once
#include <ostream>
#include <set>
#include <string>
#include <vector>

enum class InstrType {
    ASSIGN,
    BINARY,
    PRINT
};

struct Instruction {
    InstrType type = InstrType::ASSIGN;
    std::string result;
    std::string arg1;
    std::string op;
    std::string arg2;

    std::string toString() const;
};

struct Program {
    std::vector<Instruction> code;
    std::set<std::string> outputs;
};

bool isNumber(const std::string& s);
bool isVariable(const std::string& s);

bool parseProgram(const std::vector<std::string>& lines, Program& prog, std::string& error);

void printProgram(const Program& prog, std::ostream& out);

std::vector<std::string> programToLines(const Program& prog);
