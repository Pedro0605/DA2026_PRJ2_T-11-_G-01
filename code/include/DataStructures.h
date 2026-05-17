#ifndef DA2026_PRJ2_T_11__G_01_DATASTRUCTURES_H
#define DA2026_PRJ2_T_11__G_01_DATASTRUCTURES_H

#include <vector>
#include <string>

/**
 * @brief Represents a contiguous live range interval [start, end].
 */
struct Interval {
    int start; /**< Start of the interval (inclusive). */
    int end;   /**< End of the interval (inclusive). */

    /**
     * @brief Checks whether this interval overlaps with another.
     * @param other The other interval to compare against.
     * @return True if the intervals overlap, false otherwise.
     */
    bool overlaps(const Interval& other) const {
        return !(end < other.start || start > other.end);
    }
};

/**
 * @brief A program point identified by a line number and a symbol suffix.
 */
struct ProgramPoint {
    int line;   /**< Source line number. */
    char symbol; /**< Symbol suffix ('+', '-', or space). */

    /**
     * @brief Orders program points by line number.
     * @param other The other program point.
     * @return True if this line < other line.
     */
    bool operator<(const ProgramPoint& other) const {
        return line < other.line;
    }
};

/**
 * @brief Represents a web (a live range variable) in the register allocation problem.
 *
 * Each web has a unique identifier, a set of live range intervals, a label,
 * program points, and an assigned resource (register name or "M" for memory).
 */
struct Web {
    int id;                        /**< Unique web identifier. */
    std::vector<Interval> liveRanges; /**< Live range intervals for this web. */
    std::string label;             /**< Original variable label from the input. */
    std::vector<ProgramPoint> points; /**< Program points where this web is live. */
    std::string assignedResource;  /**< Assigned resource ("r0", "r1", ..., or "M"). */

    /**
     * @brief Checks whether this web interferes with another.
     * @details Two webs interfere if any of their live range intervals overlap.
     * @param other The other web to check against.
     * @return True if the webs interfere, false otherwise.
     */
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
