#pragma once
#include "IR.h"

bool foldInstruction(Instruction& ins);

int constantFolding(Program& prog);
