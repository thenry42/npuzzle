#include "../includes/SolutionLogger.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sys/stat.h>
#include <algorithm>
#include <iostream>

SolutionLogger::SolutionLogger() {}

SolutionLogger::~SolutionLogger() {}

void SolutionLogger::ensureResultsDirectory() {
    struct stat st;
    if (stat("results", &st) != 0) {
        mkdir("results", 0755);
    }
}

std::string SolutionLogger::generateFilename(const std::string& algorithmName, bool success) {
    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << "results/";
    
    // Clean algorithm name (replace spaces with underscores)
    std::string cleanAlgo = algorithmName;
    std::replace(cleanAlgo.begin(), cleanAlgo.end(), ' ', '_');
    // Replace * with A (for A*)
    std::replace(cleanAlgo.begin(), cleanAlgo.end(), '*', 'A');
    
    ss << cleanAlgo << "_";
    ss << (success ? "SUCCESS" : "FAILURE") << "_";
    
    // Format: YYYYMMDD_HHMMSS_mmm
    std::tm* tm_info = std::localtime(&time);
    ss << std::put_time(tm_info, "%Y%m%d_%H%M%S");
    ss << "_" << std::setfill('0') << std::setw(3) << ms.count();
    ss << ".txt";
    
    return ss.str();
}

std::string SolutionLogger::stateToString(const std::vector<uint8_t>& state, int size) {
    std::stringstream ss;
    
    // Find the maximum number width for proper alignment
    int maxVal = size * size - 1;
    int width = 1;
    while (maxVal >= 10) {
        maxVal /= 10;
        width++;
    }
    
    for (int i = 0; i < size; i++) {
        ss << "    ";  // Indentation
        for (int j = 0; j < size; j++) {
            int val = state[i * size + j];
            if (val == 0) {
                ss << std::setw(width) << "_";
            } else {
                ss << std::setw(width) << val;
            }
            if (j < size - 1) {
                ss << " ";
            }
        }
        ss << "\n";
    }
    
    return ss.str();
}

std::vector<std::shared_ptr<Node>> SolutionLogger::reconstructPath(std::shared_ptr<Node> goalNode) {
    std::vector<std::shared_ptr<Node>> path;
    
    std::shared_ptr<Node> current = goalNode;
    while (current) {
        path.push_back(current);
        current = current->getParent();
    }
    
    // Reverse to get path from start to goal
    std::reverse(path.begin(), path.end());
    
    return path;
}

void SolutionLogger::logSolution(
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
    double weight,
    int beamWidth
) {
    ensureResultsDirectory();
    
    std::string filename = generateFilename(algorithmName, true);
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Warning: Could not create solution log file: " << filename << "\n";
        return;
    }
    
    // Write header
    file << std::string(70, '=') << "\n";
    file << "N-PUZZLE SOLUTION LOG\n";
    file << std::string(70, '=') << "\n\n";
    
    // Write algorithm info
    file << "Algorithm: " << algorithmName << "\n";
    if (!heuristicName.empty() && heuristicName != "None (uninformed)") {
        file << "Heuristic: " << heuristicName << "\n";
    }
    if (weight != 1.0 && weight > 0) {
        file << "Weight: " << weight << "\n";
    }
    if (beamWidth > 0) {
        file << "Beam Width: " << beamWidth << "\n";
    }
    file << "Puzzle Size: " << size << "x" << size << "\n";
    
    // Write statistics
    file << "\n" << std::string(70, '-') << "\n";
    file << "STATISTICS\n";
    file << std::string(70, '-') << "\n";
    file << "Solution Found: YES\n";
    file << "Total Moves: " << moves << "\n";
    file << "Time Complexity (states opened): " << timeComplexity << "\n";
    file << "Space Complexity (max states in memory): " << spaceComplexity << "\n";
    file << "Execution Time: " << std::fixed << std::setprecision(4) << executionTime << "s\n";
    
    // Write initial state
    file << "\n" << std::string(70, '-') << "\n";
    file << "INITIAL STATE\n";
    file << std::string(70, '-') << "\n";
    file << stateToString(initialState, size);
    
    // Write goal state
    file << "\n" << std::string(70, '-') << "\n";
    file << "GOAL STATE\n";
    file << std::string(70, '-') << "\n";
    file << stateToString(goalState, size);
    
    // Write solution path
    file << "\n" << std::string(70, '-') << "\n";
    file << "SOLUTION PATH (" << moves << " moves)\n";
    file << std::string(70, '-') << "\n\n";
    
    std::vector<std::shared_ptr<Node>> path = reconstructPath(goalNode);
    
    for (size_t i = 0; i < path.size(); i++) {
        file << "Step " << i << ": ";
        if (i == 0) {
            file << "Initial State\n";
        } else {
            file << "Move " << path[i]->getAction() << "\n";
        }
        file << stateToString(path[i]->getState(), size);
        file << "\n";
    }
    
    file << std::string(70, '=') << "\n";
    file << "END OF SOLUTION LOG\n";
    file << std::string(70, '=') << "\n";
    
    file.close();
    
    std::cout << "\n[Solution Log] Saved to: " << filename << "\n";
}

void SolutionLogger::logFailure(
    const std::string& algorithmName,
    const std::string& heuristicName,
    const std::vector<uint8_t>& initialState,
    const std::vector<uint8_t>& goalState,
    int size,
    const std::string& failureReason,
    int timeComplexity,
    int spaceComplexity,
    double executionTime
) {
    ensureResultsDirectory();
    
    std::string filename = generateFilename(algorithmName, false);
    std::ofstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Warning: Could not create failure log file: " << filename << "\n";
        return;
    }
    
    // Write header
    file << std::string(70, '=') << "\n";
    file << "N-PUZZLE FAILURE LOG\n";
    file << std::string(70, '=') << "\n\n";
    
    // Write algorithm info
    file << "Algorithm: " << algorithmName << "\n";
    if (!heuristicName.empty() && heuristicName != "None (uninformed)") {
        file << "Heuristic: " << heuristicName << "\n";
    }
    file << "Puzzle Size: " << size << "x" << size << "\n";
    
    // Write statistics
    file << "\n" << std::string(70, '-') << "\n";
    file << "STATISTICS\n";
    file << std::string(70, '-') << "\n";
    file << "Solution Found: NO\n";
    file << "Failure Reason: " << failureReason << "\n";
    file << "Time Complexity (states opened): " << timeComplexity << "\n";
    file << "Space Complexity (max states in memory): " << spaceComplexity << "\n";
    file << "Execution Time: " << std::fixed << std::setprecision(4) << executionTime << "s\n";
    
    // Write initial state
    file << "\n" << std::string(70, '-') << "\n";
    file << "INITIAL STATE\n";
    file << std::string(70, '-') << "\n";
    file << stateToString(initialState, size);
    
    // Write goal state
    file << "\n" << std::string(70, '-') << "\n";
    file << "GOAL STATE\n";
    file << std::string(70, '-') << "\n";
    file << stateToString(goalState, size);
    
    file << "\n" << std::string(70, '=') << "\n";
    file << "END OF FAILURE LOG\n";
    file << std::string(70, '=') << "\n";
    
    file.close();
    
    std::cout << "\n[Failure Log] Saved to: " << filename << "\n";
}

