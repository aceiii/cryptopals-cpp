#ifndef __BYTE_VECTOR_HPP__
#define __BYTE_VECTOR_HPP__

#include <vector>
#include <sstream>

typedef unsigned char byte;
typedef std::vector<unsigned char> byte_vector;

std::string bytes_to_str(const byte_vector& v) {
    std::stringstream ss;

    for (int i = 0; i < v.size(); i += 1) {
        ss << v[i];
    }

    return ss.str();
}

#endif//__BYTE_VECTOR_HPP__
