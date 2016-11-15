#ifndef __STRING_HPP__
#define __STRING_HPP__

#include <vector>
#include <string>

#include "keyval.hpp"

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

keyval_list parse_query_string(const std::string& qs) {
    const char separator = '&';
    const char keyval_separator = '=';

    keyval_list result;

    std::vector<std::string> parts = split(qs, separator);
    std::for_each(begin(parts), end(parts), [&] (const std::string& part) {
        std::vector<std::string> keyval = split(part, keyval_separator);
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

std::string sanitize_query_string_val(const std::string& line) {
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

std::string keyval_list_to_querystring(const keyval_list& m) {
    constexpr char separator = '&';
    constexpr char keyval_separator = '=';

    std::stringstream ss;

    bool first = true;
    for (auto it = begin(m); it != end(m); it++) {
        std::string key = sanitize_query_string_val(it->first);
        std::string val = sanitize_query_string_val(it->second);

        if (first) {
            first = false;
        } else {
            ss << separator;
        }

        ss << key << keyval_separator << val;
    }

    return ss.str();
}

#endif//__STRING_HPP__
