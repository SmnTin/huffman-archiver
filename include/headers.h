#ifndef HW2_HEADERS_H
#define HW2_HEADERS_H

#include <cstddef>
#include <fstream>
#include <exception>

#include "codes.h"

namespace huffman_impl {
    const char correct_signature[5] = {'%', 'H', 'U', 'F', 'F'};

    struct __attribute__((packed)) file_header {
        char signature[sizeof(correct_signature)] {};
        uint64_t filesize = 0; // original file length in bytes
        code_length_table lengths {};
    };

    file_header read_file_header(std::ifstream &input_file);
    void write_file_header(file_header header, std::ofstream &output_file);

    class invalid_file_header : public std::invalid_argument {
    public:
        explicit invalid_file_header(const std::string &what)
                : std::invalid_argument("Invalid file header. " + what) {}
    };
}

#endif //HW2_HEADERS_H
