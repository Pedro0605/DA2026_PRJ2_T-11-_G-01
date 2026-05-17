#ifndef DA2026_PRJ2_T_11__G_01_OUTPUT_H
#define DA2026_PRJ2_T_11__G_01_OUTPUT_H

#include "DataStructures.h"
#include <string>
#include <vector>

/**
 * @brief Handles writing allocation results to an output file.
 */
class OutputManager {
public:
    /**
     * @brief Writes the allocation results to a formatted output file.
     * @details The output contains the number of webs, sorted program points
     * for each web, the number of registers used, and the register/memory
     * assignment for each web.
     * @param filename Path to the output file.
     * @param webs The list of webs with assigned resources.
     * @param numRegisters Number of registers used.
     * @param allocationPossible Whether a valid allocation was found.
     */
    static void writeOutputFile(const std::string& filename,
                                const std::vector<Web>& webs,
                                int numRegisters,
                                bool allocationPossible);
};

#endif
