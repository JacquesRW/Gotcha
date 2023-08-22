#include <iostream>

#include "board.hpp"

void tryMove(std::uint16_t x, std::uint16_t y, std::uint16_t size, Board& board)
{
    auto tile = Tile(x, y, size);
    auto isLegal = board.tryMakeMove(tile);
    if (!isLegal)
        std::cout << "woah" << std::endl;
    board.display(true);
}

template <bool isRoot>
std::uint64_t perft(Board& board, uint8_t depth)
{
    if (depth == 0)
        return 1;

    if (board.board.isGameOver())
        return 0;

    const auto head = board.board.moveHead();
    auto count = 0;

    for (auto move = head.first;; move = board.board[move].next)
    {
        const bool isLegal = board.tryMakeMove(move);
        if (!isLegal)
            continue;

        const auto subCount = perft<false>(board, depth - 1);

        if constexpr (isRoot)
            std::cout << move.index() << ": " << subCount << std::endl;

        count += subCount;

        board.undoMove();

        if (move.isNull())
            break;
    }

    return count;
}

int main()
{
    const auto size = 3;
    const auto depth = 10;

    auto board = Board(size);
    board.display(true);

    auto count = perft<true>(board, depth);
    std::cout << "depth " << depth << " nodes " << count << std::endl;
}