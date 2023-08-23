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
    board = Board(size);
    reportSuccess("");
}

void GtpRunner::clearBoard()
{
    board = Board(size);
    reportSuccess("");
};

void GtpRunner::komi()
{
    board.setKomi(std::stof(storedMessage));
    reportSuccess("");
}

void GtpRunner::showBoard() const
{
    board.display(false);
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

    board.setStm(colour);
    const auto isLegal = board.tryMakeMove(tile);

    if (!isLegal)
        reportFailure("illegal move");

    reportSuccess("");
}

void GtpRunner::genMove()
{
    const auto colour = parseColour(storedMessage);

    board.setStm(colour);

    const auto head = board.board.moveHead();

    Tile move;
    for (move = head.first;; move = board.board[move].next)
    {
        const bool isLegal = board.tryMakeMove(move);
        if (!isLegal)
            continue;
        else
            // at the moment just return first legal move
            break;


        board.undoMove();

        if (move.isNull())
            break;
    }

    const auto moveStr = tileToString(move);

    reportSuccess(moveStr);
}

void GtpRunner::stones()
{
    const auto numStones = board.stones();
    const auto black = std::to_string(numStones[0]);
    const auto white = std::to_string(numStones[1]);
    reportSuccess("black " + black + "white " + white);
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