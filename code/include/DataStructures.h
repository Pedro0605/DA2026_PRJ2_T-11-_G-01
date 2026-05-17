#ifndef DA2026_PRJ2_T_11__G_01_DATASTRUCTURES_H
#define DA2026_PRJ2_T_11__G_01_DATASTRUCTURES_H

#include <vector>
#include <string>

struct Interval {
    int start;
    int end;

    bool overlaps(const Interval& other) const {
        return !(end < other.start || start > other.end);
    }
};

struct ProgramPoint {
    int line;
    char symbol;

    bool operator<(const ProgramPoint& other) const {
        return line < other.line;
    }
};

struct Web {
    int id;
    std::vector<Interval> liveRanges;
    std::string label;
    std::vector<ProgramPoint> points;
    std::string assignedResource;

    bool interferesWith(const Web& other) const {
        for (const auto& a : liveRanges) {
            for (const auto& b : other.liveRanges) {
                if (a.overlaps(b)) return true;
            }
        }
        return false;
    }
};

#endif
