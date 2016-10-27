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

byte_vector operator^ (const byte_vector& lhs, const byte& x) {
    const int size = lhs.size();
    byte_vector b(size);

    for (int i = 0; i < size; i += 1) {
        b[i] = lhs[i] ^ x;
    }

    return b;
}

template <typename Acc>
struct xor_key_mode_t {
    Acc acc;

    xor_key_mode_t(Acc accessor):acc(accessor) {}

    const byte& operator [] (size_t pos) const {
        return acc[pos];
    }
};

template <typename T>
struct cycle_t {
    typedef T value_type;

    const T& obj;
    size_t len;

    cycle_t(const T& obj_):obj(obj_),len(obj_.size()) {}

    const typename T::value_type& operator [] (typename T::size_type pos) const {
        return obj[pos % len];
    }
};

template <typename T>
cycle_t<T> cycle(const T& container) {
    return cycle_t<T>(container);
}

xor_key_mode_t<cycle_t<byte_vector>> cycle_xor_key(const byte_vector& b) {
    return xor_key_mode_t<cycle_t<byte_vector>>{cycle_t<byte_vector>(b)};
}

template <typename T>
byte_vector operator ^ (const byte_vector& lhs, const xor_key_mode_t<T>& rhs) {
    size_t len = lhs.size();
    byte_vector b(len);

    for (int i = 0; i < len; i += 1) {
        b[i] = lhs[i] ^ rhs[i];
    }

    return b;
}

#endif//__XOR_HPP__
