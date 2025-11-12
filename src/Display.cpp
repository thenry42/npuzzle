#include "../includes/Display.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

// ANSI color codes
#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define MAGENTA "\033[35m"

Display::Display() {}

Display::~Display() {}

Display::Display(const Display& other) {
    (void)other;
}

Display& Display::operator=(const Display& other) {
    (void)other;
    return *this;
}

void Display::printBox(const std::string& title, const std::string& content, const std::string& color) {
    int width = 60;
    
    std::cout << "\n" << color;
    std::cout << "+" << std::string(width, '=') << "+\n";
    
    if (!title.empty()) {
        size_t titleLen = title.length();
        // Account for ANSI color codes in title - they don't take visual space
        // Simple heuristic: count escape sequences
        size_t escapeCount = 0;
        for (size_t i = 0; i < title.length(); i++) {
            if (title[i] == '\033') {
                escapeCount++;
                // Skip until 'm'
                while (i < title.length() && title[i] != 'm') i++;
            }
        }
        
        int visualLen = titleLen;
        int padding = (width - visualLen) / 2;
        if (padding < 0) padding = 0;
        
        int rightPadding = width - padding - visualLen;
        if (rightPadding < 0) rightPadding = 0;
        
        std::cout << "|" << std::string(padding, ' ') << title;
        std::cout << std::string(rightPadding, ' ') << color << "|\n";
        std::cout << "+" << std::string(width, '=') << "+\n";
    }
    
    if (!content.empty()) {
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
            // Count actual visual length (excluding ANSI codes)
            size_t visualLen = 0;
            bool inEscape = false;
            for (char c : line) {
                if (c == '\033') {
                    inEscape = true;
                } else if (inEscape && c == 'm') {
                    inEscape = false;
                } else if (!inEscape) {
                    visualLen++;
                }
            }
            
            int spacePadding = width - visualLen - 1;
            if (spacePadding < 0) spacePadding = 0;
            
            std::cout << "| " << line << std::string(spacePadding, ' ') << color << "|\n";
        }
    }
    
    std::cout << "+" << std::string(width, '=') << "+" << RESET << "\n\n";
}

int Display::promptForGridSize() {
    printBox("N-Puzzle Solver", "", CYAN);
    
    int size;
    while (true) {
        std::cout << BOLD << YELLOW << "Enter the grid size" << RESET 
                  << DIM << " (must be > 2 but more than 5 is insane)" << RESET << " [default: 3]: ";
        
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            size = 3;
        } else {
            std::istringstream iss(input);
            if (!(iss >> size)) {
                std::cout << BOLD << RED << "✗" << RESET << " Invalid input. Please enter a valid integer.\n";
                continue;
            }
        }
        
        if (size <= 2) {
            std::cout << BOLD << RED << "✗" << RESET 
                      << " Size must be strictly bigger than 2. You entered: " 
                      << RED << size << RESET << "\n";
            continue;
        }
        
        std::cout << BOLD << GREEN << "✓" << RESET 
                  << " Grid size set to: " << BOLD << CYAN << size << "x" << size << RESET << "\n\n";
        break;
    }
    
    return size;
}

bool Display::promptForUseCustomPuzzle() {
    std::string input;
    std::cout << BOLD << YELLOW << "Do you want to use a custom puzzle?" << RESET << " (y/n) [default: n]: ";
    std::getline(std::cin, input);
    
    if (input.empty()) return false;
    return (input[0] == 'y' || input[0] == 'Y');
}

