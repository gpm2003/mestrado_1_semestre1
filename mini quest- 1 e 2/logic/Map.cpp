#include "Map.h"
#include "Player.h"
#include "Dragon.h"
#include <iostream>
#include <algorithm>
#include <random>
using namespace std;

namespace dkeep {
    namespace logic {

        Map::Map(const vector<vector<char>>& layout)
            : grid(layout), swordRow(-1), swordCol(-1), berserkDragon(nullptr) {}

        char Map::getCell(int r, int c) const {
            return grid[r][c];
        }

        void Map::setCell(int r, int c, char value) {
            grid[r][c] = value;
        }

        void Map::addDragon(shared_ptr<Dragon> dragon) {
            dragons.push_back(dragon);
        }

        vector<shared_ptr<Dragon>>& Map::getDragons() {
            return dragons;
        }

        void Map::show() const {
            for (const auto& rowLine : grid) {
                for (char tile : rowLine) {
                    cout << tile << " ";
                }
                cout << endl;
            }
        }

        bool Map::isPositionValid(int r, int c) const {
            return (r >= 0 && r < static_cast<int>(grid.size()) &&
                    c >= 0 && c < static_cast<int>(grid[0].size()) &&
                    grid[r][c] != 'X');
        }

        bool Map::hasBerserkDragon() const { 
            if (berserkDragon == nullptr) return false;
            return berserkDragon->getIsBerserk(); 
        }

        void Map::setBerserkDragon(shared_ptr<Dragon> dragon) { 
            berserkDragon = dragon; 
        }

        void Map::clearBerserkDragon() { 
            berserkDragon = nullptr; 
        }

        void Map::updateDragonPositions(const Player& player) {
            if (!hasBerserkDragon()) {
                // 35% chance to set a dragon to berserk
                vector<shared_ptr<Dragon>> shuffledDragons = dragons;
                random_device rd;
                mt19937 g(rd());
                shuffle(shuffledDragons.begin(), shuffledDragons.end(), g);

                uniform_real_distribution<> dis(0.0, 1.0);
                for (auto& dragon : shuffledDragons) {
                    if (!dragon->alive()) continue;
                    double chance = dis(g);
                    if (chance <= 0.35) { // 35% chance
                        dragon->setBerserk(true);
                        berserkDragon = dragon;
                        break;
                    }
                }
            }

            for (auto& dragon : dragons) {
                if (!dragon->alive()) continue;

                // Clear dragon's current position on map
                if (swordRow == dragon->getRow() && swordCol == dragon->getCol()) {
                    setCell(dragon->getRow(), dragon->getCol(), 'S');
                }
                else {
                    setCell(dragon->getRow(), dragon->getCol(), ' ');
                }

                // Move dragon
                dragon->move(*this, player);

                // Update dragon's new position on map
                if (dragon->getRow() == swordRow && dragon->getCol() == swordCol) {
                    if (dragon->getIsBerserk()) {
                        setCell(dragon->getRow(), dragon->getCol(), 'B');  
                    }
                    else {
                        setCell(dragon->getRow(), dragon->getCol(), 'F');  
                    }
                }
                else {
                    setCell(dragon->getRow(), dragon->getCol(), dragon->getSymbol());
                }

                // Handle berserk turns
                if (dragon->getIsBerserk()) {
                    dragon->decrementBerserkTurns();
                    if (!dragon->getIsBerserk()) {
                        if (berserkDragon == dragon) {
                            berserkDragon = nullptr;
                        }
                    }
                }
            }
        }

        void Map::setSwordPosition(int r, int c) {
            swordRow = r;
            swordCol = c;
        }

        pair<int, int> Map::getSwordPosition() const {
            return {swordRow, swordCol};
        }

        int Map::getGridRows() const {
            return grid.size();
        }

        int Map::getGridCols() const {
            return grid[0].size();
        }

    }
}
