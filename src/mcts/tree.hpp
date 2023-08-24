#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "../io/parse.hpp"
#include "../state/board.hpp"

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

class Node
{
    public:
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

        [[nodiscard]] auto& operator[](std::int32_t i) {
            try { return legalMoves.at(i); }
            catch(std::out_of_range e) {
                std::cout << numChildren() << " " << i << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        [[nodiscard]] const auto& operator[](std::int32_t i) const { return legalMoves.at(i); }

        State state{};
        std::uint16_t leftToExplore{};
        std::uint32_t visits{};
        std::uint32_t wins{};

    private:
        std::vector<MoveInfo> legalMoves{};
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

    std::int32_t size() const { return static_cast<std::int32_t>(nodes.size()); }

    void add(Node n) { nodes.push_back(n); }

    [[nodiscard]] auto& operator[](std::int32_t i) { return nodes.at(i); }

    std::uint64_t playouts{};

    private:
        std::vector<Node> nodes{};
};
