#ifndef __BYTE_VECTOR_HPP__
#define __BYTE_VECTOR_HPP__

#include <vector>
#include <sstream>
#include <algorithm>

#include "byte.hpp"

typedef std::vector<unsigned char> byte_vector;
typedef std::vector<byte_vector> byte_vector_list;

std::string bytes_to_str(const byte_vector& v) {
    return std::string(begin(v), end(v));
}

byte_vector str_to_bytes(const std::string& s) {
    byte_vector v;
    std::copy(begin(s), end(s), back_inserter(v));
    return v;
}


#endif//__BYTE_VECTOR_HPP__
