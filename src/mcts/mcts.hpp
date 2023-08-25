#include "tree.hpp"

class Mcts
{
    public:
        Board board;

        Mcts()
        {
            board = Board(3);
            tree = SearchTree(board);
            capacity = 1000;
        }

        Tile search();

        void setNodes(std::int32_t nodes) { capacity = nodes; }

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
