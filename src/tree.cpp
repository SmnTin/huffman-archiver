#include "tree.h"

#include <algorithm>
#include <cassert>

namespace huffman_impl {
    struct huffman_tree::node_ptr_less {
        bool operator()(const node_ptr &first, const node_ptr &second) {
            if (first->freq != second->freq)
                return first->freq < second->freq;
            else if (first->leaf != second->leaf)
                return !first->leaf;
            else
                return first->byte < second->byte;
        }
    };

    huffman_tree::huffman_tree(const frequency_table &frequencies) {
        for (std::size_t byte = 0; byte <= UINT8_MAX; ++byte) {
            if (frequencies[byte] == 0)
                continue;

            node_ptr node = std::make_unique<node_t>();
            node->freq = frequencies[byte];
            node->leaf = true;
            node->byte = (uint8_t) byte;

            queue.push_back(std::move(node));
        }
    }

    void huffman_tree::build_tree() {
        auto cmp = std::not_fn(node_ptr_less());
        std::make_heap(queue.begin(), queue.end(), cmp);

        uint8_t aux_cnt = 0;
        while (queue.size() > 1) {
            std::pop_heap(queue.begin(), queue.end(), cmp);
            node_ptr first = std::move(queue.back());
            queue.pop_back();

            std::pop_heap(queue.begin(), queue.end(), cmp);
            node_ptr second = std::move(queue.back());
            queue.pop_back();

            queue.push_back(merge(std::move(first), std::move(second), aux_cnt));
        }
    }

    code_length_table huffman_tree::build_code_lengths() {
        code_length_table lens {};

        if (!queue.empty()) {
            assert(queue.size() == 1);
            const node_ptr &root = queue.front();

            // corner-case: singe character in the input
            uint8_t initial_len = root->leaf;
            traverse_codes(root, lens, initial_len);
        }

        return lens;
    }

    huffman_tree::node_ptr huffman_tree::merge(node_ptr left, node_ptr right, uint8_t &aux_cnt) {
        node_ptr result = std::make_unique<node_t>();
        result->freq = left->freq + right->freq;
        result->left = std::move(left);
        result->right = std::move(right);
        result->leaf = false;
        result->byte = aux_cnt++;
        return result;
    }

    void huffman_tree::traverse_codes(const node_ptr &node, code_length_table &lens, uint8_t length) {
        if (!node->leaf) {
            traverse_codes(node->left, lens, length + 1);
            traverse_codes(node->right, lens, length + 1);
        } else {
            lens[node->byte] = length;
        }
    }

    code_length_table build_code_lengths(const frequency_table &frequencies) {
        huffman_tree tree(frequencies);
        tree.build_tree();
        return tree.build_code_lengths();
    }
}