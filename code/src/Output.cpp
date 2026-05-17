#include "Output.h"
#include <iostream>
#include <fstream>
#include <algorithm>

/**
 * @brief Implementation of OutputManager::writeOutputFile.
 * @see Output.h for full documentation.
 */
void OutputManager::writeOutputFile(const std::string& filename,
                                    const std::vector<Web>& webs,
                                    int numRegisters,
                                    bool allocationPossible) {

    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not create output file: " << filename << std::endl;
        return;
    }

    if (!allocationPossible) {
        std::cout << "Warning: the assignment to the provided number of registers was not possible." << std::endl;
        numRegisters = 0;
    }

    outFile << "webs: " << webs.size() << std::endl;

    for (const auto& web : webs) {
        outFile << "web" << web.id << ": ";
        std::vector<ProgramPoint> sortedPoints = web.points;
        std::sort(sortedPoints.begin(), sortedPoints.end());
        bool first = true;
        for (const auto& p : sortedPoints) {
            if (!first) outFile << ",";
            outFile << p.line;
            if (p.symbol != ' ') outFile << p.symbol;
            first = false;
        }
        outFile << std::endl;
    }

    outFile << "registers: " << numRegisters << std::endl;

    for (const auto& web : webs) {
        std::string resource = allocationPossible ? web.assignedResource : "M";
        outFile << resource << ": web" << web.id << std::endl;
    }

    outFile.close();
}
