#ifndef __BASE64_H__
#define __BASE64_H__

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "base64.h"


typedef std::vector<unsigned char> byte_vector;


byte_vector hex_to_bytes(const std::string& s) {
    size_t len = s.size() / 2;
    byte_vector b(len);

    size_t pos = 0;
    for (auto it = begin(s); it != end(s); it++) {
        std::stringstream ss;
        ss << *it++;
        ss << *it;

        unsigned int c;
        ss >> std::hex >> c;

        b[pos++] = c;
    }

    return b;
}

std::string bytes_to_base64(const byte_vector& b) {
    static const char* codes = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";
    std::stringstream ss;

    for (int i = 0; i < b.size(); i += 3) {
        unsigned char c = (b[i] & 0xFC) >> 2;
        ss << codes[c];
        c = (b[i] & 0x03) << 4;
        if (i + 1 < b.size()) {
            c |= (b[i + 1] & 0xF0) >> 4;
            ss << codes[c];
            c = (b[i + 1] & 0x0F) << 2;
            if (i + 2 < b.size()) {
                c |= (b[i + 2] & 0xC0) >> 6;
                ss << codes[c];
                c = b[i + 2] & 0x3F;
                ss << codes[c];
            } else {
                ss << codes[c];
                ss << "=";
            }
        } else {
            ss << codes[c];
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
        //os << std::hex << (int)c << ", ";
        os << c;
    }

    return (os << ")");
}

#endif//__BASE64_H__

