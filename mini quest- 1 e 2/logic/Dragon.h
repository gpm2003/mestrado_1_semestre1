#pragma once
#include "GameElement.h"
using namespace std;

namespace dkeep {
    namespace logic {

        class Player; // Forward declaration
        class Map;    // Forward declaration

        class Dragon : public GameElement {
        private:
            bool isAlive;
            bool isBerserk;
            int berserkTurnsLeft;

        public:
            Dragon(int r, int c);
            void setAlive(bool alive);
            bool alive() const;

            bool getIsBerserk() const;
            void setBerserk(bool berserk);
            void decrementBerserkTurns();

            char getSymbol() const override;

            void move(Map& map, const Player& player);
        };

    }
}
