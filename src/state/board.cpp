#include <deque>
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

State BoardState::gameState(float komi) const
{
    if (!isGameOver())
        return State::Ongoing;

    const auto netScore = getScore(komi);

    const auto winBlack = netScore > 0 ? State::Win : State::Loss;

    return winBlack;
}

float BoardState::getScore(float komi) const
{
    auto scoreBlack = stones[0];
    auto scoreWhite = stones[1];

    const auto head = empty;

    const auto territory = getTerritory();

    for (auto tile = head.first; !tile.isNull(); tile = tiles[tile.index()].next)
    {
        const auto ownedBy = territory[tile.index()];
        if (ownedBy == Territory::Black)
            scoreBlack += 1;
        else if (ownedBy == Territory::White)
            scoreWhite += 1;
    }

    return static_cast<float>(scoreBlack) - static_cast<float>(scoreWhite) - komi;
}

std::vector<Territory> BoardState::getTerritory() const
{
    auto territory = std::vector<Territory>(sizeOf(), Territory::Neither);

    std::deque<Tile> todo{};

    const auto head = empty;

    for (auto tile = head.first; !tile.isNull(); tile = tiles[tile.index()].next)
    {
        auto reachBlack = false;
        auto reachWhite = false;

        const auto dirs = tile.getAdjacent(size);

        for (auto i = 0; i < dirs.length; i++)
        {
            const auto offset = dirs.elements[i];
            const auto adjTile = Tile(tile.index() + offset);

            const auto groupId = tiles[adjTile.index()].group;
            if (groupId == 1024)
                continue;

            const auto stoneAt = groups[groupId].belongsTo;
            if (stoneAt == Colour::Black)
                reachBlack = true;
            if (stoneAt == Colour::White)
                reachWhite = true;
        }

        if (reachBlack || reachWhite)
        {
            territory[tile.index()] = territoryFrom(reachBlack, reachWhite);
            todo.push_back(tile);
        }
    }

    while (todo.size() > 0)
    {
        const auto curr = todo.front();
        todo.pop_front();

        const auto currState = territory[curr.index()];

        const auto dirs = curr.getAdjacent(size);
        for (auto i = 0; i < dirs.length; i++)
        {
            const auto offset = dirs.elements[i];
            const auto adjTile = Tile(curr.index() + offset);
            if (tiles[curr.index()].group == 1024)
            {
                const auto oldState = territory[adjTile.index()];
                const auto newState = territoryMerge(oldState, currState);
                if (newState != oldState)
                {
                    territory[adjTile.index()] = newState;
                    todo.push_back(adjTile);
                }
            }
        }
    }

    return territory;
}

void Board::makeMove(const Tile tile)
{
    nodes++;
    history.push_back(board);
    const auto moving = stm;
    stm = flipColour(stm);

    if (tile.index() == 1024)
        board.passMove();
    else
        board.placeStone(tile, moving);
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
    board.display(showGroups, komi);
    const auto side = stm == Colour::Black ? "black" : "white";
    std::cout << "STM: " << side << std::endl;
    std::cout << "Moves Played: " << history.size() << "\n" << std::endl;
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

void BoardState::display(const bool showGroups, float komi) const
{
    std::cout << "=\nBoard:" << std::endl;
    std::cout << "Score: " << getScore(komi) << " (komi = " << komi << ")" << std::endl;
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
