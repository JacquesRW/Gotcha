#pragma once

#include <cstdint>
#include <vector>

#include "core.hpp"

struct Group
{
    Group(Tile tile, Colour colour)
    {
        belongTo = colour;
        stones = LinkHead(tile, tile, 1);
    }

    void join(Group& other, std::vector<LinkNode>& tiles)
    {
        stones.join(other.stones, tiles);
    }

    Colour belongTo;
    LinkHead stones;
    LinkNode deadLink;
    std::uint16_t libertyUpperBound;
    std::uint64_t hash;
};

class BoardState
{
    public:
        BoardState(std::uint16_t s)
        {
            size = s;
            tiles = std::vector<LinkNode>(size * size);
        }

        [[nodiscard]] constexpr auto getHash() const { return hash; }

        void placeStone(Tile tile)
        {
            // Step 1: Place a stone and resolve new groupings.

            // Remove stone from empty.
            empty.remove(tile, tiles);

            // Add stone as new group
            tiles[tile.index()] = LinkNode(groups.size());
            auto newGroup = Group(tile, stm);

            // Iterate over directions
            const auto dirs = tile.getAdjacent(size);
            for (auto i = 0; i < dirs.length; i++)
            {
                const auto offset = dirs.dirs[i];
                const auto adjTile = Tile(tile.index() + offset);
                const auto adjId = tiles[adjTile.index()].group;

                // Non-empty adjacent tile
                if (adjId != 1024)
                {
                    Group& adjGroup = groups[adjId];
                    // Deprive a different coloured group of a liberty
                    if (adjGroup.belongTo != stm)
                        adjGroup.libertyUpperBound--;
                    // Join onto own group
                    else
                    {
                        newGroup.join(adjGroup, tiles);
                    }
                }
                // Empty adjacent tile is a free liberty
                else
                {
                    newGroup.libertyUpperBound++;
                }
            }

            // push to list of groups
            groups.push_back(newGroup);

            stm = flipColour(stm);
        }

    private:
        Colour stm;
        LinkHead empty;
        LinkHead dead;
        std::uint16_t size;
        std::vector<LinkNode> tiles;
        std::vector<Group> groups;
        std::uint64_t hash;
};

