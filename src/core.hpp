#pragma once

#include <array>
#include <cstdint>
#include <vector>

enum struct Colour : std::uint8_t
{
    Black = 0,
    White = 1,
};

[[nodiscard]] constexpr auto flipColour(Colour colour)
{
    return static_cast<Colour>(static_cast<std::uint8_t>(colour) ^ 1);
}

struct Directions
{
    std::array<uint16_t, 4> dirs;
    std::uint16_t length;

    constexpr void push(std::uint16_t dir)
    {
        dirs[length] = dir;
        length++;
    }
};

class Tile
{
    public:
        constexpr Tile(std::uint16_t t)
        {
            tile = t;
        }

        constexpr Tile()
        {
            tile = 1024;
        }

        [[nodiscard]] constexpr auto index() const { return tile; }
        [[nodiscard]] constexpr auto isNull() const { return tile == 1024; }

        [[nodiscard]] constexpr auto getAdjacent(std::uint16_t size) const
        {
            auto adj = Directions{};

            if ((tile % size) > 0)
                adj.push(static_cast<std::uint16_t>(-1));

            if ((tile % size) < (size - 1))
                adj.push(static_cast<std::uint16_t>(1));

            if ((tile / size) > 0)
                adj.push(static_cast<std::uint16_t>(-size));

            if ((tile / size) < (size - 1))
                adj.push(static_cast<std::uint16_t>(size));

            return adj;
        }

    private:
        std::uint16_t tile = 1024;
};

struct LinkNode
{
    Tile prev;
    Tile next;
    std::uint16_t group;

    LinkNode(std::uint16_t groupId)
    {
        group = groupId;
    }

    LinkNode() {}
};

class LinkHead
{
    public:
        Tile first;
        Tile last;

        LinkHead(const Tile x, const Tile y, const std::uint32_t z)
        {
            first = x;
            last = y;
            length = z;
        }

        LinkHead()
        {
            first = Tile{};
            last = Tile{};
            length = 0;
        }


        [[nodiscard]] constexpr auto len() const { return length; }
        [[nodiscard]] constexpr auto isEmpty() const { return length == 0; }

        void join(LinkHead& other, std::vector<LinkNode>& tiles)
        {
            if (!other.last.isNull())
                tiles[other.last.index()].next = first;

            if (!first.isNull())
                tiles[first.index()].prev = other.last;

            length += other.length;
            if (!other.first.isNull())
                first = other.first;
            if (last.isNull())
                last = other.last;

            other = LinkHead(Tile{}, Tile{}, 0);
        }

        void remove(const Tile tile, std::vector<LinkNode>& tiles)
        {
            const auto index = tile.index();
            const auto prev = tiles[index].prev;
            const auto next = tiles[index].next;
            tiles[index] = LinkNode(1024);

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

    private:
        std::uint32_t length;
};
