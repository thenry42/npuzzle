#ifndef GAME_HPP
#define GAME_HPP

#include "Display.hpp"
#include "Astar.hpp"
#include "UCS.hpp"
#include "WeightedAstar.hpp"
#include "Greedy.hpp"
#include "BeamSearch.hpp"

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
        UCS _ucs;
        WeightedAstar _weightedAstar;
        Greedy _greedy;
        BeamSearch _beamSearch;
        
        // Default limits for all algorithms
        /*
            100mb = 1100000 states
            200mb = 2200000 states
            500mb = 5500000 states
            1GB = 11000000 states
            2GB = 22000000 states
        */
        static constexpr size_t DEFAULT_MAX_STATES = 10000000;   // 10 million states (~1GB)
        static constexpr double DEFAULT_MAX_TIME = 60;        // 1 minute
};

#endif
