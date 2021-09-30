#include <iostream>
#include <string>
#include <stdexcept>
#include <exception>
#include <fstream>

#include "argparse.h"

#include "huffman.h"

struct args_t {
    enum mode_t {
        COMPRESS,
        DECOMPRESS
    } mode = COMPRESS;
    std::string input_filename;
    std::string output_filename;
};

args_t parse_args(int argc, char **argv) {
    argparse::ArgumentParser parser("huffman_impl decoder");

    parser.add_argument("-c")
        .help("Compression mode")
        .default_value(false)
        .implicit_value(true);

    parser.add_argument("-u")
            .help("Decompression mode")
            .default_value(false)
            .implicit_value(true);

    parser.add_argument("-f", "--file")
            .help("Input file")
            .required();

    parser.add_argument("-o", "--output")
            .help("Output file")
            .required();

    parser.parse_args(argc, argv);

    args_t args;
    args.input_filename = parser.get<std::string>("-f");
    args.output_filename = parser.get<std::string>("-o");

    auto compression_mode = parser.get<bool>("-c");
    auto decompression_mode = parser.get<bool>("-u");
    if (!(compression_mode ^ decompression_mode))
        throw std::invalid_argument("You must choose exactly one mode.");
    if (compression_mode)
        args.mode = args_t::mode_t::COMPRESS;
    else
        args.mode = args_t::mode_t::DECOMPRESS;

    return args;
}

void open_files(const args_t& args, std::ifstream &input, std::ofstream &output) {
    input.open(args.input_filename, std::ios::binary);
    if (!input.is_open())
        throw std::invalid_argument("Input file can't be opened.");

    output.open(args.output_filename, std::ios::binary);
    if (!output.is_open())
        throw std::invalid_argument("Output file can't be opened.");
}

int main(int argc, char **argv) {
    try {
        auto args = parse_args(argc, argv);

        std::ifstream input;
        std::ofstream output;
        open_files(args, input, output);

        if (args.mode == args_t::COMPRESS) {
            auto info = huffman::compress_file(input, output);
            std::cout << info.original_file_length << std::endl;
            std::cout << info.compressed_file_body_length << std::endl;
            std::cout << info.compressed_file_header_length << std::endl;
        } else {
            auto info = huffman::decompress_file(input, output);
            std::cout << info.compressed_file_body_length << std::endl;
            std::cout << info.original_file_length << std::endl;
            std::cout << info.compressed_file_header_length << std::endl;
        }

        input.close();
        output.close();
        return 0;

    } catch (const std::exception &e) {
        std::cerr << "Something went wrong:" << std::endl;
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
