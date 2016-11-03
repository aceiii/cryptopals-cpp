#ifndef __BYTE_FREQUENCY_HPP__
#define __BYTE_FREQUENCY_HPP__

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "hex.hpp"
#include "xor.hpp"
#include "base64.hpp"

typedef std::map<byte, size_t> byte_freq_map;
typedef std::pair<byte, size_t> byte_freq_pair;
typedef std::vector<byte_freq_pair> byte_freq_vector;

byte_freq_map map_byte_frequency(const byte_vector& bytes) {
    byte_freq_map m;

    for (const byte b: bytes) {
        m[b] += 1;
    }

    return m;
}

byte_freq_vector map_to_vector(const byte_freq_map& m) {
    byte_freq_vector v;
    std::copy(begin(m), end(m), std::back_inserter(v));

    return v;
}

template <typename Op = std::less<byte_freq_pair::first_type>>
struct byte_freq_cmp {
    const Op op;

    byte_freq_cmp(const Op op_ = Op()):op(op_) {}

    bool operator () (const byte_freq_pair& lhs, const byte_freq_pair& rhs) const {
        return op(lhs.second, rhs.second);
    }
};

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

void sort_freq_vec_desc(byte_freq_vector& freq_vec) {
    std::sort(begin(freq_vec), end(freq_vec), byte_freq_cmp<std::greater<byte_freq_pair::second_type>>());
}

void sort_freq_vec_asc(byte_freq_vector& freq_vec) {
    std::sort(begin(freq_vec), end(freq_vec), byte_freq_cmp<std::less<byte_freq_pair::second_type>>());
}

struct char_or_hex_t {
    byte b;
};

std::ostream& operator << (std::ostream& os, const char_or_hex_t& c) {
    if (c.b == '\'') {
        return (os << "\\'");
    }
    if (c.b >= 32 && c.b < 127) {
        return (os << "'" << c.b << "'");
    }
    return (os << "0x" << std::hex << (int)c.b);
}

std::ostream& operator << (std::ostream& os, const byte_vector& v) {
    os << "[";
    auto it = begin(v);
    if (it != end(v)) {
        os << char_or_hex_t { *it };
        for (it++; it != end(v); it++) {
            os << ", " << char_or_hex_t { *it };
        }
    }
    return (os << "]");
}

#endif//__BYTE_FREQUENCY_HPP__
