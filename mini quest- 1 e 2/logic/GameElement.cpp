#include "GameElement.h"
using namespace std;

namespace dkeep {
    namespace logic {

        GameElement::GameElement(int r, int c) : row(r), col(c) {}

        int GameElement::getRow() const {
            return row;
        }

        int GameElement::getCol() const {
            return col;
        }

    }
}
