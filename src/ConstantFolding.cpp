#include "ConstantFolding.h"
#include <stdexcept>

bool foldInstruction(Instruction& ins) {
    if (ins.type != InstrType::BINARY) return false;
    if (!isNumber(ins.arg1) || !isNumber(ins.arg2)) return false;

    long long x, y, r;
    try {
        x = std::stoll(ins.arg1);
        y = std::stoll(ins.arg2);
    } catch (const std::out_of_range&) {
        return false;
    }

    if (ins.op == "+") r = x + y;
    else if (ins.op == "-") r = x - y;
    else if (ins.op == "*") r = x * y;
    else if (ins.op == "/") {
        if (y == 0) return false;
        r = x / y;
    } else return false;

    ins.type = InstrType::ASSIGN;
    ins.arg1 = std::to_string(r);
    ins.op.clear();
    ins.arg2.clear();
    return true;
}

int constantFolding(Program& prog) {
    int changes = 0;
    for (Instruction& ins : prog.code)
        if (foldInstruction(ins)) changes++;
    return changes;
}
