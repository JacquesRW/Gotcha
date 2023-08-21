#pragma once

#include <cstdint>
#include <vector>

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
        BoardState(std::uint16_t withSize);

        bool placeStone(Tile tile);

        void killGroup(std::uint16_t groupId);

        void display() const;

    private:
        Colour stm;
        LinkHead empty;
        std::uint16_t size;
        std::vector<LinkNode> tiles;
        std::vector<Group> groups;
};

