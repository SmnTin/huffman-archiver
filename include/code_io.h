#ifndef HW2_CODE_IO_H
#define HW2_CODE_IO_H

#include <sstream>

#include "decode_table.h"
#include "fast_io.h"

namespace huffman_impl {
    enum nesting_factor {
        NO_NESTING,
        SINGLE_NESTING,
        UNLIMITED_NESTING
    };

    /**
     * This implementation should be visible
     * to other parts to be inlined.
     */
    template<std::size_t bits = 8>
    class code_reader {
    public:
        inline code_reader(const code_table &codes, std::istream &in);

        // profiling showed that this function is not
        // inlined automatically but inlining it
        // gives significant speed boost
        template<nesting_factor nesting = UNLIMITED_NESTING>
        __attribute__((always_inline))
        inline uint8_t read_next() noexcept;

        inline std::size_t read_bytes() const;

    private:
        static constexpr std::size_t bits_mask = (1 << bits) - 1;

        decode_table<bits> table;
        bit_reader reader;
        uint16_t buffer;
    };

    /**
     * This implementation should be visible
     * to other parts to be inlined.
     */
    class code_writer {
    public:
        explicit code_writer(std::ostream &out) : writer(out) {}

        __attribute__((always_inline))
        inline void write(code_t code);

        inline std::size_t written_bytes() const;

        inline void flush();
        inline ~code_writer();

    private:
        static constexpr std::size_t buffer_size_bits = 64;
        static constexpr std::size_t block_size_bits = 32;

        byte_writer writer;
        uint8_t len = 0;
        uint64_t buffer = 0;
        std::size_t written_bytes_cnt = 0;

        inline void write_block(uint32_t block);
        inline void write_byte(uint8_t byte);
        inline void write_buffer_block();

        inline void append(uint32_t block, uint8_t a_len);
    };
}

#include "impl/code_io.h"

#endif //HW2_CODE_IO_H
