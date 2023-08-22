#include <cstddef>

#include "hash.hpp"

namespace {
    auto generateHashes()
    {
    	std::array<Zobrist, HashSize> hashes{};

        constexpr auto SeedUpper = UINT64_C(0xD06C659954EC904A);
        constexpr auto SeedLower = UINT64_C(0xA0B2342C523532E2);

    	auto rng = Zobrist(SeedUpper, SeedLower);

    	for (auto &hash : hashes)
    		hash = rng.randomise();

    	return hashes;
    }
}

const std::array<Zobrist, HashSize> Zobrist::Hashes = generateHashes();