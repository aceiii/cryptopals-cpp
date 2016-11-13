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

int main() {
    test1();
    return 0;
}

