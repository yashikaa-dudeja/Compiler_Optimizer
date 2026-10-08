# Compiler Optimization Module using Machine-Independent Optimizations

## Problem Statement
Compilers translate source code into intermediate code. That code often contains work that
can be done earlier (constant arithmetic), values that are known but still looked up
through variables, and computations whose results are never used. Left alone, this makes the
generated code larger and slower than necessary.

## Objective
Develop a compiler optimization module that performs at least three machine-independent
optimizations and compares the intermediate code before and after optimization.

## Intermediate Code / Three-Address Code (TAC)
TAC is a simple intermediate form where every instruction has at most one operator and at
most three names: `result = arg1 op arg2`. Complex expressions are broken into temporaries:

    x = (a + b) * 5     becomes     t1 = a + b
                                    t2 = t1 * 5
                                    x  = t2

Supported in this prototype: `a = 10`, `a = b`, `t = a + b` (operators `+ - * /`),
`print(x)`, and `output a b c` (declares program outputs). Spaces around operators are required.
Variables that are never assigned are treated as program inputs.

## Machine-Independent Optimizations
Optimizations that improve the intermediate code without knowing anything about the target
CPU (registers, instruction set). They work on the IR, so they can be reused for any machine.

## Implemented Optimizations
1. **Constant Folding** - evaluate operations on constants at compile time: `t1 = 5 + 3` -> `t1 = 8`.
   Division by zero is never folded.
2. **Constant Propagation** - replace a variable with its known constant value:
   `a = 10; b = a + 5` -> `b = 15` (propagation folds the new constant expression immediately).
3. **Dead Code Elimination** - backward liveness analysis. A variable is *live* if its value
   is used later, printed, or declared as output. Assignments to non-live variables are removed.
   `print(x)` statements are always kept and make `x` live.

Pass order: Folding -> Propagation -> Dead Code Elimination (propagation creates dead code,
so DCE runs last). `print(x)` operands are deliberately not replaced by constants, so the
computation of a printed value is always preserved.

## Example
Input (built-in demo):

    a = 10
    b = 20
    k = 5 + 3
    t1 = a + b
    t2 = t1 * 5
    c = t2
    m = k * 2
    d = 100
    print(c)
    print(m)

Output (abbreviated):

    ===== AFTER CONSTANT FOLDING =====        k = 8 ...
    ===== AFTER CONSTANT PROPAGATION =====    t1 = 30, t2 = 150, c = 150, m = 16 ...
    ===== AFTER DEAD CODE ELIMINATION =====
    c = 150
    m = 16
    print(c)
    print(m)

    Original instructions  : 10
    Optimized instructions : 4
    Instructions removed   : 6
    Instruction count reduction: 60.0%

The reduction is **instruction count**, not a measurement of execution time.

## How to Compile
Using g++ (Windows / VS Code terminal, Linux, macOS):

    g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/IR.cpp src/ConstantFolding.cpp src/ConstantPropagation.cpp src/DeadCodeElimination.cpp src/Optimizer.cpp -o optimizer

Using CMake:

    cmake -S . -B build
    cmake --build build

## How to Run

    optimizer                              # built-in demo
    optimizer tests/test_cases.txt         # all test cases with every stage printed
    optimizer tests/test_cases.txt --quiet # only PASS/FAIL
    optimizer myprogram.txt                # your own TAC file

On Windows PowerShell use `.\optimizer.exe`. Test file format: `### name`, program lines,
then `--- expect` followed by the expected final code. Lines starting with `#` are comments.

## Project Structure
- `include/IR.h`, `src/IR.cpp` - instruction struct, parser, printer
- `src/ConstantFolding.cpp`, `src/ConstantPropagation.cpp`, `src/DeadCodeElimination.cpp` - one file per pass
- `src/Optimizer.cpp` - runs the passes in order and prints the comparison (adding a pass = one line)
- `src/main.cpp` - command-line driver and test runner
- `tests/test_cases.txt` - 9 test cases with expected results

## Current 25% Implementation
- TAC representation and parser
- Three optimizations as separate modules
- Liveness-based DCE that respects `print` and `output`
- Before/after printing at every stage, instruction-count comparison
- Self-checking test suite (9 cases)

## Future Work
- Algebraic simplification (`x + 0`, `x * 1`, `x * 0`), strength reduction (`x * 2` -> `x + x`)
- Copy propagation and common subexpression elimination
- Peephole optimization
- Run passes repeatedly until nothing changes (fixed point)
- Branches/labels and basic blocks with control-flow-aware data-flow analysis
- A small front end that generates TAC from source expressions
- More operators (`%`, comparisons) and an input/read instruction
