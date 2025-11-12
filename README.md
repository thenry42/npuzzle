# N-Puzzle Solver

A high-performance N-Puzzle solver implementing the A* search algorithm with multiple heuristics in C++.

## Overview

This project solves the N-Puzzle problem (sliding tile puzzles) using the A* search algorithm with various heuristics. It supports puzzles of different sizes (3x3, 4x4, 5x5, etc.) and provides heuristic performance comparisons.

## Features

- **Search Algorithm:**
  - A* (A-Star) - Optimal pathfinding with configurable memory limits
  - Greedy Search - Best First Search
  - Weighted A* - Trade optimality for speed with configurable weight parameter
  - Beam Search - Best First Search with a width limit

- **Multiple Heuristics:**
  - Manhattan Distance
  - Hamming Distance (Misplaced Tiles)
  - Linear Conflict
  - Uninformed Search (Dijkstra's/UCS baseline)

## Compilation

```bash
make
```

## Usage

```bash
./npuzzle
```

Follow the interactive prompts to:
1. Choose puzzle size (3, 4, 5, etc.)
2. Select custom puzzle or random generation
3. Select algorithm
4. Select heuristic (if applicable)
5. Select algorithm-specific parameters (if applicable)
6. View solution and performance metrics

## Performance by Puzzle Size

### A* Algorithm Performance Table

| Heuristic | 3×3 | 4×4 | 5×5 | 6×6 | 7×7 |
|-----------|:---:|:---:|:---:|:---:|:---:|
| **Manhattan Distance** | <0.01s ✅ | ✅ | ❌ | ❌ | ❌ |
| **Hamming Distance** | <0.01s ✅ | ❌ | ❌ | ❌ | ❌ |
| **Linear Conflict** | <0.01s ✅ | ✅ | ❌ | ❌ | ❌ |
| **Uninformed Search** | <0.1s ✅ | ❌ | ❌ | ❌ | ❌ |

### Greedy Search Algorithm Performance Table

| Heuristic | 3×3 | 4×4 | 5×5 | 6×6 | 7×7 |
|-----------|:---:|:---:|:---:|:---:|:---:|
| **Manhattan Distance** | <0.01s ✅ | ✅ | ❌ | ❌ | ❌ |
| **Hamming Distance** | <0.01s ✅ | ❌ | ❌ | ❌ | ❌ |
| **Linear Conflict** | <0.01s ✅ | ✅ | ❌ | ❌ | ❌ |

### Weighted A* Algorithm Performance Table

| Heuristic | 3×3 | 4×4 | 5×5 | 6×6 | 7×7 |
|-----------|:---:|:---:|:---:|:---:|:---:|
| **Manhattan Distance** | <0.01s ✅ | ✅ | ❌ | ❌ | ❌ |
| **Hamming Distance** | <0.01s ✅ | ❌ | ❌ | ❌ | ❌ |
| **Linear Conflict** | <0.01s ✅ | ✅ | ❌ | ❌ | ❌ |

### Beam Search Algorithm Performance Table

| Heuristic | 3×3 | 4×4 | 5×5 | 6×6 | 7×7 |
|-----------|:---:|:---:|:---:|:---:|:---:|
| **Manhattan Distance** | <0.01s ✅ | ✅ | ❌ | ❌ | ❌ |
| **Hamming Distance** | <0.01s ✅ | ❌ | ❌ | ❌ | ❌ |
| **Linear Conflict** | <0.01s ✅ | ✅ | ❌ | ❌ | ❌ |

Legend:
- ✅ Works well
- ❌ Fails due to memory limit or timeout (60s, 1 million states or 90 MB)


## License

This project is part of the 42 School curriculum (after Common Core).
