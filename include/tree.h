#ifndef HW2_TREE_H
#define HW2_TREE_H

#include <array>
#include <cstdint>
#include <vector>
#include <memory>

#include "codes.h"

namespace huffman_impl {
    typedef std::array<std::size_t, 256> frequency_table;

    class huffman_tree {
    public:
        explicit huffman_tree(const frequency_table &frequencies);

        void build_tree();
        code_length_table build_code_lengths();

    private:
        struct node_t {
            std::unique_ptr<node_t> left, right;
            std::size_t freq = 0;
            bool leaf = false;
            uint8_t byte = 0;
        };
        struct node_ptr_less;

        typedef std::unique_ptr<node_t> node_ptr;
        static node_ptr merge(node_ptr left, node_ptr right, uint8_t &aux_cnt);

        void traverse_codes(const node_ptr &node, code_length_table &lens, uint8_t length);

        std::vector<node_ptr> queue;
    };

    code_length_table build_code_lengths(const frequency_table &frequencies);
}

#endif //HW2_TREE_H
