#ifndef HW2_HUFFMAN_H
#define HW2_HUFFMAN_H

#include <fstream>

#include "codes.h"
#include "tree.h"

namespace huffman_impl {
    void build_frequency_table(std::istream &in, frequency_table &table, std::size_t &filesize);

    std::size_t encode_body(std::istream &in, std::ostream &out, const code_table &codes);

    std::size_t decode_body(std::istream &in, std::ostream &out,
                            const code_length_table &lengths,
                            const code_table &codes, std::size_t filesize);
}

namespace huffman {
    struct compression_info {
        std::size_t original_file_length = 0;
        std::size_t compressed_file_body_length = 0;
        std::size_t compressed_file_header_length = 0;
    };

    compression_info compress_file(std::ifstream &input_file, std::ofstream &output_file);
    compression_info decompress_file(std::ifstream &input_file, std::ofstream &output_file);
}

#endif //HW2_HUFFMAN_H
