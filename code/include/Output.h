#ifndef DA2026_PRJ2_T_11__G_01_OUTPUT_H
#define DA2026_PRJ2_T_11__G_01_OUTPUT_H

#include <string>
#include <vector>
#include <map>
#include <set>

// Estrutura para representar um ponto de execução (ex: 7+)
struct ProgramPoint {
    int line;
    char symbol;

    // Para garantir a ordenação ascendente exigida no output [2]
    bool operator<(const ProgramPoint& other) const {
        return line < other.line;
    }
};

// Estrutura para representar uma Web e a sua alocação final
struct Web {
    int id;
    std::set<ProgramPoint> points;
    std::string assignedResource;
class OutputManager {
public:
    static void writeOutputFile(const std::string& filename,
                                const std::vector<Web>& webs,
                                int numRegisters,
                                bool allocationPossible);
};

#endif //DA2026_PRJ2_T_11__G_01_OUTPUT_H
