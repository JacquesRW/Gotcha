#pragma once

#include <sstream>

#include "../state/core.hpp"

Colour parseColour(std::string& str);

std::pair<std::string, std::string> splitAt(const std::string &str, char delim);

std::pair<Tile, Colour> parseMove(std::string &moveStr, std::uint16_t size);

std::string tileToString(Tile tile);
