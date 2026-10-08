#include "DeadCodeElimination.h"
#include <algorithm>
#include <set>

int deadCodeElimination(Program& prog) {
    std::set<std::string> live = prog.outputs;
    std::vector<Instruction> kept;
    int removed = 0;

    auto markUsed = [&](const std::string& operand) {
        if (isVariable(operand)) live.insert(operand);
    };

    for (int i = static_cast<int>(prog.code.size()) - 1; i >= 0; i--) {
        const Instruction& ins = prog.code[i];

        if (ins.type == InstrType::PRINT) {
            markUsed(ins.arg1);
            kept.push_back(ins);
            continue;
        }

        if (live.count(ins.result)) {
            live.erase(ins.result);
            markUsed(ins.arg1);
            if (ins.type == InstrType::BINARY) markUsed(ins.arg2);
            kept.push_back(ins);
        } else {
            removed++;
        }
    }

    std::reverse(kept.begin(), kept.end());
    prog.code = kept;
    return removed;
}
