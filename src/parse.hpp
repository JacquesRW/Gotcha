#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>

#include "core.hpp"

auto splitAt(const std::string &str, char delim)
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


auto makeLower(std::string& str)
{
    transform(str.begin(), str.end(), str.begin(), ::tolower);
};

auto parseColour(std::string& str)
{
    return (str.at(0) == 'b') ? Colour::Black : Colour::White;
}

std::pair<Tile, Colour> parseMove(std::string &moveStr, std::uint16_t size)
{
    auto [colourStr, tileStr] = splitAt(moveStr, ' ');

    makeLower(colourStr);
    makeLower(tileStr);

    const auto colour = parseColour(colourStr);
    const auto columnStr = tileStr.at(0);
    const auto rowStr = splitAt(tileStr, columnStr).second;

    const auto column = static_cast<int>(columnStr) - 97;
    const auto row = std::stoi(rowStr) - 1;

    if (column >= size || column < 0 || row >= size || row < 0)
        throw std::invalid_argument("out of board bounds");

    const auto tile = Tile(column, row, size);

    return {tile, colour};
}

auto tileToString(Tile tile)
{
    const auto row = tile.index() % 19;
    const auto column = tile.index() / 19;

    return static_cast<char>(97 + row) + std::to_string(column + 1);
}
