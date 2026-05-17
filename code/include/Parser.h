#ifndef DA2026_PRJ2_T_11__G_01_PARSER_H
#define DA2026_PRJ2_T_11__G_01_PARSER_H

#include "GraphColoring.h"
#include "DataStructures.h"
#include <string>
#include <vector>


/**
 * @brief Stores the parsed configuration from the registers file.
 */
struct Config {
    int numRegisters = 0;       /**< Number of available registers (K). */
    std::string algorithm = "basic"; /**< Algorithm name: basic, spilling, splitting, free. */
    int algorithmParam = 0;     /**< Optional algorithm parameter. */
};

/**
 * @brief Parses input files for the register allocator.
 *
 * Provides static methods to parse the registers configuration file and
 * the ranges file containing web live ranges.
 */
class Parser {

public:
    /**
     * @brief Parses a registers file and populates a Config struct.
     * @details The file supports "registers: <N>" and "algorithm: <name>"
     * directives, with optional comma-separated parameters.
     * @param filename Path to the registers file.
     * @param config Output config struct to populate.
     * @return True if the file was successfully parsed, false otherwise.
     */
    static bool parseRegisters(const std::string& filename, Config& config);

    /**
     * @brief Parses a ranges file and builds a list of Web objects.
     * @details The file format is "label: point1,point2,...". Points support
     * '+' and '-' suffixes for interval notation. Fragments with overlapping
     * live ranges under the same label are merged into a single web.
     *
     * @par Time complexity
     * O(L * F^3 * I) where L is the number of unique labels, F is the number
     * of fragments per label, and I is the average number of intervals per fragment.
     *
     * @param filename Path to the ranges file.
     * @param webs Output vector of parsed Web objects.
     * @return True if the file was successfully parsed, false otherwise.
     */
    static bool parseRanges(const std::string& filename, std::vector<Web>& webs);

private:
    /** @brief Strips leading and trailing whitespace from a string. */
    static std::string trim(const std::string& s);
    /** @brief Parses a comma-separated list of points into live-range Intervals. */
    static std::vector<Interval> parsePoints(const std::string& pointsStr);
    /** @brief Parses a comma-separated list of points into ProgramPoint objects. */
    static std::vector<ProgramPoint> parseProgramPoints(const std::string& pointsStr);
    /** @brief Checks if any interval in set A overlaps with any interval in set B. */
    static bool intervalsOverlap(const std::vector<Interval>& a, const std::vector<Interval>& b);
};

#endif //DA2026_PRJ2_T_11__G_01_PARSER_H
