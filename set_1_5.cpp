#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "byte_vector.hpp"
#include "hex.hpp"
#include "xor.hpp"

using namespace std::string_literals;

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

int main() {

    auto str = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal"s;
    auto key = "ICE"s;

    auto str_bytes = str_to_bytes(str);
    auto key_bytes = str_to_bytes(key);

    auto xor_str_bytes = str_bytes ^ cycle_xor_key(key_bytes);
    auto xor_str_hex = bytes_to_hex(xor_str_bytes);

    std::cout << xor_str_hex << std::endl;

    return 0;
}

