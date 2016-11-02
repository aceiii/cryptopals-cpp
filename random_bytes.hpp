#ifndef __RANDOM_BYTES_HPP__
#define __RANDOM_BYTES_HPP__

#include <random>
#include <limits>

#include "byte_vector.hpp"
#include "random_int.hpp"

class random_bytes: public random_int<byte> {
public:
    random_bytes():
        random_int<byte>(std::numeric_limits<byte>::min(), std::numeric_limits<byte>::max())
    {
    }
};

#endif//__RANDOM_BYTES_HPP__
