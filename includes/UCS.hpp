#ifndef UCS_HPP
#define UCS_HPP

#include "Puzzle.hpp"
#include "Node.hpp"
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

struct UCSResult {
    bool solved;
    int moves;
    int timeComplexity;
    int spaceComplexity;
    double executionTime;
    int heuristicId;
    std::string heuristicName;
    int algorithmId;  // 5 = UCS
    std::string algorithmName;
    bool memoryLimitReached;
    std::string failureReason;
};

// Comparator for priority queue of shared_ptr<Node> - UCS only uses g(n)
struct UCSNodePtrComparator {
    bool operator()(const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) const {
        // For min-heap, return true if a has HIGHER cost (worse g-value)
        return a->getCost() > b->getCost();
    }
};

class UCS {
    public:
        UCS();
        ~UCS();
        UCS(const UCS& other);
        UCS& operator=(const UCS& other);

        // UCS (Uniform Cost Search / Dijkstra's) with memory limit and timeout
        UCSResult solve(Puzzle& puzzle, int size, bool silent = false, 
                       size_t maxStates = 0, double maxTimeSeconds = 0);
        
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

