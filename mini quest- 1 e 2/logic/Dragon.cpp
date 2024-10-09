#include "Dragon.h"
#include "Map.h"
#include "Player.h"
#include <algorithm>
#include <random>
using namespace std;

namespace dkeep {
    namespace logic {

        Dragon::Dragon(int r, int c) : GameElement(r, c), isAlive(true), isBerserk(false), berserkTurnsLeft(0) {}

        void Dragon::setAlive(bool alive) {
            isAlive = alive;
        }

        bool Dragon::alive() const {
            return isAlive;
        }

        bool Dragon::getIsBerserk() const {
            return isBerserk;
        }

        void Dragon::setBerserk(bool berserk) {
            isBerserk = berserk;
            if (berserk) {
                berserkTurnsLeft = 3;
            }
        }

        void Dragon::decrementBerserkTurns() {
            if (isBerserk) {
                berserkTurnsLeft--;
                if (berserkTurnsLeft <= 0) {
                    isBerserk = false;
                }
            }
        }

        char Dragon::getSymbol() const {
            return isBerserk ? 'B' : 'D';
        }

        void Dragon::move(Map& map, const Player& player) {
            vector<pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };
            static random_device rd;
            static mt19937 g(rd());
            shuffle(directions.begin(), directions.end(), g);

            if (isBerserk) {
                uniform_int_distribution<> stepDist(2, 3);
                int steps = stepDist(g);

                for (int i = 0; i < steps; ++i) {
                    bool moved = false;
                    shuffle(directions.begin(), directions.end(), g);

                    for (const auto& dir : directions) {
                        int new_row = row + dir.first;
                        int new_col = col + dir.second;

                        if (map.isPositionValid(new_row, new_col) && map.getCell(new_row, new_col) != 'E') {
                            if (!(new_row == player.getRow() && new_col == player.getCol())) {
                                row = new_row;
                                col = new_col;
                                moved = true;
                                break;
                            }
                        }
                    }

                    if (!moved) {
                        continue;
                    }
                }
            }
            else {
                for (const auto& dir : directions) {
                    int new_row = row + dir.first;
                    int new_col = col + dir.second;

                    if (map.isPositionValid(new_row, new_col) && map.getCell(new_row, new_col) != 'E') {
                        if (!(new_row == player.getRow() && new_col == player.getCol())) {
                            row = new_row;
                            col = new_col;
                            break;
                        }
                    }
                }
            }
        }

    }
}
