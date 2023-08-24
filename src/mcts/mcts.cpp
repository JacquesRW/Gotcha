#include <cassert>

#include "mcts.hpp"

std::uint64_t Mcts::getRandom()
{
    random ^= random << 13;
    random ^= random >> 7;
    random ^= random << 17;

    return random;
}

Node& Mcts::selectLeaf()
{
    selectionLine.clear();
    Node& leaf = tree.rootNode;

    while (1)
    {
        const auto randomIdx = getRandom() % leaf.numChildren();

        if (leaf.legalMoves[randomIdx].child)
        {
            // verified legal move
            board.makeMove(leaf.legalMoves[randomIdx].move);
            selectionLine.push_back(leaf.legalMoves[randomIdx]);
            leaf = *leaf.legalMoves[randomIdx].child;
        }
        else
            break;
    }

    return leaf;
}

Node& Mcts::expandNode(Node& node)
{
    assert(node.leftToExplore > 0);
    const auto randomIdx = getRandom() % node.leftToExplore;

    node.leftToExplore--;

    if (node.leftToExplore)
        std::swap(node.legalMoves[randomIdx], node.legalMoves[node.leftToExplore]);

    auto& nodeToExplore = node.legalMoves[node.leftToExplore];
    selectionLine.push_back(nodeToExplore);

    // verified legal move
    board.makeMove(nodeToExplore.move);

    nodeToExplore.child = std::make_unique<Node>(Node(board));

    return *nodeToExplore.child;
};