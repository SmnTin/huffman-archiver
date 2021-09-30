#include "huffman.h"

#include "headers.h"
#include "fast_io.h"
#include "code_io.h"

#include <algorithm>

namespace huffman_impl {
    static void rewind(std::ifstream &in) {
        in.clear();
        in.seekg(0);
    }

    void build_frequency_table(std::istream &in, frequency_table &table, std::size_t &filesize) {
        filesize = 0;
        table.fill(0);

        uint8_t byte;
        byte_reader reader(in);
        while (reader.read(byte)) {
            ++table[byte];
            ++filesize;
        }
    }

    std::size_t encode_body(std::istream &in, std::ostream &out, const code_table &codes) {
        uint8_t read_byte;
        byte_reader reader(in);
        code_writer writer(out);
        while (reader.read(read_byte))
            writer.write(codes[read_byte]);
        writer.flush();
        return writer.written_bytes();
    }

    std::size_t decode_body(
            std::istream &in, std::ostream &out,
            const code_length_table &lengths,
            const code_table &codes, std::size_t filesize) {

        /**
         * 9 bits is a good trade-off between memory
         * and time efficiency.

         * 512 entries in a decoding table and decoding
         * 9 bits at a time.
         */
        static const std::size_t bits = 9;

        code_reader<bits> reader(codes, in);
        byte_writer writer(out);

        /**
         * branchless-optimization
         * We define max nesting factor in advance
         * and then use algorithm with reduced
         * branching.
         */
        uint8_t max_code_len = *std::max_element(lengths.begin(), lengths.end());
        std::size_t nesting_ratio = (max_code_len + bits - 1) / bits; // round up

        if (nesting_ratio <= 1) {
            while (filesize) {
                writer.write(reader.read_next<NO_NESTING>());
                --filesize;
            }
        }
        else if (nesting_ratio == 2) {
            while (filesize) {
                writer.write(reader.read_next<SINGLE_NESTING>());
                --filesize;
            }
        } else {
            while (filesize) {
                writer.write(reader.read_next<UNLIMITED_NESTING>());
                --filesize;
            }
        }

        return reader.read_bytes();
    }
}

namespace huffman {
    compression_info compress_file(std::ifstream &input_file, std::ofstream &output_file) {
        huffman_impl::frequency_table freq;
        std::size_t filesize;
        huffman_impl::build_frequency_table(input_file, freq, filesize);

        auto lengths = huffman_impl::build_code_lengths(freq);
        auto codes = huffman_impl::construct_codes(lengths);

        huffman_impl::file_header header {};
        header.filesize = filesize;
        header.lengths = lengths;

        write_file_header(header, output_file);
        huffman_impl::rewind(input_file);
        auto compressed_size = encode_body(input_file, output_file, codes);

        compression_info info;
        info.compressed_file_body_length = compressed_size;
        info.compressed_file_header_length = sizeof(huffman_impl::file_header);
        info.original_file_length = filesize;
        return info;
    }

    compression_info decompress_file(std::ifstream &input_file, std::ofstream &output_file) {
        compression_info info;

        auto header = huffman_impl::read_file_header(input_file);
        auto codes = huffman_impl::construct_codes(header.lengths);
        info.original_file_length = header.filesize;

        std::size_t read_bytes = decode_body(input_file, output_file,
                                             header.lengths, codes, header.filesize);

        info.compressed_file_header_length = sizeof(huffman_impl::file_header);
        info.compressed_file_body_length = read_bytes;

        return info;
    }
}