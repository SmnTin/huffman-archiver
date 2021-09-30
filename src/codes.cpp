#include "codes.h"

#include <algorithm>
#include <vector>
#include <utility>
#include <cassert>

namespace huffman_impl {
    code_t::code_t(uint64_t code) : data(code) {
        recalc_len();
    }

    code_t::code_t(uint64_t code, uint8_t len) : data(code), len(len) {}

    code_t &code_t::operator++() {
        ++data;
        recalc_len();
        return *this;
    }

    code_t &code_t::operator<<=(uint8_t shl) {
        assert(shl < bit_size);
        data <<= shl;
        len += shl;
        return *this;
    }

    void code_t::recalc_len() {
        uint64_t d = data;
        uint8_t msb_pos = 0;
        while (d > 0) {
            d /= 2;
            ++msb_pos;
        }
        len = std::max(len, msb_pos);
    }

    bool operator==(const code_t &a, const code_t &b) {
        return a.len == b.len && a.data == b.data;
    }

    bool operator!=(const code_t &a, const code_t &b) {
        return a.len != b.len && a.data != b.data;
    }

    std::ostream &operator<<(std::ostream &out, const code_t &code) {
        for (std::size_t i = code.len; i--;)
            out << ((code.data >> i) & 1);
        return out;
    }

    code_table construct_codes(const code_length_table &lengths) {
        std::vector<std::pair<uint8_t, uint8_t>> length_byte_pairs;
        length_byte_pairs.reserve(256);
        for (std::size_t byte = 0; byte < 256; ++byte)
            if (lengths[byte] > 0)
                length_byte_pairs.emplace_back(lengths[byte], byte);
        // Lexicographical order
        std::sort(length_byte_pairs.begin(), length_byte_pairs.end());

        code_table codes {};
        code_t current_code;
        for (auto[len, byte] : length_byte_pairs) {
            current_code <<= len - current_code.len;
            codes[byte] = current_code;
            ++current_code;
        }

        return codes;
    }
}