#include "headers.h"

#include <cstring>

namespace huffman_impl {
    file_header read_file_header(std::ifstream &input_file) {
        file_header header {};
        if (!input_file.read(reinterpret_cast<char *>(&header), sizeof(header)))
            throw invalid_file_header("File header can't be read.");
        if (memcmp(header.signature, correct_signature, sizeof(correct_signature)) != 0)
            throw invalid_file_header("Signature doesn't match.");
        return header;
    }

    void write_file_header(file_header header, std::ofstream &output_file) {
        memcpy(header.signature, correct_signature, sizeof(correct_signature));
        output_file.write(reinterpret_cast<const char *>(&header), sizeof(header));
    }
}