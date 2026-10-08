#include "IR.h"
#include <cctype>
#include <sstream>

std::string Instruction::toString() const {
    switch (type) {
        case InstrType::ASSIGN: return result + " = " + arg1;
        case InstrType::BINARY: return result + " = " + arg1 + " " + op + " " + arg2;
        case InstrType::PRINT:  return "print(" + arg1 + ")";
    }
    return "";
}

bool isNumber(const std::string& s) {
    if (s.empty()) return false;
    size_t i = (s[0] == '-') ? 1 : 0;
    if (i == s.size()) return false;
    for (; i < s.size(); i++)
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
    return true;
}

bool isVariable(const std::string& s) {
    if (s.empty()) return false;
    if (!(std::isalpha(static_cast<unsigned char>(s[0])) || s[0] == '_')) return false;
    for (char c : s)
        if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_')) return false;
    return true;
}

static std::string trim(const std::string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

static bool isOperand(const std::string& s) { return isNumber(s) || isVariable(s); }

bool parseProgram(const std::vector<std::string>& lines, Program& prog, std::string& error) {
    prog = Program();
    int lineNo = 0;
    for (const std::string& raw : lines) {
        lineNo++;
        std::string line = raw;
        size_t hash = line.find('#');
        if (hash != std::string::npos) line = line.substr(0, hash);
        line = trim(line);
        if (line.empty()) continue;

        std::string where = "line " + std::to_string(lineNo) + ": ";
        Instruction ins;

        if (line.compare(0, 7, "output ") == 0) {
            std::istringstream names(line.substr(7));
            std::string name;
            while (names >> name) {
                if (!name.empty() && name.back() == ',') name.pop_back();
                if (!isVariable(name)) { error = where + "bad output name '" + name + "'"; return false; }
                prog.outputs.insert(name);
            }
            continue;
        }

        if (line.compare(0, 6, "print(") == 0 && line.back() == ')') {
            std::string arg = trim(line.substr(6, line.size() - 7));
            if (!isOperand(arg)) { error = where + "bad print argument '" + arg + "'"; return false; }
            ins.type = InstrType::PRINT;
            ins.arg1 = arg;
            prog.code.push_back(ins);
            continue;
        }

        size_t eq = line.find('=');
        if (eq == std::string::npos) { error = where + "expected '=' in '" + line + "'"; return false; }
        std::string lhs = trim(line.substr(0, eq));
        if (!isVariable(lhs)) { error = where + "bad left-hand side '" + lhs + "'"; return false; }

        std::istringstream rhs(line.substr(eq + 1));
        std::vector<std::string> tok;
        std::string t;
        while (rhs >> t) tok.push_back(t);

        ins.result = lhs;
        if (tok.size() == 1 && isOperand(tok[0])) {
            ins.type = InstrType::ASSIGN;
            ins.arg1 = tok[0];
        } else if (tok.size() == 3 && isOperand(tok[0]) && isOperand(tok[2]) &&
                   (tok[1] == "+" || tok[1] == "-" || tok[1] == "*" || tok[1] == "/")) {
            ins.type = InstrType::BINARY;
            ins.arg1 = tok[0];
            ins.op = tok[1];
            ins.arg2 = tok[2];
        } else {
            error = where + "cannot parse '" + line + "' (write e.g.  t1 = a + b  with spaces)";
            return false;
        }
        prog.code.push_back(ins);
    }
    return true;
}

void printProgram(const Program& prog, std::ostream& out) {
    if (prog.code.empty()) out << "(no instructions)\n";
    for (const Instruction& ins : prog.code) out << ins.toString() << "\n";
    if (!prog.outputs.empty()) {
        out << "[outputs:";
        for (const std::string& o : prog.outputs) out << " " << o;
        out << "]\n";
    }
}

std::vector<std::string> programToLines(const Program& prog) {
    std::vector<std::string> v;
    for (const Instruction& ins : prog.code) v.push_back(ins.toString());
    return v;
}
