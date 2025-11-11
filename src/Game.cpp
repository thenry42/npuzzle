#include "../includes/Game.hpp"
#include "../includes/Puzzle.hpp"
#include <map>
#include <string>

Game::Game() {}

Game::~Game() {}

Game::Game(const Game& other) : _display(other._display), _astar(other._astar) {}

Game& Game::operator=(const Game& other) {
    if (this != &other) {
        _display = other._display;
        _astar = other._astar;
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
        
        // Prompt for the number of iterations for the shuffle
        iterations = _display.promptForIterations();
    }

    // Prompt the user for preferred heuristic
    int heuristic = _display.promptForHeuristic();

    // Map heuristic choice to name for display
    std::map<int, std::string> heuristicNames;
    heuristicNames[1] = "Manhattan Distance";
    heuristicNames[2] = "Hamming Distance";
    heuristicNames[3] = "Linear Conflict";
    heuristicNames[4] = "Uninformed Search";

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

    // Solve the puzzle using A* algorithm
    _astar.solve(puzzle, size, heuristic, false);
}

