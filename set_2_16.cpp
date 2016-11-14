#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include "byte_vector.hpp"
#include "padding.hpp"
#include "aes.hpp"

using namespace std::string_literals;

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

int main() {
    test1();
    test2();
    return 0;
}

