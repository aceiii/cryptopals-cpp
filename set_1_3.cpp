#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "hex.hpp"
#include "xor.hpp"
#include "base64.hpp"
#include "byte_vector.hpp"
#include "byte_freqency.hpp"

using namespace std::string_literals;

int main() {
    auto cipher_str_hex = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736"s;
    auto cipher_str_bytes = hex_to_bytes(cipher_str_hex);

    auto freq = map_byte_frequency(cipher_str_bytes);
    auto freq_vec = map_to_vector(freq);

    sort_freq_vec_desc(freq_vec);

    //std::cout << freq_vec << std::endl;

    auto decipher_str_bytes = cipher_str_bytes ^ freq_vec[0].first;
    auto decipher_str = bytes_to_str(decipher_str_bytes);

    std::cout << decipher_str << std::endl;

    return 0;
}

