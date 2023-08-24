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

    [[nodiscard]] auto isTerminal() const { return state != State::Ongoing; }
    [[nodiscard]] auto numChildren() const { return legalMoves.size(); }

    State state{};
    std::vector<MoveInfo> legalMoves{};
    std::uint16_t leftToExplore{};
    std::uint32_t visits{};
    std::uint32_t wins{};
};

struct MoveInfo
{
    MoveInfo(Tile tile)
    {
        move = tile;
        ptr = -1;
    }

    Tile move;
    std::int32_t ptr;
};

struct SearchTree
{
    SearchTree(Board& board)
    {
        playouts = 0;
        nodes.push_back(Node(board));
    }

    SearchTree() {}

    void clear(Board& board)
    {
        nodes.clear();
        nodes.push_back(Node(board));
        playouts = 0;
    }

    std::int32_t size() const { return static_cast<int32_t>(nodes.size()); }

    std::vector<Node> nodes{};
    std::uint64_t playouts{};
};
