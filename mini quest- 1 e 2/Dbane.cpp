#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <algorithm>
#include <random>



using namespace std;

//i need to declarate this first so i can use it in other classes
class Map;

//base class for all elements
class GameElement {
protected:
    int row;
    int col;

public:
    GameElement(int r, int c) : row(r), col(c) {}
    virtual ~GameElement() = default;

    int getRow() const { return row; }
    int getCol() const { return col; }

    virtual char getSymbol() const = 0;
};

// player
class Player : public GameElement {
private:
    bool armed = false;

public:
    Player(int r, int c) : GameElement(r, c) {}

    void pickSword() {
        armed = true;
    }

    bool isArmed() const {
        return armed;
    }

    char getSymbol() const override {
        return armed ? 'A' : 'H';
    }

    //move function
    bool move(char direction, class Map& map);
};

//dragon
class Dragon : public GameElement {
private:
    //state of dragon
    bool isAlive = true;
    bool isBerserk = false;
    int berserkTurnsLeft = 0;

public:
    Dragon(int r, int c) : GameElement(r, c) {}

    void setAlive(bool alive) { isAlive = alive; }
    bool alive() const { return isAlive; }

    bool getIsBerserk() const { return isBerserk; }
    //gives a maximum turns for the berserk
    void setBerserk(bool berserk) { 
        isBerserk = berserk; 
        if (berserk) berserkTurnsLeft = 3;
    }

    void decrementBerserkTurns() { 
        if (isBerserk) {
            berserkTurnsLeft--;
            if (berserkTurnsLeft <= 0) {
                isBerserk = false;
            }
        }
    }

    char getSymbol() const override {
        return isBerserk ? 'B' : 'D';
    }

    //move dragons 
    void move(class Map& map, const Player& player);
};

//map
class Map {
private:
    vector<vector<char>> grid;
    vector<shared_ptr<Dragon>> dragons;
    //sets the sword in an invalid position so it can spawn randomly
    int swordRow = -1;  
    int swordCol = -1;
    shared_ptr<Dragon> berserkDragon = nullptr;

public:
    Map(const vector<vector<char>>& layout)
        : grid(layout) {}

    char getCell(int r, int c) const {
        return grid[r][c];
    }

    void setCell(int r, int c, char value) {
        grid[r][c] = value;
    }

    void addDragon(shared_ptr<Dragon> dragon) {
        dragons.push_back(dragon);
    }

    vector<shared_ptr<Dragon>>& getDragons() {
        return dragons;
    }

    void show() const {
        for (const auto& rowLine : grid) {
            for (char tile : rowLine) {
                cout << tile << " ";
            }
            cout << endl;
        }
    }
    //checks if the position is an empty tile 
    bool isPositionValid(int r, int c) const {
        return (r >= 0 && r < static_cast<int>(grid.size()) &&
                c >= 0 && c < static_cast<int>(grid[0].size()) &&
                grid[r][c] != 'X');
    }
    //checks if there is a berserk dragon
    bool hasBerserkDragon() const { 
        if (berserkDragon == nullptr) return false;
        return berserkDragon->getIsBerserk(); 
    }

    void setBerserkDragon(shared_ptr<Dragon> dragon) { 
        berserkDragon = dragon; 
    }
    //removes the berserk from the dragon
    void clearBerserkDragon() { 
        berserkDragon = nullptr; 
    }

    //updates the position of the dragons based on the player and the sword
    void updateDragonPositions(const Player& player) {
        //checks if there is already a berserk dragon
        if (!hasBerserkDragon()) {
            //35% chance to set a dragon to berserk
            //Only one dragon can be berserk at a time
            //Iterate through dragons and set the first one that succeeds
            vector<shared_ptr<Dragon>> shuffledDragons = dragons;
            random_device rd;
            mt19937 g(rd());
            shuffle(shuffledDragons.begin(), shuffledDragons.end(), g);

            uniform_real_distribution<> dis(0.0, 1.0);
            for (auto& dragon : shuffledDragons) {
                if (!dragon->alive()) continue;
                double chance = dis(g);
                if (chance <= 0.35) {  // 35% chance
                    dragon->setBerserk(true);
                    berserkDragon = dragon;
                    break;
                }
            }
        }

        //moves the dragons if they are alive
        for (auto& dragon : dragons) {
            if (!dragon->alive()) continue;

            //clear the dragons that are dead
            //changes the sword to S if there was a dragon above it
            if (swordRow == dragon->getRow() && swordCol == dragon->getCol()) {
                setCell(dragon->getRow(), dragon->getCol(), 'S');
            } else {
                setCell(dragon->getRow(), dragon->getCol(), ' ');
            }

            //move the dragon
            dragon->move(*this, player);

            //checks if the dragon is on the sword if he is change to F and checks if the dragon is on berserk if he is change to B
            if (dragon->getRow() == swordRow && dragon->getCol() == swordCol) {
                if (dragon->getIsBerserk()) {
                    setCell(dragon->getRow(), dragon->getCol(), 'B');  
                } else {
                    setCell(dragon->getRow(), dragon->getCol(), 'F');  
                }
            } else {
                //change the symbol based on the condition of the dragon
                setCell(dragon->getRow(), dragon->getCol(), dragon->getSymbol());
            }

            //affect the dragon if he is in berserk
            if (dragon->getIsBerserk()) {
                dragon->decrementBerserkTurns();
                if (!dragon->getIsBerserk()) {
                    // Berserk ended
                    if (berserkDragon == dragon) {
                        berserkDragon = nullptr;
                    }
                }
            }
        }
    }

