#include "output.h"
#include <iostream>
#include <fstream>
#include <algorithm>

void OutputManager::writeOutputFile(const std::string& filename,
                                    const std::vector<Web>& webs,
                                    int numRegisters,
                                    bool allocationPossible) {

    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Erro: Não foi possível criar o ficheiro de saída: " << filename << std::endl;
        return;
    }

    if (!allocationPossible) {
        std::cout << "Warning: the assignment to the provided number of registers was not possible." << std::endl;
        numRegisters = 0;

    outFile << "# Total number of webs followed by the listing of the program points of each one" << std::endl;
    outFile << "# program points in each web are sorted in ascending order" << std::endl;
    outFile << "webs: " << webs.size() << std::endl;

    for (const auto& web : webs) {
        outFile << "web" << web.id << ": ";
        bool first = true;
        for (const auto& p : web.points) {
            if (!first) outFile << ",";
            outFile << p.line;
            if (p.symbol != ' ') outFile << p.symbol;
            first = false;
        }
        outFile << std::endl;
    }

    outFile << "# Total number of registers used, followed by assignment to webs" << std::endl;
    outFile << "registers: " << numRegisters << std::endl;

    for (const auto& web : webs) {
        std::string resource = allocationPossible ? web.assignedResource : "M";
        outFile << resource << ": web" << web.id << std::endl;
    }

    outFile.close();
}