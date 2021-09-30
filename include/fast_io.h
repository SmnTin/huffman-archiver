#ifndef HW2_FAST_IO_H
#define HW2_FAST_IO_H

#include <sstream>
#include <cstdint>

namespace huffman_impl {
    /**
     * Fast bit reader. It uses internal buffer to
     * avoid expensive virtual calls of istream
     * therefore the input stream can't be appended
     * after the first read happened.
     */
    class bit_reader {
    public:
        explicit bit_reader(std::istream &in) : in(in) {}

        // length is limited to 16 bits because, more is not needed
        __attribute__((always_inline))
        inline uint16_t read(uint8_t bits);

        inline std::size_t read_bytes() const;

    private:
        static const std::size_t buffer_size = 1024;
        static_assert(buffer_size >= sizeof(uint16_t));

        bool first_read = true;
        uint8_t buffer[buffer_size];
        std::size_t offset = 0;
        std::size_t read_cnt = 0;

        std::istream &in;
    };

    /**
     * std::ostream wrapper with internal buffer
     * to avoid virtual calls.
     */
    class byte_writer {
    public:
        inline explicit byte_writer(std::ostream &out) : out(out) {}

        inline void write(uint8_t byte);
        inline void write(const uint8_t *data, std::size_t n);

        inline void flush();
        inline ~byte_writer();

    private:
        static const std::size_t buffer_size = 1024;
        std::ostream &out;

        uint8_t buffer[buffer_size];
        std::size_t buffer_pnt = 0;
    };

    /**
     * std::istream wrapper with internal buffer
     * to avoid virtual calls.
     */
    class byte_reader {
    public:
        explicit byte_reader(std::istream &in) : in(in) {}

        inline bool read(uint8_t &byte);

    private:
        static const std::size_t buffer_size = 1024;
        std::istream &in;

        uint8_t buffer[buffer_size];
        std::size_t buffer_pnt = 0;
        std::size_t filled_size = 0;
        bool first_read = true;
    };
}

#include "impl/fast_io.h"

#endif //HW2_FAST_IO_H
