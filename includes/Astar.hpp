#ifndef ASTAR_HPP
#define ASTAR_HPP

#include "Puzzle.hpp"
#include "Node.hpp"
#include "Heuristic.hpp"
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

struct AStarResult {
    bool solved;
    int moves;
    int timeComplexity;
    int spaceComplexity;
    double executionTime;
    int heuristicId;
    std::string heuristicName;
    int algorithmId;  // 1 = A*
    std::string algorithmName;
    bool memoryLimitReached;
    std::string failureReason;
    double weight;  // Weight parameter for weighted A* (1.0 = standard A*)
};

// Comparator for priority queue of shared_ptr<Node>
struct NodePtrComparator {
    bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const {
        // For min-heap, return true if a has HIGHER priority (worse f-value)
        return a->getFValue() > b->getFValue();
    }
};

class Astar {
    public:
        Astar();
        ~Astar();
        Astar(const Astar& other);
        Astar& operator=(const Astar& other);

        // A* with memory limit and timeout
        AStarResult solve(Puzzle& puzzle, int size, int heuristic, bool silent = false, 
                         size_t maxStates = 0, double maxTimeSeconds = 0);  // 0 = use default from macros
        
        // Memory management
        static size_t estimateMemoryUsage(size_t numStates);

    private:
        // Helper functions
        struct Neighbor {
            std::vector<uint8_t> state;
            uint8_t zeroPos;
            std::string action;
        };
        
        void getNeighbors(const Node& node, std::vector<Neighbor>& neighbors);
        bool isGoal(const std::vector<uint8_t>& state, 
                   const std::vector<uint8_t>& goal);
};

#endif
