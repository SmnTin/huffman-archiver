#include <cstring>
#include <cassert>

// htons
#ifdef _WIN32
#include <winsock.h>
#else

#include <arpa/inet.h>

#endif

namespace huffman_impl {
    uint16_t bit_reader::read(uint8_t bits) {
        static constexpr std::size_t bits_in_byte = 8;
        assert(bits <= 2 * bits_in_byte);

        std::size_t byte_pos = offset / bits_in_byte;
        std::size_t bit_pos = offset % bits_in_byte;

        // this condition happens rarely so it is predicted well
        if (first_read || byte_pos == buffer_size) {
            in.read((char *) buffer, buffer_size);
            first_read = false, offset = byte_pos = 0;

            std::size_t read_cnt_cur = in.gcount();
            read_cnt += read_cnt_cur;
            memset(buffer + read_cnt_cur, 0, buffer_size - read_cnt_cur); // for valgrind only
        } else if (byte_pos + 1 == buffer_size) {
            // special case to make two contiguous bytes of the buffer be always available for reading
            buffer[0] = buffer[byte_pos];
            in.read((char *) buffer + 1, buffer_size - 1);
            offset = bit_pos, byte_pos = 0;

            std::size_t read_cnt_cur = in.gcount();
            read_cnt += read_cnt_cur;
            memset(buffer + 1 + read_cnt_cur, 0, buffer_size - read_cnt_cur - 1); // for valgrind only
        }

        // the following part is made branch-less

        // make byte order it big-endian
        uint16_t block = htons(*(uint16_t *) (buffer + byte_pos));

        uint16_t mask = (1 << bits) - 1;
        uint16_t res = (block >> (16 - bit_pos - bits)) & mask;
        offset += bits;
        return res;
    }

    std::size_t bit_reader::read_bytes() const {
        return read_cnt; // rounding up
    }

    void byte_writer::write(uint8_t byte) {
        if (buffer_pnt == buffer_size)
            flush();
        buffer[buffer_pnt++] = byte;
    }

    void byte_writer::write(const uint8_t *data, std::size_t n) {
        if (buffer_pnt + n > buffer_size) {
            std::size_t prefix = buffer_size - buffer_pnt;
            memcpy(buffer + buffer_pnt, data, prefix);
            buffer_pnt += prefix, data += prefix, n -= prefix;
            flush();
        }
        memcpy(buffer + buffer_pnt, data, n);
        buffer_pnt += n;
    }

    void byte_writer::flush() {
        out.write((const char *) buffer, (std::streamsize) buffer_pnt);
        buffer_pnt = 0;
    }

    byte_writer::~byte_writer() {
        flush();
    }

    bool byte_reader::read(uint8_t &byte) {
        if (first_read || buffer_pnt == buffer_size) {
            in.read((char *) &buffer, buffer_size);
            filled_size = in.gcount(), buffer_pnt = 0;
            first_read = false;
        }
        byte = buffer[buffer_pnt++];
        return buffer_pnt <= filled_size;
    }
}
