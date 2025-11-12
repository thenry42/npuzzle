#ifndef SOLUTIONLOGGER_HPP
#define SOLUTIONLOGGER_HPP

#include "Node.hpp"
#include <string>
#include <vector>
#include <memory>
#include <cstdint>

class SolutionLogger {
    public:
        SolutionLogger();
        ~SolutionLogger();
        
        // Log a complete solution to a file
        static void logSolution(
            const std::string& algorithmName,
            const std::string& heuristicName,
            const std::vector<uint8_t>& initialState,
            const std::vector<uint8_t>& goalState,
            int size,
            std::shared_ptr<Node> goalNode,
            int moves,
            int timeComplexity,
            int spaceComplexity,
            double executionTime,
            double weight = 1.0,
            int beamWidth = 0
        );
        
        // Log when a solution fails (unsolvable, timeout, memory limit)
        static void logFailure(
            const std::string& algorithmName,
            const std::string& heuristicName,
            const std::vector<uint8_t>& initialState,
            const std::vector<uint8_t>& goalState,
            int size,
            const std::string& failureReason,
            int timeComplexity,
            int spaceComplexity,
            double executionTime
        );
        
    private:
        // Helper to convert flat state to 2D grid string
        static std::string stateToString(const std::vector<uint8_t>& state, int size);
        
        // Helper to generate unique filename with timestamp
        static std::string generateFilename(const std::string& algorithmName, bool success);
        
        // Helper to reconstruct path from goal node
        static std::vector<std::shared_ptr<Node>> reconstructPath(std::shared_ptr<Node> goalNode);
        
        // Helper to ensure results directory exists
        static void ensureResultsDirectory();
};

#endif

