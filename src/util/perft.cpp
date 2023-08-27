#include "perft.hpp"

bool PerftBoard::tryMakeMove(const Tile tile)
{
    history.push_back(board);
    const auto moving = stm;
    stm = flipColour(stm);

    // passing turn is always legal
    if (tile.index() == 1024)
    {
        board.passMove();
        return true;
    }

    // make move
    const auto isSuicide = board.placeStone(tile, moving);

    // suicides are not legal
    if (isSuicide)
    {
        undoMove();
        return false;
    }

    // repetitions are not legal
    for (const auto& prior : history)
    {
        if (prior.getHash() == board.getHash())
        {
            undoMove();
            return false;
        }
    }

    return true;
}

std::uint64_t PerftBoard::runPerft(uint8_t depth)
{
    if (depth == 0)
        return 1;

    if (board.isGameOver())
        return 0;

    const auto head = board.moveHead();
    auto count = 0;

    for (auto move = head.first;; move = board[move].next)
    {
        const bool isLegal = tryMakeMove(move);
        if (!isLegal)
            continue;

        const auto subCount = runPerft(depth - 1);

        count += subCount;

        undoMove();

        if (move.isNull())
            break;
    }

    return count;
}