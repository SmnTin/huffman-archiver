#include "doctest.h"

#include "tree.h"

using namespace huffman_impl;

TEST_SUITE("huffman_impl tree") {
    TEST_CASE("code length table construction") {
        frequency_table freq {};
        code_length_table exp_lens {};

        SUBCASE("simple one") {
            freq[0] = 10;
            freq[1] = 1;
            freq[2] = 15;
            freq[3] = 7;

            exp_lens[0] = 2;
            exp_lens[1] = 3;
            exp_lens[2] = 1;
            exp_lens[3] = 3;
        }

        // https://en.wikipedia.org/wiki/Huffman_coding
        SUBCASE("wiki example") {
            freq[' '] = 7;
            freq['a'] = freq['e'] = 4;
            freq['f'] = 3;
            freq['h'] = freq['i'] = freq['m'] = 2;
            freq['n'] = freq['s'] = freq['t'] = 2;
            freq['l'] = freq['o'] = freq['p'] = 1;
            freq['r'] = freq['u'] = freq['x'] = 1;

            exp_lens[' '] = exp_lens['a'] = exp_lens['e'] = 3;
            exp_lens['f'] = exp_lens['h'] = 4;
            exp_lens['i'] = exp_lens['m'] = 4;
            exp_lens['n'] = exp_lens['s'] = exp_lens['t'] = 4;
            exp_lens['l'] = exp_lens['o'] = exp_lens['p'] = 5;
            exp_lens['r'] = exp_lens['u'] = exp_lens['x'] = 5;
        }

        auto actual_lens = build_code_lengths(freq);
        CHECK(exp_lens == actual_lens);
    }
}