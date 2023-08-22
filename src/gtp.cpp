#include <sstream>

#include "gtp.hpp"

GtpRunner::GtpRunner()
{
    commands.insert({"protocol_version", &GtpRunner::protocolVersion});
    commands.insert({"name", &GtpRunner::name});
    commands.insert({"version", &GtpRunner::version});
    commands.insert({"known_command", &GtpRunner::knownCommand});
    commands.insert({"list_commands", &GtpRunner::listCommands});
    commands.insert({"quit", &GtpRunner::quit});
    commands.insert({"boardsize", &GtpRunner::boardSize});
    commands.insert({"clear_board", &GtpRunner::clearBoard});
    commands.insert({"komi", &GtpRunner::komi});
    commands.insert({"play", &GtpRunner::play});
    commands.insert({"genmove", &GtpRunner::genMove});
    commands.insert({"showboard", &GtpRunner::showBoard});
    commands.insert({"perft", &GtpRunner::perft});
}

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

void GtpRunner::run()
{
    for (std::string line{}; std::getline(std::cin, line);)
	{
        const auto tokens = split_at(line, ' ');
        const auto command = tokens.first;

        if (commands.find(command) == commands.end())
        {
            reportFailure("unknown command");
            continue;
        }

        storedMessage = tokens.second;

        auto func = commands[command];

        func(*this);
    }
}

void GtpRunner::report(char status, std::string message) const
{
    std::cout << status << " " << message << "\n" << std::endl;
}

void GtpRunner::listCommands() const
{
    std::string list = "";
    for (const auto& entry : commands)
    {
        list += "\n";
        list += entry.first;
    }

    reportSuccess(list);
}

void GtpRunner::knownCommand() const
{
    if (commands.find(storedMessage) == commands.end())
        reportSuccess("false");
    else
        reportSuccess("true");
}

void GtpRunner::boardSize()
{
    const auto newSize = std::stoi(storedMessage);
    if (newSize > 25)
        return reportFailure("unacceptable size");
    size = newSize;
}

void GtpRunner::showBoard() const
{
    board.display(false);
}

std::uint64_t runPerft(Board& board, uint8_t depth)
{
    if (depth == 0)
        return 1;

    if (board.board.isGameOver())
        return 0;

    const auto head = board.board.moveHead();
    auto count = 0;

    for (auto move = head.first;; move = board.board[move].next)
    {
        const bool isLegal = board.tryMakeMove(move);
        if (!isLegal)
            continue;

        const auto subCount = runPerft(board, depth - 1);

        count += subCount;

        board.undoMove();

        if (move.isNull())
            break;
    }

    return count;
}

void GtpRunner::perft()
{
    const auto depth = std::stoi(storedMessage);
    const auto count = runPerft(board, depth);
    reportSuccess("nodes " + std::to_string(count));
}