#ifndef __BASE64_HPP__
#define __BASE64_HPP__

#include <cassert>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "byte.hpp"
#include "byte_vector.hpp"
#include "hex.hpp"

namespace {
    using namespace std::string_literals;

    const std::string Codes = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/="s;
}

std::string bytes_to_base64(const byte_vector& b) {
    std::stringstream ss;

    for (int i = 0; i < b.size(); i += 3) {
        unsigned char c = (b[i] & 0xFC) >> 2;
        ss << Codes[c];
        c = (b[i] & 0x03) << 4;
        if (i + 1 < b.size()) {
            c |= (b[i + 1] & 0xF0) >> 4;
            ss << Codes[c];
            c = (b[i + 1] & 0x0F) << 2;
            if (i + 2 < b.size()) {
                c |= (b[i + 2] & 0xC0) >> 6;
                ss << Codes[c];
                c = b[i + 2] & 0x3F;
                ss << Codes[c];
            } else {
                ss << Codes[c];
                ss << "=";
            }
        } else {
            ss << Codes[c];
            ss << "==";
        }
    }

    return ss.str();
}

std::string string_to_base64(const std::string& s) {
    byte_vector b(s.size());
    for (int i = 0; i < b.size(); i += 1) {
        b[i] = s[i];
    }
    return bytes_to_base64(b);
}

byte_vector base64_to_bytes(const std::string& s) {
    assert(s.size() % 4 == 0);

    auto eq_pos = s.find('=');
    byte_vector d(((s.size() * 3) / 4) - (eq_pos != std::string::npos ? s.size() - eq_pos : 0));

    int j = 0;
    int b[4];

    for (int i = 0; i < s.size(); i += 4) {
        b[0] = Codes.find(s[i]);
        b[1] = Codes.find(s[i + 1]);
        b[2] = Codes.find(s[i + 2]);
        b[3] = Codes.find(s[i + 3]);

        d[j++] = (byte) ((b[0] << 2) | (b[1] >> 4));
        if (b[2] < 64) {
            d[j++] = (byte) ((b[1] << 4) | (b[2] >> 2));
            if (b[3] < 64) {
                d[j++] = (byte) ((b[2] << 6) | b[3]);
            }
        }
    }

    return d;
}

struct print_byte_vector_t{
    const byte_vector& bytes;
    print_byte_vector_t(const byte_vector& b):bytes(b) {}
};

print_byte_vector_t print_byte_vector(const byte_vector& b) {
    return print_byte_vector_t { b };
}

std::ostream& operator << (std::ostream& os, const print_byte_vector_t& bvt) {
    os << "b(";

    for (auto c: bvt.bytes) {
        os << c;
    }

    return (os << ")");
}

#endif//__BASE64_HPP__