std::vector<std::vector<int>> Display::promptForCustomPuzzle(int size) {
    std::cout << "\n" << BOLD << CYAN << "Custom Puzzle Input (" << size << "x" << size << ")" << RESET << "\n";
    std::cout << DIM << "Provide " << size * size << " numbers (0 to " << (size * size - 1) 
              << ") separated by spaces or commas" << RESET << "\n";
    std::cout << DIM << "Each number must appear exactly once" << RESET << "\n\n";
    
    while (true) {
        std::cout << BOLD << YELLOW << "Enter all " << size * size << " numbers" << RESET << ": ";
        
        std::string input;
        std::getline(std::cin, input);
        
        // Replace commas with spaces
        for (char& c : input) {
            if (c == ',') c = ' ';
        }
        
        // Parse numbers
        std::istringstream iss(input);
        std::vector<int> numbers;
        int num;
        while (iss >> num) {
            numbers.push_back(num);
        }
        
        // Validate count
        if (numbers.size() != static_cast<size_t>(size * size)) {
            std::cout << BOLD << RED << "✗" << RESET << " Expected " << size * size 
                      << " numbers, got " << numbers.size() << "\n";
            continue;
        }
        
        // Validate range and uniqueness
        std::vector<bool> seen(size * size, false);
        bool valid = true;
        
        for (int n : numbers) {
            if (n < 0 || n >= size * size) {
                std::cout << BOLD << RED << "✗" << RESET << " Invalid number: " << n << "\n";
                valid = false;
                break;
            }
            if (seen[n]) {
                std::cout << BOLD << RED << "✗" << RESET << " Duplicate number: " << n << "\n";
                valid = false;
                break;
            }
            seen[n] = true;
        }
        
        if (!valid) continue;
        
        // Convert to 2D grid
        std::vector<std::vector<int>> grid(size, std::vector<int>(size));
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                grid[i][j] = numbers[i * size + j];
            }
        }
        
        // Display for confirmation
        std::cout << "\n" << BOLD << GREEN << "✓" << RESET << " Puzzle entered successfully!\n";
        std::cout << "\n" << BOLD << CYAN << "Your puzzle:" << RESET << "\n";
        for (const auto& row : grid) {
            std::cout << "  ";
            for (int val : row) {
                if (val == 0) {
                    std::cout << " · ";
                } else {
                    std::cout << std::setw(2) << val << " ";
                }
            }
            std::cout << "\n";
        }
        
        // Confirm
        std::cout << "\n" << BOLD << YELLOW << "Is this correct?" << RESET << " (y/n) [default: y]: ";
        std::string confirm;
        std::getline(std::cin, confirm);
        
        if (confirm.empty() || confirm[0] == 'y' || confirm[0] == 'Y') {
            std::cout << BOLD << GREEN << "✓" << RESET << " Custom puzzle accepted!\n\n";
            return grid;
        } else {
            std::cout << YELLOW << "Let's try again..." << RESET << "\n\n";
        }
    }
}

bool Display::promptSolvableOrUnsolvable() {
    std::string input;
    std::cout << BOLD << YELLOW << "Do you want a solvable puzzle?" << RESET << " (y/n) [default: y]: ";
    std::getline(std::cin, input);
    
    bool solvable = (input.empty() || input[0] == 'y' || input[0] == 'Y');
    
    if (solvable) {
        std::cout << BOLD << GREEN << "✓" << RESET << " Generating a " 
                  << BOLD << CYAN << "solvable" << RESET << " puzzle\n\n";
    } else {
        std::cout << BOLD << GREEN << "✓" << RESET << " Generating an " 
                  << BOLD << CYAN << "unsolvable" << RESET << " puzzle\n\n";
    }
    
    return solvable;
}


int Display::promptForAlgorithm() {
    std::vector<std::string> options = {
        "A* (Optimal pathfinding with heuristic)",
        "UCS / Dijkstra (Optimal, uninformed search)",
        "Greedy Search (Fast, not optimal)",
        "Weighted A* (Configurable speed/optimality trade-off)",
        "Beam Search (Memory-efficient for large puzzles)",
    };
    
    std::cout << "\n" << BOLD << YELLOW << "Available Algorithms:" << RESET << "\n";
    for (size_t i = 0; i < options.size(); i++) {
        std::cout << "  " << CYAN << (i + 1) << RESET << ". " << options[i] << "\n";
    }
    
    int choice;
    while (true) {
        std::cout << "\n" << BOLD << YELLOW << "Enter your choice" << RESET << " [default: 1]: ";
        
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            choice = 1;
        } else {
            std::istringstream iss(input);
            if (!(iss >> choice)) {
                std::cout << BOLD << RED << "✗" << RESET << " Invalid input. Please enter a valid number.\n";
                continue;
            }
        }
        
        if (choice < 1 || choice > static_cast<int>(options.size())) {
            std::cout << BOLD << RED << "✗" << RESET << " Invalid choice. Please select a number between 1 and " 
                     << options.size() << ".\n";
            continue;
        }
        
        std::cout << BOLD << GREEN << "✓" << RESET << " Using algorithm: " 
                  << BOLD << CYAN << options[choice - 1] << RESET << "\n";
        break;
    }
    
    return choice;  // Returns 1=A*, 2=UCS, 3=Greedy, 4=Weighted A*, 5=Beam Search
}

