#include <algorithm>
#include <array>
#include <iostream>
#include <sstream>

#include "parse.hpp"

void makeLower(std::string& str)
{
    transform(str.begin(), str.end(), str.begin(), ::tolower);
};

Colour parseColour(std::string& str)
{
    makeLower(str);
    return (str.at(0) == 'b') ? Colour::Black : Colour::White;
}


std::pair<std::string, std::string> splitAt(const std::string &str, char delim)
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

std::pair<Tile, Colour> parseMove(std::string &moveStr, std::uint16_t size)
{
    auto [colourStr, tileStr] = splitAt(moveStr, ' ');

    makeLower(colourStr);
    makeLower(tileStr);

    const auto colour = parseColour(colourStr);

    if (tileStr == "pass")
        return {Tile{}, colour};

    const auto columnStr = tileStr.at(0);
    const auto rowStr = splitAt(tileStr, columnStr).second;

    auto column = static_cast<int>(columnStr) - 97;
    if (column > 8)
        column--;

    const auto row = std::stoi(rowStr) - 1;

    if (column >= size || column < 0 || row >= size || row < 0)
        throw std::invalid_argument("out of board bounds");

    const auto tile = Tile(column, row, size);

    return {tile, colour};
}

std::string tileToString(Tile tile, std::uint16_t size)
{
    if (tile.index() == 1024)
        return "pass";

    auto column = tile.index() % size;
    const auto row = tile.index() / size;

    if (column > 7)
        column++;

    return static_cast<char>(97 + column) + std::to_string(row + 1);
}