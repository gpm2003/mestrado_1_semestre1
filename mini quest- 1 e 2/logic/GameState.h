#pragma once
#include "Map.h"
#include "Player.h"
using namespace std;

namespace dkeep {
    namespace logic {

        class GameState {
        private:
            Map gameMap;
            Player player;
            bool gameOver;

            // Helper functions
            void placeDragons(int numDragons);
            void placeSword();

        public:
            GameState(const vector<vector<char>>& layout, int playerRow, int playerCol);

            void initialize(int numDragons);

            bool movePlayer(char direction);
            void updateDragons();
            void checkEncounters();

            void displayMap() const;
        };

    }
}
