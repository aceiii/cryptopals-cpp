#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "hex.hpp"
#include "xor.hpp"
#include "base64.hpp"


using namespace std::string_literals;

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
    }
    for (it++; it != end(v); it++) {
        os << ", " << *it;
    }
    return (os << "]");
}

int main() {
    auto cipher_str_hex = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736"s;
    auto cipher_str_bytes = hex_to_bytes(cipher_str_hex);

    auto freq = map_byte_frequency(cipher_str_bytes);
    auto freq_vec = map_to_vector(freq);

    std::sort(begin(freq_vec), end(freq_vec), byte_freq_cmp<std::greater<byte_freq_pair::second_type>>());

    //std::cout << freq_vec << std::endl;

    auto decipher_str_bytes = cipher_str_bytes ^ freq_vec[0].first;
    auto decipher_str = bytes_to_str(decipher_str_bytes);

    std::cout << decipher_str << std::endl;

    return 0;
}

