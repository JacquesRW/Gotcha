#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>

#include "core.hpp"

auto split_at(const std::string &str, char delim)
{
	std::pair<std::string, std::string> result{};

    auto split = str.find(delim);

    if (split == std::string::npos)
    {
        result.first = str;
        result.second = "";
    }
    else
    {
        result.first = str.substr(0, split);
        const auto singleton = (split + 1) >= str.length();
        result.second = singleton ? "" : str.substr(split + 1, str.length());
    }

	return result;
}

std::pair<Tile, Colour> parse_move(std::string &moveStr, std::uint16_t size)
{
    auto [colourStr, tileStr] = split_at(moveStr, ' ');

    auto makeLower = [](std::string& str)
    {
        transform(str.begin(), str.end(), str.begin(), ::tolower);
    };

    makeLower(colourStr);
    makeLower(tileStr);

    const std::array<std::string, 4> possible = {"b", "black", "w", "white"};

    const auto colour = (colourStr.at(0) == 'b') ? Colour::Black : Colour::White;
    const auto columnStr = tileStr.at(0);
    const auto rowStr = split_at(tileStr, columnStr).second;

    const auto column = static_cast<int>(columnStr) - 97;
    const auto row = std::stoi(rowStr) - 1;

    if (column >= size || column < 0 || row >= size || row < 0)
        throw std::invalid_argument("out of board bounds");

    const auto tile = Tile(column, row, size);

    return {tile, colour};
}