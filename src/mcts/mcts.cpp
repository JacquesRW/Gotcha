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
    Node& leaf = tree.rootNode;

    while (leaf.numChildren() > 0)
    {
        // 50% chance of selecting further down (for now)
        if (getRandom() & 1)
        {
            const auto randomIdx = getRandom() % leaf.numChildren();
            leaf = *leaf.exploredMoves[randomIdx].child;
        }
        // using this node
        else
            break;
    }

    return leaf;
}

Node& Mcts::expandNode(Node& node)
{

}