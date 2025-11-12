#include "../includes/BeamSearch.hpp"
#include "../includes/Heuristic.hpp"
#include "../includes/SolutionLogger.hpp"
#include <iostream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <climits>
#include <cfloat>

BeamSearch::BeamSearch() {}

BeamSearch::~BeamSearch() {}

BeamSearch::BeamSearch(const BeamSearch& other) {
    (void)other;
}

BeamSearch& BeamSearch::operator=(const BeamSearch& other) {
    (void)other;
    return *this;
}

// Estimate memory usage in bytes
size_t BeamSearch::estimateMemoryUsage(size_t numStates) {
    const size_t BYTES_PER_NODE = 60;   // Optimized: removed parent & action
    const size_t HASH_OVERHEAD = 16;    // Per entry in unordered_set
    return numStates * (BYTES_PER_NODE + HASH_OVERHEAD);
}

void BeamSearch::getNeighbors(const Node& node, std::vector<Neighbor>& neighbors) {
    neighbors.clear();
    
    const std::vector<uint8_t>& state = node.getState();
    int size = node.getSize();
    uint8_t zeroPos = node.getZeroPos();
    
    int row = zeroPos / size;
    int col = zeroPos % size;
    
    // Define possible moves: up, down, left, right
    struct Move {
        int dr, dc;
        const char* action;
    };
    
    static const Move moves[] = {
        {-1, 0, "up"},
        {1, 0, "down"},
        {0, -1, "left"},
        {0, 1, "right"}
    };
    
    neighbors.reserve(4);
    
    for (const auto& move : moves) {
        int newRow = row + move.dr;
        int newCol = col + move.dc;
        
        // Check if move is valid
        if (newRow >= 0 && newRow < size && newCol >= 0 && newCol < size) {
            uint8_t newZeroPos = newRow * size + newCol;
            
            // Create new state by swapping blank with target tile
            std::vector<uint8_t> newState = state;
            std::swap(newState[zeroPos], newState[newZeroPos]);
            
            // Use emplace_back with move semantics to avoid extra copy
            neighbors.emplace_back(Neighbor{std::move(newState), newZeroPos, move.action});
        }
    }
}


bool BeamSearch::isGoal(const std::vector<uint8_t>& state,
                        const std::vector<uint8_t>& goal) {
    return state == goal;
}

