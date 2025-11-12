#ifndef WEIGHTEDASTAR_HPP
#define WEIGHTEDASTAR_HPP

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

class WeightedAstar {
    public:
        WeightedAstar();
        ~WeightedAstar();
        WeightedAstar(const WeightedAstar& other);
        WeightedAstar& operator=(const WeightedAstar& other);

        // Weighted A* with configurable weight parameter
        AStarResult solve(Puzzle& puzzle, int size, int heuristic, bool silent = false, 
                         size_t maxStates = 0, double maxTimeSeconds = 0, double weight = 1.5);

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

