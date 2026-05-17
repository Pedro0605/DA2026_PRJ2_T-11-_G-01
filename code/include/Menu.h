#ifndef DA2026_PRJ2_T_11__G_01_MENU_H
#define DA2026_PRJ2_T_11__G_01_MENU_H

#include "GraphColoring.h"
#include "Parser.h"
#include <string>
#include <vector>
#include <unordered_map>

class Menu {
public:
    void run();

private:
    void displayMenu();
    void loadRanges();
    void loadRegisters();
    void runBasic();
    void runSpilling();
    void runSplitting();
    void runCustom();
    void writeOutput();
    void displayResults();
    void clearState();
    void applyResults();
    void runConfiguredAlgorithm();
    std::string resourceForWeb(int id) const;

    Config config;
    std::vector<Web> webs;
    std::unordered_map<int, int> colorAssignment;
    std::vector<int> spilledWebs;
    std::string rangesFilename;
    std::string registersFilename;
    std::string outputFilename;
    bool rangesLoaded = false;
    bool registersLoaded = false;
    bool allocationDone = false;
    bool allocationSuccess = false;
};

#endif
