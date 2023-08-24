#include "tree.hpp"

class Mcts
{
    public:
        Board board;

        Mcts()
        {
            board = Board(5);
            tree = SearchTree(board);
            capacity = 100;
        }

        Tile search();

    private:
        std::uint64_t getRandom();

        std::int32_t selectLeaf();

        std::int32_t expandNode(std::int32_t nodePtr);

        State simulate();

        void backprop(State result);

        SearchTree tree;
        std::uint64_t random = UINT64_C(2078630127);
        std::int32_t capacity{};
        std::vector<std::int32_t> selectionLine{};
};
