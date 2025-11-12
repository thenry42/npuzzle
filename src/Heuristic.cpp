#include "../includes/Heuristic.hpp"
#include <cmath>
#include <algorithm>

// GoalLookup constructor - pre-compute all goal positions
GoalLookup::GoalLookup(const std::vector<uint8_t>& goal, int size) {
    int total = size * size;
    position.resize(total);
    row.resize(total);
    col.resize(total);
    
    for (int i = 0; i < total; i++) {
        uint8_t value = goal[i];
        if (value != 0) {
            position[value] = i;
            row[value] = i / size;
            col[value] = i % size;
        }
    }
}

Heuristic::Heuristic() {}

Heuristic::~Heuristic() {}

Heuristic::Heuristic(const Heuristic& other) {
    (void)other;
}

Heuristic& Heuristic::operator=(const Heuristic& other) {
    (void)other;
    return *this;
}

int Heuristic::manhattanDistance(const std::vector<uint8_t>& state, 
                                 const GoalLookup& goalLookup, int size) {
    // Calculate Manhattan distance for each tile using pre-computed goal positions
    // f(n) = g(n) + h(n)
    // g(n) = cost from start to current node
    // h(n) = sum(abs(currentRow - goalRow) + abs(currentCol - goalCol))
    
    int distance = 0;
    for (int i = 0; i < size * size; i++) {
        uint8_t value = state[i];
        if (value != 0) {
            int currentRow = i / size;
            int currentCol = i % size;
            int goalRow = goalLookup.row[value];
            int goalCol = goalLookup.col[value];
            
            distance += std::abs(currentRow - goalRow) + std::abs(currentCol - goalCol);
        }
    }
    
    return distance;
}

int Heuristic::hammingDistance(const std::vector<uint8_t>& state, 
                               const GoalLookup& goalLookup, int size) {
    // Count misplaced tiles using pre-computed goal positions
    
    // f(n) = g(n) + h(n)
    // g(n) = cost from start to current node
    // h(n) = sum(misplaced tiles)
    
    int distance = 0;
    for (int i = 0; i < size * size; i++) {
        uint8_t value = state[i];
        if (value != 0 && goalLookup.position[value] != i) {
            distance++;
        }
    }
    
    return distance;
}

int Heuristic::linearConflict(const std::vector<uint8_t>& state, 
                              const GoalLookup& goalLookup, int size) {
    // Linear Conflict = Manhattan Distance + (Number of conflicts * 2)
    
    // f(n) = g(n) + h(n)
    // g(n) = cost from start to current node
    // h(n) = Manhattan distance + (Number of conflicts * 2)
    
    int distance = manhattanDistance(state, goalLookup, size);
    int conflicts = 0;
    
    // Check for row conflicts
    for (int row = 0; row < size; row++) {
        for (int i = 0; i < size; i++) {
            int posI = row * size + i;
            uint8_t tileI = state[posI];
            
            if (tileI == 0) continue;
            
            // Only check tiles that are in their goal row
            if (goalLookup.row[tileI] != row) continue;
            
            for (int j = i + 1; j < size; j++) {
                int posJ = row * size + j;
                uint8_t tileJ = state[posJ];
                
                if (tileJ == 0) continue;
                
                // Only check tiles that are in their goal row
                if (goalLookup.row[tileJ] != row) continue;
                
                // If tiles are in reversed order relative to goal
                if (goalLookup.col[tileI] > goalLookup.col[tileJ]) {
                    conflicts++;
                }
            }
        }
    }
    
    // Check for column conflicts
    for (int col = 0; col < size; col++) {
        for (int i = 0; i < size; i++) {
            int posI = i * size + col;
            uint8_t tileI = state[posI];
            
            if (tileI == 0) continue;
            
            // Only check tiles that are in their goal column
            if (goalLookup.col[tileI] != col) continue;
            
            for (int j = i + 1; j < size; j++) {
                int posJ = j * size + col;
                uint8_t tileJ = state[posJ];
                
                if (tileJ == 0) continue;
                
                // Only check tiles that are in their goal column
                if (goalLookup.col[tileJ] != col) continue;
                
                // If tiles are in reversed order relative to goal
                if (goalLookup.row[tileI] > goalLookup.row[tileJ]) {
                    conflicts++;
                }
            }
        }
    }
    
    return distance + (conflicts * 2);
}

int Heuristic::uninformedSearch(const std::vector<uint8_t>& state, 
                                const GoalLookup& goalLookup, int size) {
    // Uninformed search = Dijkstra's algorithm = Uniform Cost Search = UCS
    // No heuristic, always returns 0
    // f(n) = g(n)
    // g(n) = cost from start to current node
    
    (void)state;
    (void)goalLookup;
    (void)size;
    return 0;
}


int Heuristic::getHeuristicValue(const std::vector<uint8_t>& state,
                                 const GoalLookup& goalLookup,
                                 int size, int heuristic) {
    switch (heuristic) {
        case 1:
            return manhattanDistance(state, goalLookup, size);
        case 2:
            return hammingDistance(state, goalLookup, size);
        case 3:
            return linearConflict(state, goalLookup, size);
        case 4:
            return uninformedSearch(state, goalLookup, size);
        default:
            // For Greedy (5), Weighted A* (6), and Beam Search (7),
            // use Manhattan distance as the base heuristic
            return manhattanDistance(state, goalLookup, size);
    }
}
