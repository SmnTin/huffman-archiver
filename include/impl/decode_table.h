namespace huffman_impl {
    /**
     * This algorithm is very similar to the construction of a trie.
     */
    template<std::size_t bits>
    decode_table<bits> build_decode_table(const code_table &codes)  {
        decode_table<bits> table;
        table.emplace_back();

        for (std::size_t byte = 0; byte < 256; ++byte) {
            code_t code = codes[byte];
            if (code.len == 0)
                continue;

            code.left_align();
            uint64_t data = code.data; // left align

            std::size_t blocks = (code.len + bits - 1) / bits;
            decode_elem_t *cur = table.front().data();
            uint16_t cur_ind = 0;
            for (std::size_t i = 0; i < blocks; ++i) {
                std::size_t remainder = code.len % bits;
                std::size_t rem_round_up = (remainder ? remainder : bits);
                std::size_t block_len = (i + 1 < blocks ? bits : rem_round_up);

                std::size_t mask = ~((1 << (bits - block_len)) - 1);
                std::size_t block = (data >> (64 - bits)) & mask;
                data <<= block_len;

                if (i + 1 == blocks) { // leaf table
                    for (auto index = block; (index & mask) == block; ++index) {
                        cur[index].len = block_len;
                        cur[index].byte = byte;
                    }
                } else { // needs nesting
                    if (!cur[block].table) {
                        table.emplace_back();
                        cur = table[cur_ind].data(); // extract the value again because of possible reallocation

                        cur[block].len = block_len;
                        cur[block].table = table.size() - 1; // last index
                    }
                    cur_ind = cur[block].table;
                    cur = table[cur_ind].data();
                }
            }
        }
        return table;
    }
}