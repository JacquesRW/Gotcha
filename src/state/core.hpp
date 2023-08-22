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
    return static_cast<Colour>(!static_cast<std::uint8_t>(colour));
}

struct Vec4
{
    std::array<uint16_t, 4> elements;
    std::uint16_t length;

    constexpr void push(std::uint16_t element)
    {
        elements[length] = element;
        length++;
    }
};

class Tile
{
    public:
        constexpr Tile(std::uint16_t t) { tile = t; }

        constexpr Tile() { tile = 1024; }

        constexpr Tile(std::uint16_t x, std::uint16_t y, std::uint16_t size)
        {
            tile = size * y + x;
        }

        [[nodiscard]] constexpr auto index() const { return tile; }
        [[nodiscard]] constexpr auto isNull() const { return tile == 1024; }

        [[nodiscard]] constexpr auto getAdjacent(std::uint16_t size) const
        {
            auto adj = Vec4{};

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

    LinkNode(std::uint16_t groupId) { group = groupId; }

    LinkNode() { group = 1024; }
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

        void join(LinkHead& other, std::vector<LinkNode>& tiles);

        void remove(const Tile tile, std::vector<LinkNode>& tiles);

    private:
        std::uint32_t length;
};
