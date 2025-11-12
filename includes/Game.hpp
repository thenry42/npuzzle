#ifndef GAME_HPP
#define GAME_HPP

#include "Display.hpp"
#include "Astar.hpp"
#include "WeightedAstar.hpp"
#include "Greedy.hpp"

class Game {
    public:
        Game();
        ~Game();
        Game(const Game& other);
        Game& operator=(const Game& other);

        void gameLoop();
    
    private:
        Display _display;
        Astar _astar;
        WeightedAstar _weightedAstar;
        Greedy _greedy;
        
        // Default limits for all algorithms
        static constexpr size_t DEFAULT_MAX_STATES = 1000000;   // 1 million states (~90 MB)
        static constexpr double DEFAULT_MAX_TIME = 60;        // 1 minute
};

#endif
