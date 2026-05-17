#ifndef DA2026_PRJ2_T_11__G_01_PARSER_H
#define DA2026_PRJ2_T_11__G_01_PARSER_H

#include "GraphColoring.h"
#include "DataStructures.h"
#include <string>
#include <vector>
#include <unordered_set>

struct Config {

    int numRegisters = 0;
    std::string algorithm = "basic";
    int algorithmParam = 0;

};

class Parser {

public:

    static bool parseRegisters(const std::string& filename, Config& config);
    static bool parseRanges(const std::string& filename, std::vector<Web>& webs);

private:

    static std::string trim(const std::string& s);
    static std::vector<Interval> parsePoints(const std::string& pointsStr);
    static std::vector<ProgramPoint> parseProgramPoints(const std::string& pointsStr);
    static std::unordered_set<int> pointSet(const std::vector<Interval>& intervals);

};

#endif //DA2026_PRJ2_T_11__G_01_PARSER_H