int Display::promptForHeuristic(int algorithm) {
    std::vector<std::string> options;
    
    // Different heuristics available depending on the algorithm
    if (algorithm == 1) {
        // A* - all heuristics available
        options = {
            "Manhattan Distance",
            "Hamming Distance",
            "Linear Conflict (Best overall)",
        };
        std::cout << "\n" << BOLD << YELLOW << "Select Heuristic for A*:" << RESET << "\n";
    } else if (algorithm == 2) {
        // UCS - no heuristic needed (uses only g(n))
        std::cout << "\n" << BOLD << CYAN << "ℹ  UCS doesn't use a heuristic (h(n) = 0)" << RESET << "\n";
        return 0;  // Return 0 to indicate no heuristic
    } else if (algorithm == 3) {
        // Greedy Search - uses heuristics
        options = {
            "Manhattan Distance",
            "Hamming Distance",
            "Linear Conflict (Best overall)",
        };
        std::cout << "\n" << BOLD << YELLOW << "Select Heuristic for Greedy Search:" << RESET << "\n";
    } else if (algorithm == 4) {
        // Weighted A* - uses heuristics
        options = {
            "Manhattan Distance",
            "Hamming Distance",
            "Linear Conflict (Best overall)",
        };
        std::cout << "\n" << BOLD << YELLOW << "Select Heuristic for Weighted A*:" << RESET << "\n";
    } else if (algorithm == 5) {
        // Beam Search - uses heuristics
        options = {
            "Manhattan Distance",
            "Hamming Distance",
            "Linear Conflict (Best overall)",
        };
        std::cout << "\n" << BOLD << YELLOW << "Select Heuristic for Beam Search:" << RESET << "\n";
    } else {
        // Default fallback
        options = {"Manhattan Distance"};
    }
    
    for (size_t i = 0; i < options.size(); i++) {
        std::cout << "  " << CYAN << (i + 1) << RESET << ". " << options[i] << "\n";
    }
    
    int choice;
    while (true) {
        std::cout << "\n" << BOLD << YELLOW << "Enter your choice" << RESET << " [default: 1]: ";
        
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            choice = 1;
        } else {
            std::istringstream iss(input);
            if (!(iss >> choice)) {
                std::cout << BOLD << RED << "✗" << RESET << " Invalid input. Please enter a valid number.\n";
                continue;
            }
        }
        
        if (choice < 1 || choice > static_cast<int>(options.size())) {
            std::cout << BOLD << RED << "✗" << RESET << " Invalid choice. Please select a number between 1 and " 
                     << options.size() << ".\n";
            continue;
        }
        
        std::cout << BOLD << GREEN << "✓" << RESET << " Using heuristic: " 
                  << BOLD << CYAN << options[choice - 1] << RESET << "\n";
        break;
    }
    
    return choice;  // Returns 1-3 depending on algorithm and choice
}

double Display::promptForWeight() {
    std::cout << "\n" << BOLD << YELLOW << "Weighted A* Configuration:" << RESET << "\n";
    std::cout << "  " << CYAN << "Weight controls the balance between optimality and speed" << RESET << "\n";
    std::cout << "  • weight = 1.0: Standard A* (optimal, slower)\n";
    std::cout << "  • weight > 1.0: Faster but suboptimal (e.g., 1.5, 2.0, 5.0)\n";
    
    double weight;
    while (true) {
        std::cout << "\n" << BOLD << YELLOW << "Enter weight" << RESET << " [default: 1.5]: ";
        
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            weight = 1.5;
        } else {
            std::istringstream iss(input);
            if (!(iss >> weight)) {
                std::cout << BOLD << RED << "✗" << RESET << " Invalid input. Please enter a valid number.\n";
                continue;
            }
        }
        
        if (weight < 0.1 || weight > 10.0) {
            std::cout << BOLD << RED << "✗" << RESET << " Weight should be between 0.1 and 10.0\n";
            continue;
        }
        
        std::cout << BOLD << GREEN << "✓" << RESET << " Using weight: " 
                  << BOLD << CYAN << weight << RESET << "\n";
        break;
    }
    
    return weight;
}

