#include "IR.h"
#include "Optimizer.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct TestCase {
    std::string name;
    std::vector<std::string> program;
    std::vector<std::string> expected;
    bool hasExpected = false;
};

static std::string trimEnds(const std::string& s) {
    size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    return s.substr(b, s.find_last_not_of(" \t\r\n") - b + 1);
}

static std::vector<TestCase> loadCases(const std::string& path, bool& ok) {
    std::vector<TestCase> cases;
    std::ifstream f(path);
    ok = f.is_open();
    if (!ok) return cases;

    TestCase cur;
    bool started = false, inExpect = false;
    std::string line;
    while (std::getline(f, line)) {
        std::string t = trimEnds(line);
        if (t.compare(0, 4, "### ") == 0) {
            if (started) cases.push_back(cur);
            cur = TestCase();
            cur.name = t.substr(4);
            started = true;
            inExpect = false;
        } else if (t == "--- expect") {
            cur.hasExpected = true;
            inExpect = true;
        } else {
            if (!started) { cur.name = path; started = true; }
            if (inExpect) { if (!t.empty()) cur.expected.push_back(t); }
            else cur.program.push_back(line);
        }
    }
    if (started) cases.push_back(cur);
    return cases;
}

int main(int argc, char* argv[]) {
    std::vector<TestCase> cases;
    bool quiet = false;

    if (argc >= 2) {
        bool ok;
        cases = loadCases(argv[1], ok);
        if (!ok) { std::cerr << "Cannot open file: " << argv[1] << "\n"; return 2; }
        if (argc >= 3 && std::string(argv[2]) == "--quiet") quiet = true;
    } else {
        TestCase demo;
        demo.name = "Built-in demo";
        demo.program = {"a = 10", "b = 20", "k = 5 + 3", "t1 = a + b", "t2 = t1 * 5",
                        "c = t2", "m = k * 2", "d = 100", "print(c)", "print(m)"};
        cases.push_back(demo);
    }

    int passed = 0, failed = 0, errors = 0;
    for (const TestCase& tc : cases) {
        Program prog;
        std::string err;
        if (!parseProgram(tc.program, prog, err)) {
            std::cout << "[ERROR] " << tc.name << ": " << err << "\n";
            errors++;
            continue;
        }

        if (!quiet) std::cout << "################ " << tc.name << " ################\n\n";
        Program result = optimize(prog, std::cout, !quiet);

        if (tc.hasExpected) {
            bool same = programToLines(result) == tc.expected;
            std::cout << (same ? "[PASS] " : "[FAIL] ") << tc.name << "\n";
            if (!same) {
                std::cout << "  expected:\n";
                for (const auto& l : tc.expected) std::cout << "    " << l << "\n";
                std::cout << "  got:\n";
                for (const auto& l : programToLines(result)) std::cout << "    " << l << "\n";
            }
            same ? passed++ : failed++;
            if (!quiet) std::cout << "\n";
        }
    }

    if (cases.size() > 1 || passed + failed > 0)
        std::cout << "Summary: " << passed << " passed, " << failed << " failed, "
                  << errors << " error(s)\n";
    return (failed > 0 || errors > 0) ? 1 : 0;
}
