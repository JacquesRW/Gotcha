#pragma once

#include "core.hpp"

struct Group
{
    Group(Tile tile, Colour colour)
    {
        belongsTo = colour;
        stones = LinkHead(tile, tile, 1);
        liberties = 0;
    }

    void join(Group& other, std::vector<LinkNode>& tiles)
    {
        liberties += other.liberties - 1;
        stones.join(other.stones, tiles);
    }

    Colour belongsTo;
    LinkHead stones;
    std::uint16_t liberties;
};

class BoardState
{
    public:
        BoardState(const std::uint16_t withSize);

        BoardState() { BoardState(5); };

        bool placeStone(const Tile tile);

        void killGroup(const std::uint16_t groupId);

        void display(const bool showGroups) const;

        void passMove()
        {
            stm = flipColour(stm);
            passes++;
        }

        [[nodiscard]] auto isGameOver() const { return passes >= 2; }
        [[nodiscard]] auto sizeOf() const { return size * size; }
        [[nodiscard]] auto getHash() const { return hash; }
        [[nodiscard]] auto moveHead() const { return empty; }
        [[nodiscard]] auto operator[](Tile tile) const { return tiles[tile.index()]; }

    private:
        Colour stm;
        LinkHead empty;
        std::uint16_t passes;
        std::uint16_t size;
        std::vector<LinkNode> tiles;
        std::vector<Group> groups;
        std::uint64_t hash;
};

class Board
{
    public:
        BoardState board;

        Board(const std::uint16_t withSize) { board = BoardState(withSize); }

        bool tryMakeMove(const Tile tile);

        void undoMove()
        {
            board = history.back();
            history.pop_back();
        }

    private:
        std::vector<BoardState> history;
};
