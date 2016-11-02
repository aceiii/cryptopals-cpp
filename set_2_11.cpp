#include <cassert>
#include <iostream>
#include <string>
#include <vector>

#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "base64.hpp"
#include "aes.hpp"
#include "random_bytes.hpp"

int main() {

    random_bytes rand;

    constexpr size_t block_size = 16;

    byte_vector rand_key = rand.next_n_bytes(block_size);

    std::cout << rand_key << std::endl;

    return 0;
}

