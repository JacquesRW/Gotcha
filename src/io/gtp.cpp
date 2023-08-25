#include "gtp.hpp"
#include "parse.hpp"

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
    commands.insert({"stones", &GtpRunner::stones});
    commands.insert({"get_komi", &GtpRunner::getKomi});
}

void GtpRunner::run()
{
    for (std::string line{}; std::getline(std::cin, line);)
	{
        auto tokens = splitAt(line, ' ');

        try { currId = std::stoi(tokens.first); }
        catch(...) { currId = -1; }

        if (currId != -1)
            tokens = splitAt(tokens.second, ' ');

        const auto command = tokens.first;

        if (commands.find(command) == commands.end())
        {
            reportFailure("unknown command");
            continue;
        }

        storedMessage = tokens.second;

        auto func = commands[command];

        try { func(*this); }
        catch(...) { reportFailure("unknown command"); }
    }
}

void GtpRunner::report(char status, std::string message) const
{
    std::cout << status;
    if (currId != -1)
        std::cout << currId;
    std::cout << " " << message << "\n" << std::endl;
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
    const auto komi = searcher.board.getKomi();
    searcher.board = Board(size);
    searcher.board.setKomi(komi);
    reportSuccess("");
}

void GtpRunner::clearBoard()
{
    const auto komi = searcher.board.getKomi();
    searcher.board = Board(size);
    searcher.board.setKomi(komi);
    reportSuccess("");
};

void GtpRunner::komi()
{
    searcher.board.setKomi(std::stof(storedMessage));
    reportSuccess("");
}

void GtpRunner::showBoard() const
{
    searcher.board.display(false);
}

void GtpRunner::play()
{
    std::pair<Tile, Colour> move;
    try { move = parseMove(storedMessage, size); }
    catch(...)
    {
        reportFailure("illegal move");
        return;
    }

    const auto tile = move.first;
    const auto colour = move.second;

    searcher.board.setStm(colour);
    const auto isLegal = searcher.board.tryMakeMove(tile);

    if (isLegal)
        reportSuccess("");
    else
        reportFailure("illegal move");
}

void GtpRunner::genMove()
{
    const auto colour = parseColour(storedMessage);

    searcher.board.setStm(colour);

    const auto move = searcher.search();

    searcher.board.makeMove(move);

    const auto moveStr = tileToString(move, searcher.board.size());
    reportSuccess(moveStr);
}

void GtpRunner::stones()
{
    const auto numStones = searcher.board.stones();
    const auto black = std::to_string(numStones[0]);
    const auto white = std::to_string(numStones[1]);
    reportSuccess("black " + black + "white " + white);
}

void GtpRunner::perft()
{
    const auto depth = std::stoi(storedMessage);
    const auto count = searcher.board.runPerft(depth);
    reportSuccess("nodes " + std::to_string(count));
}

void GtpRunner::getKomi() const
{
    const auto komi = searcher.board.getKomi();
    reportSuccess(std::to_string(komi));
}