#include <vector>
#include <fstream>

std::vector<std::string> read_lines_from_file(const std::string &filename) {
    std::vector<std::string> lines;

    std::ifstream file(filename);

    while (!file.fail()) {
        std::string line;
        file >> line;

        if (file.eof()) {
            break;
        }

        lines.push_back(line);
    }

    return lines;
}

