#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "../state/board.hpp"

struct Node
{
    Node(Board& board)
    {
        state = board.gameState();

        const auto head = board.board.moveHead();
        auto count = 0;

        for (auto move = head.first;; move = board.board[move].next)
        {
            const bool isLegal = board.tryMakeMove(move);
            if (!isLegal)
                continue;

            legalMoves.push_back(MoveInfo(move));

            board.undoMove();

            if (move.isNull())
                break;
        }

        leftToExplore = legalMoves.size();
    }

    Node()
    {
        state = State::Ongoing;
    }

    [[nodiscard]] auto isTerminal() const { return state != State::Ongoing; }
    [[nodiscard]] auto numChildren() const { return legalMoves.size(); }

    std::vector<MoveInfo> legalMoves{};
    std::uint16_t leftToExplore{};
    State state{};
};

struct MoveInfo
{
    MoveInfo(Tile tile)
    {
        move = tile;
        child = std::make_unique<Node>(nullptr);
    }

    Tile move;
    std::unique_ptr<Node> child;
    std::uint32_t visits{};
};

struct SearchTree
{
    SearchTree(Board& board)
    {
        rootNode = Node(board);
    }

    SearchTree() {}

    Node rootNode{};
    std::uint64_t nodes{};
    std::uint64_t playouts{};
};
