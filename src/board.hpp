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
        BoardState(std::uint16_t withSize)
        {
            auto tilesLength = withSize * withSize;

            size = withSize;
            tiles = std::vector<LinkNode>(tilesLength);
            empty = LinkHead(0, tilesLength - 1, tilesLength);

            // create `empty` list
            for (auto i = 0; i < tilesLength; i++)
            {
                if (i > 0)
                    tiles[i].prev = Tile(i - 1);
                if (i < tilesLength - 1)
                    tiles[i].next = Tile(i + 1);
            }
        }

        bool placeStone(Tile tile)
        {
            // Step 1: Place a stone and resolve new groupings.
            empty.remove(tile, tiles);

            const auto groupId = groups.size();
            tiles[tile.index()] = LinkNode(groupId);
            auto newGroup = Group(tile, stm);

            Vec4 adjEnemies{};
            const auto dirs = tile.getAdjacent(size);

            for (auto i = 0; i < dirs.length; i++)
            {
                const auto adjId = tiles[tile.index() + dirs.elements[i]].group;

                if (adjId != 1024)
                {
                    Group& adjGroup = groups[adjId];
                    if (adjGroup.belongsTo != stm && !adjEnemies.contains(adjId))
                    {
                        adjGroup.liberties--;
                        adjEnemies.push(adjId);
                    }
                    else
                        newGroup.join(adjGroup, tiles);
                }
                else
                    newGroup.liberties++;
            }

            groups.push_back(newGroup);

            // Step 2: Capture surrounded enemy stones.
            for (auto i = 0; i < adjEnemies.length; i++)
            {
                const auto adjId = adjEnemies.elements[i];
                if (groups[adjId].liberties <= 0)
                    killGroup(adjId);
            }

            // Step 3: Commit suicide if appropriate.
            const bool wasSuicide = groups[groupId].liberties <= 0;
            if (wasSuicide)
                killGroup(groupId);

            stm = flipColour(stm);

            return wasSuicide;
        }

        void killGroup(std::uint16_t groupId)
        {
            Group& dying = groups[groupId];
            Tile tile = dying.stones.first;

            while (!tile.isNull())
            {
                Vec4 adj{};
                const auto dirs = tile.getAdjacent(size);

                for (auto i = 0; i < dirs.length; i++)
                {
                    const auto adjId = tiles[tile.index() + dirs.elements[i]].group;

                    if (!adj.contains(adjId))
                    {
                        groups[adjId].liberties++;
                        adj.push(adjId);
                    }
                }

                tile = tiles[tile.index()].next;
            }

            empty.join(dying.stones, tiles);
        }

        void display() const
        {
            const auto side = static_cast<std::uint16_t>(stm) ? "White" : "Black";
            std::cout << "\nBoard: " << side << " to play" << std::endl;

            for (auto i = 0; i < size; i++)
            {
                for (auto j = 0; j < size; j++)
                {
                    const auto tileGroup = tiles[size * i + j].group;
                    if (tileGroup == 1024)
                        std::cout << ". ";
                    else
                    {
                        const auto side = groups[tileGroup].belongsTo;
                        const char stone = side == Colour::Black ? 'x' : 'o';
                        std::cout << stone << ' ';
                    }
                }

                std::cout << std::endl;
            }
        }

    private:
        Colour stm;
        LinkHead empty;
        std::uint16_t size;
        std::vector<LinkNode> tiles;
        std::vector<Group> groups;
};

