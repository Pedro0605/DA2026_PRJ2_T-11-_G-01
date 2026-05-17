#include "Menu.h"
#include "Output.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <set>
#include <unistd.h>

/**
 * @brief Implementation of Menu::run.
 * @see Menu.h for full documentation.
 */
void Menu::run() {
    int option = -1;
    while (option != 9) {
        displayMenu();
        std::string input;
        std::getline(std::cin, input);
        if (input.empty() && rangesLoaded && registersLoaded) {
            runConfiguredAlgorithm();
            continue;
        }
        std::stringstream ss(input);
        ss >> option;
        if (ss.fail()) {
            option = -1;
        }
        switch (option) {
            case 1: loadRanges(); break;
            case 2: loadRegisters(); break;
            case 3: runBasic(); break;
            case 4: runSpilling(); break;
            case 5: runSplitting(); break;
            case 6: runCustom(); break;
            case 7: writeOutput(); break;
            case 8: displayResults(); break;
            case 9: std::cout << "Exiting...\n"; break;
            default: std::cout << "Invalid option. Please try again.\n"; break;
        }
        if (option != 9 && isatty(STDIN_FILENO)) {
            std::cout << "\nPress Enter to continue...";
            std::cin.get();
        }
    }
}

/**
 * @brief Implementation of Menu::displayMenu.
 * @see Menu.h for full documentation.
 */
void Menu::displayMenu() {
    std::cout << "\n===== Global Register Allocator =====\n";
    std::cout << " 1. Load ranges file\n";
    std::cout << " 2. Load registers file\n";
    std::cout << " 3. Run basic allocation\n";
    std::cout << " 4. Run allocation with spilling\n";
    std::cout << " 5. Run allocation with splitting\n";
    std::cout << " 6. Run custom algorithm\n";
    std::cout << " 7. Write output to file\n";
    std::cout << " 8. Display current results\n";
    std::cout << " 9. Exit\n";
    std::cout << "=====================================\n";
    std::cout << "Ranges: " << (rangesLoaded ? rangesFilename : "(not loaded)") << "\n";
    std::cout << "Registers: " << (registersLoaded ? registersFilename : "(not loaded)")
              << " (K=" << config.numRegisters << ")\n";
    std::cout << "Algorithm: " << config.algorithm
              << (config.algorithmParam > 0 ? " (param=" + std::to_string(config.algorithmParam) + ")" : "")
              << "\n";
    std::cout << "Status: "
              << (!rangesLoaded || !registersLoaded ? "load files first"
                  : allocationDone ? (allocationSuccess ? "allocation succeeded" : "allocation failed")
                  : "ready to run")
              << "\n";
    std::cout << "Choose an option: ";
}

/**
 * @brief Implementation of Menu::loadRanges.
 * @see Menu.h for full documentation.
 */
void Menu::loadRanges() {
    std::cout << "Enter ranges file path: ";
    std::string path;
    std::getline(std::cin, path);
    if (path.empty()) return;

    std::vector<Web> newWebs;
    if (!Parser::parseRanges(path, newWebs)) {
        std::cout << "Failed to load ranges file.\n";
        return;
    }

    webs = std::move(newWebs);
    rangesFilename = path;
    rangesLoaded = true;
    clearState();
    std::cout << "Loaded " << webs.size() << " web(s) from " << path << "\n";
}

/**
 * @brief Implementation of Menu::loadRegisters.
 * @see Menu.h for full documentation.
 */
void Menu::loadRegisters() {
    std::cout << "Enter registers file path: ";
    std::string path;
    std::getline(std::cin, path);
    if (path.empty()) return;

    Config newConfig;
    if (!Parser::parseRegisters(path, newConfig)) {
        std::cout << "Failed to load registers file.\n";
        return;
    }

    config = newConfig;
    registersFilename = path;
    registersLoaded = true;
    clearState();
    std::cout << "Loaded configuration: K=" << config.numRegisters
              << ", algorithm=" << config.algorithm;
    if (config.algorithmParam > 0) {
        std::cout << ", param=" << config.algorithmParam;
    }
    std::cout << "\n";

    if (rangesLoaded) {
        std::cout << "Running " << config.algorithm << " allocation...\n";
        runConfiguredAlgorithm();
    }
}

/**
 * @brief Implementation of Menu::runBasic.
 * @see Menu.h for full documentation.
 */
void Menu::runBasic() {
    if (!rangesLoaded || !registersLoaded) {
        std::cout << "Please load both ranges and registers files first.\n";
        return;
    }

    clearState();

    InterferenceGraph ig(config.numRegisters);
    ig.build(webs);

    allocationSuccess = GraphColoring::basicColoring(ig.getGraph(), config.numRegisters,
                                                     colorAssignment);
    allocationDone = true;

    applyResults();

    if (allocationSuccess) {
        std::cout << "Basic allocation succeeded.\n";
    } else {
        std::cout << "Basic allocation failed: graph not " << config.numRegisters
                  << "-colorable.\n";
    }
}

/**
 * @brief Implementation of Menu::runSpilling.
 * @see Menu.h for full documentation.
 */
void Menu::runSpilling() {
    if (!rangesLoaded || !registersLoaded) {
        std::cout << "Please load both ranges and registers files first.\n";
        return;
    }

    clearState();

    InterferenceGraph ig(config.numRegisters);
    ig.build(webs);

    int maxSpills = config.algorithmParam > 0 ? config.algorithmParam : ig.getGraph().getVertexSet().size();
    allocationSuccess = GraphColoring::coloringWithSpilling(ig.getGraph(),
                                                            config.numRegisters,
                                                            colorAssignment,
                                                            spilledWebs,
                                                            maxSpills);
    allocationDone = true;

    applyResults();

    if (allocationSuccess) {
        std::cout << "Allocation with spilling succeeded.\n";
        if (!spilledWebs.empty()) {
            std::cout << "Spilled " << spilledWebs.size() << " web(s) to memory.\n";
        }
    } else {
        std::cout << "Allocation with spilling failed.\n";
    }
}

