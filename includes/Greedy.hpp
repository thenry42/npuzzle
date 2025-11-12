#ifndef GREEDY_HPP
#define GREEDY_HPP

#include "Puzzle.hpp"
#include "Node.hpp"
#include "Heuristic.hpp"
#include "Astar.hpp"
#include <map>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

class Greedy {
    public:
        Greedy();
        ~Greedy();
        Greedy(const Greedy& other);
        Greedy& operator=(const Greedy& other);

        // Greedy Best-First Search (f(n) = h(n))
        AStarResult solve(Puzzle& puzzle, int size, int heuristic, bool silent = false, 
                         size_t maxStates = 0, double maxTimeSeconds = 0);

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
        size_t estimateMemoryUsage(size_t numStates);
};

#endif

