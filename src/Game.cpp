#include "../includes/Game.hpp"
#include "../includes/Puzzle.hpp"
#include <map>
#include <string>

Game::Game() {}

Game::~Game() {}

Game::Game(const Game& other) : _display(other._display), _astar(other._astar), 
                                 _weightedAstar(other._weightedAstar), _greedy(other._greedy) {}

Game& Game::operator=(const Game& other) {
    if (this != &other) {
        _display = other._display;
        _astar = other._astar;
        _weightedAstar = other._weightedAstar;
        _greedy = other._greedy;
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

    // Prompt the user for preferred heuristic
    int heuristic = _display.promptForHeuristic();

    // Prompt for weight if weighted A* is selected
    double weight = 1.0;  // Default weight for standard A*
    if (heuristic == 6) {
        weight = _display.promptForWeight();
    }

    // Map heuristic choice to name for display
    std::map<int, std::string> heuristicNames;
    heuristicNames[1] = "Manhattan Distance";
    heuristicNames[2] = "Hamming Distance";
    heuristicNames[3] = "Linear Conflict";
    heuristicNames[4] = "Uninformed Search";
    heuristicNames[5] = "Greedy Search";
    heuristicNames[6] = "Weighted A*";
    heuristicNames[7] = "Beam Search";

    std::string heuristicName = heuristicNames.count(heuristic) ? 
                                heuristicNames[heuristic] : "Unknown";

    // Display recap of the user's choices
    if (!_display.displayRecap(size, solvable, iterations, heuristicName, useCustom)) {
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

    // Route to appropriate algorithm based on heuristic selection
    // All algorithms use the same time and memory limits defined in Game class
    if (heuristic == 5) {
        // Greedy Search (Best-First Search)
        _greedy.solve(puzzle, size, heuristic, false, DEFAULT_MAX_STATES, DEFAULT_MAX_TIME);
    } else if (heuristic == 6) {
        // Weighted A*
        _weightedAstar.solve(puzzle, size, heuristic, false, DEFAULT_MAX_STATES, DEFAULT_MAX_TIME, weight);
    } else {
        // Standard A* (Manhattan, Hamming, Linear Conflict, Uninformed)
        _astar.solve(puzzle, size, heuristic, false, DEFAULT_MAX_STATES, DEFAULT_MAX_TIME);
    }
}

