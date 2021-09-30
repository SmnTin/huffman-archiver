#include "long_code.h"

#include <bitset>
#include <cstring>

namespace huffman_impl {
    const std::size_t long_code_t::blocks_num;

    long_code_t::long_code_t(uint64_t code, uint8_t len) : len(len) {
        blocks[0] = (uint32_t) code;
        blocks[1] = (uint32_t) (code >> 32);
    }

    long_code_t::long_code_t(uint64_t code) : long_code_t(code, 0) {
        recalc_len();
    }

    void long_code_t::recalc_len() {
        uint8_t last_non_zero = 0;
        for (uint8_t i = 0; i < blocks_num; ++i)
            if (blocks[i] > 0)
                last_non_zero = i;

        uint32_t block = blocks[last_non_zero];
        uint8_t msb_pos = 0;
        while (block > 0) {
            block /= 2;
            ++msb_pos;
        }

        len = std::max<uint8_t>(len, last_non_zero * 32 + msb_pos);
    }

    long_code_t &long_code_t::operator++() {
        bool carry = true;
        for (std::size_t i = 0; i < blocks_num && carry; ++i) {
            ++blocks[i]; // signed overflow is UB but unsigned isn't
            carry = blocks[i] == 0;
        }

        recalc_len();

        return *this;
    }

    long_code_t &long_code_t::operator<<=(uint8_t shl) {
        uint8_t big_shl = shl / 32;
        // Using memmove because unlike std::copy it allows overlapping memory regions
        memmove(blocks.data() + big_shl, blocks.data(), (blocks_num - big_shl) * sizeof(uint32_t));
        std::fill(blocks.begin(), blocks.begin() + big_shl, 0);

        uint8_t small_shl = shl % 32;
        if (small_shl != 0) {
            uint32_t shifted_bits_mask = ((1ll << small_shl) - 1);
            uint32_t remainder_bits_mask = shifted_bits_mask << (32 - small_shl);

            uint32_t carry = 0;
            for (std::size_t i = 0; i < blocks_num; ++i) {
                uint32_t p_carry = carry;
                carry = (blocks[i] & remainder_bits_mask) >> (32 - small_shl);
                blocks[i] <<= small_shl;
                blocks[i] |= p_carry;
            }
        }

        len += shl;

        return *this;
    }

    bool operator==(const long_code_t &a, const long_code_t &b) {
        return a.blocks == b.blocks && a.len == b.len;
    }

    bool operator!=(const long_code_t &a, const long_code_t &b) {
        return a.blocks != b.blocks && a.len != b.len;
    }

    std::ostream &operator<<(std::ostream &out, const long_code_t &code) {
        uint8_t b = code.len / 32;
        uint8_t l = code.len % 32;
        for (std::size_t i = l; i--;)
            out << ((code.blocks[b] >> i) & 1);
        for (std::size_t i = b; i--;)
            out << std::bitset<sizeof(code.blocks[0]) * 8>(code.blocks[i]);
        return out;
    }
}