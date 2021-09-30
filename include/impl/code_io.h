namespace huffman_impl {
    template<std::size_t bits>
    code_reader<bits>::code_reader(const code_table &codes, std::istream &in)
            : table(build_decode_table<bits>(codes)), reader(bit_reader(in)) {
        buffer = reader.read(bits);
    }

    template<std::size_t bits>
    template<nesting_factor nesting>
    uint8_t code_reader<bits>::read_next() noexcept {
        decode_elem_t *cur = table.front().data();

        /**
         * Here we are trying to improve
         * branch-prediction based on
         * the evidence that table is
         * rarely nested more than 2 or 3
         * times.
         */
        if constexpr(nesting == SINGLE_NESTING) {
            if (cur[buffer].table) {
                cur = table[cur[buffer].table].data();
                buffer <<= bits;
                buffer &= bits_mask;
                buffer |= reader.read(bits);
            }
        } else if constexpr(nesting == UNLIMITED_NESTING) {
            while (cur[buffer].table) {
                cur = table[cur[buffer].table].data();
                buffer <<= bits;
                buffer &= bits_mask;
                buffer |= reader.read(bits);
            }
        }

        const decode_elem_t &cell = cur[buffer];
        buffer <<= cell.len;
        buffer &= bits_mask;
        buffer |= reader.read(cell.len);

        return cell.byte;
    }

    template<std::size_t bits>
    std::size_t code_reader<bits>::read_bytes() const {
        return reader.read_bytes();
    }

    void code_writer::write(code_t code) {
        code.left_align();
        if (code.len >= block_size_bits) {
            append(code.data >> block_size_bits, block_size_bits);
            code.data <<= block_size_bits;
            code.len -= block_size_bits;
            write_buffer_block();
        }
        append(code.data >> block_size_bits, code.len);
        if (len >= block_size_bits)
            write_buffer_block();
    }

    void code_writer::flush() {
        static constexpr std::size_t bits_in_byte = 8;
        uint8_t remaining_bytes = (len + bits_in_byte - 1) / bits_in_byte; // round up
        for (std::size_t i = 0; i < remaining_bytes; ++i) {
            // extracting most significant byte
            auto byte = (uint8_t) (buffer >> (buffer_size_bits - bits_in_byte));
            buffer <<= bits_in_byte; // shift the most significant byte
            write_byte(byte);
        }
        len = 0;

        writer.flush();
    }

    std::size_t code_writer::written_bytes() const {
        return written_bytes_cnt;
    }

    code_writer::~code_writer() {
        flush();
    }

    void code_writer::write_block(uint32_t block) {
        block = htonl(block); // reverse bytes, LE machines only
        writer.write((const uint8_t *) &block, sizeof(uint32_t));
        written_bytes_cnt += sizeof(uint32_t);
    }

    void code_writer::write_byte(uint8_t byte) {
        writer.write(byte);
        ++written_bytes_cnt;
    }

    void code_writer::write_buffer_block() {
        write_block(buffer >> block_size_bits);
        buffer <<= block_size_bits, len -= block_size_bits;
    }

    void code_writer::append(uint32_t block, uint8_t a_len) {
        buffer |= ((uint64_t) block) << (block_size_bits - len);
        len += a_len;
    }
}
