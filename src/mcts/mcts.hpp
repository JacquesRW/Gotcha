#include "tree.hpp"

class Mcts
{
    public:
        Mcts(Board b)
        {
            board = b;
            tree = SearchTree(board);
        }

    private:
        std::uint64_t getRandom();

        Node& selectLeaf();

        Node& expandNode(Node& node);

        Board board;
        SearchTree tree;
        std::uint64_t random = UINT64_C(2078630127);
        std::vector<MoveInfo&> selectionLine{};
};
