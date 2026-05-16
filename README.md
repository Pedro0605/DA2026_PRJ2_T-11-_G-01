# Global Register Allocation

Compiler register allocation using graph coloring heuristics. Decides which variables (webs) are stored in CPU registers vs main memory to minimize slow memory accesses.

## The Problem

Given a set of **webs** (variable live ranges), a limited number of **K registers**, and an **interference graph** where edges connect webs whose live ranges overlap, assign each web to a register such that interfering webs never share the same register. When K registers are insufficient, some webs must be **spilled** to memory or **split** into smaller ranges.

## Inputs

- **Live ranges file** (`ranges.txt`): Defines each web and the program points where it is live.
- **Registers file** (`registers.txt`): Specifies the number K of available registers.

## Output (`allocation.txt`)

- List of all webs with their execution points.
- Register assignment (`r0`, `r1`, ...) or memory spill (`M`) for each web.

## Algorithms (Implemented)

| Algorithm | Description |
|-----------|-------------|
| **T2.1 - Basic Allocation** | Greedy Chaitin-style graph coloring using a stack. Iteratively removes nodes with degree < K, then assigns colors in reverse order. |
| **T2.2 - Web Spilling** | When coloring fails, selects high-degree webs to spill to memory, simplifying the graph until K-colorable. |
| **T2.3 - Web Splitting** | Splits webs into smaller live-range fragments to reduce interference graph connectivity. |
| **T2.4 - Custom Algorithm** | Optimized allocation using custom heuristics (planarity metrics, node partitioning, etc.). |

## Build & Run

```bash
# Batch mode
myProg -b ranges.txt registers.txt allocation.txt

# Interactive menu
myProg
```

## Project Structure

```
code/
├── include/
│   ├── Graph.h              # Generic directed graph (provided)
│   ├── GraphColoring.h      # Web, InterferenceGraph, coloring algorithms
│   └── include.h            # Constants
├── src/
│   ├── main.cpp             # Entry point
│   ├── GraphColoring.cpp    # T2.1 & T2.2 implementations
│   ├── WebSplitting.cpp     # T2.3 implementation
│   ├── CustomAlgo.cpp       # T2.4 implementation
│   ├── Parser.cpp           # Input file parsing
│   └── Menu.cpp             # Interactive menu
└── ...
```

## Team

- Pedro - T2.1 (Basic Allocation), T2.2 (Web Spilling)
- Lucas - T1.1/T1.2 (Interface, Parsing, Graph setup), T2.3 (Web Splitting), T3.5 (Output)
- Gustavo - T2.4 (Custom Algorithm)
