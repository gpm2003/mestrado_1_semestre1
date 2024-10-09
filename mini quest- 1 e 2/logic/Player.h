#pragma once
#include "GameElement.h"
using namespace std;

namespace dkeep {
    namespace logic {

        class Map; // Forward declaration

        class Player : public GameElement {
        private:
            bool armed;

        public:
            Player(int r, int c);
            void pickSword();
            bool isArmed() const;
            char getSymbol() const override;

            bool move(char direction, Map& map);
        };

    }
}
