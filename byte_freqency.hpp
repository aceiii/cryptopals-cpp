#ifndef __BYTE_FREQUENCY_HPP__
#define __BYTE_FREQUENCY_HPP__

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <cmath>

#include "hex.hpp"
#include "xor.hpp"
#include "base64.hpp"
#include "file.hpp"

typedef std::map<byte, double> byte_freq_map;
typedef std::pair<byte, double> byte_freq_pair;
typedef std::vector<byte_freq_pair> byte_freq_vector;

byte_freq_map map_byte_frequency(const byte_vector& bytes) {
    byte_freq_map m;

    double count = double(bytes.size());

    for (const byte b: bytes) {
        m[b] += 1.0 / count;
    }

    return m;
}

byte_freq_map map_byte_frequency_from_file(const std::string &filename) {
    std::vector<std::string> lines = read_lines_from_file(filename);
    byte_vector bytes;

    for (const auto &line : lines) {
        byte_vector line_bytes = str_to_bytes(line);
        std::copy(line_bytes.begin(), line_bytes.end(), std::back_inserter(bytes));
    }

    return map_byte_frequency(bytes);
}

byte_freq_vector map_to_vector(const byte_freq_map& m) {
    byte_freq_vector v;
    std::copy(begin(m), end(m), std::back_inserter(v));

    return v;
}

std::vector<double> freq_map_to_list(const byte_freq_map &freq_map) {
    std::vector<double> freq_list(256, 0.0);

    for (const auto &pair : freq_map) {
        freq_list[pair.first] = pair.second;
    }

    return freq_list;
}

double compare_freq_map(const byte_freq_map &f1, const byte_freq_map &f2) {
    std::set<byte> visited_keys;
    double result = 0.0;

    for (const auto &p1 : f1) {
        const auto &key = p1.first;
        if (visited_keys.find(key) != visited_keys.end()) {
            continue;
        }

        double f2_val = 0.0;
        auto p2 = f2.find(key);
        if (p2 != f2.end()) {
            f2_val = p2->second;
        }

        result += std::abs(p1.second);// - f2_val);
    }

    for (const auto &p2 : f2) {
        const auto &key = p2.first;
        if (visited_keys.find(key) != visited_keys.end()) {
            continue;
        }

        double f1_val = 0.0;
        auto p1 = f1.find(key);
        if (p1 != f1.end()) {
            f1_val = p1->second;
        }

        result += std::abs(p2.second - f1_val);
    }

    return result;
}

double compare_freq_list(const std::vector<double> &f1, const std::vector<double> &f2) {
    size_t min_size = std::min(f1.size(), f2.size());
    size_t max_size = std::max(f1.size(), f2.size());

    double result = double(max_size) - double(min_size);

    for (int i = 0; i < (int)min_size; i += 1) {
        result += std::abs(double(f1[i]) - double(f2[i]));
    }

    return result;
}

template <typename Op = std::less<byte_freq_pair::first_type>>
struct byte_freq_cmp {
    const Op op;

    byte_freq_cmp(const Op op_ = Op()):op(op_) {}

    bool operator () (const byte_freq_pair& lhs, const byte_freq_pair& rhs) const {
        return op(lhs.second, rhs.second);
    }
};

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
