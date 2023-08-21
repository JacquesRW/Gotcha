#include <iostream>

#include "board.hpp"

void tryMove(std::uint16_t x, std::uint16_t y, std::uint16_t size, BoardState& board)
{
    auto tile = Tile(x, y, size);
    board.placeStone(tile);
    board.display();
}

int main()
{
    const auto size = 5;

    auto board = BoardState(size);
    board.display();

    tryMove(0, 0, size, board);

    tryMove(0, 1, size, board);

    tryMove(4, 4, size, board);

    tryMove(1, 0, size, board);
}