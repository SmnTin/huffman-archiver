#include "doctest.h"

#include "codes.h"

#include <vector>

using namespace huffman_impl;

TEST_SUITE("code_t") {
    TEST_CASE("constructor") {
        SUBCASE("single param") {
            uint8_t shl = 35;
            uint64_t val = 1ll << shl;
            code_t code(val);

            CHECK(code == code_t(val, shl + 1));
        }

        SUBCASE("zero") {
            code_t code(0);
            CHECK(code.len == 0);
        }

        SUBCASE("zero with len") {
            uint8_t len = 50;
            code_t code(0, len);
            CHECK(code.len == len);
        }

        SUBCASE("full of ones") {
            code_t code(UINT64_MAX);
            CHECK(code == code_t(UINT64_MAX, 64));
        }
    }

    TEST_CASE("increment") {
        SUBCASE("zero to one") {
            code_t code(0);
            ++code;
            CHECK(code == code_t(1));
        }

        SUBCASE("zero to one with fixed len") {
            uint8_t len = 5;
            code_t code(0, len);
            ++code;
            CHECK(code == code_t(1, len));
        }

        SUBCASE("fixed len overflow") {
            uint8_t len = 5;
            code_t code((1 << len) - 1, len);
            ++code;
            CHECK(code == code_t(1 << len, len + 1));
        }
    }

    TEST_CASE("shift left") {
        SUBCASE("shl 0") {
            code_t code(1);
            code <<= 0;
            CHECK(code == code_t(1));
        }

        SUBCASE("1 shl 5") {
            uint8_t shl = 5;
            code_t code(1);
            code <<= shl;
            CHECK(code == code_t(1 << shl));
        }

        SUBCASE("1 shl 5 with fixed len") {
            uint8_t len = 4;
            uint8_t shl = 5;
            code_t code(1, len);
            code <<= shl;
            CHECK(code == code_t(1 << shl, len + shl));
        }
    }
}

TEST_SUITE("canonical codes") {
    TEST_CASE("construction") {
        code_length_table lens{};
        code_table expected;

        SUBCASE("simple one") {
            lens['a'] = 2;
            lens['b'] = 3;
            lens['c'] = 1;
            lens['d'] = 3;

            expected['c'] = code_t(0b0, 1);
            expected['a'] = 0b10;
            expected['b'] = 0b110;
            expected['d'] = 0b111;
        }

        SUBCASE("wiki one") {
            lens[' '] = lens['a'] = lens['e'] = 3;
            lens['f'] = lens['h'] = 4;
            lens['i'] = lens['m'] = 4;
            lens['n'] = lens['s'] = lens['t'] = 4;
            lens['l'] = lens['o'] = lens['p'] = 5;
            lens['r'] = lens['u'] = lens['x'] = 5;

            expected[' '] = code_t(0b000, 3);
            expected['a'] = code_t(0b001, 3);
            expected['e'] = code_t(0b010, 3);
            expected['f'] = code_t(0b0110, 4);
            expected['h'] = code_t(0b0111, 4);
            expected['i'] = code_t(0b1000, 4);
            expected['m'] = code_t(0b1001, 4);
            expected['n'] = code_t(0b1010, 4);
            expected['s'] = code_t(0b1011, 4);
            expected['t'] = code_t(0b1100, 4);
            expected['l'] = code_t(0b11010, 5);
            expected['o'] = code_t(0b11011, 5);
            expected['p'] = code_t(0b11100, 5);
            expected['r'] = code_t(0b11101, 5);
            expected['u'] = code_t(0b11110, 5);
            expected['x'] = code_t(0b11111, 5);
        }

        auto actual = construct_codes(lens);
        for (std::size_t i = 0; i < 256; ++i)
            CHECK(expected[i] == actual[i]);
    }
}