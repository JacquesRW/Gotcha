#include "tree.hpp"

class Mcts
{
    public:
        Mcts(Board b, std::int32_t cap)
        {
            board = b;
            tree = SearchTree(board, cap);
        }

    private:
        std::uint64_t getRandom();

        std::int32_t selectLeaf();

        std::int32_t expandNode(std::int32_t nodePtr);

        Board board;
        SearchTree tree;
        std::uint64_t random = UINT64_C(2078630127);
        std::vector<std::int32_t> selectionLine{};
};
