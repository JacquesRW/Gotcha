#include <cassert>

#include "mcts.hpp"

std::uint64_t Mcts::getRandom()
{
    random ^= random << 13;
    random ^= random >> 7;
    random ^= random << 17;

    return random;
}

std::int32_t Mcts::selectLeaf()
{
    selectionLine.clear();
    auto nodePtr = 0;

    while (1)
    {
        const auto& node = tree.nodes[nodePtr];
        const auto randomIdx = getRandom() % node.numChildren();
        const auto next = node.legalMoves[randomIdx].ptr;

        if (next == -1)
            break;

        // verified legal move
        board.makeMove(node.legalMoves[randomIdx].move);
        selectionLine.push_back(next);
        nodePtr = next;
    }

    return nodePtr;
}

std::int32_t Mcts::expandNode(std::int32_t nodePtr)
{
    auto& node = tree.nodes[nodePtr];

    assert(node.leftToExplore > 0);
    const auto randomIdx = getRandom() % node.leftToExplore;

    node.leftToExplore--;

    if (node.leftToExplore)
        std::swap(node.legalMoves[randomIdx], node.legalMoves[node.leftToExplore]);

    auto& nodeToExplore = node.legalMoves[node.leftToExplore];

    // verified legal move
    board.makeMove(nodeToExplore.move);

    tree.nodes.push_back(Node(board));
    nodeToExplore.ptr = tree.nodes.size() - 1;

    selectionLine.push_back(nodeToExplore.ptr);

    return nodeToExplore.ptr;
};