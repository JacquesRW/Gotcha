#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

#include "../mcts/mcts.hpp"
#include "../state/board.hpp"

class GtpRunner
{
    public:
        GtpRunner();

        void run();

    private:
        Mcts searcher{};
        std::uint16_t size = 3;
        std::string storedMessage = "";
        std::unordered_map<std::string, std::function<void(GtpRunner&)>> commands{};
        int currId = -1;

        void report(char status, std::string message) const;

        void reportSuccess(std::string message) const { report('=', message); }

        void reportFailure(std::string message) const { report('?', message); }

        void protocolVersion() const { reportSuccess("2"); };

        void name() const { reportSuccess("Gotcha"); };

        void version() const { reportSuccess("1"); };

        void knownCommand() const;

        void listCommands() const;

        void quit() const { exit(EXIT_SUCCESS); }

        void boardSize();

        void clearBoard();

        void komi();

        void play();

        void genMove();

        void showBoard() const;

        void stones();

        void perft();
};