BeamSearchResult BeamSearch::solve(Puzzle& puzzle, int size, int heuristic, bool silent, 
                                   size_t maxStates, double maxTimeSeconds, int beamWidth) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Get initial state and goal
    const std::vector<uint8_t>& initialState = puzzle.getGrid();
    const std::vector<uint8_t>& goal = puzzle.getGoal();
    
    // Display memory and time limits (set by Game class)
    if (!silent) {
        size_t estimatedMB = estimateMemoryUsage(maxStates) / (1024 * 1024);
        std::cout << "\n[Memory Safety] Max states limited to " << maxStates 
                 << " (~" << estimatedMB << " MB)\n";
        std::cout << "[Time Safety] Max execution time: " << maxTimeSeconds << " seconds\n";
        std::cout << "[Beam Search] Beam width (k): " << beamWidth << "\n";
    }
    
    // Check if already at goal
    if (isGoal(initialState, goal)) {
        if (!silent) {
            std::cout << "\nPuzzle is already solved!\n";
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        return {true, 0, 0, 0, duration, heuristic, "", 7, "Beam Search", false, "", beamWidth};
    }
    
    // Check if puzzle is solvable
    if (!puzzle.isSolvable()) {
        if (!silent) {
            std::cout << "\nPuzzle is unsolvable!\n";
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        return {false, 0, 0, 0, duration, heuristic, "", 7, "Beam Search", false, "Puzzle is unsolvable", beamWidth};
    }
    
    // Pre-compute goal lookup table once
    GoalLookup goalLookup(goal, size);
    
    // Initialize statistics
    int totalSetOpened = 0;
    int maxStatesEnqueued = 0;
    
    // Find initial blank position
    uint8_t initialZeroPos = 0;
    for (size_t i = 0; i < initialState.size(); i++) {
        if (initialState[i] == 0) {
            initialZeroPos = static_cast<uint8_t>(i);
            break;
        }
    }
    
    // Calculate initial heuristic
    int initialH = Heuristic::getHeuristicValue(initialState, goalLookup, size, heuristic);
    
    // Create initial node (with optional parent tracking for logging)
    auto startNode = std::make_shared<Node>(initialState, size, initialZeroPos, 0, initialH, nullptr, "");
    
    // Beam: current level of nodes to expand
    std::vector<std::shared_ptr<Node>> beam;
    beam.push_back(startNode);
    
    // Set to track visited states (closed set)
    std::unordered_set<size_t> closedSet;
    
    // Reusable neighbor vector to avoid repeated allocations
    std::vector<Neighbor> neighbors;
    neighbors.reserve(4);
    
    while (!beam.empty()) {
        // Timeout check - prevent infinite execution
        if (maxTimeSeconds > 0) {
            auto currentTime = std::chrono::high_resolution_clock::now();
            double elapsed = std::chrono::duration<double>(currentTime - startTime).count();
            
            if (elapsed > maxTimeSeconds) {
                if (!silent) {
                    std::cout << "\n" << std::string(50, '=') << "\n";
                    std::cout << "⏱️  TIMEOUT REACHED!\n";
                    std::cout << std::string(50, '=') << "\n";
                    std::cout << "Explored " << totalSetOpened << " states before timeout\n";
                    std::cout << "Maximum states in memory: " << maxStatesEnqueued << "\n";
                    std::cout << "Time limit: " << maxTimeSeconds << "s\n";
                    std::cout << "Execution time: " << std::fixed << std::setprecision(4) 
                             << elapsed << "s\n";
                }
                
                return {false, 0, totalSetOpened, maxStatesEnqueued, elapsed, heuristic, "", 
                        7, "Beam Search", false, "Timeout reached", beamWidth};
            }
        }
        
        // Memory limit check - prevent crash
        if (totalSetOpened >= static_cast<int>(maxStates)) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            if (!silent) {
                std::cout << "\n" << std::string(50, '=') << "\n";
                std::cout << "⚠️  MEMORY LIMIT REACHED!\n";
                std::cout << std::string(50, '=') << "\n";
                std::cout << "Explored " << totalSetOpened << " states before hitting limit\n";
                std::cout << "Maximum states in memory: " << maxStatesEnqueued << "\n";
                std::cout << "Estimated memory used: " 
                         << (estimateMemoryUsage(totalSetOpened) / (1024 * 1024)) << " MB\n";
                std::cout << "Execution time: " << std::fixed << std::setprecision(4) 
                         << duration << "s\n";
            }
            
            return {false, 0, totalSetOpened, maxStatesEnqueued, duration, heuristic, "", 
                    7, "Beam Search", true, "Memory limit reached", beamWidth};
        }
        
        // Next level of candidates
        std::vector<std::shared_ptr<Node>> candidates;
        
        // Expand all nodes in current beam
        for (const auto& current : beam) {
            // Check if we reached the goal
            if (isGoal(current->getState(), goal)) {
                int moves = current->getCost();
                
                auto endTime = std::chrono::high_resolution_clock::now();
                double duration = std::chrono::duration<double>(endTime - startTime).count();
                
                if (!silent) {
                    std::cout << "\n" << std::string(50, '=') << "\n";
                    std::cout << "Solution found!\n";
                    std::cout << std::string(50, '=') << "\n";
                    std::cout << "Total moves required: " << moves << "\n";
                    std::cout << "Total states opened (time complexity): " << totalSetOpened << "\n";
                    std::cout << "Maximum states in memory (space complexity): " << maxStatesEnqueued << "\n";
                    std::cout << "Beam width (k): " << beamWidth << "\n";
                    std::cout << "Execution time: " << std::fixed << std::setprecision(4) 
                             << duration << "s\n";
                    std::cout << std::string(50, '=') << "\n";
                    
                    // Log solution to file
                    std::string heuristicName;
                    if (heuristic == 1) heuristicName = "Manhattan Distance";
                    else if (heuristic == 2) heuristicName = "Hamming Distance";
                    else if (heuristic == 3) heuristicName = "Linear Conflict";
                    
                    SolutionLogger::logSolution(
                        "Beam Search", heuristicName, initialState, goal, size,
                        current, moves, totalSetOpened, maxStatesEnqueued, duration, 1.0, beamWidth
                    );
                }
                
                return {true, moves, totalSetOpened, maxStatesEnqueued, duration, heuristic, "", 7, "Beam Search", false, "", beamWidth};
            }
            
            // Add current state to closed set
            size_t currentHash = current->hash();
            if (closedSet.find(currentHash) != closedSet.end()) {
                continue;
            }
            closedSet.insert(currentHash);
            totalSetOpened++;
            
            // Generate and process neighbors
            getNeighbors(*current, neighbors);
            
            for (auto& neighbor : neighbors) {
                // Calculate costs
                int gCost = current->getCost() + 1;
                int hCost = Heuristic::getHeuristicValue(neighbor.state, goalLookup, size, heuristic);
                
                // Create neighbor node
                auto neighborNode = std::make_shared<Node>(
                    std::move(neighbor.state),
                    size,
                    neighbor.zeroPos,
                    gCost,
                    hCost,
                    current,          // Track parent for path reconstruction
                    neighbor.action   // Track action that led to this state
                );
                
                size_t neighborHash = neighborNode->hash();
                
                // Skip if already visited
                if (closedSet.find(neighborHash) != closedSet.end()) {
                    continue;
                }
                
                // Add to candidates
                candidates.push_back(neighborNode);
            }
        }
        
        // If no candidates, search has failed
        if (candidates.empty()) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            if (!silent) {
                std::cout << "\n" << std::string(50, '=') << "\n";
                std::cout << "Beam Search exhausted! No solution found.\n";
                std::cout << std::string(50, '=') << "\n";
                std::cout << "Total states opened: " << totalSetOpened << "\n";
                std::cout << "Maximum states in memory: " << maxStatesEnqueued << "\n";
                std::cout << "Beam width (k): " << beamWidth << "\n";
                std::cout << "Execution time: " << std::fixed << std::setprecision(4) 
                         << duration << "s\n";
                std::cout << std::string(50, '=') << "\n";
            }
            
            return {false, 0, totalSetOpened, maxStatesEnqueued, duration, heuristic, "", 
                    7, "Beam Search", false, "Beam search exhausted (no candidates)", beamWidth};
        }
        
        // Sort candidates by heuristic value (best first)
        std::sort(candidates.begin(), candidates.end(), 
                  [](const std::shared_ptr<Node>& a, const std::shared_ptr<Node>& b) {
                      return a->getHeuristic() < b->getHeuristic();
                  });
        
        // Keep only the best k candidates for the next beam
        int actualBeamWidth = std::min(beamWidth, static_cast<int>(candidates.size()));
        beam.clear();
        beam.reserve(actualBeamWidth);
        for (int i = 0; i < actualBeamWidth; i++) {
            beam.push_back(candidates[i]);
        }
        
        // Update max states in memory
        maxStatesEnqueued = std::max(maxStatesEnqueued, static_cast<int>(beam.size()));
    }
    
    // No solution found (should not reach here normally)
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    if (!silent) {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "No solution found!\n";
        std::cout << std::string(50, '=') << "\n";
        std::cout << "Total states opened: " << totalSetOpened << "\n";
        std::cout << "Maximum states in memory: " << maxStatesEnqueued << "\n";
        std::cout << "Beam width (k): " << beamWidth << "\n";
        std::cout << "Execution time: " << std::fixed << std::setprecision(4) 
                 << duration << "s\n";
        std::cout << std::string(50, '=') << "\n";
    }
    
    return {false, 0, totalSetOpened, maxStatesEnqueued, duration, heuristic, "", 
            7, "Beam Search", false, "No solution found", beamWidth};
}

