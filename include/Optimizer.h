#pragma once
#include "IR.h"
#include <ostream>

Program optimize(const Program& original, std::ostream& out, bool verbose = true);
