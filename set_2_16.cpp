#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cassert>

#include "byte_vector.hpp"
#include "padding.hpp"
#include "aes.hpp"
#include "random_bytes.hpp"
#include "hex.hpp"
#include "string.hpp"

using namespace std::string_literals;

namespace {
    random_bytes rand_bytes;

    byte_vector random_key = rand_bytes.next_n(aes::block_size);
    byte_vector random_iv = rand_bytes.next_n(aes::block_size);
}

std::string quote_cookie_string_val(const std::string& str) {
    std::stringstream ss;

    for (auto it = begin(str); it != end(str); it++) {
        auto c = *it;
        if (c == '=' || c == ';' || c == '%') {
            ss << "%" << std::setw(2) << std::setfill('0') << std::hex << (int) c;
        } else {
            ss << c;
        }
    }

    return ss.str();
}

std::string gen_cookie_string(const std::string& str) {
    static const auto prepend = "comment1=cooking%20MCs;userdata="s;
    static const auto append = ";comment2=%20like%20a%20pound%20of%20bacon"s;

    std::string sanitized_string = quote_cookie_string_val(str);

    return prepend + sanitized_string + append;
}

byte_vector encryption_oracle(const std::string& str) {
    constexpr size_t block_size = aes::block_size;

    byte_vector bytes = str_to_bytes(gen_cookie_string(str));
    byte_vector padded_bytes = padded_pkcs7(bytes, ((bytes.size() / block_size) + 1) * block_size);

    byte_vector enc = aes_cbc_encrypt(padded_bytes, random_key, random_iv);

    return enc;
}

byte_vector decryption_oracle(const byte_vector& ciphertext) {
    byte_vector dec = aes_cbc_decrypt(ciphertext, random_key, random_iv);

    byte_vector plaintext;
    if (strip_pkcs7_padding(dec, plaintext)) {
        return plaintext;
    }
    return dec;
}

bool is_admin(const std::string& cookie_str) {
    std::vector<std::string> pairs = split(cookie_str, ';');
    return std::any_of(begin(pairs), end(pairs), [] (const std::string& pair) { return pair == "admin=true"; });
}

void print_blocks(const byte_vector& blocks, const size_t& block_size) {

}

void test1() {
    auto s1 = "1 + 1 = 2; 3 % 2 = 1"s;
    auto s2 = "1 + 1 %3d 2%3b 3 %25 2 %3d 1"s;
    auto result = quote_cookie_string_val(s1);

    std::cout << s1 << std::endl;
    std::cout << result << std::endl;

    assert(s2 == result);
    std::cout << "Success!" << std::endl;
}

void test2() {
    const auto prepend = "comment1=cooking%20MCs;userdata="s;
    const auto append = ";comment2=%20like%20a%20pound%20of%20bacon"s;

    std::string cookie1 = gen_cookie_string("hello world!"s);
    std::string cookie2 = prepend + "hello world!"s + append;

    assert(cookie1 == cookie2);
    std::cout << "Success!" << std::endl;
}

void test3() {
    constexpr size_t block_size = aes::block_size;
    const auto str = "Hello world!"s;

    std::string cookie_str = gen_cookie_string(str);
    byte_vector enc = encryption_oracle(str);

    int padded_cookie_str_size = ((cookie_str.size() / block_size) + 1) * block_size;

    assert(enc.size()  == padded_cookie_str_size);
    std::cout << "Success!" << std::endl;
}

void test4() {
    auto s1 = "helo=world;test;admin=true"s;
    auto s2 = "admin=false;blah=foobar"s;
    auto s3 = "this is=wtf"s;
    auto s4 = "admin=true;blah blah"s;
    auto s5 = "blah=foobar ;admin=true;wtf=hmm"s;

    assert(is_admin(s1) == true);
    assert(is_admin(s2) == false);
    assert(is_admin(s3) == false);
    assert(is_admin(s4) == true);
    assert(is_admin(s5) == true);

    std::cout << "Success!" << std::endl;
}

int main() {
    test1();
    test2();
    test3();
    test4();
    return 0;
}

