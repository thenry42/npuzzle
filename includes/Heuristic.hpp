#ifndef HEURISTIC_HPP
#define HEURISTIC_HPP

#include <vector>
#include <cstdint>

// Pre-computed goal positions for efficient heuristic calculation
struct GoalLookup {
    std::vector<uint8_t> position;  // position[tile_value] = flat_index
    std::vector<int> row;           // row[tile_value] = row_index
    std::vector<int> col;           // col[tile_value] = col_index
    
    GoalLookup(const std::vector<uint8_t>& goal, int size);
};

class Heuristic {
    public:
        Heuristic();
        ~Heuristic();
        Heuristic(const Heuristic& other);
        Heuristic& operator=(const Heuristic& other);

        // Optimized heuristic functions with pre-computed goal lookup
        static int manhattanDistance(const std::vector<uint8_t>& state, 
                                    const GoalLookup& goalLookup, int size);
        static int hammingDistance(const std::vector<uint8_t>& state, 
                                  const GoalLookup& goalLookup, int size);
        static int linearConflict(const std::vector<uint8_t>& state, 
                                 const GoalLookup& goalLookup, int size);
        static int uninformedSearch(const std::vector<uint8_t>& state, 
                                   const GoalLookup& goalLookup, int size);
        // Get heuristic value based on choice
        static int getHeuristicValue(const std::vector<uint8_t>& state,
                                    const GoalLookup& goalLookup,
                                    int size, int heuristic);
};

#endif
