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
        BoardState(std::uint16_t s)
        {
            size = s;
            tiles = std::vector<LinkNode>(size * size);
        }

        bool placeStone(Tile tile)
        {
            // Step 1: Place a stone and resolve new groupings.
            empty.remove(tile, tiles);

            const auto groupId = groups.size();
            tiles[tile.index()] = LinkNode(groupId);
            auto newGroup = Group(tile, stm);

            Vec4 adjEnemies;
            const auto dirs = tile.getAdjacent(size);

            for (auto i = 0; i < dirs.length; i++)
            {
                const auto offset = dirs.elements[i];
                const auto adjTile = Tile(tile.index() + offset);
                const auto adjId = tiles[adjTile.index()].group;

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
            bool wasSuicide = false;
            Group& newGroupRef = groups[groupId];
            if (newGroupRef.liberties <= 0)
            {
                killGroup(groupId);
                wasSuicide = true;
            }

            stm = flipColour(stm);

            return wasSuicide;
        }

        void killGroup(std::uint16_t groupId)
        {
            Group& dying = groups[groupId];
            Tile tile = dying.stones.first;
            while (!tile.isNull())
            {
                Vec4 adj;
                const auto dirs = tile.getAdjacent(size);

                for (auto i = 0; i < dirs.length; i++)
                {
                    const auto adjId = tiles[tile.index() + dirs.elements[i]].group;

                    // this condition shouldn't trigger!
                    if (adjId == 1024)
                        continue;

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

        void display()
        {
            std::cout << "Board:" << std::endl;
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

