#include "ConstantPropagation.h"
#include "ConstantFolding.h"
#include <unordered_map>

int constantPropagation(Program& prog) {
    std::unordered_map<std::string, std::string> known;
    int changes = 0;

    auto substitute = [&](std::string& operand) {
        if (!isVariable(operand)) return;
        auto it = known.find(operand);
        if (it != known.end()) {
            operand = it->second;
            changes++;
        }
    };

    for (Instruction& ins : prog.code) {
        if (ins.type == InstrType::PRINT) continue;

        substitute(ins.arg1);
        if (ins.type == InstrType::BINARY) substitute(ins.arg2);

        foldInstruction(ins);

        if (ins.type == InstrType::ASSIGN && isNumber(ins.arg1))
            known[ins.result] = ins.arg1;
        else
            known.erase(ins.result);
    }
    return changes;
}
