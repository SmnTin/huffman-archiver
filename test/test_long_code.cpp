#include "doctest.h"

#include "long_code.h"

using namespace huffman_impl;

TEST_SUITE("long_code_t") {
    TEST_CASE("constructor") {
        SUBCASE("single param") {
            uint8_t shl = 35;
            uint64_t val = 1ll << shl;
            long_code_t code(val);

            long_code_t expected;
            expected.blocks[shl / 32] = 1 << (shl % 32);
            expected.len = shl + 1;

            CHECK(expected == code);
        }

        SUBCASE("zero") {
            long_code_t code(0);
            CHECK(code.len == 0);
        }

        SUBCASE("zero with len") {
            uint8_t len = 50;
            long_code_t code(0, len);
            CHECK(code.len == len);
        }

        SUBCASE("two blocks of ones") {
            long_code_t code(UINT64_MAX);

            long_code_t expected;
            expected.blocks[0] = expected.blocks[1] = UINT32_MAX;
            expected.len = 64;
            CHECK(code == expected);
        }
    }

    TEST_CASE("increment") {
        SUBCASE("zero to one") {
            long_code_t code(0);
            ++code;
            CHECK(code == long_code_t(1));
        }

        SUBCASE("zero to one with fixed len") {
            uint8_t len = 5;
            long_code_t code(0, len);
            ++code;
            CHECK(code == long_code_t(1, len));
        }

        SUBCASE("fixed len overflow") {
            uint8_t len = 5;
            long_code_t code((1 << len) - 1, len);
            ++code;
            CHECK(code == long_code_t(1 << len, len + 1));
        }

        SUBCASE("block len overflow") {
            uint8_t len = 32;
            long_code_t code((1ll << len) - 1);
            ++code;
            CHECK(code == long_code_t(1ll << len));
        }
    }

    TEST_CASE("shift left") {
        SUBCASE("shl 0") {
            long_code_t code(1);
            code <<= 0;
            CHECK(code == long_code_t(1));
        }

        SUBCASE("1 shl 5") {
            uint8_t shl = 5;
            long_code_t code(1);
            code <<= shl;
            CHECK(code == long_code_t(1 << shl));
        }

        SUBCASE("1 shl 5 with fixed len") {
            uint8_t len = 4;
            uint8_t shl = 5;
            long_code_t code(1, len);
            code <<= shl;
            CHECK(code == long_code_t(1 << shl, len + shl));
        }

        SUBCASE("shl through blocks") {
            uint8_t shl = 5;
            uint64_t data = 0b10111;
            long_code_t code(data << (32 - shl));
            code <<= shl;
            CHECK(code == long_code_t(data << 32));
        }

        SUBCASE("big shl through blocks") {
            uint8_t width = 5;
            uint64_t data = (uint64_t) 0b10111 << (32 - width);
            long_code_t code(data);
            code <<= 32 * 3;

            long_code_t expected;
            expected.blocks[3] = data;
            expected.len = 32 * 4;
            CHECK(code == expected);
        }
    }
}