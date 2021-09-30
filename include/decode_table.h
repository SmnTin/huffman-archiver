#ifndef HW2_DECODE_TABLE_H
#define HW2_DECODE_TABLE_H

#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <array>

#include "codes.h"

namespace huffman_impl {
    /**
     * Decode table is recursively nested table.
     * It allows for a much faster decoding than
     * reading bit by bit and working with the tree
     * directly.
     * Codes that are less that bit width of indices
     * are decoded in a single lookup.
     *
     * Link: https://commandlinefanatic.com/cgi-bin/showarticle.cgi?article=art007
     */
    struct decode_elem_t {
        uint16_t table = 0; // 0 means it points nowhere
        uint8_t byte = 0, len = 0;
    };

    template<std::size_t bits>
    using decode_table_t = std::array<decode_elem_t, 1 << bits>;

    /**
     * Storing tables in a contiguous block of memory improves cache locality.
     */
    template<std::size_t bits>
    using decode_table = std::vector<decode_table_t<bits>>;

    template<std::size_t bits = 8>
    decode_table<bits> build_decode_table(const code_table &codes);
}

#include "impl/decode_table.h"

#endif //HW2_DECODE_TABLE_H