int Display::promptForBeamWidth() {
    std::cout << "\n" << BOLD << YELLOW << "Beam Search Configuration:" << RESET << "\n";
    std::cout << "  " << CYAN << "Beam width (k) controls the number of best states kept at each level" << RESET << "\n";
    std::cout << "  • Smaller k (e.g., 10-50): Faster but may miss solution\n";
    std::cout << "  • Larger k (e.g., 100-1000): More thorough but slower\n";
    
    int beamWidth;
    while (true) {
        std::cout << "\n" << BOLD << YELLOW << "Enter beam width (k)" << RESET << " [default: 100]: ";
        
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            beamWidth = 100;
        } else {
            std::istringstream iss(input);
            if (!(iss >> beamWidth)) {
                std::cout << BOLD << RED << "✗" << RESET << " Invalid input. Please enter a valid number.\n";
                continue;
            }
        }
        
        if (beamWidth < 1 || beamWidth > 10000) {
            std::cout << BOLD << RED << "✗" << RESET << " Beam width should be between 1 and 10000\n";
            continue;
        }
        
        std::cout << BOLD << GREEN << "✓" << RESET << " Using beam width: " 
                  << BOLD << CYAN << beamWidth << RESET << "\n";
        break;
    }
    
    return beamWidth;
}

int Display::promptForIterations() {
    // Size-based defaults for reasonable difficulty
    int defaultIterations = 10000;
    int iterations = defaultIterations;
    
    while (true) {
        std::cout << BOLD << YELLOW << "Enter the number of iterations for the shuffle" << RESET 
                  << " [default: " << defaultIterations << "]: ";
        
        std::string input;
        std::getline(std::cin, input);
        
        if (input.empty()) {
            iterations = defaultIterations;
            break;
        }
        
        std::istringstream iss(input);
        if (!(iss >> iterations)) {
            std::cout << BOLD << RED << "✗" << RESET << " Invalid input. Please enter a valid number.\n";
            continue;
        }
        
        if (iterations < 0) {
            std::cout << BOLD << RED << "✗" << RESET << " Iterations must be non-negative.\n";
            continue;
        }
        
        break;
    }
    
    return iterations;
}

bool Display::displayRecap(int size, bool solvable, int iterations,
                          const std::string& algorithmDescription, bool useCustom) {
    std::ostringstream content;
    content << BOLD << CYAN << "Recap of your choices" << RESET << "\n";
    content << BOLD << GREEN << "✓" << RESET << " Grid size: " << BOLD << CYAN << size << "x" << size << RESET;
    
    if (useCustom) {
        content << "\n" << BOLD << GREEN << "✓" << RESET << " Puzzle type: " << BOLD << CYAN << "Custom puzzle" << RESET;
    } else {
        content << "\n" << BOLD << GREEN << "✓" << RESET << " Puzzle type: " << BOLD << CYAN << "Generated" << RESET;
        content << "\n" << BOLD << GREEN << "✓" << RESET << " Solvable: " << BOLD << CYAN << (solvable ? "Yes" : "No") << RESET;
        content << "\n" << BOLD << GREEN << "✓" << RESET << " Iterations: " << BOLD << CYAN << iterations << RESET;
    }
    
    content << "\n" << BOLD << GREEN << "✓" << RESET << " Algorithm: " << BOLD << CYAN << algorithmDescription << RESET;
    
    printBox("", content.str(), CYAN);
    
    std::cout << BOLD << YELLOW << "Start the solver?" << RESET << " (y/n) [default: y]: ";
    std::string input;
    std::getline(std::cin, input);
    
    return (input.empty() || input[0] == 'y' || input[0] == 'Y');
}

