#ifndef __PADDING_HPP__
#define __PADDING_HPP__

#include <string>
#include <iterator>
#include "byte_vector.hpp"

template <typename T, typename OutputIt>
void pkcs7(const T& obj, size_t padded_size, OutputIt it) {
    assert(padded_size >= obj.size());

    size_t diff = padded_size - obj.size();
    for (int i = 0; i < diff; i += 1) {
        *it++ = (typename T::value_type) diff;
    }
}

template <typename T>
void pad_pkcs7(T& obj, size_t padded_size) {
    pkcs7(obj, padded_size, std::back_inserter(obj));
}

template <typename T>
T padded_pkcs7(const T& obj, size_t padded_size) {
    T res(obj);
    pkcs7(res, padded_size, std::back_inserter(res));
    return res;
}

int get_pkcs7_padding(const byte_vector& bytes) {
    if (bytes.size() == 0) {
        return false;
    }

    const int size = bytes.size();
    byte last_byte = bytes[size - 1];

    bool is_valid = std::all_of(next(begin(bytes), size - last_byte), end(bytes),
        [&] (const byte& b) { return b == last_byte; });

    return is_valid ? last_byte : 0;
}

bool has_valid_pkcs7_padding(const byte_vector& bytes) {
    return get_pkcs7_padding(bytes) > 0;
}

bool strip_pkcs7_padding(const byte_vector& in, byte_vector& out) {
    int padding_size = get_pkcs7_padding(in);
    if (padding_size == -1) {
        return false;
    }

    out = byte_vector(begin(in), next(begin(in), in.size() - padding_size));
    return true;
}

#endif//__PADDING_HPP__
