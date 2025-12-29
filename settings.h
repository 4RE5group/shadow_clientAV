#ifndef SETTINGSFILEHANDLER_H_
#define SETTINGSFILEHANDLER_H_

#include <limits>
#include <string>
#include <fstream>
#include <stdexcept>
#include <ios>

char* getSetting(char* name,  char* path);
void writeSetting(char* name, char* value, char* path);



std::string get_hash_from_line(const std::string& line) {
    auto pos = line.find(":");
    if (pos != std::string::npos) {
        return line.substr(0, pos);  // Return only the hash part
    }
    return "";  // Return empty string if format is invalid
}

std::string search_hex_hash(const std::string& file_path, const std::string& target_hash) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + file_path);
    }

    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();
    std::streampos start = 0;
    std::streampos end = file_size;

    std::string line;
    while (start <= end) {
        std::streampos mid = (start + end) / 2;
        file.seekg(mid);

        // Move to the beginning of the current line
        if (mid > 0) {
            file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        if (!std::getline(file, line)) {
            break;  // End of file reached
        }

        std::string hash = get_hash_from_line(line);

        if (hash == target_hash) {
            return line.substr(hash.length() + 2);  // Return the text part after ": "
        } else if (hash < target_hash) {
            start = file.tellg();  // Move to the right
        } else {
            end = mid - 1;  // Move to the left
        }

        // Handle special case where the file has only one line
        if (start == 0 && end == 0) {
            file.seekg(0);
            if (std::getline(file, line)) {
                hash = get_hash_from_line(line);
                if (hash == target_hash) {
                    return line.substr(hash.length() + 2);
                }
            }
            break;
        }
    }

    return "";  // Hash not found
}


#endif
