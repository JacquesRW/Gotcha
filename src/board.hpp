#pragma once

#include <cstdint>
#include <vector>

enum struct Colour : std::uint8_t
{
    Black = 0,
    White = 1,
};

class Group
{
    private:
        Colour belongs_to;
        std::uint16_t liberty_upper_bound;
        std::uint64_t hash;
        std::vector<std::uint16_t> stones;
};

[[nodiscard]] constexpr auto oppColour(Colour colour)
{
    return static_cast<Colour>(static_cast<std::uint8_t>(colour) ^ 1);
}

class BoardState
{
    public:
        [[nodiscard]] constexpr auto getHash() const { return hash; }

    private:
        Colour stm;

        std::vector<Group> groups;
        std::vector<std::uint8_t> tiles;
        std::uint64_t hash;
};

class Tile
{
    public:
        [[nodiscard]] constexpr auto index() const { return tile; }
        [[nodiscard]] constexpr auto isNull() const { return tile == 1024; }

    private:
        std::uint16_t tile = 1024;
};
