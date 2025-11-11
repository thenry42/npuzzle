#ifndef PUZZLE_HPP
#define PUZZLE_HPP

#include <vector>
#include <cstdint>

class Puzzle {
    public:
        Puzzle(int size, bool solvable, int iterations);
        ~Puzzle();
        Puzzle(const Puzzle& other);
        Puzzle& operator=(const Puzzle& other);

        void generatePuzzle();
        const std::vector<uint8_t>& getGrid() const;
        void setGrid(const std::vector<uint8_t>& grid);
        void setGridFrom2D(const std::vector<std::vector<int>>& grid);  // For compatibility
        const std::vector<uint8_t>& getGoal() const;
        std::vector<std::vector<int>> getGrid2D() const;  // For display compatibility
        std::vector<std::vector<int>> getGoal2D() const;  // For display compatibility
        int getSize() const;
        bool isSolvable() const;

    private:
        int _size;
        bool _solvable;
        int _iterations;
        std::vector<uint8_t> _grid;   // Flat array representation
        std::vector<uint8_t> _goal;   // Flat array representation
        int _goalParity;

        bool isSolvableCheck(const std::vector<uint8_t>& grid) const;
        int countInversions(const std::vector<uint8_t>& grid) const;
        uint8_t findBlankPos(const std::vector<uint8_t>& grid) const;
        int calculateGoalParity();
        void generateSolvablePuzzle();
        void generateUnsolvablePuzzle();
        void swapEmpty(std::vector<uint8_t>& puzzle);
        void generateGoalSnail();
};

#endif