/**
 * @brief Implementation of Menu::runSplitting.
 * @see Menu.h for full documentation.
 */
void Menu::runSplitting() {
    if (!rangesLoaded || !registersLoaded) {
        std::cout << "Please load both ranges and registers files first.\n";
        return;
    }

    clearState();

    std::vector<std::tuple<int,int,int>> splitLog;
    allocationSuccess = GraphColoring::coloringWithSplitting(webs,
                                                             config.numRegisters,
                                                             config.algorithmParam > 0 ? config.algorithmParam : 3,
                                                             colorAssignment,
                                                             splitLog);
    allocationDone = true;

    applyResults();

    if (allocationSuccess) {
        std::cout << "Allocation with splitting succeeded.\n";
        if (!splitLog.empty()) {
            std::cout << "Performed " << splitLog.size() << " split(s):\n";
            for (const auto& entry : splitLog) {
                std::cout << "  web" << std::get<0>(entry)
                          << " split at point " << std::get<2>(entry)
                          << " -> created web" << std::get<1>(entry) << "\n";
            }
        }
    } else {
        std::cout << "Allocation with splitting failed.\n";
    }
}

/**
 * @brief Implementation of Menu::runCustom.
 * @see Menu.h for full documentation.
 */
void Menu::runCustom() {
    if (!rangesLoaded || !registersLoaded) {
        std::cout << "Please load both ranges and registers files first.\n";
        return;
    }

    clearState();

    InterferenceGraph ig(config.numRegisters);
    ig.build(webs);

    // Call your custom algorithm!
    std::map<int, std::string> result = 
        GraphColoring::allocateRegistersFree(&ig.getGraph(), config.numRegisters);

    allocationSuccess = true; 
    
    // Translate your string results to Menu's internal state
    for (const auto& pair : result) {
        int webId = pair.first;
        std::string assignment = pair.second;
        
        if (assignment == "M") {
            spilledWebs.push_back(webId);
        } else if (assignment.length() > 1 && assignment[0] == 'r') {
            int color = std::stoi(assignment.substr(1));
            colorAssignment[webId] = color;
        }
    }
    
    allocationDone = true;
    applyResults();
    
    std::cout << "Custom allocation (Welsh-Powell) completed.\n";
    if (!spilledWebs.empty()) {
        std::cout << "Spilled " << spilledWebs.size() << " web(s) to memory.\n";
    }
}

/**
 * @brief Implementation of Menu::writeOutput.
 * @see Menu.h for full documentation.
 */
void Menu::writeOutput() {
    if (!allocationDone) {
        std::cout << "No allocation results to write. Run an algorithm first.\n";
        return;
    }

    std::cout << "Enter output file path: ";
    std::string path;
    std::getline(std::cin, path);
    if (path.empty()) return;

    outputFilename = path;
    OutputManager::writeOutputFile(path, webs, config.numRegisters, allocationSuccess);
    std::cout << "Output written to " << path << "\n";
}

/**
 * @brief Implementation of Menu::displayResults.
 * @see Menu.h for full documentation.
 */
void Menu::displayResults() {
    if (!allocationDone) {
        std::cout << "No allocation results to display. Run an algorithm first.\n";
        return;
    }

    std::cout << "\n===== Allocation Results =====\n";
    std::cout << "Webs: " << webs.size() << "\n";
    std::cout << "Registers available: " << config.numRegisters << "\n";
    std::cout << "Allocation: " << (allocationSuccess ? "SUCCESS" : "FAILED") << "\n";

    if (!spilledWebs.empty()) {
        std::cout << "Spilled webs: ";
        for (size_t i = 0; i < spilledWebs.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << "web" << spilledWebs[i];
        }
        std::cout << "\n";
    }

    std::cout << "\nRegister assignments:\n";
    for (const auto& web : webs) {
        std::string res = resourceForWeb(web.id);
        std::cout << "  web" << web.id << " (" << web.label << "): " << res << "\n";
    }
}

/**
 * @brief Implementation of Menu::runConfiguredAlgorithm.
 * @see Menu.h for full documentation.
 */
void Menu::runConfiguredAlgorithm() {
    if (config.algorithm == "spilling") {
        runSpilling();
    } else if (config.algorithm == "splitting") {
        runSplitting();
    } else if (config.algorithm == "free") {
        runCustom();
    } else {
        runBasic();
    }
}

/**
 * @brief Implementation of Menu::applyResults.
 * @see Menu.h for full documentation.
 */
void Menu::applyResults() {
    for (auto& web : webs) {
        web.assignedResource = resourceForWeb(web.id);
    }
}

/**
 * @brief Implementation of Menu::clearState.
 * @see Menu.h for full documentation.
 */
void Menu::clearState() {
    colorAssignment.clear();
    spilledWebs.clear();
    allocationDone = false;
    allocationSuccess = false;
}

/**
 * @brief Implementation of Menu::resourceForWeb.
 * @see Menu.h for full documentation.
 */
std::string Menu::resourceForWeb(int id) const {
    if (!allocationSuccess) {
        return "M";
    }
    if (std::find(spilledWebs.begin(), spilledWebs.end(), id) != spilledWebs.end()) {
        return "M";
    }
    auto it = colorAssignment.find(id);
    if (it != colorAssignment.end()) {
        return "r" + std::to_string(it->second);
    }
    return "M";
}
