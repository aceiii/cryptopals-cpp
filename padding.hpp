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

#endif//__PADDING_HPP__
