#ifndef __HEX_HPP__
#define __HEX_HPP__

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

#include "byte.hpp"
#include "byte_vector.hpp"

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

std::string bytes_to_hex(const byte_vector& b) {
    std::stringstream ss;
    for (int i = 0; i < b.size(); i += 1) {
        ss << std::setw(2) << std::setfill('0') << std::hex << (int)b[i];
    }

    return ss.str();
}

#endif//__HEX_HPP__
