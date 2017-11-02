#ifndef __MT19937_STREAM_HPP__
#define __MT19937_STREAM_HPP__

#include "mt19937.hpp"
#include <random>

class mt19937_stream : public mt19937 {
protected:
    byte_vector byte_queue;

public:
    mt19937_stream(uint32_t seed)
        :mt19937(seed) {
    }

    mt19937_stream() {
        std::random_device rd;
        seed_mt(rd());
    }

    byte_vector next_bytes(size_t num_bytes) {
        byte_vector bytes;

        for (int i = 0; i < (int)num_bytes; i += 1) {
            bytes.push_back(next_byte());
        }

        return bytes;
    }

    byte next_byte() {
        if (byte_queue.empty()) {
            uint32_t r = extract_number();

            for (int i = 0; i < 4; i += 1) {
                byte rb = (byte)((r >> (i * 8)) & 255);
                byte_queue.push_back(rb);
            }
        }

        auto b = byte_queue.back();
        byte_queue.pop_back();

        return b;
    }
};

#endif//__MT19937_STREAM_CIPHER_HPP__
