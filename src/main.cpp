#include <iostream>

#include "board.hpp"

void tryMove(std::uint16_t x, std::uint16_t y, std::uint16_t size, BoardState& board)
{
    auto tile = Tile(x, y, size);
    board.placeStone(tile);
    board.display(true);
}

std::uint64_t perft(Board& board, uint8_t depth)
{
    if (depth == 0)
        return 1;

    if (board.board.isGameOver())
        return 0;

    const auto head = board.board.moveHead();
    auto count = 0;

    auto tile = head.first;
    while (1)
    {
        const auto move = tile;

        tile = board.board[tile].next;

        const bool isLegal = board.tryMakeMove(move);
        if (!isLegal)
            continue;

        count += perft(board, depth - 1);

        board.undoMove();

        if (move.isNull())
            break;
    }

    return count;
}

int main()
{
    const auto size = 5;
    const auto depth = 5;

    auto board = Board(size);
    board.board.display(true);

    auto count = perft(board, depth);

    std::cout << "depth " << depth << " nodes " << count << std::endl;
}