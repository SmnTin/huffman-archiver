#include "doctest.h"

#include "fast_io.h"

using namespace huffman_impl;

TEST_SUITE("bit_reader") {
    TEST_CASE("read") {
        std::stringstream in;
        bit_reader reader(in);

        SUBCASE("read several bit") {
            char byte = (char) 0b11000000;
            in.write(&byte, 1);

            CHECK(reader.read(1) == 0b1);
            CHECK(reader.read(2) == 0b10);
            CHECK(reader.read(3) == 0);
        }

        SUBCASE("read more than byte") {
            char byte1 = (char) 0b11001011;
            char byte2 = (char) 0b11000000;
            in.write(&byte1, 1);
            in.write(&byte2, 1);

            CHECK(reader.read(11) == 0b11001011110);
        }
    }
}