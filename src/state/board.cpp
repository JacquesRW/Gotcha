#include <iostream>
#include <iomanip>

#include "board.hpp"

BoardState::BoardState(const std::uint16_t withSize)
{
    auto tilesLength = withSize * withSize;

    size = withSize;
    tiles = std::vector<LinkNode>(tilesLength);
    empty = LinkHead(0, tilesLength - 1, tilesLength);
    passes = 0;
    stones = {0, 0};
    hash = Zobrist(0, 0);

    // create `empty` list
    for (auto i = 0; i < tilesLength; i++)
    {
        if (i > 0)
            tiles[i].prev = Tile(i - 1);
        if (i < tilesLength - 1)
            tiles[i].next = Tile(i + 1);
    }
}

bool BoardState::placeStone(const Tile tile, Colour colour)
{
    passes = 0;
    const auto stm = static_cast<std::uint8_t>(colour);
    const auto oppStm = 1 - stm;

    // Step 1: Place a stone and resolve new groupings.
    empty.remove(tile, tiles);
    hash ^= Zobrist::hashFor(tile, colour);
    stones[stm] += 1;

    const auto groupId = groups.size();
    tiles[tile.index()] = LinkNode(groupId);

    groups.push_back(Group(tile, colour));
    auto& newGroup = groups.back();

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
            if (adjGroup.belongsTo != colour)
            {
                adjGroup.liberties--;
                adjEnemies.push(adjId);
            }
            else if (adjId != groupId)
                newGroup.join(adjGroup, tiles);
            else
                newGroup.liberties--;
        }
        else
            newGroup.liberties++;
    }

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

    return wasSuicide;
}

void BoardState::killGroup(const std::uint16_t groupId)
{
    Group& dying = groups[groupId];
    Tile tile = dying.stones.first;
    hash ^= dying.hash;
    const auto dyingColour = static_cast<std::uint8_t>(dying.belongsTo);
    stones[dyingColour] -= dying.stones.len();

    while (!tile.isNull())
    {
        Vec4 adj{};
        const auto dirs = tile.getAdjacent(size);

        for (auto i = 0; i < dirs.length; i++)
        {
            const auto offset = dirs.elements[i];
            const auto adjTile = Tile(tile.index() + offset);
            const auto adjId = tiles[adjTile.index()].group;
            groups[adjId].liberties++;
            adj.push(adjId);
        }

        tile = tiles[tile.index()].next;
    }

    empty.join(dying.stones, tiles);
}

State BoardState::gameState(Colour stm, float komi) const
{
    if (!isGameOver())
        return State::Ongoing;

    auto scoreBlack = static_cast<float>(stones[0]);
    auto scoreWhite = static_cast<float>(stones[1]) + komi;

    const auto winBlack = scoreBlack > scoreWhite ? State::Win : State::Loss;

    if (stm == Colour::White)
        return flipState(winBlack);

    return winBlack;
}

bool Board::tryMakeMove(const Tile tile)
{
    history.push_back(board);
    const auto moving = stm;
    stm = flipColour(stm);

    // passing turn is always legal
    if (tile.index() == 1024)
    {
        board.passMove();
        return true;
    }

    // make move
    const auto isSuicide = board.placeStone(tile, moving);

    // suicides are not legal
    if (isSuicide)
    {
        undoMove();
        return false;
    }

    // repetitions are not legal
    for (const auto& prior : history)
    {
        if (prior.getHash() == board.getHash())
        {
            undoMove();
            return false;
        }
    }

    return true;
}

void Board::display(const bool showGroups) const
{
    board.display(showGroups);
    std::cout << "Moves Played: " << history.size() << "\n" << std::endl;
}

std::vector<Tile> Board::moveList(std::vector<Tile> used) const
{
    const auto head = board.moveHead();

    std::vector<Tile> moves{};

    for (auto move = head.first;; move = board[move].next)
    {
        if (std::find(used.begin(), used.end(), move) != used.end())
            moves.push_back(move);

        if (move.isNull())
            break;
    }

    return moves;
}

std::uint64_t Board::runPerft(uint8_t depth)
{
    if (depth == 0)
        return 1;

    if (board.isGameOver())
        return 0;

    const auto head = board.moveHead();
    auto count = 0;

    for (auto move = head.first;; move = board[move].next)
    {
        const bool isLegal = tryMakeMove(move);
        if (!isLegal)
            continue;

        const auto subCount = runPerft(depth - 1);

        count += subCount;

        undoMove();

        if (move.isNull())
            break;
    }

    return count;
}

void BoardState::display(const bool showGroups) const
{
    std::cout << "=\nBoard:" << std::endl;
    hash.display();

    for (auto i = 0; i < size; i++)
    {
        const auto k = size - i - 1;
        for (auto j = 0; j < size; j++)
        {
            const auto tileGroup = tiles[size * k + j].group;
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
