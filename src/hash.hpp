#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "core.hpp"

constexpr std::uint16_t MaxBoardSize = 25 * 25;
constexpr std::uint16_t HashSize = 2 * MaxBoardSize;

class Zobrist
{
    public:
        constexpr Zobrist(std::uint64_t x, std::uint64_t y)
        {
            upper = x;
            lower = y;
        }

        constexpr Zobrist() {}

        [[nodiscard]] constexpr auto operator==(Zobrist other) const
        {
            return (upper == other.upper) && (lower == other.lower);
        }

        constexpr auto operator^=(Zobrist other)
        {
            upper ^= other.upper;
            lower ^= other.lower;
        }

        [[nodiscard]] constexpr auto operator<<(std::uint8_t shift) const
        {
            return Zobrist(upper << shift, lower << shift);
        }

        [[nodiscard]] constexpr auto operator>>(std::uint8_t shift) const
        {
            return Zobrist(upper >> shift, lower >> shift);
        }

        constexpr auto randomise()
        {
            *this ^= *this << 13;
            *this ^= *this >> 7;
            *this ^= *this << 17;

            return *this;
        }

        static const std::array<Zobrist, HashSize> Hashes;

        static auto hashFor(Tile tile, Colour colour)
        {
            const auto half = MaxBoardSize * static_cast<std::uint16_t>(colour);
            return Hashes[half + tile.index()];
        }

    private:
        std::uint64_t upper = 0;
        std::uint64_t lower = 0;
};

auto generateHashes()
{
	std::array<Zobrist, HashSize> hashes{};

    constexpr std::uint64_t SeedUpper = UINT64_C(0xD06C659954EC904A);
    constexpr std::uint64_t SeedLower = UINT64_C(0xA0B2342C523532E2);

	auto rng = Zobrist(SeedUpper, SeedLower);

	for (auto &hash : hashes)
		hash = rng.randomise();

	return hashes;
}

const std::array<Zobrist, HashSize> Zobrist::Hashes = generateHashes();
