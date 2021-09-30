#ifndef HW2_CODES_H
#define HW2_CODES_H

#include <climits>
#include <sstream>
#include <array>

namespace huffman_impl {
    class code_t {
    public:
        static constexpr std::size_t bit_size = 64;

        uint8_t len = 0;
        uint64_t data = 0;

        code_t() = default;
        code_t(uint64_t code, uint8_t len);
        code_t(uint64_t code);

        code_t &operator++();
        code_t &operator<<=(uint8_t shl);

        /**
         * Shifts len bits from "right" to "left"
         * (from the least significant bits to the most).
         *
         * @note After this operation the code_t is not valid
         * for other operations.
         *
         * @note Behavior is undefined if len == 0
         */
        void left_align() {
            // implementation should be visible
            data <<= bit_size - len;
        }

    private:
        void recalc_len();
    };

    bool operator==(const code_t &a, const code_t &b);
    bool operator!=(const code_t &a, const code_t &b);
    std::ostream &operator<<(std::ostream &out, const code_t &code);

    /**
     * In this implementation compressed symbols are bytes
     * thus we build tables for bytes.
     */
    typedef std::array<uint8_t, 256> code_length_table;
    typedef std::array<code_t, 256> code_table;

    /**
     * Constructs canonical codes.
     * Link: https://en.wikipedia.org/wiki/Canonical_Huffman_code
     */
    code_table construct_codes(const code_length_table &lengths);
}

#endif //HW2_CODES_H
