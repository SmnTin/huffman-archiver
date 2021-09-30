#include "doctest.h"

#include "huffman.h"

#include <sstream>

// https://stackoverflow.com/questions/15118661/in-c-whats-the-fastest-way-to-tell-whether-two-string-or-binary-files-are-di
template<typename InputIterator1, typename InputIterator2>
bool range_equal(InputIterator1 first1, InputIterator1 last1,
                 InputIterator2 first2, InputIterator2 last2) {
    while (first1 != last1 && first2 != last2) {
        if (*first1 != *first2) return false;
        ++first1;
        ++first2;
    }
    return (first1 == last1) && (first2 == last2);
}

bool files_equal(const std::string &filename1, const std::string &filename2) {
    std::ifstream file1(filename1);
    std::ifstream file2(filename2);

    std::istreambuf_iterator<char> begin1(file1);
    std::istreambuf_iterator<char> begin2(file2);

    std::istreambuf_iterator<char> end;

    return range_equal(begin1, end, begin2, end);
}

TEST_SUITE("huffman") {
    TEST_CASE("building frequency table") {
        huffman_impl::frequency_table actual_freq, expected_freq{};
        std::size_t actual_size = 0, expected_size = 0;
        std::stringstream in;
        std::vector<uint8_t> in_data;

        SUBCASE("empty stream") {
            in_data = {};
            expected_size = 0;
        }
        SUBCASE("single element") {
            in_data = {
                    255
            };
            expected_freq[255] = 1;
            expected_size = 1;
        }
        SUBCASE("simple one") {
            in_data = {
                    0, 5, 5, 5, 0, 1, 2
            };
            expected_freq[0] = 2, expected_freq[5] = 3;
            expected_freq[1] = expected_freq[2] = 1;
            expected_size = 7;
        }

        CAPTURE(in_data);
        in.write((const char *) in_data.data(), (std::streamsize) in_data.size());

        huffman_impl::build_frequency_table(in, actual_freq, actual_size);

        CHECK(expected_freq == actual_freq);
        CHECK(expected_size == actual_size);
    }

    TEST_CASE("test on files") {
        std::string sample;
        std::string encoded;
        std::string decoded;

        SUBCASE("empty") {
            sample = "samples/empty.txt";
            encoded = "bin/empty.cmp";
            decoded = "bin/empty.txt";
        }

        SUBCASE("small") {
            sample = "samples/small.txt";
            encoded = "bin/small.cmp";
            decoded = "bin/small.txt";
        }

        SUBCASE("pi") {
            sample = "samples/pi.txt";
            encoded = "bin/pi.cmp";
            decoded = "bin/pi.txt";
        }

        SUBCASE("holy bible") {
            sample = "samples/bible.txt";
            encoded = "bin/bible.cmp";
            decoded = "bin/bible.txt";
        }

        CAPTURE(sample);

        std::ifstream sample_in(sample, std::ios::binary);
        std::ofstream encoded_out(encoded, std::ios::binary);
        auto c_info = huffman::compress_file(sample_in, encoded_out);
        sample_in.close();
        encoded_out.close();

        std::ifstream encoded_in(encoded, std::ios::binary);
        std::ofstream decoded_out(decoded, std::ios::binary);
        auto d_info = huffman::decompress_file(encoded_in, decoded_out);
        encoded_in.close();
        decoded_out.close();

        CHECK(c_info.compressed_file_body_length == d_info.compressed_file_body_length);
        CHECK(c_info.compressed_file_header_length == d_info.compressed_file_header_length);
        CHECK(c_info.original_file_length == d_info.original_file_length);

        CHECK(files_equal(sample, decoded));
    }
}