#include "../includes/Puzzle.hpp"
#include <algorithm>
#include <random>
#include <ctime>

Puzzle::Puzzle(int size, bool solvable, int iterations)
    : _size(size), _solvable(solvable), _iterations(iterations), _goalParity(0) {
    
    // Initialize flat arrays
    int totalSize = size * size;
    _grid.resize(totalSize);
    _goal.resize(totalSize);
    
    // Generate goal state in snail/spiral pattern
    generateGoalSnail();
    
    // Calculate goal parity once
    _goalParity = calculateGoalParity();
}

Puzzle::~Puzzle() {}

Puzzle::Puzzle(const Puzzle& other)
    : _size(other._size), _solvable(other._solvable),
      _iterations(other._iterations), _grid(other._grid), _goal(other._goal),
      _goalParity(other._goalParity) {}

Puzzle& Puzzle::operator=(const Puzzle& other) {
    if (this != &other) {
        _size = other._size;
        _solvable = other._solvable;
        _iterations = other._iterations;
        _grid = other._grid;
        _goal = other._goal;
        _goalParity = other._goalParity;
    }
    return *this;
}

void Puzzle::generateGoalSnail() {
    // Generate goal state in snail/spiral pattern (matching Python make_goal)
    int ts = _size * _size;
    std::vector<int> goalFlat(ts, -1);
    
    int cur = 1;
    int x = 0;
    int ix = 1;
    int y = 0;
    int iy = 0;
    
    while (true) {
        goalFlat[x + y * _size] = cur;
        if (cur == 0) {
            break;
        }
        cur++;
        
        // Check if we need to change direction
        if (x + ix == _size || x + ix < 0 || (ix != 0 && goalFlat[x + ix + y * _size] != -1)) {
            iy = ix;
            ix = 0;
        } else if (y + iy == _size || y + iy < 0 || (iy != 0 && goalFlat[x + (y + iy) * _size] != -1)) {
            ix = -iy;
            iy = 0;
        }
        
        x += ix;
        y += iy;
        
        if (cur == _size * _size) {
            cur = 0;
        }
    }
    
    // Convert to uint8_t flat array
    for (int i = 0; i < ts; i++) {
        _goal[i] = static_cast<uint8_t>(goalFlat[i]);
    }
}

void Puzzle::generatePuzzle() {
    if (_solvable) {
        generateSolvablePuzzle();
    } else {
        generateUnsolvablePuzzle();
    }
}

void Puzzle::generateSolvablePuzzle() {
    // Generate a solvable puzzle by starting from goal state and shuffling
    _grid = _goal;  // Copy goal state
    
    // Shuffle by swapping blank tile with adjacent tiles
    std::srand(std::time(nullptr));
    for (int i = 0; i < _iterations; i++) {
        swapEmpty(_grid);
    }
}

void Puzzle::generateUnsolvablePuzzle() {
    // Generate an unsolvable puzzle by creating a solvable one and swapping two tiles
    _grid = _goal;  // Copy goal state
    
    // Shuffle by swapping blank tile with adjacent tiles
    std::srand(std::time(nullptr));
    for (int i = 0; i < _iterations; i++) {
        swapEmpty(_grid);
    }
    
    // Make it unsolvable by swapping two non-zero tiles
    int ts = _size * _size;
    if (_grid[0] == 0 || _grid[1] == 0) {
        std::swap(_grid[ts - 1], _grid[ts - 2]);
    } else {
        std::swap(_grid[0], _grid[1]);
    }
}

void Puzzle::swapEmpty(std::vector<uint8_t>& puzzle) {
    // Find blank tile position
    uint8_t idx = findBlankPos(puzzle);
    
    int row = idx / _size;
    int col = idx % _size;
    
    std::vector<uint8_t> poss;
    
    // Can move left
    if (col > 0) {
        poss.push_back(idx - 1);
    }
    // Can move right
    if (col < _size - 1) {
        poss.push_back(idx + 1);
    }
    // Can move up
    if (row > 0) {
        poss.push_back(idx - _size);
    }
    // Can move down
    if (row < _size - 1) {
        poss.push_back(idx + _size);
    }
    
    // Choose random adjacent position
    uint8_t swi = poss[std::rand() % poss.size()];
    puzzle[idx] = puzzle[swi];
    puzzle[swi] = 0;
}

