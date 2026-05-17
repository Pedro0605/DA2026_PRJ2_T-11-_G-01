#ifndef DA2026_PRJ2_T_11__G_01_OUTPUT_H
#define DA2026_PRJ2_T_11__G_01_OUTPUT_H

#include "GraphColoring.h"
#include <string>
#include <vector>

class OutputManager {
public:
    static void writeOutputFile(const std::string& filename,
                                const std::vector<Web>& webs,
                                int numRegisters,
                                bool allocationPossible);
};

#endif
