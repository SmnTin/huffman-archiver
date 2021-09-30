#ifndef HW2_LONG_CODE_H
#define HW2_LONG_CODE_H

#include <climits>
#include <cstdint>
#include <sstream>
#include <array>

namespace huffman_impl {

    /**
     * Firstly, I wrote this long arithmetics version
     * because codes on the alphabet of
     * 256 symbols can theoretically be up to 255 bits long.
     * But then I realized that in practice file with such
     * frequency table can't fit into any disk size.
     * Moreover, its size can't be represented with std::size_t.
     *
     * So I left this implementation in case I would need it in
     * the future. Unfortunately, I wasted a lot of time to
     * cover it with tests and debug it.
     */
    class long_code_t {
    public:
        uint8_t len = 0; // max len for 256 symbols is 255 bits
        static const std::size_t blocks_num = 8; // 255 bits fit in 32 blocks
        std::array<uint32_t, blocks_num> blocks {};

        long_code_t() = default;
        long_code_t(uint64_t code, uint8_t len);
        long_code_t(uint64_t code);

        long_code_t &operator++();
        long_code_t &operator<<=(uint8_t shl);

    private:
        void recalc_len();
    };

    bool operator==(const long_code_t &a, const long_code_t &b);
    bool operator!=(const long_code_t &a, const long_code_t &b);
    std::ostream &operator<<(std::ostream &out, const long_code_t &code);
}

#endif //HW2_LONG_CODE_H
