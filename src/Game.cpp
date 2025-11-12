#include "../includes/Game.hpp"
#include "../includes/Puzzle.hpp"
#include <map>
#include <string>

Game::Game() {}

Game::~Game() {}

Game::Game(const Game& other) : _display(other._display), _astar(other._astar), 
                                 _ucs(other._ucs), _weightedAstar(other._weightedAstar), 
                                 _greedy(other._greedy), _beamSearch(other._beamSearch) {}

Game& Game::operator=(const Game& other) {
    if (this != &other) {
        _display = other._display;
        _astar = other._astar;
        _ucs = other._ucs;
        _weightedAstar = other._weightedAstar;
        _greedy = other._greedy;
        _beamSearch = other._beamSearch;
    }
    return *this;
}

void Game::gameLoop() {
    // Get grid size from user
    int size = _display.promptForGridSize();

    // Prompt user to choose if they want a custom puzzle
    bool useCustom = _display.promptForUseCustomPuzzle();
    
    bool solvable = true;
    int iterations = 0;
    std::vector<std::vector<int>> customGrid;

    if (useCustom) {
        // User will provide a custom puzzle
        customGrid = _display.promptForCustomPuzzle(size);
        // solvable will be determined from the puzzle
        // iterations not applicable for custom puzzles
    } else {
        // Generate a random puzzle
        // Prompt user to choose if they want a solvable or unsolvable puzzle
        solvable = _display.promptSolvableOrUnsolvable();
        
        // Prompt for the number of iterations for the shuffle (pass size for smart defaults)
        iterations = _display.promptForIterations();
    }

    // Step 1: Prompt for algorithm choice
    int algorithm = _display.promptForAlgorithm();  // 1=A*, 2=UCS, 3=Greedy, 4=Weighted A*, 5=Beam Search
    
    // Step 2: Prompt for heuristic based on algorithm (UCS returns 0)
    int heuristic = _display.promptForHeuristic(algorithm);
    
    // Step 3: Prompt for algorithm-specific parameters
    double weight = 1.0;  // For Weighted A*
    int beamWidth = 100;  // For Beam Search
    
    if (algorithm == 4) {
        // Weighted A* - ask for weight
        weight = _display.promptForWeight();
    } else if (algorithm == 5) {
        // Beam Search - ask for beam width
        beamWidth = _display.promptForBeamWidth();
    }

    // Map choices to names for display
    std::map<int, std::string> algorithmNames;
    algorithmNames[1] = "A*";
    algorithmNames[2] = "UCS";
    algorithmNames[3] = "Greedy Search";
    algorithmNames[4] = "Weighted A*";
    algorithmNames[5] = "Beam Search";
    
    std::map<int, std::string> heuristicNames;
    heuristicNames[0] = "None (uninformed)";
    heuristicNames[1] = "Manhattan Distance";
    heuristicNames[2] = "Hamming Distance";
    heuristicNames[3] = "Linear Conflict";

    std::string algorithmName = algorithmNames.count(algorithm) ? 
                                algorithmNames[algorithm] : "Unknown";
    std::string heuristicName = heuristicNames.count(heuristic) ? 
                                heuristicNames[heuristic] : "Unknown";
    
    // Combine for display
    std::string displayName;
    if (algorithm == 2) {
        // UCS doesn't use heuristic
        displayName = algorithmName;
    } else {
        displayName = algorithmName + " with " + heuristicName;
    }

    // Display recap of the user's choices
    if (!_display.displayRecap(size, solvable, iterations, displayName, useCustom)) {
        return;
    }

    Puzzle puzzle(size, solvable, iterations);

    if (useCustom) {
        // Use the custom grid (convert 2D to flat)
        puzzle.setGridFrom2D(customGrid);
    } else {
        // Generate a solvable or unsolvable puzzle
        puzzle.generatePuzzle();
    }

    // Display the puzzle (convert flat to 2D for display)
    _display.displayPuzzle(puzzle.getGrid2D());

    // Display the puzzle inline (convert flat to 2D for display)
    _display.displayPuzzleInline(puzzle.getGrid2D(), puzzle.getGoal2D());

    // Route to appropriate algorithm based on user's choice
    // All algorithms use the same time and memory limits defined in Game class
    if (algorithm == 1) {
        // A* - pass heuristic (1=Manhattan, 2=Hamming, 3=Linear Conflict)
        _astar.solve(puzzle, size, heuristic, false, DEFAULT_MAX_STATES, DEFAULT_MAX_TIME);
    } else if (algorithm == 2) {
        // UCS - no heuristic needed (uses only g(n))
        _ucs.solve(puzzle, size, false, DEFAULT_MAX_STATES, DEFAULT_MAX_TIME);
    } else if (algorithm == 3) {
        // Greedy Search - pass heuristic (1=Manhattan, 2=Hamming, 3=Linear Conflict)
        _greedy.solve(puzzle, size, heuristic, false, DEFAULT_MAX_STATES, DEFAULT_MAX_TIME);
    } else if (algorithm == 4) {
        // Weighted A* - pass heuristic (1=Manhattan, 2=Hamming, 3=Linear Conflict) and weight
        _weightedAstar.solve(puzzle, size, heuristic, false, DEFAULT_MAX_STATES, DEFAULT_MAX_TIME, weight);
    } else if (algorithm == 5) {
        // Beam Search - pass heuristic (1=Manhattan, 2=Hamming, 3=Linear Conflict) and beam width
        _beamSearch.solve(puzzle, size, heuristic, false, DEFAULT_MAX_STATES, DEFAULT_MAX_TIME, beamWidth);
    }
}

