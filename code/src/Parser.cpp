#include "Parser.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

std::string Parser::trim(const std::string& s) {

    size_t start = s.find_first_not_of(" \t\r\n");
    size_t end   = s.find_last_not_of(" \t\r\n");

    if (start == std::string::npos) {
        return "";
    }

    return s.substr(start, end - start + 1);

}

std::unordered_set<int> Parser::pointSet(const std::vector<Interval>& intervals) {

    std::unordered_set<int> pts;

    for (const auto& iv : intervals)
        for (int p = iv.start; p <= iv.end; ++p)
            pts.insert(p);
    return pts;

}

std::vector<Interval> Parser::parsePoints(const std::string& pointsStr) {

    std::vector<Interval> result;
    std::vector<std::string> tokens;
    std::stringstream ss(pointsStr);
    std::string tok;

    while (std::getline(ss, tok, ',')) {
        tok = trim(tok);
        if (!tok.empty()) tokens.push_back(tok);
    }

    if (tokens.empty()) return result;

    int intervalStart = -1;

    for (size_t i = 0; i < tokens.size(); ++i) {
        const std::string& t = tokens[i];
        bool isPlus  = (!t.empty() && t.back() == '+');
        bool isMinus = (!t.empty() && t.back() == '-');

        std::string numStr = (isPlus || isMinus) ? t.substr(0, t.size() - 1) : t;
        int point = std::stoi(numStr);

        if (intervalStart == -1) {
            intervalStart = point;
        }

        if (isMinus) {
            result.push_back({intervalStart, point});
            intervalStart = -1;
        } else if (isPlus && intervalStart != point) {
            if (intervalStart != -1) {
                result.push_back({intervalStart, point - 1});
            }
            intervalStart = point;
        }
    }

    if (intervalStart != -1 && !tokens.empty()) {
        const std::string& last = tokens.back();
        bool lastIsMinus = (!last.empty() && last.back() == '-');
        bool lastIsPlus  = (!last.empty() && last.back() == '+');
        std::string numStr = (lastIsMinus || lastIsPlus)
                             ? last.substr(0, last.size() - 1) : last;
        int lastPoint = std::stoi(numStr);
        result.push_back({intervalStart, lastPoint});
    }

    return result;
}

std::vector<ProgramPoint> Parser::parseProgramPoints(const std::string& pointsStr) {
    std::vector<ProgramPoint> result;
    std::stringstream ss(pointsStr);
    std::string tok;

    while (std::getline(ss, tok, ',')) {
        tok = trim(tok);
        if (tok.empty()) continue;

        bool hasSymbol = (!tok.empty() && (tok.back() == '+' || tok.back() == '-'));
        char symbol = hasSymbol ? tok.back() : ' ';
        std::string numStr = hasSymbol ? tok.substr(0, tok.size() - 1) : tok;
        int line = std::stoi(numStr);

        result.push_back({line, symbol});
    }

    return result;
}

bool Parser::parseRegisters(const std::string& filename, Config& config) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Parser] Cannot open registers file: " << filename << "\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        if (line.rfind("registers:", 0) == 0) {
            std::string val = trim(line.substr(10));
            config.numRegisters = std::stoi(val);

        } else if (line.rfind("algorithm:", 0) == 0) {
            std::string val = trim(line.substr(10));
            size_t commaPos = val.find(',');
            if (commaPos != std::string::npos) {
                config.algorithm     = trim(val.substr(0, commaPos));
                config.algorithmParam = std::stoi(trim(val.substr(commaPos + 1)));
            } else {
                config.algorithm = val;
                config.algorithmParam = 0;
            }
        }
    }
    return true;
}

bool Parser::parseRanges(const std::string& filename, std::vector<Web>& webs) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Parser] Cannot open ranges file: " << filename << "\n";
        return false;
    }

    std::vector<std::string> labelOrder;
    struct Fragment {
        std::vector<Interval> intervals;
        std::vector<ProgramPoint> points;
    };
    std::unordered_map<std::string, std::vector<Fragment>> fragmentsByLabel;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) continue;

        std::string label      = trim(line.substr(0, colonPos));
        std::string pointsStr  = trim(line.substr(colonPos + 1));

        std::vector<Interval> intervals = parsePoints(pointsStr);
        std::vector<ProgramPoint> pts = parseProgramPoints(pointsStr);
        if (intervals.empty()) continue;

        if (fragmentsByLabel.find(label) == fragmentsByLabel.end())
            labelOrder.push_back(label);

        fragmentsByLabel[label].push_back({intervals, pts});
    }

    int nextId = 0;

    for (const auto& label : labelOrder) {

        std::vector<Fragment> groups = fragmentsByLabel[label];

        bool merged = true;
        while (merged) {
            merged = false;
            for (size_t i = 0; i < groups.size() && !merged; ++i) {
                std::unordered_set<int> ptsI = pointSet(groups[i].intervals);
                for (size_t j = i + 1; j < groups.size(); ++j) {
                    std::unordered_set<int> ptsJ = pointSet(groups[j].intervals);
                    bool intersects = false;
                    for (int p : ptsJ) {
                        if (ptsI.count(p)) { intersects = true; break; }
                    }
                    if (intersects) {
                        groups[i].intervals.insert(groups[i].intervals.end(),
                                                   groups[j].intervals.begin(), groups[j].intervals.end());
                        groups[i].points.insert(groups[i].points.end(),
                                                groups[j].points.begin(), groups[j].points.end());
                        groups.erase(groups.begin() + j);
                        merged = true;
                        break;
                    }
                }
            }
        }
        
        for (auto& group : groups) {
            Web w;
            w.id         = nextId++;
            w.label      = label;
            w.liveRanges = std::move(group.intervals);
            w.points     = std::move(group.points);
            webs.push_back(w);
        }
    }

    return true;
}
