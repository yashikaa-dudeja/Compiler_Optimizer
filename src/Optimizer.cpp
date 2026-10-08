#include "Optimizer.h"
#include "ConstantFolding.h"
#include "ConstantPropagation.h"
#include "DeadCodeElimination.h"
#include <iomanip>
#include <vector>

struct Pass {
    const char* name;
    int (*run)(Program&);
};

Program optimize(const Program& original, std::ostream& out, bool verbose) {
    const std::vector<Pass> passes = {
        {"CONSTANT FOLDING",      constantFolding},
        {"CONSTANT PROPAGATION",  constantPropagation},
        {"DEAD CODE ELIMINATION", deadCodeElimination},
    };

    Program current = original;
    if (verbose) {
        out << "===== ORIGINAL INTERMEDIATE CODE =====\n\n";
        printProgram(original, out);
        out << "\n";
    }

    for (const Pass& p : passes) {
        int changes = p.run(current);
        if (verbose) {
            out << "===== AFTER " << p.name << " =====\n\n";
            printProgram(current, out);
            out << "(" << changes << " change(s) made by this pass)\n\n";
        }
    }

    if (verbose) {
        out << "===== FINAL OPTIMIZED CODE =====\n\n";
        printProgram(current, out);
        out << "\n";
    }

    int before = static_cast<int>(original.code.size());
    int after = static_cast<int>(current.code.size());
    int removed = before - after;
    double pct = before == 0 ? 0.0 : 100.0 * removed / before;

    if (verbose) {
        out << "===== INSTRUCTION COUNT COMPARISON =====\n";
        out << "Original instructions  : " << before << "\n";
        out << "Optimized instructions : " << after << "\n";
        out << "Instructions removed   : " << removed << "\n";
        out << "Instruction count reduction: " << std::fixed << std::setprecision(1) << pct << "%\n";
        out << "(This measures code size in instructions, not execution time.)\n\n";
    }
    return current;
}
