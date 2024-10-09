#include "GameState.h"
#include "Dragon.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <cstdlib>
using namespace std;

namespace dkeep {
    namespace logic {

        GameState::GameState(const vector<vector<char>>& layout, int playerRow, int playerCol)
            : gameMap(layout), player(playerRow, playerCol), gameOver(false) {}

        void GameState::initialize(int numDragons) {
            placeDragons(numDragons);
            placeSword();
            gameMap.setCell(player.getRow(), player.getCol(), player.getSymbol());
        }

        // Helper function to place dragons
        void GameState::placeDragons(int numDragons) {
            vector<pair<int, int>> emptyCells;

            for (int r = 0; r < gameMap.getGridRows(); ++r) {
                for (int c = 0; c < gameMap.getGridCols(); ++c) {
                    char cell = gameMap.getCell(r, c);
                    if (cell == ' ' && !(r == player.getRow() && c == player.getCol()) && cell != 'E') {
                        emptyCells.emplace_back(r, c);
                    }
                }
            }

            if (numDragons > static_cast<int>(emptyCells.size())) {
                cerr << "Not enough empty spaces to place all dragons!" << endl;
                numDragons = emptyCells.size();
                cout << "Number of dragons set to " << numDragons << "." << endl;
            }

            random_device rd;
            mt19937 g(rd());
            shuffle(emptyCells.begin(), emptyCells.end(), g);

            for (int i = 0; i < numDragons; ++i) {
                int r = emptyCells[i].first;
                int c = emptyCells[i].second;
                shared_ptr<Dragon> dragon = make_shared<Dragon>(r, c);
                gameMap.addDragon(dragon);
                gameMap.setCell(r, c, dragon->getSymbol());
            }
        }

        // Helper function to place the sword
        void GameState::placeSword() {
            vector<pair<int, int>> emptyCells;

            for (int r = 0; r < gameMap.getGridRows(); ++r) {
                for (int c = 0; c < gameMap.getGridCols(); ++c) {
                    char cell = gameMap.getCell(r, c);
                    if (cell == ' ' && !(r == player.getRow() && c == player.getCol()) && cell != 'E') {
                        emptyCells.emplace_back(r, c);
                    }
                }
            }

            if (emptyCells.empty()) {
                cerr << "No empty space to place the sword!" << endl;
                return;
            }

            random_device rd;
            mt19937 g(rd());
            shuffle(emptyCells.begin(), emptyCells.end(), g);

            int swordRow = emptyCells[0].first;
            int swordCol = emptyCells[0].second;
            gameMap.setCell(swordRow, swordCol, 'S');
            gameMap.setSwordPosition(swordRow, swordCol);
        }

        bool GameState::movePlayer(char direction) {
            bool moved = player.move(direction, gameMap);
            return moved;
        }

        void GameState::updateDragons() {
            gameMap.updateDragonPositions(player);
        }

        void GameState::checkEncounters() {
            for (auto& dragon : gameMap.getDragons()) {
                if (!dragon->alive()) continue;

                int dr = abs(dragon->getRow() - player.getRow());
                int dc = abs(dragon->getCol() - player.getCol());
                if ((dr + dc) == 1) { // Close encounter
                    if (player.isArmed()) {
                        dragon->setAlive(false);
                        cout << "You slayed a dragon. Congrats!" << endl;
                        gameMap.setCell(dragon->getRow(), dragon->getCol(), ' ');
                    }
                    else {
                        cout << "Game over! You were slain by the dragon." << endl;
                        exit(0);
                    }
                }
            }
        }

        void GameState::displayMap() const {
            gameMap.show();
        }

    }
}
