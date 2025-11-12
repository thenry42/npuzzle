#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <vector>
#include <string>

class Display {
    public:
        Display();
        ~Display();
        Display(const Display& other);
        Display& operator=(const Display& other);

        int promptForGridSize();
        bool promptForUseCustomPuzzle();
        std::vector<std::vector<int>> promptForCustomPuzzle(int size);
        bool promptSolvableOrUnsolvable();
        int promptForHeuristic();
        double promptForWeight();
        int promptForIterations();
        bool displayRecap(int size, bool solvable, int iterations, 
                         const std::string& heuristicName, bool useCustom);
        void displayPuzzle(const std::vector<std::vector<int>>& grid);
        void displayPuzzleInline(const std::vector<std::vector<int>>& grid,
                                const std::vector<std::vector<int>>& goal);

    private:
        void printBox(const std::string& title, const std::string& content, const std::string& color);
        int countInversionsRelativeToGoal(const std::vector<int>& puzzle, const std::vector<int>& goal);
};

#endif

