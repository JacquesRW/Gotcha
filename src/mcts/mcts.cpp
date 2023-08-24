#include <cassert>
#include <iostream>

#include "../io/parse.hpp"
#include "mcts.hpp"

Tile Mcts::search()
{
    tree.clear(board);

    while (tree.size() < capacity)
    {
        // Stage 1: Select a lead node already in the search tree.
        const auto selectedNode = selectLeaf();

        // Stage 2: If not a terminal node, pick a child of the leaf
        // node that isn't currently in the tree.
        if (selectedNode != -1)
            const auto newNode = expandNode(selectedNode);

        // Stage 3: Randomly simulate the outcome of the game from there.
        const auto result = simulate();

        // Stage 4: Backpropogate the result towards the root.
        backprop(result);
    }

    const auto& rootNode = tree.nodes[0];
    auto bestIdx = 0;
    auto bestScore = 0.0;

    for (auto i = 0; i < rootNode.numChildren(); i++)
    {
        const auto move = rootNode.legalMoves[i];
        const auto& node = tree.nodes[move.ptr];

        const auto visits = static_cast<double>(node.visits);
        const auto wins = static_cast<double>(node.wins);

        const auto score = wins / visits;

        std::cout << tileToString(move.move) << ": " << 100.0 * score << std::endl;

        if (score > bestScore)
        {
            bestScore = score;
            bestIdx = i;
        }
    }

    std::cout << "win probability: " << 100.0 * bestScore << std::endl;

    return rootNode.legalMoves[bestIdx].move;
}

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
    selectionLine.push_back(0);

    auto nodePtr = 0;

    while (1)
    {
        const auto& node = tree.nodes[nodePtr];

        // found a terminal node
        if (node.isTerminal())
            return -1;

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

State Mcts::simulate()
{
    const auto state = board.gameState();

    // This is a terminal node, end of rollout.
    if (state != State::Ongoing)
        return state;

    auto moves = std::vector<Tile>(0);

    board.genLegal(moves);

    const auto numLegal = moves.size();
    const auto randIdx = numLegal > 1 ? getRandom() % (numLegal - 1) : 0;
    const auto randMove = moves[randIdx];

    board.makeMove(randMove);

    const auto result = flipState(simulate());

    board.undoMove();

    return result;
}

void Mcts::backprop(State result)
{
    while (selectionLine.size() > 0)
    {
        const auto nodePtr = selectionLine.back();
        selectionLine.pop_back();

        auto& node = tree.nodes[nodePtr];

        node.visits += 1;

        if (result == State::Win)
            node.wins += 1;

        result = flipState(result);
    }
}