const std::vector<uint8_t>& Puzzle::getGrid() const {
    return _grid;
}

void Puzzle::setGrid(const std::vector<uint8_t>& grid) {
    _grid = grid;
}

void Puzzle::setGridFrom2D(const std::vector<std::vector<int>>& grid) {
    // Convert 2D grid to flat array
    int idx = 0;
    for (int i = 0; i < _size; i++) {
        for (int j = 0; j < _size; j++) {
            _grid[idx++] = static_cast<uint8_t>(grid[i][j]);
        }
    }
}

const std::vector<uint8_t>& Puzzle::getGoal() const {
    return _goal;
}

std::vector<std::vector<int>> Puzzle::getGrid2D() const {
    // Convert flat array to 2D grid for display
    std::vector<std::vector<int>> grid2D(_size, std::vector<int>(_size));
    for (int i = 0; i < _size; i++) {
        for (int j = 0; j < _size; j++) {
            grid2D[i][j] = static_cast<int>(_grid[i * _size + j]);
        }
    }
    return grid2D;
}

std::vector<std::vector<int>> Puzzle::getGoal2D() const {
    // Convert flat array to 2D grid for display
    std::vector<std::vector<int>> goal2D(_size, std::vector<int>(_size));
    for (int i = 0; i < _size; i++) {
        for (int j = 0; j < _size; j++) {
            goal2D[i][j] = static_cast<int>(_goal[i * _size + j]);
        }
    }
    return goal2D;
}

int Puzzle::getSize() const {
    return _size;
}

bool Puzzle::isSolvable() const {
    return isSolvableCheck(_grid);
}

bool Puzzle::isSolvableCheck(const std::vector<uint8_t>& grid) const {
    // Check if the puzzle is solvable based on inversion count and grid size
    int inversions = countInversions(grid);
    int currentParity;
    
    if (_size % 2 == 1) {
        // Odd grid size: parity is determined by inversions only
        currentParity = inversions % 2;
    } else {
        // Even grid size: parity depends on inversions AND blank position
        uint8_t blankPos = findBlankPos(grid);
        int blankRow = blankPos / _size;
        int blankRowFromBottom = _size - blankRow;
        
        currentParity = (inversions + blankRowFromBottom) % 2;
    }
    
    // Puzzle is solvable if current parity matches goal parity
    return currentParity == _goalParity;
}

int Puzzle::countInversions(const std::vector<uint8_t>& grid) const {
    // Count inversions relative to goal state
    // Create a mapping of tile value -> goal position
    std::vector<int> goalPosition(_size * _size);
    
    for (size_t i = 0; i < _goal.size(); i++) {
        if (_goal[i] != 0) {
            goalPosition[_goal[i]] = i;
        }
    }
    
    // Get puzzle tiles (excluding blank)
    std::vector<uint8_t> puzzleTiles;
    puzzleTiles.reserve(_size * _size - 1);
    
    for (size_t i = 0; i < grid.size(); i++) {
        if (grid[i] != 0) {
            puzzleTiles.push_back(grid[i]);
        }
    }
    
    // Count inversions based on goal positions
    int inversions = 0;
    for (size_t i = 0; i < puzzleTiles.size(); i++) {
        for (size_t j = i + 1; j < puzzleTiles.size(); j++) {
            if (goalPosition[puzzleTiles[i]] > goalPosition[puzzleTiles[j]]) {
                inversions++;
            }
        }
    }
    
    return inversions;
}

uint8_t Puzzle::findBlankPos(const std::vector<uint8_t>& grid) const {
    for (size_t i = 0; i < grid.size(); i++) {
        if (grid[i] == 0) {
            return static_cast<uint8_t>(i);
        }
    }
    return 0;
}

int Puzzle::calculateGoalParity() {
    // Calculate the parity of the goal state
    // Goal has 0 inversions (relative to itself) by definition
    int inversions = 0;
    
    if (_size % 2 == 1) {
        // Odd grid: parity from inversions only
        return inversions % 2;
    } else {
        // Even grid: parity from inversions + blank position
        uint8_t blankPos = findBlankPos(_goal);
        int blankRow = blankPos / _size;
        int blankRowFromBottom = _size - blankRow;
        return (inversions + blankRowFromBottom) % 2;
    }
}
