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

- **Interactive Interface:**
  - Custom puzzle input
  - Random puzzle generation (solvable/unsolvable)
  - Algorithm and heuristic comparison mode
  - Real-time performance metrics

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
3. Select heuristic function
4. View solution and performance metrics

## Performance by Puzzle Size

### Heuristic Performance Table

#### <u>Mandatory Heuristics</u>

| Heuristic | 3×3 | 4×4 | 5×5 | 6×6 | 7×7 |
|-----------|:---:|:---:|:---:|:---:|:---:|
| **Manhattan Distance** | <0.01s ✅ | ✅ | ❌ | ❌ | ❌ |
| **Hamming Distance** | <0.01s ✅ | ❌ | ❌ | ❌ | ❌ |
| **Linear Conflict** | <0.01s ✅ | ✅ | ❌ | ❌ | ❌ |
| **Uninformed Search** | <0.1s ✅ | ❌ | ❌ | ❌ | ❌ |

#### <u>Bonus Heuristics & Algorithms</u>

| Heuristic & Algorithm | 3×3 | 4×4 | 5×5 | 6×6 | 7×7 |
|-----------------------|:---:|:---:|:---:|:---:|:---:|
| **Greedy Search** | <0.01s ✅ | ✅ | ✅ | ✅ | ❌ |
| **Weighted A\* (w >= 3)** | <0.1s ✅ | ✅ | ✅ | ✅ | ❌ |
| **Beam Search** | <0.1s ✅ | ✅ | ✅ | ✅ | ✅ |

Legend:
- ✅ Works well
- ❌ Fails due to memory limit or timeout (60s, 1 million states or 90 MB)

### Key Takeaways

**3×3 Puzzles:**
- All heuristics work reasonably well with A*
- **Best: A\* with Linear Conflict** - fastest and most efficient
- Uninformed Search struggles even on small puzzles

**4×4 Puzzles:**
- Memory becomes critical with A*
- **Best: A\* with Linear Conflict or Manhattan Distance**
- Hamming Distance shows significant degradation
- Strong heuristics are essential

**5×5 Puzzles:**
- A* may run out of memory on difficult puzzles
- **Best: A\* with Linear Conflict** - most reliable option
- Solving time: seconds to minutes depending on difficulty and available memory
- Very hard puzzles may hit memory limits

## A* Algorithm

### Strengths

✅ **Best for:**
- 3x3 puzzles (all difficulties)
- 4x4 puzzles with strong heuristics
- Systems with abundant memory (8GB+)
- When you need guaranteed optimal solution quickly

⚠️ **May struggle with:**
- Very large (5x5+) puzzles
- Memory-constrained systems
- Extremely difficult puzzle configurations

## Heuristic Comparison

### Linear Conflict (Enhanced)
- ✅ **Best overall heuristic**
- Most informed (finds solutions with fewer explored states)
- Works well with A*
- Slightly more computation per state, but worth it

### Manhattan Distance
- ✅ **Good general-purpose heuristic**
- Fast to compute
- Works well for most puzzle sizes
- Good balance of speed and accuracy

### Hamming Distance
- ⚠️ **Weak heuristic**
- Only counts misplaced tiles
- Results in many more explored states
- Avoid for 4x4+

### Uninformed Search
- ❌ **Baseline only**
- No heuristic guidance (equivalent to Dijkstra's)
- Only useful for algorithm comparison
- Impractical for actual solving

### Greedy Search
- ⚠️ **Fast but not optimal**
- Uses f(n) = h(n) (ignores path cost g(n))
- Explores fewer states than A*

### Weighted A*
- ✅ **Configurable speed/optimality trade-off**
- Uses f(n) = g(n) + weight × h(n)
- **weight = 1.0**: Standard A* (optimal, slower)
- **weight > 1.0**: Faster but suboptimal (e.g., 1.5, 2.0)
- Typical values: 1.0 to 3.0
- Excellent for larger puzzles where strict optimality isn't required
- Higher weights = faster solving but potentially longer solution paths
- May find suboptimal solutions
- Good for difficult puzzles where A* runs out of memory

## Performance Metrics

The solver provides the following metrics:
- **Solution Length**: Number of moves to solve
- **Time Complexity**: Total states explored
- **Space Complexity**: Maximum states in memory
- **Execution Time**: Wall-clock time in seconds
- **Memory Usage**: Estimated memory consumption

## Technical Details

- **State Representation**: Optimized uint8_t vector
- **Hash Function**: Fast state hashing for closed set
- **Memory Management**: Configurable memory limits for A*
- **Goal Pattern**: Snail/spiral pattern (standard)

## Recommendations

| Puzzle Size | Best Heuristic | Shuffle Iterations | Expected Performance |
|-------------|----------------|-------------------|---------------------|
| **3x3** | Linear Conflict | 500 (default) | < 1 second, < 10MB |
| **4x4** | Linear Conflict | 500 (default) | 1-30 seconds, < 500MB |
| **5x5** | Linear Conflict | 500 (default) | Variable (may fail on hard puzzles) |

### 5x5 Puzzle Limitations

A* with Linear Conflict may struggle with 5x5 puzzles:
- **Easy puzzles** (30-40 moves): Usually solves in seconds
- **Medium puzzles** (40-60 moves): May solve in minutes
- **Hard puzzles** (60+ moves): Often runs out of memory (2.25GB limit)

For truly difficult 5x5 puzzles, more advanced techniques are needed (Pattern Databases, IDA*, etc.).

## License

This project is part of the 42 School curriculum (after Common Core).

## TO DO

- [X] Add UCS Algo (Uniform Cost Search, no heuristic, 5x5 impossible)
- [X] Add Greedy Search Algo (5x5 feasible but not optimal)
- [ ] Add Weighted A* Algo (5x5 feasible but not optimal)
- [ ] Add Beam Search Algo (5x5 feasible but not optimal)
