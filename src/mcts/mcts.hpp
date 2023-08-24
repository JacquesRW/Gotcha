#include "tree.hpp"

class Mcts
{
    public:
        std::uint64_t getRandom();

        Node& selectLeaf();

        Node& expandNode(Node& node);



    private:
        Board board;
        SearchTree tree;
        std::uint64_t random = UINT64_C(2078630127);
};
