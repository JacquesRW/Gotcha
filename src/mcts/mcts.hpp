#include "timer.hpp"
#include "tree.hpp"

class Mcts
{
    public:
        Board board;
        bool logging = false;

        Mcts()
        {
            board = Board(3);
            tree = SearchTree(board);
            maxNodes = 10000;
            timer = Timer(0, 1000, 1);
        }

        Tile search();

        void setNodes(std::int32_t nodes) { maxNodes = nodes; }

    private:
        double getUct(const Node& node, std::uint32_t childIdx);

        std::uint64_t getRandom();

        std::int32_t selectLeaf();

        void expandNode(std::int32_t nodePtr);

        State simulate();

        void backprop(State result);

        SearchTree tree;
        Timer timer;
        std::uint64_t random = UINT64_C(2078630127);
        std::int32_t maxNodes{};
        std::vector<std::int32_t> selectionLine{};
};
