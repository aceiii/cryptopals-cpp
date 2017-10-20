#ifndef __BYTE_UTIL_HPP__
#define __BYTE_UTIL_HPP__

#include "byte.hpp"
#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "base64.hpp"

enum DecodeAsEnum {
    kDecodeAs_ASCII,
    kDecodeAs_BASE64,
};

std::vector<byte_vector> lines_to_bytes(const std::vector<std::string> &lines, DecodeAsEnum decodeAs = kDecodeAs_ASCII) {
    std::vector<byte_vector> lines_bytes;

    for (const auto &line : lines) {
        byte_vector bytes;

        switch (decodeAs) {
            case kDecodeAs_BASE64:
                bytes = str_to_bytes(line);
                break;

            case kDecodeAs_ASCII:
            default:
                bytes = base64_to_bytes(line);
                break;
        }

        lines_bytes.push_back(bytes);
    }

    return lines_bytes;
}

byte_vector_list transpose_blocks(const byte_vector_list &blocks, size_t max_length) {
    byte_vector_list transposed(max_length);

    for (int i = 0; i < max_length; i += 1) {
        for (const auto &block : blocks) {
            transposed[i].push_back(block[i]);
        }
    }

    return transposed;
}

std::ostream& operator << (std::ostream& os, const byte_freq_pair& pair) {
    return (os << "{" << (int)pair.first << ", " << pair.second << "}");
}

template <typename T>
std::ostream& operator << (std::ostream& os, const std::vector<T>& v) {
    os << "[";
    auto it = begin(v);
    if (it != end(v)) {
        os << *it;
        for (it++; it != end(v); it++) {
            os << ", " << *it;
        }
    }
    return (os << "]");
}

#endif//__BYTE_UTIL_HPP__
