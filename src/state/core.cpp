#include "core.hpp"

void LinkHead::join(LinkHead& other, std::vector<LinkNode>& tiles)
{
    // overwrite group IDs
    const auto newGroupId = first.isNull() ? 1024 : tiles[first.index()].group;
    for (Tile tile = other.first; !tile.isNull(); tile = tiles[tile.index()].next)
        tiles[tile.index()].group = newGroupId;

    // join up the groups
    if (!other.last.isNull())
        tiles[other.last.index()].next = first;

    if (!first.isNull())
        tiles[first.index()].prev = other.last;

    length += other.length;
    if (!other.first.isNull())
        first = other.first;
    if (last.isNull())
        last = other.last;

    other = LinkHead{};
}

void LinkHead::remove(const Tile tile, std::vector<LinkNode>& tiles)
{
    const auto index = tile.index();
    const auto prev = tiles[index].prev;
    const auto next = tiles[index].next;
    tiles[index] = LinkNode{};

    if (prev.isNull())
        first = next;
    else
        tiles[prev.index()].next = next;

    if (next.isNull())
        last = prev;
    else
        tiles[next.index()].prev = prev;

    length--;
}