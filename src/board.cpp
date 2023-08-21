#include <iostream>
#include <iomanip>

#include "board.hpp"

bool Board::tryMakeMove(const Tile tile)
{
    history.push_back(board);
    bool isLegal = true;

    // passing turn
    if (tile.index() == 1024)
        board.passMove();
    else
        isLegal = !board.placeStone(tile);

    if (!isLegal)
        undoMove();

    return isLegal;
}

BoardState::BoardState(const std::uint16_t withSize)
{
    auto tilesLength = withSize * withSize;

    size = withSize;
    tiles = std::vector<LinkNode>(tilesLength);
    empty = LinkHead(0, tilesLength - 1, tilesLength);
    passes = 0;
    stm = Colour::Black;

    // create `empty` list
    for (auto i = 0; i < tilesLength; i++)
    {
        if (i > 0)
            tiles[i].prev = Tile(i - 1);
        if (i < tilesLength - 1)
            tiles[i].next = Tile(i + 1);
    }
}

bool BoardState::placeStone(const Tile tile)
{
    passes = 0;

    // Step 1: Place a stone and resolve new groupings.
    empty.remove(tile, tiles);

    const auto groupId = groups.size();
    tiles[tile.index()] = LinkNode(groupId);
    auto newGroup = Group(tile, stm);

    Vec4 adjEnemies{};
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
    const bool wasSuicide = groups[groupId].liberties <= 0;
    if (wasSuicide)
        killGroup(groupId);

    stm = flipColour(stm);

    return wasSuicide;
}

void BoardState::killGroup(const std::uint16_t groupId)
{
    Group& dying = groups[groupId];
    Tile tile = dying.stones.first;

    while (!tile.isNull())
    {
        Vec4 adj{};
        const auto dirs = tile.getAdjacent(size);

        for (auto i = 0; i < dirs.length; i++)
        {
            const auto offset = dirs.elements[i];
            const auto adjTile = Tile(tile.index() + offset);
            const auto adjId = tiles[adjTile.index()].group;

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

void BoardState::display(const bool showGroups) const
{
    const auto side = static_cast<std::uint16_t>(stm) ? "White" : "Black";
    std::cout << "\nBoard: " << side << " to play" << std::endl;

    for (auto i = 0; i < size; i++)
    {
        for (auto j = 0; j < size; j++)
        {
            const auto tileGroup = tiles[size * i + j].group;
            if (showGroups)
                std::cout << std::setw(4) << tileGroup << " ";
            else
            {
                if (tileGroup == 1024)
                    std::cout << ". ";
                else
                {
                    const auto side = groups[tileGroup].belongsTo;
                    const char stone = side == Colour::Black ? 'o' : 'x';
                    std::cout << stone << ' ';
                }
            }
        }

        std::cout << std::endl;
    }
}