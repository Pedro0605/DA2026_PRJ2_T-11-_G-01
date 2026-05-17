#include "Menu.h"
#include "Parser.h"
#include "Output.h"
#include "GraphColoring.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <tuple>

/**
 * @brief Runs the allocator in batch mode.
 * @details Parses input files, builds the interference graph, runs the algorithm
 * specified in the registers config, and writes results to the output file.
 * @param rangesPath Path to the ranges file.
 * @param registersPath Path to the registers file.
 * @param outputPath Path for the output file.
 * @return 0 on success, 1 on failure.
 */
static int runBatch(const std::string& rangesPath, const std::string& registersPath,
                    const std::string& outputPath) {
    Config config;
    if (!Parser::parseRegisters(registersPath, config)) {
        std::cerr << "Failed to parse registers file: " << registersPath << "\n";
        return 1;
    }

    std::vector<Web> webs;
    if (!Parser::parseRanges(rangesPath, webs)) {
        std::cerr << "Failed to parse ranges file: " << rangesPath << "\n";
        return 1;
    }

    InterferenceGraph ig(config.numRegisters);
    ig.build(webs);

    bool success = false;
    std::unordered_map<int, int> colorAssignment;
    std::vector<int> spilledWebs;

    if (config.algorithm == "spilling") {
        int maxSpills = config.algorithmParam > 0 ? config.algorithmParam : ig.getGraph().getVertexSet().size();
        success = GraphColoring::coloringWithSpilling(ig.getGraph(), config.numRegisters,
                                                       colorAssignment, spilledWebs, maxSpills);
        if (!spilledWebs.empty()) {
            std::cout << "Spilled " << spilledWebs.size() << " web(s) to memory.\n";
        }
    } else if (config.algorithm == "splitting") {
        std::vector<std::tuple<int,int,int>> splitLog;
        int maxSplits = config.algorithmParam > 0 ? config.algorithmParam : 3;
        success = GraphColoring::coloringWithSplitting(webs, config.numRegisters,
                                                        maxSplits, colorAssignment, splitLog);
        if (!splitLog.empty()) {
            std::cout << "Performed " << splitLog.size() << " split(s).\n";
        }
    } else if (config.algorithm == "free") {
        auto result = GraphColoring::allocateRegistersFree(&ig.getGraph(), config.numRegisters);
        int spilled = 0;
        for (const auto& pair : result) {
            if (pair.second == "M") {
                spilledWebs.push_back(pair.first);
                spilled++;
            } else if (pair.second.length() > 1 && pair.second[0] == 'r') {
                int color = std::stoi(pair.second.substr(1));
                colorAssignment[pair.first] = color;
            }
        }
        success = true;
        if (!spilledWebs.empty()) {
            std::cout << "Spilled " << spilled << " web(s) to memory.\n";
        }
    } else {
        success = GraphColoring::basicColoring(ig.getGraph(), config.numRegisters,
                                                colorAssignment);
    }

    for (auto& web : webs) {
        auto it = colorAssignment.find(web.id);
        if (it != colorAssignment.end()) {
            web.assignedResource = "r" + std::to_string(it->second);
        } else if (std::find(spilledWebs.begin(), spilledWebs.end(), web.id) != spilledWebs.end()) {
            web.assignedResource = "M";
        } else {
            web.assignedResource = "M";
        }
    }

    OutputManager::writeOutputFile(outputPath, webs, config.numRegisters, success);

    if (success) {
        std::cout << "Allocation " << (config.algorithm.empty() ? "basic" : config.algorithm)
                  << " succeeded. Output written to " << outputPath << "\n";
    } else {
        std::cout << "Allocation failed: graph not " << config.numRegisters << "-colorable.\n";
        return 1;
    }

    return 0;
}

/**
 * @brief Prints usage information to stdout.
 * @param prog The program name (argv[0]).
 */
static void printUsage(const char* prog) {
    std::cout << "Usage:\n"
              << "  " << prog << "                        Interactive mode\n"
              << "  " << prog << " -b <ranges> <registers> <output>   Batch mode\n";
}

/**
 * @brief Entry point. Launches interactive mode or batch mode.
 * @details
 * - No arguments: starts the interactive menu.
 * - `--help`: prints usage.
 * - `-b <ranges> <registers> <output>`: runs batch mode.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return 0 on success, 1 on error.
 */
int main(int argc, char* argv[]) {
    if (argc == 1) {
        Menu menu;
        menu.run();
        return 0;
    }

    if (argc == 2 && std::string(argv[1]) == "--help") {
        printUsage(argv[0]);
        return 0;
    }

    if (argc == 5 && std::string(argv[1]) == "-b") {
        return runBatch(argv[2], argv[3], argv[4]);
    }

    printUsage(argv[0]);
    return 1;
}
