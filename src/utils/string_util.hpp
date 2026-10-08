
#include <vector>
#include <string>

#pragma once

std::vector<std::string> split_string(std::string s, std::string delim) {
    std::vector<std::string> result;

    if (delim.empty()) {
        result.push_back(s);
        return result;
    }

    size_t start = 0;
    size_t pos = s.find(delim, start);
    while (pos != std::string::npos) {
        result.push_back(s.substr(start, pos - start));
        start = pos + delim.length();
        pos = s.find(delim, start);
    }
    result.push_back(s.substr(start));
    return result;
}
