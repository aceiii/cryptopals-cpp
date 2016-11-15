#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cassert>

#include "byte_vector.hpp"
#include "byte_freqency.hpp"
#include "random_bytes.hpp"
#include "aes.hpp"
#include "string.hpp"
#include "keyval.hpp"

using namespace std::string_literals;

namespace {
    byte_vector random_key = random_bytes().next_n(aes::block_size);
}

template <typename K, typename V>
std::ostream& operator << (std::ostream& os, const std::map<K, V>& m) {
    os << "{ ";
    auto start = begin(m);
    auto stop = end(m);
    if (start != stop) {
        os << start->first << ": " << start->second;
        while (++start != stop) {
            os << ", " << start->first << ": " << start->second;
        }
    }
    os << " }";
    return os;
}

keyval_list profile_for(const std::string& email) {
    return {
        {"email", sanitize_query_string_val(email)},
        {"uid", "10"},
        {"role", "user"},
    };
}

template <typename T>
std::vector<T> split_blocks(const T& buf, const size_t& block_size) {
    const int num_blocks = buf.size() / block_size;
    const int remainder = buf.size() % block_size;

    std::vector<T> blocks;
    for (int i = 0; i < num_blocks; i += 1) {
        auto start = next(begin(buf), i * block_size);
        auto stop = next(start, block_size);
        T block(start, stop);
        blocks.push_back(block);
    }
    if (remainder > 0) {
        auto start = next(begin(buf), num_blocks * block_size);
        auto stop = end(buf);
        T block(start, stop);
        blocks.push_back(block);
    }

    return blocks;
}

byte_vector encryption_oracle(const std::string& email) {
    constexpr size_t block_size = aes::block_size;

    keyval_list profile = profile_for(email);
    std::string query_string = keyval_list_to_querystring(profile);

    auto split_qs = split_blocks(query_string, block_size);

    byte_vector bytes = str_to_bytes(query_string);
    byte_vector buffer(((bytes.size() / block_size) + 1) * block_size, 0);
    std::copy(begin(bytes), end(bytes), begin(buffer));

    byte_vector out = aes_ecb_encrypt(buffer, random_key);

    return out;
}

keyval_list decryption_oracle(const byte_vector& ciphertext) {
    constexpr size_t block_size = aes::block_size;

    byte_vector plaintext = aes_ecb_decrypt(ciphertext, random_key);
    std::string query_string = bytes_to_str(plaintext);

    return parse_query_string(query_string);
}

void test1() {
    auto query_string = "foo=bar&baz=qux&zap=zazzle"s;
    auto out = parse_query_string(query_string);
    keyval_list res = {
        {"foo", "bar"},
        {"baz", "qux"},
        {"zap", "zazzle"},
    };

    std::cout << out << std::endl;
    std::cout << res << std::endl;

    assert(out == res);
    std::cout << "Success!" << std::endl;
}

void test2() {
    std::string str = "my&string=with&forbidden=chars"s;
    std::string res = "mystringwithforbiddenchars"s;

    std::string santized = sanitize_query_string_val(str);

    std::cout << str << " => " << santized << std::endl;

    assert(santized == res);
    std::cout << "Success!" << std::endl;
}

void test3() {
    keyval_list profile = profile_for("foo@bar.com&role=admin");
    std::string qs = keyval_list_to_querystring(profile);
    std::cout << qs << std::endl;
}

int main() {
    constexpr size_t block_size = aes::block_size;

    byte_vector enc1 = encryption_oracle("aceiii\0\0\0\0"s);
    byte_vector enc2 = encryption_oracle("0123456789\0\0\0\0\0\0\0\0\0\0"s);
    byte_vector enc3 = encryption_oracle("0123456789admin\0\0\0\0\0\0\0\0\0\0\0"s);
    byte_vector enc4 = encryption_oracle("0123456789012345"s);

    std::vector<byte_vector> enc1_blocks = split_blocks(enc1, block_size);
    std::vector<byte_vector> enc2_blocks = split_blocks(enc2, block_size);
    std::vector<byte_vector> enc3_blocks = split_blocks(enc3, block_size);
    std::vector<byte_vector> enc4_blocks = split_blocks(enc4, block_size);

    byte_vector new_bytes;

    std::copy(begin(enc1_blocks[0]), end(enc1_blocks[0]), std::back_inserter(new_bytes));
    std::copy(begin(enc2_blocks[1]), end(enc2_blocks[1]), std::back_inserter(new_bytes));
    std::copy(begin(enc3_blocks[1]), end(enc3_blocks[1]), std::back_inserter(new_bytes));
    std::copy(begin(enc4_blocks[2]), end(enc4_blocks[2]), std::back_inserter(new_bytes));

    std::cout << "ciphertext: " << bytes_to_hex(new_bytes) << std::endl;

    auto profile = decryption_oracle(new_bytes);
    std::cout << "profile: " << profile << std::endl;

    return 0;
}

