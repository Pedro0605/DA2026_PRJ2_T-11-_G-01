# Global Register Allocation

Compiler register allocation using graph coloring heuristics. Decides which variables (webs) are stored in CPU registers vs main memory to minimize slow memory accesses.

Built for the **Design and Analysis of Algorithms** (DA) course at FEUP.

## The Problem

Given a set of **webs** (variable live ranges), a limited number of **K registers**, and an **interference graph** where edges connect webs whose live ranges overlap, assign each web to a register such that interfering webs never share the same register. When K registers are insufficient, some webs must be **spilled** to memory or **split** into smaller ranges.

## Inputs

- **Live ranges file** (`ranges.txt`): Defines each web and the program points where it is live.
- **Registers file** (`registers.txt`): Specifies the number K of available registers and the algorithm variant.

## Output (`allocation.txt`)

- List of all webs with their execution points.
- Register assignment (`r0`, `r1`, ...) or memory spill (`M`) for each web.

## Algorithms

| Algorithm | Description |
|-----------|-------------|
| **T2.1 - Basic Allocation** | Chaitin-style greedy graph coloring. Iteratively removes nodes with degree < K (simplify), then assigns colors in reverse order (select). |
| **T2.2 - Web Spilling** | When coloring fails, spills the highest-degree webs to memory until the graph becomes K-colorable. |
| **T2.3 - Web Splitting** | Splits webs into smaller live-range fragments at their midpoints to reduce interference graph connectivity. |
| **T2.4 - Custom Algorithm** | Welsh-Powell greedy coloring: sorts vertices by degree descending, assigns colors greedily, spills any remaining uncolored webs. |

## Datasets

Pre-built test cases are available in `code/basic/`:

| Ranges File | Registers File | Ideal Registers |
|-------------|---------------|-----------------|
| `ranges1.txt` | `registers2.txt` | 2 |
| `ranges2.txt` | `registers2.txt` | 2 |
| `ranges3.txt` | `registers2.txt` | 2 |
| `ranges4.txt` | `registers1.txt` | 1 |
| `ranges5.txt` | `registers1.txt` | 1 |
| `ranges6.txt` | `registers3.txt` | 3 |

## Build & Run

```bash
# Build
cmake -S code -B code/build
cmake --build code/build

# Batch mode
code/build/bin/myProg -b code/basic/ranges/ranges1.txt code/basic/registers/registers2.txt allocation.txt

# Interactive menu
code/build/bin/myProg
```

## Documentation

Doxygen-style comments are provided throughout the source code. To generate HTML documentation:

```bash
sudo apt install doxygen     # Ubuntu/Debian
doxygen Doxyfile
open docs/html/index.html
```

Time complexity of key functions:
- **InterferenceGraph::build()** — O(V²)
- **GraphColoring::basicColoring()** — O(V²)
- **GraphColoring::coloringWithSpilling()** — O(S · V²), worst-case O(V³)
- **Parser::parseRanges()** — O(L · F² · P)

## Project Structure

```
code/
├── CMakeLists.txt
├── include/
│   ├── DataStructures.h      # Web, Interval, ProgramPoint structs
│   ├── Graph.h               # Generic directed graph (template)
│   ├── GraphColoring.h       # InterferenceGraph & GraphColoring classes
│   ├── Menu.h                # Interactive menu handler
│   ├── Output.h              # Output file writer
│   └── Parser.h              # Config struct & file parser
├── src/
│   ├── main.cpp              # Entry point
│   ├── GraphColoring.cpp     # T2.1, T2.2, T2.3, T2.4 implementations
│   ├── Menu.cpp              # Interactive CLI menu
│   ├── Output.cpp            # Formatted output writer
│   └── Parser.cpp            # Input file parsing
└── basic/                    # Test datasets
    ├── ranges/               # Live-range input files
    ├── registers/            # Register-count input files
    └── README.md
```

## Team

- **Pedro** — T2.1 (Basic Allocation), T2.2 (Web Spilling)
- **Lucas** — T1.1/T1.2 (Interface, Parsing, Graph setup), T2.3 (Web Splitting), T3.5 (Output)
- **Gustavo** — T2.4 (Custom Algorithm)