    //position of the sword
    void setSwordPosition(int r, int c) {
        swordRow = r;
        swordCol = c;
    }
    pair<int, int> getSwordPosition() const {
        return {swordRow, swordCol};
    }
    
    //getters for grid size, just to simplify the code
    int getGridRows() const {
        return grid.size();
    }

    int getGridCols() const {
        return grid[0].size();
    }
};

//player move func
bool Player::move(char direction, Map& map) {
    int new_row = row;
    int new_col = col;

    //changes the position based on the pressed key
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

    //gets the position of the player
    char tile = map.getCell(new_row, new_col);

    //clear the sword tile after the hero picked the sword
    if (tile == 'S') { 
        pickSword();
        cout << "You picked up the sword! You are now armed." << endl;
        map.setCell(new_row, new_col, ' ');  // Remove the sword from the map
    }
    //if the hero has a sword he can kill if not he dies
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
        //the hero can escape if all the dragons were slayn
        bool allDragonsSlain = all_of(map.getDragons().begin(), map.getDragons().end(), [](const shared_ptr<Dragon>& d) { return !d->alive(); });
        if (allDragonsSlain) {
            cout << "You have slain all dragons and escaped! You win!" << endl;
            exit(0);
        } else {
            cout << "The exit is locked. You must slay all dragons first!" << endl;
            return false;
        }
    }

    //update player's position on the map
    map.setCell(row, col, ' ');
    row = new_row;
    col = new_col;
    map.setCell(row, col, getSymbol());

    return true;
}

//dragons move func
void Dragon::move(Map& map, const Player& player) {
    //up, down, left, right
    vector<pair<int, int>> directions = { {-1,0}, {1,0}, {0,-1}, {0,1} };

    //random number generator for the shuffle
    static random_device rd;
    static mt19937 g(rd());

    //shuffle the array of the possible directions to move the dragon randomly
    shuffle(directions.begin(), directions.end(), g);

    if (isBerserk) {
        //if the dragon is on berserk he can move 2 to 3 tiles 
        uniform_int_distribution<> stepDist(2, 3);
        int steps = stepDist(g);

        for (int i = 0; i < steps; ++i) {
            bool moved = false;
            //shuffle directions again for each step
            shuffle(directions.begin(), directions.end(), g);

            for (const auto& dir : directions) {
                int new_row = row + dir.first;
                int new_col = col + dir.second;

                //the dragon cant go through exit 
                if (map.isPositionValid(new_row, new_col) && map.getCell(new_row, new_col) != 'E') {
                    //check if the new position is not occupied by the player
                    if (!(new_row == player.getRow() && new_col == player.getCol())) {
                        //move to the new position
                        row = new_row;
                        col = new_col;
                        moved = true;
                        break;
                    }
                }
            }

            if (!moved) {
                //if the hero didnt move the dragon cant move so if it doesnt move it just skip this
                continue;
            }
        }
    }
    else {
        //normal movement: move 1 step randomly (without berserk)
        for (const auto& dir : directions) {
            int new_row = row + dir.first;
            int new_col = col + dir.second;

            //prevent dragons from moving onto the exit
            if (map.isPositionValid(new_row, new_col) && map.getCell(new_row, new_col) != 'E') {
                // Check if the new position is not occupied by the player
                if (!(new_row == player.getRow() && new_col == player.getCol())) {
                    // Move to the new position
                    row = new_row;
                    col = new_col;
                    break;
                }
            }
        }
    }
}

