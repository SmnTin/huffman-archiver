#include "doctest.h"

#include <vector>

#include "code_io.h"

using namespace huffman_impl;

TEST_SUITE("code_writer") {
    TEST_CASE("writing") {
        std::vector<uint8_t> expected;
        std::stringstream out;
        code_writer writer(out);

        SUBCASE("one byte code") {
            uint8_t byte = 'a';
            code_t code(byte, 8);
            writer.write(code);

            expected.push_back(byte);
        }

        SUBCASE("many one bit codes") {
            code_t code(1, 1);
            std::size_t num = 100;
            for (std::size_t i = 0; i < num; ++i)
                writer.write(1);

            std::size_t bytes = num / 8;
            std::size_t rem_bits = num % 8;

            for (std::size_t i = 0; i < bytes; ++i)
                expected.push_back(0xFF);
            expected.push_back(~((1u << (8 - rem_bits)) - 1));
        }

        SUBCASE("long code") {
            code_t code(0xFBABCDEDAFBEB0, 7 * 8);
            writer.write(code);

            expected.push_back(0xFB);
            expected.push_back(0xAB);
            expected.push_back(0xCD);
            expected.push_back(0xED);
            expected.push_back(0xAF);
            expected.push_back(0xBE);
            expected.push_back(0xB0);
        }

        SUBCASE("code after code") {
            code_t code1(0x1FF, 8 + 1);
            code_t code2(0x7BABCD, 3 * 8 - 1);
            writer.write(code1);
            writer.write(code2);

            expected.push_back(0xFF);
            expected.push_back(0xFB);
            expected.push_back(0xAB);
            expected.push_back(0xCD);
        }

        writer.flush();
        std::vector<uint8_t> actual;
        uint8_t read_byte;
        while (out.read((char *) &read_byte, 1))
            actual.push_back(read_byte);

        CHECK(actual == expected);
    }

    TEST_CASE("double flush") {
        std::stringstream out;
        code_t code = code_t(0b111111, 6);

        code_writer writer(out);
        writer.write(code);

        // once
        writer.flush();
        CHECK(writer.written_bytes() == 1);

        // and again
        writer.flush();
        CHECK(writer.written_bytes() == 1);
    }
}

TEST_SUITE("code_reader") {
    TEST_CASE("read with small codes") {
        code_table codes;
        codes['a'] = code_t(0b0, 1);
        codes['b'] = code_t(0b100, 3);
        codes['c'] = code_t(0b101, 3);
        codes['d'] = code_t(0b110, 3);
        codes['e'] = code_t(0b111, 3);

        std::stringstream ss;
        SUBCASE("one by one") {
            uint8_t data[] = {0b01001011, 0b10111101};
            for (uint8_t byte : data)
                ss.write((const char *) &byte, 1);

            std::string expected_str = {'a', 'b', 'c', 'd', 'e', 'c'};

            code_reader<3> reader(codes, ss);
            std::string real_str;
            for (std::size_t i = 0; i < expected_str.size(); ++i)
                real_str += (char) reader.read_next();

            CHECK(expected_str == real_str);
        }
    }

    TEST_CASE("read with big codes") {
        code_table codes;
        codes['a'] = code_t(0b0, 1);
        codes['b'] = code_t(0b100, 3);
        codes['c'] = code_t(0b111000100011100010001110001000, 30);
        codes['d'] = code_t(0b101010101, 9);

        std::stringstream ss;
        SUBCASE("one by one") {
            uint8_t data[] = {0b01001110, 0b00100011, 0b10001000, 0b11100010, 0b00101010, 0b10100000};
            for (uint8_t byte : data)
                ss.write((const char *) &byte, 1);

            std::string expected_str = {'a', 'b', 'c', 'd'};

            code_reader<9> reader(codes, ss);
            std::string real_str;
            for (std::size_t i = 0; i < expected_str.size(); ++i)
                real_str += (char) reader.read_next();

            CHECK(expected_str == real_str);
        }
    }
}