#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <cassert>

#include "byte_vector.hpp"
#include "byte_freqency.hpp"

using namespace std::string_literals;

std::vector<std::string> split(const std::string& line, const char& separator) {
    std::vector<std::string> result;

    auto first = begin(line);
    auto last = first;
    do {
        last = find(first, end(line), separator);

        std::string part(first, last);
        result.push_back(part);

        first = next(last, 1);

    } while (last != end(line));

    return result;
}

std::map<std::string, std::string> parse_query_string(const std::string& qs) {
    const char separator = '&';
    const char key_val_separator = '=';

    std::map<std::string, std::string> result;

    std::vector<std::string> parts = split(qs, separator);
    std::for_each(begin(parts), end(parts), [&] (const std::string& part) {
        std::vector<std::string> keyval = split(part, key_val_separator);
        if (keyval.empty()) {
            return;
        }
        std::string key = keyval[0];
        std::string val;
        if (keyval.size() > 1) {
            val = keyval[1];
        }

        result[key] = val;
    });

    return result;
}

std::string sanitize_query_string_key_val(const std::string& line) {
    static const std::vector<char> chars_to_remove = {'&', '='};

    std::stringstream ss;

    auto first = begin(line);
    auto last = first;
    do {
        last = find_first_of(first, end(line), begin(chars_to_remove), end(chars_to_remove));

        std::string part(first, last);
        ss << part;

        first = next(last, 1);

    } while (last != end(line));

    return ss.str();
}

std::string keyval_map_to_querystring(const std::map<std::string, std::string>& m) {
    constexpr char separator = '&';
    constexpr char key_val_separator = '=';

    std::stringstream ss;

    bool first = true;
    for (auto it = begin(m); it != end(m); it++) {
        std::string key = sanitize_query_string_key_val(it->first);
        std::string val = sanitize_query_string_key_val(it->second);

        if (first) {
            first = false;
        } else {
            ss << separator;
        }

        ss << key << key_val_separator << val;
    }

    return ss.str();
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

std::map<std::string, std::string> profile_for(const std::string& email) {
    return {
        {"email", sanitize_query_string_key_val(email)},
        {"uid", "10"},
        {"role", "user"},
    };
}

void test1() {
    auto query_string = "foo=bar&baz=qux&zap=zazzle"s;
    auto out = parse_query_string(query_string);
    std::map<std::string, std::string> res = {
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

    std::string santized = sanitize_query_string_key_val(str);

    std::cout << str << " => " << santized << std::endl;

    assert(santized == res);
    std::cout << "Success!" << std::endl;
}

void test3() {
    std::map<std::string, std::string> profile = profile_for("foo@bar.com&role=admin");
    std::string qs = keyval_map_to_querystring(profile);
    std::cout << qs << std::endl;
}

int main() {
    test1();
    test2();
    test3();
    return 0;
}

