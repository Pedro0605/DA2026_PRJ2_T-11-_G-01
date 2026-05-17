#ifndef DA2026_PRJ2_T_11__G_01_MENU_H
#define DA2026_PRJ2_T_11__G_01_MENU_H

#include "GraphColoring.h"
#include "Parser.h"
#include <string>
#include <vector>
#include <unordered_map>

/**
 * @brief Interactive CLI menu for the Global Register Allocator.
 *
 * Provides a console-based interface for loading input files, running
 * allocation algorithms (basic, spilling, splitting, custom), viewing
 * results, and writing output. Also supports automatic execution based
 * on configuration loaded from the registers file.
 */
class Menu {
public:
    /**
     * @brief Runs the interactive menu loop.
     * @details Displays options, processes user input, and delegates to the
     * appropriate handler until option 9 (Exit) is selected.
     */
    void run();

private:
    /** @brief Prints the menu options and current state. */
    void displayMenu();
    /** @brief Prompts for and loads a ranges file. */
    void loadRanges();
    /** @brief Prompts for and loads a registers file. */
    void loadRegisters();
    /** @brief Runs the basic (Chaitin) allocation algorithm. */
    void runBasic();
    /** @brief Runs allocation with spilling. */
    void runSpilling();
    /** @brief Runs allocation with splitting (T2.3, not yet implemented). */
    void runSplitting();
    /** @brief Runs the custom algorithm (T2.4, not yet implemented). */
    void runCustom();
    /** @brief Prompts for and writes output to a file. */
    void writeOutput();
    /** @brief Displays current allocation results on the console. */
    void displayResults();
    /** @brief Clears previous allocation state. */
    void clearState();
    /** @brief Applies color assignments to webs. */
    void applyResults();
    /** @brief Runs the algorithm specified in the loaded config. */
    void runConfiguredAlgorithm();
    /**
     * @brief Returns the resource string for a given web ID.
     * @param id The web ID.
     * @return "rN" if assigned to register N, "M" if spilled or failed.
     */
    std::string resourceForWeb(int id) const;

    Config config;                                    /**< Parsed configuration. */
    std::vector<Web> webs;                            /**< Loaded webs. */
    std::unordered_map<int, int> colorAssignment;     /**< Web ID -> color mapping. */
    std::vector<int> spilledWebs;                     /**< IDs of spilled webs. */
    std::string rangesFilename;                       /**< Path to loaded ranges file. */
    std::string registersFilename;                    /**< Path to loaded registers file. */
    std::string outputFilename;                       /**< Path for output file. */
    bool rangesLoaded = false;                        /**< Whether ranges have been loaded. */
    bool registersLoaded = false;                     /**< Whether registers config has been loaded. */
    bool allocationDone = false;                      /**< Whether an algorithm has been run. */
    bool allocationSuccess = false;                   /**< Whether the last allocation succeeded. */
};

#endif
