#pragma once

#include <cstdint>

class Board
{
    public:
        constexpr auto getHash() const { return hash; }

    private:
        std::uint64_t hash;
};
