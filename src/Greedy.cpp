#include "../includes/Greedy.hpp"
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

Greedy::Greedy() {}

Greedy::~Greedy() {}

Greedy::Greedy(const Greedy& other) {
    (void)other;
}

Greedy& Greedy::operator=(const Greedy& other) {
    (void)other;
    return *this;
}

// Estimate memory usage in bytes
size_t Greedy::estimateMemoryUsage(size_t numStates) {
    const size_t BYTES_PER_NODE = 60;
    const size_t HASH_OVERHEAD = 16;
    return numStates * (BYTES_PER_NODE + HASH_OVERHEAD * 2);
}

void Greedy::getNeighbors(const Node& node, std::vector<Neighbor>& neighbors) {
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

bool Greedy::isGoal(const std::vector<uint8_t>& state,
                    const std::vector<uint8_t>& goal) {
    return state == goal;
}

AStarResult Greedy::solve(Puzzle& puzzle, int size, int heuristic, bool silent, 
                          size_t maxStates, double maxTimeSeconds) {
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
    }
    
    // Check if already at goal
    if (isGoal(initialState, goal)) {
        if (!silent) {
            std::cout << "\nPuzzle is already solved!\n";
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        return {true, 0, 0, 0, duration, heuristic, "", 1, "Greedy Search", false, "", 1.0};
    }
    
    // Check if puzzle is solvable
    if (!puzzle.isSolvable()) {
        if (!silent) {
            std::cout << "\nPuzzle is unsolvable!\n";
        }
        auto endTime = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(endTime - startTime).count();
        
        return {false, 0, 0, 0, duration, heuristic, "", 1, "Greedy Search", false, 
                "Puzzle is unsolvable", 1.0};
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
    
    // For Greedy Search: f(n) = h(n), so we set g=0 (with optional parent tracking for logging)
    auto startNode = std::make_shared<Node>(initialState, size, initialZeroPos, 0, initialH, nullptr, "");
    
    // Priority queue for open set
    std::priority_queue<std::shared_ptr<Node>, 
                       std::vector<std::shared_ptr<Node>>, 
                       NodePtrComparator> openSet;
    openSet.push(startNode);
    
    // Set to track visited states (closed set)
    std::unordered_set<size_t> closedSet;
    
    // Dictionary to track real path cost to reach each state
    std::unordered_map<size_t, int> gScores;
    gScores[startNode->hash()] = 0;
    
    // Reusable neighbor vector
    std::vector<Neighbor> neighbors;
    neighbors.reserve(4);
    
    while (!openSet.empty()) {
        // Update max states enqueued
        maxStatesEnqueued = std::max(maxStatesEnqueued, (int)openSet.size());
        
        // Timeout check
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
                        1, "Greedy Search", false, "Timeout reached", 1.0};
            }
        }
        
        // Memory limit check
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
                    1, "Greedy Search", true, "Memory limit reached", 1.0};
        }
        
        // Get node with lowest f value (which is just h for greedy)
        auto current = openSet.top();
        openSet.pop();
        
        // Check if we reached the goal
        if (isGoal(current->getState(), goal)) {
            // Get real path cost from gScores
            size_t goalHash = current->hash();
            int moves = gScores[goalHash];
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(endTime - startTime).count();
            
            if (!silent) {
                std::cout << "\n" << std::string(50, '=') << "\n";
                std::cout << "Solution found!\n";
                std::cout << std::string(50, '=') << "\n";
                std::cout << "Total moves required: " << moves << "\n";
                std::cout << "Total states opened (time complexity): " << totalSetOpened << "\n";
                std::cout << "Maximum states in memory (space complexity): " << maxStatesEnqueued << "\n";
                std::cout << "Execution time: " << std::fixed << std::setprecision(4) 
                         << duration << "s\n";
                std::cout << std::string(50, '=') << "\n";
                
                // Log solution to file
                std::string heuristicName;
                if (heuristic == 1) heuristicName = "Manhattan Distance";
                else if (heuristic == 2) heuristicName = "Hamming Distance";
                else if (heuristic == 3) heuristicName = "Linear Conflict";
                
                SolutionLogger::logSolution(
                    "Greedy Search", heuristicName, initialState, goal, size,
                    current, moves, totalSetOpened, maxStatesEnqueued, duration
                );
            }
            
            return {true, moves, totalSetOpened, maxStatesEnqueued, duration, heuristic, "", 
                    1, "Greedy Search", false, "", 1.0};
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
            // Calculate real g-cost (actual path cost)
            int realGCost = gScores[currentHash] + 1;
            
            // Calculate heuristic - for greedy, we use g=0 in the node
            // so f(n) = 0 + h(n) = h(n)
            int hCost = Heuristic::getHeuristicValue(neighbor.state, goalLookup, size, heuristic);
            
            // Create neighbor node with g=0 for greedy behavior
            auto neighborNode = std::make_shared<Node>(
                std::move(neighbor.state),
                size,
                neighbor.zeroPos,
                0,      // g=0 for greedy search (f = h only)
                hCost,  // h for sorting
                current,  // Track parent for path reconstruction
                neighbor.action  // Track action that led to this state
            );
            
            size_t neighborHash = neighborNode->hash();
            
            // Skip if already visited
            if (closedSet.find(neighborHash) != closedSet.end()) {
                continue;
            }
            
            // Check if this is a better path to this state
            if (gScores.find(neighborHash) != gScores.end() && 
                gScores[neighborHash] <= realGCost) {
                continue;
            }
            
            // Record the real path cost (for solution reporting)
            gScores[neighborHash] = realGCost;
            
            // Add to open set
            openSet.push(neighborNode);
        }
    }
    
    // No solution found
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    if (!silent) {
        std::cout << "\n" << std::string(50, '=') << "\n";
        std::cout << "No solution found! Puzzle may be unsolvable.\n";
        std::cout << std::string(50, '=') << "\n";
        std::cout << "Total states opened: " << totalSetOpened << "\n";
        std::cout << "Maximum states in memory: " << maxStatesEnqueued << "\n";
        std::cout << "Execution time: " << std::fixed << std::setprecision(4) 
                 << duration << "s\n";
        std::cout << std::string(50, '=') << "\n";
    }
    
    return {false, 0, totalSetOpened, maxStatesEnqueued, duration, heuristic, "", 
            1, "Greedy Search", false, "No solution found", 1.0};
}

