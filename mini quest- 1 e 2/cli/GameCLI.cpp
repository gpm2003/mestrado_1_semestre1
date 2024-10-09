#include "cli/GameCLI.h"
#include "../logic/GameState.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <random>
using namespace std;

namespace dkeep {
    namespace cli {
        using namespace logic;

        void GameCLI::run() {
            // Seed the random number generator
            srand(static_cast<unsigned int>(time(nullptr)));

            // Define the initial grid layout
            vector<vector<char>> initialGrid = {
                {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
                {'X', 'H', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'X'},
                {'X', ' ', 'X', 'X', ' ', 'X', ' ', 'X', ' ', 'X'},
                {'X', ' ', 'X', 'X', ' ', 'X', ' ', 'X', ' ', 'X'},
                {'X', ' ', 'X', 'X', ' ', 'X', ' ', 'X', ' ', 'X'}, 
                {'X', ' ', ' ', ' ', ' ', ' ', ' ', 'X', ' ', 'E'},
                {'X', ' ', 'X', 'X', ' ', 'X', ' ', 'X', ' ', 'X'},
                {'X', ' ', 'X', 'X', ' ', 'X', ' ', 'X', ' ', 'X'},
                {'X', ' ', 'X', 'X', ' ', ' ', ' ', ' ', ' ', 'X'},
                {'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X', 'X'},
            };

            // Initialize the player position
            int playerRow = 1;
            int playerCol = 1;

            // Initialize GameState
            GameState gameState(initialGrid, playerRow, playerCol);

            // Determine maximum number of dragons based on available empty cells
            int totalEmptyCells = 0;
            for (int r = 0; r < initialGrid.size(); ++r) {
                for (int c = 0; c < initialGrid[0].size(); ++c) {
                    char cell = initialGrid[r][c];
                    if (cell == ' ') {
                        totalEmptyCells++;
                    }
                }
            }

            // Subtract positions occupied by player and exit
            totalEmptyCells -= 2; // Player and Exit

            // Ask the user for number of dragons
            int numDragons;
            cout << "Enter number of dragons: ";
            cin >> numDragons;

            // Ensure the number of dragons does not exceed available spaces
            if (numDragons > totalEmptyCells) {
                cout << "Maximum number of dragons exceeded. Setting number of dragons to " << totalEmptyCells << "." << endl;
                numDragons = totalEmptyCells;
            }

            // Initialize the game with dragons and sword
            gameState.initialize(numDragons);

            // Initial message
            cout << "\n\nYou just tapped your way through a damp corridor, following a draft until a sturdy shut door. "
                     << "The poorly lit doorway reveals a charred knob where a dim light protrudes through an obfuscated keyhole. "
                     << "You try to pry the door open, to no avail. There must be a way to unlock this door. You press onward "
                     << "along the faintly lit walkway, trying not to lose your bearings until heavy breathing halts you... "
                     << "Something or someone is alive in here...with you...the moment you take a turn, the sight before you has "
                     << "you gasping for air, as a surge of primal fear paralyses you...its...its a dragon. Although snuggled and "
                     << "asleep, nothing would prepare you for such an encounter, as his scales sparkly reflect the dimness of the "
                     << "ambient light, revealing his imposing body, almost blocking the entire passage. But the crackly snap of "
                     << "hopelessness hits you when you spot another shiny object, hanging from his neck: a key. Surely, to the door. "
                     << "Undoubtedly, such an indomitable creature would not surrender his key on a mere request. Maybe you can snatch "
                     << "the key without waking the foul beast..But you need something to cut the necklace that holds the key...you "
                     << "carefully trace back your steps, and begin burrowing and scavenging throughout the passageways, trying to find "
                     << "something that might have been left behind by some other lost adventurer. Suddenly, you notice another dim "
                     << "sparkling light from a pile of rubble...as you move closer, you feel a foul stench of rotten and putrid flesh... "
                     << "amidst the rubble, there is a corpse of a humanoid creature you cannot recognise. The smell is so bad, you "
                     << "barely dare to touch the remains, but the shiny light draws you to push the corpse aside...its...its....a sword! "
                     << "Praise the Gods! What a holy gift! Not even in your most wild expectations would you expect to find such a valuable "
                     << "item. The sword is sturdy, long and seems robust. Oddly enough it does not seem rusty at all...as if it had always "
                     << "been there for you to find it. Luckily, it appears to be quite sharp...cutting that key off the dragon should be sweet... "
                     << "Hopeful, you hustle back to where the dragon was, careful enough not to reveal your presence, and take a careful look over "
                     << "the corner at the sleeping....wait...where is he??!... Oh no!! IT'S AWAKE!!!...\n";
            cout << "Use (w, a, s, d) in the terminal to move.\n";

            // Main game loop
            while (true) {
                gameState.displayMap();

                cout << "\n\nUse (w, a, s, d) in the terminal to move: ";
                char movementDirection;
                cin >> movementDirection;

                // Move the player
                bool moved = gameState.movePlayer(movementDirection);
                if (!moved) {
                    continue; // Invalid move
                }

                // Move the dragons
                gameState.updateDragons();

                // Check for encounters
                gameState.checkEncounters();
            }
        }

    }
}
