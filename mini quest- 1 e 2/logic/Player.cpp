#include "Player.h"
#include "Map.h"
#include "Dragon.h" // Needed for interactions with dragons
#include <iostream>
#include <algorithm>
using namespace std;

namespace dkeep {
    namespace logic {

        Player::Player(int r, int c) : GameElement(r, c), armed(false) {}

        void Player::pickSword() {
            armed = true;
        }

        bool Player::isArmed() const {
            return armed;
        }

        char Player::getSymbol() const {
            return armed ? 'A' : 'H';
        }

        bool Player::move(char direction, Map& map) {
            int new_row = row;
            int new_col = col;

            switch (direction) {
                case 'w': new_row--; break;  // Up
                case 's': new_row++; break;  // Down
                case 'a': new_col--; break;  // Left
                case 'd': new_col++; break;  // Right
                default:
                    cerr << "Invalid direction! Use 'w', 'a', 's', 'd'." << endl;
                    return false;
            }

            if (!map.isPositionValid(new_row, new_col)) {
                cerr << "There's a wall. You can't move there!" << endl;
                return false;
            }

            char tile = map.getCell(new_row, new_col);

            if (tile == 'S') {
                pickSword();
                cout << "You picked up the sword! You are now armed." << endl;
                map.setCell(new_row, new_col, ' ');  // Remove the sword from the map
            }
            else if (tile == 'D' || tile == 'F' || tile == 'B') {
                for (auto& dragon : map.getDragons()) {
                    if (dragon->getRow() == new_row && dragon->getCol() == new_col && dragon->alive()) {
                        if (isArmed()) {
                            dragon->setAlive(false);  // Dragon dies
                            cout << "Dragon killed!" << endl;
                            map.setCell(new_row, new_col, ' ');
                        } else {
                            cout << "Game over! You were slain by the dragon." << endl;
                            exit(0);  // Player dies
                        }
                        break;
                    }
                }
            }
            else if (tile == 'E') {  // Exit
                bool allDragonsSlain = all_of(map.getDragons().begin(), map.getDragons().end(),
                                                 [](const shared_ptr<Dragon>& d) { return !d->alive(); });
                if (allDragonsSlain) {
                    cout << "You have slain all dragons and escaped! You win!" << endl;
                    exit(0);
                } else {
                    cout << "The exit is locked. You must slay all dragons first!" << endl;
                    return false;
                }
            }

            // Update player's position on the map
            map.setCell(row, col, ' ');  // Clear old position
            row = new_row;
            col = new_col;
            map.setCell(row, col, getSymbol());

            return true;
        }

    }
}
