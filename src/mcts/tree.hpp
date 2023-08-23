#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "../state/board.hpp"

class Node
{
    public:
        Node(State resultingState)
        {
            state = resultingState;
        }

        void addChild(Tile move, State resultingState)
        {
            exploredMoves.push_back(MoveInfo(move, resultingState));
        }

        [[nodiscard]] auto isTerminal() const { return state != State::Ongoing; }
        [[nodiscard]] auto numChildren() const { return exploredMoves.size(); }

    private:
        std::vector<MoveInfo> exploredMoves{};
        State state{};
};

class MoveInfo
{
    public:
        MoveInfo(Tile tile, State resultingState)
        {
            move = tile;
            child = std::make_unique<Node>(Node(resultingState));
        }

    private:
        Tile move;
        std::unique_ptr<Node> child;
        std::uint32_t visits{};
};

class SearchTree
{
    private:
        Node rootNode = Node(State::Ongoing);
        std::uint64_t nodes{};
        std::uint64_t playouts{};
};