//function to place a random number of dragons randomly in the map
void placeDragons(Map& map, int numDragons, const Player& player) {
    vector<pair<int, int>> emptyCells;

    //gets all the empty cells (' ') excluding player and exit positions, to spawn the dragons
    for (int r = 0; r < map.getGridRows(); ++r) {
        for (int c = 0; c < map.getGridCols(); ++c) {
            char cell = map.getCell(r, c);
            if (cell == ' ' && !(r == player.getRow() && c == player.getCol()) && cell != 'E') {
                emptyCells.emplace_back(r, c);
            }
        }
    }

    //check if there are enough empty cells to place dragons 
    if (numDragons > static_cast<int>(emptyCells.size())) {
        cerr << "Not enough empty spaces to place all dragons!" << endl;
        numDragons = emptyCells.size();
        cout << "Number of dragons set to " << numDragons << "." << endl;
    }

    //shuffle the empty cells to place the dragons
    random_device rd;
    mt19937 g(rd());
    shuffle(emptyCells.begin(), emptyCells.end(), g);

    //place the dragons
    for (int i = 0; i < numDragons; ++i) {
        int r = emptyCells[i].first;
        int c = emptyCells[i].second;
        shared_ptr<Dragon> dragon = make_shared<Dragon>(r, c);
        map.addDragon(dragon);
        map.setCell(r, c, dragon->getSymbol());
    }
}

//place the sword randomly on the map
void placeSword(Map& map, const Player& player) {
    vector<pair<int, int>> emptyCells;

    //find all the empty cells to place the sword
    for (int r = 0; r < map.getGridRows(); ++r) {
        for (int c = 0; c < map.getGridCols(); ++c) {
            char cell = map.getCell(r, c);
            if (cell == ' ' && !(r == player.getRow() && c == player.getCol()) && cell != 'E') {
                emptyCells.emplace_back(r, c);  // Store the row and column of the empty cell
            }
        }
    }

    //mame sure there are spaces for the sword
    if (emptyCells.empty()) {
        cerr << "No empty space to place the sword!" << endl;
        return;
    }

    //randomly place the sword
    random_device rd;
    mt19937 g(rd());
    shuffle(emptyCells.begin(), emptyCells.end(), g);

    //place the sword on the first empty cell found
    int swordRow = emptyCells[0].first;
    int swordCol = emptyCells[0].second;
    map.setCell(swordRow, swordCol, 'S');

    //set the sword on the map
    map.setSwordPosition(swordRow, swordCol);
}

int main() {
    //seed the random number generator
    srand(static_cast<unsigned int>(time(nullptr)));

    //grid for the map
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

    //initialize the player in the position 1,1 on the map
    Player player(1, 1);

    //initialize map
    Map gameMap(initialGrid);

    //determine maximum number of dragons based on available empty cells, if there are more dragons than empty cells it cant place the dragon
    int totalEmptyCells = 0;
    for (int r = 0; r < gameMap.getGridRows(); ++r) {
        for (int c = 0; c < gameMap.getGridCols(); ++c) {
            char cell = gameMap.getCell(r, c);
            if (cell == ' ') {
                totalEmptyCells++;
            }
        }
    }

    //subtract positions occupied by player and exit
    totalEmptyCells -= 2; // Player and Exit

    //ask the user for number of dragons
    int numDragons;
    cout << "Enter number of dragons: ";
    cin >> numDragons;

    //ensure the number of dragons does not exceed available spaces
    if (numDragons > totalEmptyCells) {
        cout << "Maximum number of dragons exceeded. Setting number of dragons to " << totalEmptyCells << "." << endl;
        numDragons = totalEmptyCells;
    }

    placeDragons(gameMap, numDragons, player);
    placeSword(gameMap, player);

    gameMap.setCell(player.getRow(), player.getCol(), player.getSymbol());

    //initial message provided in moodle
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

    //main game loop
    while (true) {
        gameMap.show();

        cout << "\n\nUse (w, a, s, d) in the terminal to move: ";
        char movementDirection;
        cin >> movementDirection;

        //move the player
        bool moved = player.move(movementDirection, gameMap);
        if (!moved) {
            continue;  // Invalid move, ask for input again
        }

        //move the dragons
        gameMap.updateDragonPositions(player);

        //only move the dragons if they are alive
        for (auto& dragon : gameMap.getDragons()) {
            if (!dragon->alive()) continue;

            //check if a dragon is close to the hero, and if the hero has a sword, if he has he can slay the dragon
            int dr = abs(dragon->getRow() - player.getRow());
            int dc = abs(dragon->getCol() - player.getCol());
            if ((dr + dc) == 1) {  // Close encounter
                if (player.isArmed()) {
                    dragon->setAlive(false);
                    cout << "You slayed a dragon. Congrats!" << endl;
                    gameMap.setCell(dragon->getRow(), dragon->getCol(), ' ');
                } else {
                    cout << "Game over! You were slain by the dragon." << endl;
                    exit(0);
                }
            }
        }
    }

    return 0;
}
