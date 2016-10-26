#ifndef __XOR_HPP__
#define __XOR_HPP__

#include <cassert>
#include "base64.hpp"

byte_vector operator^ (const byte_vector& lhs, const byte_vector& rhs) {
    assert(lhs.size() == rhs.size());
    const int size = lhs.size();
    byte_vector b(size);

    for (int i = 0; i < size; i += 1) {
        b[i] = lhs[i] ^ rhs[i];
    }

    return b;
}

#endif//__XOR_HPP__
