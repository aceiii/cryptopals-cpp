#ifndef __RANDOM_BYTES_HPP__
#define __RANDOM_BYTES_HPP__

#include <random>
#include <limits>

#include "byte_vector.hpp"

class random_bytes
{
    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<byte> dist;

public:
    random_bytes():
        rd(),gen(rd()),
        dist(std::numeric_limits<byte>::min(), std::numeric_limits<byte>::max())
    {
    }

    byte next_byte() {
        return dist(gen);
    }

    byte_vector next_n_bytes(size_t len) {
        byte_vector bytes(len);
        std::generate(begin(bytes), end(bytes), std::bind(&random_bytes::next_byte, this));

        return bytes;
    }
};

#endif//__RANDOM_BYTES_HPP__
