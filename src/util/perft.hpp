#pragma once

#include "../state/board.hpp"

class PerftBoard
{
    public:
        BoardState board;

        PerftBoard(BoardState b, Colour c)
        {
            board = b;
            stm = c;
        }

        bool tryMakeMove(const Tile tile);

        void undoMove()
        {
            stm = flipColour(stm);
            board = history.back();
            history.pop_back();
        }

        std::uint64_t runPerft(uint8_t depth);

    private:
        Colour stm;
        std::vector<BoardState> history;
};