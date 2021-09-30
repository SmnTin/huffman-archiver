#include "doctest.h"

#include "decode_table.h"

using namespace huffman_impl;

TEST_SUITE("decode_table") {
    TEST_CASE("build") {
        code_table codes;
        codes['a'] = code_t(0b0, 1);
        codes['b'] = code_t(0b100, 3);
        codes['c'] = code_t(0b101, 3);
        codes['d'] = code_t(0b110, 3);
        codes['e'] = code_t(0b111, 3);

        SUBCASE("bit width >= max len") {
            decode_table<4> table = build_decode_table<4>(codes);
            decode_table_t<4> &root = table.front();

            CHECK(root[0b0000].byte == 'a');
            CHECK(root[0b0000].len == 1);
            CHECK(root[0b0100].byte == 'a');
            CHECK(root[0b0100].len == 1);

            CHECK(root[0b1000].byte == 'b');
            CHECK(root[0b1000].len == 3);
            CHECK(root[0b1001].byte == 'b');
            CHECK(root[0b1001].len == 3);

            CHECK(root[0b1010].byte == 'c');
            CHECK(root[0b1010].len == 3);
            CHECK(root[0b1011].byte == 'c');
            CHECK(root[0b1011].len == 3);
        }

        SUBCASE("bit width < max len") {
            decode_table<2> table = build_decode_table<2>(codes);
            decode_table_t<2> &root = table.front();

            REQUIRE(root[0b10].table);
            CHECK(root[0b10].len == 2);

            CHECK(root[0b11].table);
            CHECK(root[0b11].len == 2);

            decode_table_t<2> &nested = table[root[0b10].table];
            CHECK(nested[0b00].byte == 'b');
            CHECK(nested[0b00].len == 1);
            CHECK(nested[0b01].byte == 'b');
            CHECK(nested[0b01].len == 1);
        }

        SUBCASE("bit width == max len") {
            decode_table<3> table = build_decode_table<3>(codes);
            decode_table_t<3> &root = table.front();

            CHECK(root[0b100].byte == 'b');
            CHECK(root[0b100].len == 3);
            CHECK(root[0b100].table == 0);

            CHECK(root[0b101].byte == 'c');
            CHECK(root[0b101].len == 3);
            CHECK(root[0b101].table == 0);
        }
    }
}