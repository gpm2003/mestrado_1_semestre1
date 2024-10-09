#pragma once
using namespace std;

namespace dkeep {
    namespace logic {

        class GameElement {
        protected:
            int row;
            int col;

        public:
            GameElement(int r, int c);
            virtual ~GameElement() = default;

            int getRow() const;
            int getCol() const;

            virtual char getSymbol() const = 0;
        };

    }
}