void Display::displayPuzzle(const std::vector<std::vector<int>>& grid) {
    int size = grid.size();
    std::ostringstream content;
    
    for (int i = 0; i < size; i++) {
        content << "  ";
        for (int j = 0; j < size; j++) {
            if (grid[i][j] == 0) {
                content << DIM << "  · " << RESET;
            } else {
                content << BOLD << CYAN << std::setw(3) << grid[i][j] << RESET << " ";
            }
        }
        content << "\n";
    }
    
    printBox("Generated Puzzle", content.str(), CYAN);
}

void Display::displayPuzzleInline(const std::vector<std::vector<int>>& grid,
                                 const std::vector<std::vector<int>>& goal) {
    int size = grid.size();
    
    // Flatten grids
    std::vector<int> flatPuzzle, flatGoal;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            flatPuzzle.push_back(grid[i][j]);
            flatGoal.push_back(goal[i][j]);
        }
    }
    
    // Count inversions
    int inversions = countInversionsRelativeToGoal(flatPuzzle, flatGoal);
    
    // Find blank positions
    int blankRow = 0, goalBlankRow = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (grid[i][j] == 0) blankRow = i;
            if (goal[i][j] == 0) goalBlankRow = i;
        }
    }
    
    int blankRowFromBottom = size - blankRow;
    int goalBlankRowFromBottom = size - goalBlankRow;
    
    // Calculate parities
    int currentParity, goalParity;
    if (size % 2 == 1) {
        currentParity = inversions % 2;
        goalParity = 0;  // goal has 0 inversions
    } else {
        currentParity = (inversions + blankRowFromBottom) % 2;
        goalParity = goalBlankRowFromBottom % 2;
    }
    
    bool isSolvable = (currentParity == goalParity);
    
    // Display
    std::ostringstream content;
    content << "Goal:   [";
    for (size_t i = 0; i < flatGoal.size(); i++) {
        content << " " << BOLD << CYAN << std::setw(2) << flatGoal[i] << RESET;
        if (i < flatGoal.size() - 1) content << ",";
    }
    content << " ]\n\nPuzzle: [";
    for (size_t i = 0; i < flatPuzzle.size(); i++) {
        content << " " << BOLD << YELLOW << std::setw(2) << flatPuzzle[i] << RESET;
        if (i < flatPuzzle.size() - 1) content << ",";
    }
    content << " ]\n\n";
    content << "Inversions (relative to goal): " << BOLD << MAGENTA << inversions << RESET;
    content << " (" << (inversions % 2 == 0 ? GREEN "even" RESET : RED "odd" RESET) << ")\n";
    
    if (size % 2 == 1) {
        content << "Current parity: " << BOLD << MAGENTA << currentParity << RESET << "\n";
        content << "Goal parity: " << BOLD << CYAN << goalParity << RESET << "\n";
        content << "Solvability (odd grid): parities must match";
    } else {
        content << "Blank row from bottom: " << BOLD << CYAN << blankRowFromBottom << RESET << "\n";
        content << "Current parity: " << BOLD << MAGENTA << currentParity << RESET << "\n";
        content << "Goal parity: " << BOLD << CYAN << goalParity << RESET << "\n";
        content << "Solvability (even grid): parities must match";
    }
    
    printBox("Solvability Check (Inline View)", content.str(), CYAN);
    
    if (isSolvable) {
        printBox("", BOLD GREEN "✓ SOLVABLE" RESET, GREEN);
    } else {
        printBox("", BOLD RED "✗ UNSOLVABLE" RESET, RED);
    }
}

int Display::countInversionsRelativeToGoal(const std::vector<int>& puzzle, const std::vector<int>& goal) {
    // Create goal position mapping
    std::vector<int> goalPosition(goal.size());
    for (size_t i = 0; i < goal.size(); i++) {
        if (goal[i] != 0) {
            goalPosition[goal[i]] = i;
        }
    }
    
    // Get puzzle tiles (excluding blank)
    std::vector<int> puzzleTiles;
    for (int tile : puzzle) {
        if (tile != 0) {
            puzzleTiles.push_back(tile);
        }
    }
    
    // Count inversions
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

