#include "tree.hpp"

Node& SearchTree::selectLeaf(Board& board)
{
    Node& leaf = rootNode;

    while (leaf.exploredMoves.size() > 0)
    {
        leaf = *leaf.exploredMoves[0].child;
    }

    return leaf;
}

void SearchTree::expandNode(Board& board, Node& node)
{

}