#include <iostream>

#include "gtp.hpp"

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
    GtpRunner gtp{};
    gtp.run();
}