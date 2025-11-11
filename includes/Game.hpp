#ifndef GAME_HPP
#define GAME_HPP

#include "Display.hpp"
#include "Astar.hpp"

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
};

#endif
