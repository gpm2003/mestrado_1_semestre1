#pragma once
#include <vector>
#include <memory>
#include "Dragon.h"
using namespace std;

namespace dkeep {
    namespace logic {

        class Player;

        class Map {
        private:
            vector<vector<char>> grid;
            vector<shared_ptr<Dragon>> dragons;
            int swordRow;
            int swordCol;
            shared_ptr<Dragon> berserkDragon;

        public:
            Map(const vector<vector<char>>& layout);

            char getCell(int r, int c) const;
            void setCell(int r, int c, char value);

            void addDragon(shared_ptr<Dragon> dragon);
            vector<shared_ptr<Dragon>>& getDragons();

            void show() const;

            bool isPositionValid(int r, int c) const;

            bool hasBerserkDragon() const;
            void setBerserkDragon(shared_ptr<Dragon> dragon);
            void clearBerserkDragon();

            void updateDragonPositions(const Player& player);

            void setSwordPosition(int r, int c);
            pair<int, int> getSwordPosition() const;

            int getGridRows() const;
            int getGridCols() const;
        };

    }
}
