#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "../state/board.hpp"

struct Node
{
    Node(State resultingState)
    {
        state = resultingState;
    }

    Node()
    {
        state = State::Ongoing;
    }

    void addChild(Tile move, State resultingState)
    {
        exploredMoves.push_back(MoveInfo(move, resultingState));
    }

    [[nodiscard]] auto isTerminal() const { return state != State::Ongoing; }
    [[nodiscard]] auto numChildren() const { return exploredMoves.size(); }

    std::vector<MoveInfo> exploredMoves{};
    State state{};
};

struct MoveInfo
{
    MoveInfo(Tile tile, State resultingState)
    {
        move = tile;
        child = std::make_unique<Node>(Node(resultingState));
    }

    Tile move;
    std::unique_ptr<Node> child;
    std::uint32_t visits{};
};

struct SearchTree
{
    SearchTree(State currState)
    {
        rootNode = Node(currState);
    }

    Node rootNode{};
    std::uint64_t nodes{};
    std::uint64_t playouts{};
};
